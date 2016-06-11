/* fuzzy-index-builder.c
 *
 * Copyright (C) 2016 Christian Hergert <christian@hergert.me>
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

#define G_LOG_DOMAIN "fuzzy-index-builder"

#include <stdlib.h>

#include "fuzzy-index-builder.h"

struct _FuzzyIndexBuilder
{
  GObject       object;

  guint         case_sensitive : 1;

  GHashTable   *documents_hash;
  GPtrArray    *documents;
  GStringChunk *keys;
  GArray       *kv_pairs;
  GHashTable   *metadata;
};

typedef struct
{
  const gchar *key;
  guint        document_id;
} KVPair;

typedef struct
{
  /* The character position within the string in terms of unicode
   * characters, not byte-position.
   */
  guint position;

  /* The document id (which is the hash of the document. */
  guint document_id;
} PosDocPair;

G_DEFINE_TYPE (FuzzyIndexBuilder, fuzzy_index_builder, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_CASE_SENSITIVE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static guint
variant_hash_helper (gconstpointer data)
{
  GVariant *variant = (GVariant *)data;
  GBytes *bytes;
  guint ret;

  if (!g_variant_is_container (variant))
    return g_variant_hash (variant);

  /* Generally we wouldn't want to create a bytes to hash
   * during a hash call, since that'd be fairly expensive.
   * But since GHashTable caches hash values, its not that
   * big of a deal.
   */
  bytes = g_variant_get_data_as_bytes (variant);
  ret = g_bytes_hash (bytes);
  g_bytes_unref (bytes);

  return ret;
}

static void
fuzzy_index_builder_finalize (GObject *object)
{
  FuzzyIndexBuilder *self = (FuzzyIndexBuilder *)object;

  g_clear_pointer (&self->documents_hash, g_hash_table_unref);
  g_clear_pointer (&self->documents, g_ptr_array_unref);
  g_clear_pointer (&self->keys, g_string_chunk_free);
  g_clear_pointer (&self->kv_pairs, g_array_unref);
  g_clear_pointer (&self->metadata, g_hash_table_unref);

  G_OBJECT_CLASS (fuzzy_index_builder_parent_class)->finalize (object);
}

static void
fuzzy_index_builder_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  FuzzyIndexBuilder *self = FUZZY_INDEX_BUILDER (object);

  switch (prop_id)
    {
    case PROP_CASE_SENSITIVE:
      g_value_set_boolean (value, fuzzy_index_builder_get_case_sensitive (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
fuzzy_index_builder_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  FuzzyIndexBuilder *self = FUZZY_INDEX_BUILDER (object);

  switch (prop_id)
    {
    case PROP_CASE_SENSITIVE:
      fuzzy_index_builder_set_case_sensitive (self, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
fuzzy_index_builder_class_init (FuzzyIndexBuilderClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = fuzzy_index_builder_finalize;
  object_class->get_property = fuzzy_index_builder_get_property;
  object_class->set_property = fuzzy_index_builder_set_property;

  properties [PROP_CASE_SENSITIVE] =
    g_param_spec_boolean ("case-sensitive",
                          "Case Sensitive",
                          "Case Sensitive",
                          FALSE,
                          (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
fuzzy_index_builder_init (FuzzyIndexBuilder *self)
{
  self->documents = g_ptr_array_new_with_free_func ((GDestroyNotify)g_variant_unref);
  self->documents_hash = g_hash_table_new (variant_hash_helper, g_variant_equal);
  self->kv_pairs = g_array_new (FALSE, FALSE, sizeof (KVPair));
  self->keys = g_string_chunk_new (4096);
}

FuzzyIndexBuilder *
fuzzy_index_builder_new (void)
{
  return g_object_new (FUZZY_TYPE_INDEX_BUILDER, NULL);
}

/**
 * fuzzy_index_builder_insert:
 * @self: A #FuzzyIndexBuilder
 * @key: The UTF-8 encoded key for the document
 * @document: The document to store
 *
 * Inserts @document into the index using @key as the lookup key.
 *
 * If a matching document (checked by hashing @document) has already
 * been inserted, only a single instance of the document will be stored.
 *
 * If @document is floating, it's floating reference will be sunk using
 * g_variant_ref_sink().
 *
 * Returns: The document id registered for @document.
 */
guint64
fuzzy_index_builder_insert (FuzzyIndexBuilder *self,
                            const gchar       *key,
                            GVariant          *document)
{
  g_autofree gchar *freeme = NULL;
  GVariant *real_document = NULL;
  gpointer document_id = NULL;
  KVPair pair;

  g_return_val_if_fail (FUZZY_IS_INDEX_BUILDER (self), 0);
  g_return_val_if_fail (key != NULL, 0);
  g_return_val_if_fail (document != NULL, 0);

  if (self->case_sensitive == FALSE)
    key = freeme = g_utf8_strdown (key, -1);

  if (!g_hash_table_lookup_extended (self->documents_hash,
                                     document,
                                     (gpointer *)&real_document,
                                     &document_id))
    {
      document_id = GUINT_TO_POINTER (self->documents->len);
      real_document = g_variant_ref_sink (document);
      g_ptr_array_add (self->documents, real_document);
      g_hash_table_insert (self->documents_hash, real_document, document_id);
    }

  pair.key = g_string_chunk_insert_const (self->keys, key);
  pair.document_id = GPOINTER_TO_SIZE (document_id);

  g_array_append_val (self->kv_pairs, pair);

  return pair.document_id;
}

static gint
pos_doc_pair_compare (gconstpointer a,
                      gconstpointer b)
{
  const PosDocPair *paira = a;
  const PosDocPair *pairb = b;
  gint ret;

  ret = paira->document_id - pairb->document_id;

  if (ret == 0)
    ret = paira->position - pairb->position;

  return ret;
}

static GVariant *
fuzzy_index_builder_build_keys (FuzzyIndexBuilder *self)
{
  GVariantDict dict;
  guint i;

  g_assert (FUZZY_IS_INDEX_BUILDER (self));

  g_variant_dict_init (&dict, NULL);

  for (i = 0; i < self->kv_pairs->len; i++)
    {
      const KVPair *pair = &g_array_index (self->kv_pairs, KVPair, i);
      gchar key[16];

      g_snprintf (key, sizeof key, "%u", pair->document_id);
      g_variant_dict_insert_value (&dict, key, g_variant_new_string (pair->key));
    }

  return g_variant_dict_end (&dict);
}

static GVariant *
fuzzy_index_builder_build_index (FuzzyIndexBuilder *self)
{
  g_autoptr(GPtrArray) ar = NULL;
  g_autoptr(GHashTable) rows = NULL;
  GVariantDict dict;
  GHashTableIter iter;
  gpointer keyptr;
  GArray *row;
  guint i;

  g_assert (FUZZY_IS_INDEX_BUILDER (self));

  ar = g_ptr_array_new_with_free_func ((GDestroyNotify)g_array_unref);
  rows = g_hash_table_new (NULL, NULL);

  for (i = 0; i < self->kv_pairs->len; i++)
    {
      KVPair *kvpair = &g_array_index (self->kv_pairs, KVPair, i);
      PosDocPair cdpair = { 0, kvpair->document_id };
      const gchar *tmp;
      guint pos = 0;

      for (tmp = kvpair->key; *tmp != '\0'; tmp = g_utf8_next_char (tmp))
        {
          gunichar ch = g_utf8_get_char (tmp);

          row = g_hash_table_lookup (rows, GUINT_TO_POINTER (ch));

          if G_UNLIKELY (row == NULL)
            {
              row = g_array_new (FALSE, FALSE, sizeof (PosDocPair));
              g_hash_table_insert (rows, GUINT_TO_POINTER (ch), row);
            }

          cdpair.position = pos++;
          g_array_append_val (row, cdpair);
        }
    }

  g_variant_dict_init (&dict, NULL);

  g_hash_table_iter_init (&iter, rows);

  while (g_hash_table_iter_next (&iter, &keyptr, (gpointer *)&row))
    {
      gchar key[16];
      GVariant *variant;
      gunichar ch = GPOINTER_TO_UINT (keyptr);

      g_snprintf (key, sizeof key, "%c", ch);
      g_array_sort (row, pos_doc_pair_compare);

      variant = g_variant_new_fixed_array ((const GVariantType *)"(ii)",
                                           row->data,
                                           row->len,
                                           sizeof (PosDocPair));
      g_variant_dict_insert_value (&dict, key, variant);
    }

  return g_variant_dict_end (&dict);
}

static GVariant *
fuzzy_index_builder_build_metadata (FuzzyIndexBuilder *self)
{
  GVariantDict dict;
  GHashTableIter iter;

  g_assert (FUZZY_IS_INDEX_BUILDER (self));

  g_variant_dict_init (&dict, NULL);

  if (self->metadata != NULL)
    {
      const gchar *key;
      GVariant *value;

      g_hash_table_iter_init (&iter, self->metadata);
      while (g_hash_table_iter_next (&iter, (gpointer *)&key, (gpointer *)&value))
        g_variant_dict_insert_value (&dict, key, value);
    }

  g_variant_dict_insert (&dict, "case-sensitive", "b", self->case_sensitive);

  return g_variant_dict_end (&dict);
}

static void
fuzzy_index_builder_write_worker (GTask        *task,
                                  gpointer      source_object,
                                  gpointer      task_data,
                                  GCancellable *cancellable)
{
  FuzzyIndexBuilder *self = source_object;
  g_autoptr(GVariant) variant = NULL;
  g_autoptr(GPtrArray) array_of_keys = NULL;
  GVariantDict dict;
  GVariantDict docs_dict;
  GFile *file = task_data;
  GError *error = NULL;
  guint i;

  g_assert (G_IS_TASK (task));
  g_assert (FUZZY_IS_INDEX_BUILDER (self));
  g_assert (G_IS_FILE (file));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  g_variant_dict_init (&dict, NULL);

  /* Set our version number for the document */
  g_variant_dict_insert (&dict, "version", "i", 1);

  /* Build our dicitionary of metadata */
  g_variant_dict_insert_value (&dict, "metadata", fuzzy_index_builder_build_metadata (self));

  /* Add our doc_id → string reverse map */
  g_variant_dict_insert_value (&dict, "keys", fuzzy_index_builder_build_keys (self));

  /* Build our dicitionary of character → [(pos,doc_id),..] tuples */
  g_variant_dict_insert_value (&dict, "tables", fuzzy_index_builder_build_index (self));

  /* Build our dictionary of doc_id → documents */
  g_variant_dict_init (&docs_dict, NULL);
  for (i = 0; i < self->documents->len; i++)
    {
      GVariant *document = g_ptr_array_index (self->documents, i);
      gchar key[32];

      g_snprintf (key, sizeof key, "%u", i);
      g_variant_dict_insert_value (&docs_dict, key, document);
    }
  g_variant_dict_insert_value (&dict, "documents", g_variant_dict_end (&docs_dict));

  /* Now write the variant to disk */
  variant = g_variant_ref_sink (g_variant_dict_end (&dict));
  if (!g_file_replace_contents (file,
                                g_variant_get_data (variant),
                                g_variant_get_size (variant),
                                NULL,
                                FALSE,
                                G_FILE_CREATE_NONE,
                                NULL,
                                cancellable,
                                &error))
    g_task_return_error (task, error);
  else
    g_task_return_boolean (task, TRUE);
}

/**
 * fuzzy_index_builder_write_async:
 * @self: A #FuzzyIndexBuilder
 * @file: A #GFile to write the index to
 * @io_priority: The priority for IO operations
 * @cancellable: (nullable): An optional #GCancellable or %NULL
 * @callback: A callback for completion or %NULL
 * @user_data: User data for @callback
 *
 * Builds and writes the index to @file. The file format is a
 * GVariant on disk and can be loaded and searched using
 * #FuzzyIndex.
 */
void
fuzzy_index_builder_write_async (FuzzyIndexBuilder   *self,
                                 GFile               *file,
                                 gint                 io_priority,
                                 GCancellable        *cancellable,
                                 GAsyncReadyCallback  callback,
                                 gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  g_return_if_fail (FUZZY_IS_INDEX_BUILDER (self));
  g_return_if_fail (G_IS_FILE (file));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_source_tag (task, fuzzy_index_builder_write_async);
  g_task_set_priority (task, io_priority);
  g_task_set_task_data (task, g_object_ref (file), g_object_unref);
  g_task_run_in_thread (task, fuzzy_index_builder_write_worker);
}

gboolean
fuzzy_index_builder_write_finish (FuzzyIndexBuilder  *self,
                                  GAsyncResult       *result,
                                  GError            **error)
{
  g_return_val_if_fail (FUZZY_IS_INDEX_BUILDER (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (result), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

gboolean
fuzzy_index_builder_write (FuzzyIndexBuilder  *self,
                           GFile              *file,
                           gint                io_priority,
                           GCancellable       *cancellable,
                           GError            **error)
{
  g_autoptr(GTask) task = NULL;

  g_return_val_if_fail (FUZZY_IS_INDEX_BUILDER (self), FALSE);
  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), FALSE);

  task = g_task_new (self, cancellable, NULL, NULL);
  g_task_set_source_tag (task, fuzzy_index_builder_write);
  g_task_set_priority (task, io_priority);
  g_task_set_task_data (task, g_object_ref (file), g_object_unref);

  fuzzy_index_builder_write_worker (task, self, file, cancellable);

  return g_task_propagate_boolean (task, error);
}

/**
 * fuzzy_index_builder_get_document:
 *
 * Returns the document that was inserted in a previous call to
 * fuzzy_index_builder_insert().
 *
 * Returns: (transfer none): A #GVariant
 */
const GVariant *
fuzzy_index_builder_get_document (FuzzyIndexBuilder *self,
                                  guint64            document_id)
{
  g_return_val_if_fail (FUZZY_IS_INDEX_BUILDER (self), NULL);
  g_return_val_if_fail ((guint)document_id < self->documents->len, NULL);

  return g_ptr_array_index (self->documents, (guint)document_id);
}

void
fuzzy_index_builder_set_metadata (FuzzyIndexBuilder *self,
                                  const gchar       *key,
                                  GVariant          *value)
{
  g_return_if_fail (FUZZY_IS_INDEX_BUILDER (self));
  g_return_if_fail (key != NULL);

  if (self->metadata == NULL)
    self->metadata = g_hash_table_new_full (g_str_hash,
                                            g_str_equal,
                                            g_free,
                                            (GDestroyNotify)g_variant_unref);

  if (value != NULL)
    g_hash_table_insert (self->metadata,
                         g_strdup (key),
                         g_variant_ref_sink (value));
  else
    g_hash_table_remove (self->metadata, key);
}

void
fuzzy_index_builder_set_metadata_string (FuzzyIndexBuilder *self,
                                         const gchar       *key,
                                         const gchar       *value)
{
  g_return_if_fail (FUZZY_IS_INDEX_BUILDER (self));
  g_return_if_fail (key != NULL);
  g_return_if_fail (value != NULL);

  fuzzy_index_builder_set_metadata (self, key, g_variant_new_string (value));
}

void
fuzzy_index_builder_set_metadata_uint64 (FuzzyIndexBuilder *self,
                                         const gchar       *key,
                                         guint64            value)
{
  g_return_if_fail (FUZZY_IS_INDEX_BUILDER (self));
  g_return_if_fail (key != NULL);

  fuzzy_index_builder_set_metadata (self, key, g_variant_new_uint64 (value));
}

gboolean
fuzzy_index_builder_get_case_sensitive (FuzzyIndexBuilder *self)
{
  g_return_val_if_fail (FUZZY_IS_INDEX_BUILDER (self), FALSE);

  return self->case_sensitive;
}

void
fuzzy_index_builder_set_case_sensitive (FuzzyIndexBuilder *self,
                                        gboolean           case_sensitive)
{
  g_return_if_fail (FUZZY_IS_INDEX_BUILDER (self));

  case_sensitive = !!case_sensitive;

  if (self->case_sensitive != case_sensitive)
    {
      self->case_sensitive = case_sensitive;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_CASE_SENSITIVE]);
    }
}
