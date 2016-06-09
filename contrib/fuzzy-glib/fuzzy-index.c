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

struct _FuzzyIndex
{
  GObject       object;

  guint         loaded : 1;

  GMappedFile  *mapped_file;
  GVariant     *variant;
  GVariantDict *documents;
  GVariantDict *keys;
  GVariantDict *tables;
};

G_DEFINE_TYPE (FuzzyIndex, fuzzy_index, G_TYPE_OBJECT)

static void
fuzzy_index_finalize (GObject *object)
{
  FuzzyIndex *self = (FuzzyIndex *)object;

  g_clear_pointer (&self->mapped_file, g_mapped_file_unref);
  g_clear_pointer (&self->variant, g_variant_unref);
  g_clear_pointer (&self->documents, g_variant_dict_unref);
  g_clear_pointer (&self->keys, g_variant_dict_unref);
  g_clear_pointer (&self->tables, g_variant_dict_unref);

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

void
fuzzy_index_load_file_async (FuzzyIndex          *self,
                             GFile               *file,
                             GCancellable        *cancellable,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data)
{
  g_autofree gchar *path = NULL;
  g_autoptr(GTask) task = NULL;
  g_autoptr(GMappedFile) mapped_file = NULL;
  g_autoptr(GVariant) variant = NULL;
  g_autoptr(GVariant) documents = NULL;
  g_autoptr(GVariant) keys = NULL;
  g_autoptr(GVariant) tables = NULL;
  GVariantDict dict;
  GError *error = NULL;
  gint version = 0;

  g_return_if_fail (FUZZY_IS_INDEX (self));
  g_return_if_fail (G_IS_FILE (file));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_task_data (task, g_object_ref (file), g_object_unref);

  if (self->loaded)
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_INVAL,
                               "Cannot load index multiple times");
      return;
    }

  self->loaded = TRUE;

  /*
   * TODO: Move this to worker thread.
   */

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

  documents = g_variant_dict_lookup_value (&dict, "documents", G_VARIANT_TYPE_VARDICT);
  keys = g_variant_dict_lookup_value (&dict, "keys", G_VARIANT_TYPE_VARDICT);
  tables = g_variant_dict_lookup_value (&dict, "tables", G_VARIANT_TYPE_VARDICT);
  g_variant_dict_clear (&dict);

  if (keys == NULL || documents == NULL || tables == NULL)
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_INVAL,
                               "Invalid gvariant index");
      return;
    }

  self->mapped_file = g_steal_pointer (&mapped_file);
  self->variant = g_steal_pointer (&variant);
  self->documents = g_variant_dict_new (documents);
  self->keys = g_variant_dict_new (keys);
  self->tables = g_variant_dict_new (tables);

  g_task_return_boolean (task, TRUE);
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

  cursor = g_object_new (FUZZY_TYPE_INDEX_CURSOR,
                         "documents", self->documents,
                         "keys", self->keys,
                         "tables", self->tables,
                         "query", query,
                         "max-matches", max_matches,
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