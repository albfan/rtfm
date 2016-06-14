/* fuzzy-index.c
 *
 * Copyright (C) 2016 Christian Hergert <chergert@redhat.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define G_LOG_DOMAIN "fuzzy-index"

#include "fuzzy-index.h"
#include "fuzzy-index-cursor.h"
#include "fuzzy-index-private.h"

typedef struct
{
  guint key_id;
  guint document_id;
} LookasideEntry;

struct _FuzzyIndex
{
  GObject       object;

  guint         loaded : 1;
  guint         case_sensitive : 1;

  GMappedFile  *mapped_file;

  /*
   * Toplevel variant for the whole document. This is loaded from the entire
   * contents of @mapped_file. It contains a dictionary of "a{sv}"
   * containing all of our index data tables.
   */
  GVariant *variant;

  /*
   * This is a variant containing the array of documents. The index of the
   * document is the corresponding document_id used in other data structres.
   * This maps to the "documents" field in @variant.
   */
  GVariant *documents;

  /*
   * The keys found within the index. The index of the key is the "key_id"
   * used in other datastructures, such as the @lookaside array.
   */
  GVariant *keys;

  /*
   * The lookaside array is used to disambiguate between multiple keys
   * pointing to the same document. Each element in the array is of type
   * "(uu)" with the first field being the "key_id" and the second field
   * being the "document_id". Each of these are indexes into the
   * corresponding @documents and @keys arrays.
   *
   * This is a fixed array type and therefore can have the raw data
   * accessed with g_variant_get_fixed_array() to save on lookup
   * costs.
   */
  GVariant *lookaside;

  /*
   * Raw pointers for fast access to the lookaside buffer.
   */
  const LookasideEntry *lookaside_raw;
  gsize lookaside_len;

  /*
   * This vardict is used to get the fixed array containing the
   * (offset, lookaside_id) for each unicode character in the index.
   * These are accessed by the cursors to layout the fulltext search
   * index by each character in the input string. Doing so, is what
   * gives us the O(mn) worst-case running time.
   */
  GVariantDict *tables;

  /*
   * The metadata located within the search index. This contains
   * metadata set with fuzzy_index_builder_set_metadata() or one
   * of its typed variants.
   */
  GVariantDict *metadata;
};

G_DEFINE_TYPE (FuzzyIndex, fuzzy_index, G_TYPE_OBJECT)

static void
fuzzy_index_finalize (GObject *object)
{
  FuzzyIndex *self = (FuzzyIndex *)object;

  g_clear_pointer (&self->mapped_file, g_mapped_file_unref);
  g_clear_pointer (&self->variant, g_variant_unref);
  g_clear_pointer (&self->documents, g_variant_unref);
  g_clear_pointer (&self->keys, g_variant_unref);
  g_clear_pointer (&self->tables, g_variant_dict_unref);
  g_clear_pointer (&self->lookaside, g_variant_unref);

  G_OBJECT_CLASS (fuzzy_index_parent_class)->finalize (object);
}

static void
fuzzy_index_class_init (FuzzyIndexClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = fuzzy_index_finalize;
}

static void
fuzzy_index_init (FuzzyIndex *self)
{
}

FuzzyIndex *
fuzzy_index_new (void)
{
  return g_object_new (FUZZY_TYPE_INDEX, NULL);
}

static void
fuzzy_index_load_file_worker (GTask        *task,
                              gpointer      source_object,
                              gpointer      task_data,
                              GCancellable *cancellable)
{
  g_autofree gchar *path = NULL;
  g_autoptr(GMappedFile) mapped_file = NULL;
  g_autoptr(GVariant) variant = NULL;
  g_autoptr(GVariant) documents = NULL;
  g_autoptr(GVariant) lookaside = NULL;
  g_autoptr(GVariant) keys = NULL;
  g_autoptr(GVariant) tables = NULL;
  g_autoptr(GVariant) metadata = NULL;
  FuzzyIndex *self = source_object;
  GFile *file = task_data;
  GVariantDict dict;
  GError *error = NULL;
  gint version = 0;
  gboolean case_sensitive = FALSE;

  g_assert (FUZZY_IS_INDEX (self));
  g_assert (G_IS_FILE (file));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  if (self->loaded)
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_INVAL,
                               "Cannot load index multiple times");
      return;
    }

  self->loaded = TRUE;

  if (!g_file_is_native (file) || NULL == (path = g_file_get_path (file)))
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_INVALID_FILENAME,
                               "Index must be a local file");
      return;
    }

  if (NULL == (mapped_file = g_mapped_file_new (path, FALSE, &error)))
    {
      g_task_return_error (task, error);
      return;
    }

  variant = g_variant_new_from_data (G_VARIANT_TYPE_VARDICT,
                                     g_mapped_file_get_contents (mapped_file),
                                     g_mapped_file_get_length (mapped_file),
                                     FALSE, NULL, NULL);

  if (variant == NULL)
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_INVAL,
                               "Failed to parse GVariant");
      return;
    }

  g_variant_ref_sink (variant);

  g_variant_dict_init (&dict, variant);

  if (!g_variant_dict_lookup (&dict, "version", "i", &version) || version != 1)
    {
      g_variant_dict_clear (&dict);
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_INVAL,
                               "Version mismatch in gvariant. Got %d, expected 1",
                               version);
      return;
    }

  documents = g_variant_dict_lookup_value (&dict, "documents", G_VARIANT_TYPE_ARRAY);
  keys = g_variant_dict_lookup_value (&dict, "keys", G_VARIANT_TYPE_STRING_ARRAY);
  lookaside = g_variant_dict_lookup_value (&dict, "lookaside", G_VARIANT_TYPE_ARRAY);
  tables = g_variant_dict_lookup_value (&dict, "tables", G_VARIANT_TYPE_VARDICT);
  metadata = g_variant_dict_lookup_value (&dict, "metadata", G_VARIANT_TYPE_VARDICT);
  g_variant_dict_clear (&dict);

  if (keys == NULL || documents == NULL || tables == NULL || metadata == NULL)
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_INVAL,
                               "Invalid gvariant index");
      return;
    }

  self->mapped_file = g_steal_pointer (&mapped_file);
  self->variant = g_steal_pointer (&variant);
  self->documents = g_steal_pointer (&documents);
  self->lookaside = g_steal_pointer (&lookaside);
  self->keys = g_steal_pointer (&keys);
  self->tables = g_variant_dict_new (tables);
  self->metadata = g_variant_dict_new (metadata);

  self->lookaside_raw = g_variant_get_fixed_array (self->lookaside,
                                                   &self->lookaside_len,
                                                   sizeof (LookasideEntry));

  if (g_variant_dict_lookup (self->metadata, "case-sensitive", "b", &case_sensitive))
    self->case_sensitive = !!case_sensitive;

  g_task_return_boolean (task, TRUE);
}

void
fuzzy_index_load_file_async (FuzzyIndex          *self,
                             GFile               *file,
                             GCancellable        *cancellable,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  g_return_if_fail (FUZZY_IS_INDEX (self));
  g_return_if_fail (G_IS_FILE (file));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_source_tag (task, fuzzy_index_load_file);
  g_task_set_task_data (task, g_object_ref (file), g_object_unref);
  g_task_set_check_cancellable (task, FALSE);
  g_task_run_in_thread (task, fuzzy_index_load_file_worker);
}

gboolean
fuzzy_index_load_file_finish (FuzzyIndex    *self,
                              GAsyncResult  *result,
                              GError       **error)
{
  g_return_val_if_fail (FUZZY_IS_INDEX (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (result), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

gboolean
fuzzy_index_load_file (FuzzyIndex    *self,
                       GFile         *file,
                       GCancellable  *cancellable,
                       GError       **error)
{
  g_autoptr(GTask) task = NULL;

  g_return_val_if_fail (FUZZY_IS_INDEX (self), FALSE);
  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  task = g_task_new (self, cancellable, NULL, NULL);
  g_task_set_source_tag (task, fuzzy_index_load_file);
  g_task_set_task_data (task, g_object_ref (file), g_object_unref);
  g_task_set_check_cancellable (task, FALSE);

  fuzzy_index_load_file_worker (task, self, file, cancellable);

  return g_task_propagate_boolean (task, error);
}

static void
fuzzy_index_query_cb (GObject      *object,
                      GAsyncResult *result,
                      gpointer      user_data)
{
  FuzzyIndexCursor *cursor = (FuzzyIndexCursor *)object;
  g_autoptr(GTask) task = user_data;
  GError *error = NULL;

  g_assert (FUZZY_IS_INDEX_CURSOR (cursor));
  g_assert (G_IS_ASYNC_RESULT (result));
  g_assert (G_IS_TASK (task));

  if (!g_async_initable_init_finish (G_ASYNC_INITABLE (cursor), result, &error))
    g_task_return_error (task, error);
  else
    g_task_return_pointer (task, g_object_ref (cursor), g_object_unref);
}

void
fuzzy_index_query_async (FuzzyIndex          *self,
                         const gchar         *query,
                         guint                max_matches,
                         GCancellable        *cancellable,
                         GAsyncReadyCallback  callback,
                         gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;
  g_autoptr(FuzzyIndexCursor) cursor = NULL;

  g_return_if_fail (FUZZY_IS_INDEX (self));
  g_return_if_fail (query != NULL);
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_source_tag (task, fuzzy_index_query_async);

  cursor = g_object_new (FUZZY_TYPE_INDEX_CURSOR,
                         "case-sensitive", self->case_sensitive,
                         "index", self,
                         "query", query,
                         "max-matches", max_matches,
                         "tables", self->tables,
                         NULL);

  g_async_initable_init_async (G_ASYNC_INITABLE (cursor),
                               G_PRIORITY_DEFAULT,
                               cancellable,
                               fuzzy_index_query_cb,
                               g_object_ref (task));
}

/**
 * fuzzy_index_query_finish:
 *
 * Completes an asynchronous request to fuzzy_index_query_async().
 *
 * Returns: (transfer full): A #GListModel of results.
 */
GListModel *
fuzzy_index_query_finish (FuzzyIndex    *self,
                          GAsyncResult  *result,
                          GError       **error)
{
  g_return_val_if_fail (FUZZY_IS_INDEX (self), NULL);
  g_return_val_if_fail (G_IS_TASK (result), NULL);

  return g_task_propagate_pointer (G_TASK (result), error);
}

/**
 * fuzzy_index_get_metadata:
 *
 * Looks up the metadata for @key.
 *
 * Returns: (transfer full) (nullable): A #GVariant or %NULL.
 */
GVariant *
fuzzy_index_get_metadata (FuzzyIndex  *self,
                          const gchar *key)
{
  g_return_val_if_fail (FUZZY_IS_INDEX (self), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  if (self->metadata != NULL)
    return g_variant_dict_lookup_value (self->metadata, key, NULL);

  return NULL;
}

guint64
fuzzy_index_get_metadata_uint64 (FuzzyIndex  *self,
                                 const gchar *key)
{
  g_autoptr(GVariant) ret = NULL;

  ret = fuzzy_index_get_metadata (self, key);

  if (ret != NULL)
    return g_variant_get_uint64 (ret);

  return G_GUINT64_CONSTANT (0);
}

const gchar *
fuzzy_index_get_metadata_string (FuzzyIndex  *self,
                                 const gchar *key)
{
  g_autoptr(GVariant) ret = NULL;

  ret = fuzzy_index_get_metadata (self, key);

  /*
   * This looks unsafe, but is safe because strings are \0 terminated
   * inside the variants and the result is a pointer to internal data.
   * Since that data exists on our mmap() region, we are all good.
   */
  if (ret != NULL)
    return g_variant_get_string (ret, NULL);

  return NULL;
}

/**
 * _fuzzy_index_lookup_document:
 * @self: A #FuzzyIndex
 * @document_id: The identifier of the document
 *
 * This looks up the document found matching @document_id.
 *
 * This should be the document_id resolved through the lookaside
 * using _fuzzy_index_resolve().
 *
 * Returns: (transfer full) (nullable): A #GVariant if successful;
 *   otherwise %NULL.
 */
GVariant *
_fuzzy_index_lookup_document (FuzzyIndex *self,
                              guint       document_id)
{
  g_assert (FUZZY_IS_INDEX (self));

  return g_variant_get_child_value (self->documents, document_id);
}

gboolean
_fuzzy_index_resolve (FuzzyIndex   *self,
                      guint         lookaside_id,
                      guint        *document_id,
                      const gchar **key)
{
  const LookasideEntry *entry;

  g_assert (FUZZY_IS_INDEX (self));
  g_assert (document_id != NULL);

  if G_UNLIKELY (lookaside_id >= self->lookaside_len)
    return FALSE;

  entry = &self->lookaside_raw [lookaside_id];

  *document_id = entry->document_id;

  if (key != NULL)
    {
      if G_UNLIKELY (entry->key_id >= g_variant_n_children (self->keys))
        return FALSE;

      g_variant_get_child (self->keys, entry->key_id, "&s", key);
    }

  return TRUE;
}
