/* rtfm-gir-file.c
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

#define G_LOG_DOMAIN "rtfm-gir-file"

#include <string.h>

#include "rtfm-gir-file.h"
#include "rtfm-gir-parser.h"
#include "rtfm-gir-util.h"

#include "rtfm-gir-class.h"
#include "rtfm-gir-constructor.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-method.h"
#include "rtfm-gir-namespace.h"
#include "rtfm-gir-record.h"

#define INDEX_VERSION 2

struct _RtfmGirFile
{
  GObject            parent_instance;

  GFile             *file;
  RtfmGirRepository *repository;
  FuzzyIndex        *index;

  /*
   * The following is for tracking requests to build the
   * search index for the gir file. If we have an active
   * request, we simply queue the task instead of requesting
   * dupliated work.
   */
  GMutex             mutex;
  GSList            *index_tasks;
};

enum {
  PROP_0,
  PROP_FILE,
  PROP_REPOSITORY,
  N_PROPS
};

typedef void (*RtfmGirIndexer) (RtfmGirFile         *self,
                                FuzzyIndexBuilder   *builder,
                                RtfmGirParserObject *object);

static GParamSpec *properties [N_PROPS];
static GHashTable *indexers;

static void async_initable_iface_init (GAsyncInitableIface *iface);

G_DEFINE_TYPE_EXTENDED (RtfmGirFile, rtfm_gir_file, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_ASYNC_INITABLE,
                                               async_initable_iface_init))

static void
rtfm_gir_file_build_index (RtfmGirFile       *self,
                           FuzzyIndexBuilder *builder,
                           gpointer           instance)
{
  RtfmGirParserObject *object = instance;
  RtfmGirIndexer indexer;
  GPtrArray *children;
  guint i;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (FUZZY_IS_INDEX_BUILDER (builder));
  g_assert (RTFM_GIR_IS_PARSER_OBJECT (object));

  if (NULL != (indexer = g_hash_table_lookup (indexers, GSIZE_TO_POINTER (G_OBJECT_TYPE (object)))))
    indexer (self, builder, object);

  if (NULL != (children = rtfm_gir_parser_object_get_children (object)))
    {
      for (i = 0; i < children->len; i++)
        rtfm_gir_file_build_index (self, builder, g_ptr_array_index (children, i));
    }
}

static void
namespace_indexer (RtfmGirFile         *self,
                   FuzzyIndexBuilder   *builder,
                   RtfmGirNamespace    *namespace)
{
  g_autoptr(GVariant) document = NULL;
  g_autofree gchar *id = NULL;
  const gchar *name;
  GVariantDict dict;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (FUZZY_IS_INDEX_BUILDER (builder));
  g_assert (RTFM_GIR_IS_NAMESPACE (namespace));

  id = rtfm_gir_generate_id (namespace);
  name = rtfm_gir_namespace_get_name (namespace);

  g_variant_dict_init (&dict, NULL);
  g_variant_dict_insert (&dict, "id", "s", id);
  g_variant_dict_insert (&dict, "word", "s", name);
  document = g_variant_ref_sink (g_variant_dict_end (&dict));

#define INSERT_KEY(key)                                          \
  G_STMT_START {                                                 \
    const gchar *tmp = rtfm_gir_namespace_get_##key (namespace); \
    if (tmp != NULL)                                             \
      fuzzy_index_builder_insert (builder, tmp, document);       \
  } G_STMT_END

  INSERT_KEY (name);
  INSERT_KEY (c_identifier_prefixes);
  INSERT_KEY (c_symbol_prefixes);
  INSERT_KEY (shared_library);

#undef INSERT_KEY
}

static void
class_indexer (RtfmGirFile       *self,
               FuzzyIndexBuilder *builder,
               RtfmGirClass      *klass)
{
  g_autoptr(GVariant) document = NULL;
  g_autofree gchar *id = NULL;
  const gchar *name;
  GVariantDict dict;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (FUZZY_IS_INDEX_BUILDER (builder));
  g_assert (RTFM_GIR_IS_CLASS (klass));

  id = rtfm_gir_generate_id (klass);
  name = rtfm_gir_class_get_c_type (klass);

  if (name == NULL)
    return;

  g_variant_dict_init (&dict, NULL);
  g_variant_dict_insert (&dict, "id", "s", id);
  g_variant_dict_insert (&dict, "word", "s", name);
  document = g_variant_ref_sink (g_variant_dict_end (&dict));

#define INSERT_KEY(key)                                    \
  G_STMT_START {                                           \
    const gchar *tmp = rtfm_gir_class_get_##key (klass);   \
    if (tmp != NULL)                                       \
      fuzzy_index_builder_insert (builder, tmp, document); \
  } G_STMT_END

  INSERT_KEY (name);
  INSERT_KEY (c_symbol_prefix);
  INSERT_KEY (c_type);

#undef INSERT_KEY
}

static void
record_indexer (RtfmGirFile       *self,
                FuzzyIndexBuilder *builder,
                RtfmGirRecord     *record)
{
  g_autoptr(GVariant) document = NULL;
  g_autofree gchar *id = NULL;
  const gchar *name;
  GVariantDict dict;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (FUZZY_IS_INDEX_BUILDER (builder));
  g_assert (RTFM_GIR_IS_RECORD (record));

  id = rtfm_gir_generate_id (record);
  name = rtfm_gir_record_get_c_type (record);

  if (name == NULL)
    return;

  g_variant_dict_init (&dict, NULL);
  g_variant_dict_insert (&dict, "id", "s", id);
  g_variant_dict_insert (&dict, "word", "s", name);
  document = g_variant_ref_sink (g_variant_dict_end (&dict));

#define INSERT_KEY(key)                                    \
  G_STMT_START {                                           \
    const gchar *tmp = rtfm_gir_record_get_##key (record);   \
    if (tmp != NULL)                                       \
      fuzzy_index_builder_insert (builder, tmp, document); \
  } G_STMT_END

  INSERT_KEY (c_type);
  INSERT_KEY (name);
  INSERT_KEY (c_symbol_prefix);

#undef INSERT_KEY
}

static void
function_indexer (RtfmGirFile       *self,
                  FuzzyIndexBuilder *builder,
                  RtfmGirFunction   *function)
{
  g_autoptr(GVariant) document = NULL;
  g_autofree gchar *id = NULL;
  const gchar *name;
  GVariantDict dict;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (FUZZY_IS_INDEX_BUILDER (builder));
  g_assert (RTFM_GIR_IS_FUNCTION (function));

  id = rtfm_gir_generate_id (function);
  name = rtfm_gir_function_get_c_identifier (function);

  if (name == NULL)
    return;

  g_variant_dict_init (&dict, NULL);
  g_variant_dict_insert (&dict, "id", "s", id);
  g_variant_dict_insert (&dict, "word", "s", name);
  document = g_variant_ref_sink (g_variant_dict_end (&dict));

#define INSERT_KEY(key)                                        \
  G_STMT_START {                                               \
    const gchar *tmp = rtfm_gir_function_get_##key (function); \
    if (tmp != NULL)                                           \
      fuzzy_index_builder_insert (builder, tmp, document);     \
  } G_STMT_END

  INSERT_KEY (c_identifier);
  INSERT_KEY (name);

#undef INSERT_KEY
}

static void
method_indexer (RtfmGirFile       *self,
                FuzzyIndexBuilder *builder,
                RtfmGirMethod     *method)
{
  g_autoptr(GVariant) document = NULL;
  g_autofree gchar *id = NULL;
  const gchar *name;
  GVariantDict dict;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (FUZZY_IS_INDEX_BUILDER (builder));
  g_assert (RTFM_GIR_IS_METHOD (method));

  id = rtfm_gir_generate_id (method);
  name = rtfm_gir_method_get_c_identifier (method);

  if (name == NULL)
    return;

  g_variant_dict_init (&dict, NULL);
  g_variant_dict_insert (&dict, "id", "s", id);
  g_variant_dict_insert (&dict, "word", "s", name);
  document = g_variant_ref_sink (g_variant_dict_end (&dict));

#define INSERT_KEY(key)                                    \
  G_STMT_START {                                           \
    const gchar *tmp = rtfm_gir_method_get_##key (method); \
    if (tmp != NULL)                                       \
      fuzzy_index_builder_insert (builder, tmp, document); \
  } G_STMT_END

  INSERT_KEY (c_identifier);
  INSERT_KEY (name);

#undef INSERT_KEY
}

static void
constructor_indexer (RtfmGirFile        *self,
                     FuzzyIndexBuilder  *builder,
                     RtfmGirConstructor *constructor)
{
  g_autoptr(GVariant) document = NULL;
  g_autofree gchar *id = NULL;
  const gchar *name;
  GVariantDict dict;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (FUZZY_IS_INDEX_BUILDER (builder));
  g_assert (RTFM_GIR_IS_CONSTRUCTOR (constructor));

  id = rtfm_gir_generate_id (constructor);
  name = rtfm_gir_constructor_get_c_identifier (constructor);

  if (name == NULL)
    return;

  g_variant_dict_init (&dict, NULL);
  g_variant_dict_insert (&dict, "id", "s", id);
  g_variant_dict_insert (&dict, "word", "s", name);
  document = g_variant_ref_sink (g_variant_dict_end (&dict));

#define INSERT_KEY(key)                                              \
  G_STMT_START {                                                     \
    const gchar *tmp = rtfm_gir_constructor_get_##key (constructor); \
    if (tmp != NULL)                                                 \
      fuzzy_index_builder_insert (builder, tmp, document);           \
  } G_STMT_END

  INSERT_KEY (c_identifier);
  INSERT_KEY (name);

#undef INSERT_KEY
}

static void
rtfm_gir_file_finalize (GObject *object)
{
  RtfmGirFile *self = (RtfmGirFile *)object;

  g_clear_object (&self->file);
  g_clear_object (&self->repository);
  g_clear_object (&self->index);

  g_mutex_clear (&self->mutex);

  G_OBJECT_CLASS (rtfm_gir_file_parent_class)->finalize (object);
}

static void
rtfm_gir_file_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  RtfmGirFile *self = RTFM_GIR_FILE (object);

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, self->file);
      break;

    case PROP_REPOSITORY:
      g_value_set_object (value, self->repository);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
}

static void
rtfm_gir_file_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  RtfmGirFile *self = RTFM_GIR_FILE (object);

  switch (prop_id)
    {
    case PROP_FILE:
      self->file = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
    }
}

static void
rtfm_gir_file_class_init (RtfmGirFileClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_file_finalize;
  object_class->get_property = rtfm_gir_file_get_property;
  object_class->set_property = rtfm_gir_file_set_property;

  properties [PROP_FILE] =
    g_param_spec_object ("file",
                         "File",
                         "File",
                         G_TYPE_FILE,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  properties [PROP_REPOSITORY] =
    g_param_spec_object ("repository",
                         "Repository",
                         "Repository",
                         RTFM_GIR_TYPE_REPOSITORY,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  indexers = g_hash_table_new (NULL, NULL);

#define REGISTER_INDEXER(TYPE, type) \
  g_hash_table_insert (indexers, GSIZE_TO_POINTER (RTFM_GIR_TYPE_##TYPE), type##_indexer)

  REGISTER_INDEXER (CLASS, class);
  REGISTER_INDEXER (CONSTRUCTOR, constructor);
  REGISTER_INDEXER (FUNCTION, function);
  REGISTER_INDEXER (METHOD, method);
  REGISTER_INDEXER (NAMESPACE, namespace);
  REGISTER_INDEXER (RECORD, record);

#undef REGISTER_INDEXER
}

static void
rtfm_gir_file_init (RtfmGirFile *self)
{
  g_mutex_init (&self->mutex);
}

static void
rtfm_gir_file_init_worker (GTask        *task,
                           gpointer      source_object,
                           gpointer      task_data,
                           GCancellable *cancellable)
{
  g_autoptr(RtfmGirParser) parser = NULL;
  g_autoptr(RtfmGirRepository) repository = NULL;
  GFile *file = task_data;
  GError *error = NULL;

  g_assert (G_IS_TASK (task));
  g_assert (G_IS_FILE (file));

  parser = rtfm_gir_parser_new ();

  repository = rtfm_gir_parser_parse_file (parser, file, cancellable, &error);

  if (repository == NULL)
    {
      g_task_return_error (task, error);
      return;
    }

  g_task_return_pointer (task, g_steal_pointer (&repository), g_object_unref);
}

static void
rtfm_gir_file_init_async (GAsyncInitable      *initable,
                          gint                 io_priority,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
  RtfmGirFile *self = (RtfmGirFile *)initable;
  g_autoptr(GTask) task = NULL;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_task_data (task, g_object_ref (self->file), g_object_unref);
  g_task_set_priority (task, io_priority);

  if (self->repository != NULL)
    {
      g_task_return_pointer (task, g_object_ref (self->repository), g_object_unref);
      return;
    }

  g_task_run_in_thread (task, rtfm_gir_file_init_worker);
}

static gboolean
rtfm_gir_file_init_finish (GAsyncInitable  *initable,
                           GAsyncResult    *result,
                           GError         **error)
{
  RtfmGirFile *self = (RtfmGirFile *)initable;
  g_autoptr(RtfmGirRepository) repository = NULL;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (G_IS_TASK (result));

  repository = g_task_propagate_pointer (G_TASK (result), error);

  if (self->repository == NULL)
    self->repository = g_steal_pointer (&repository);

  return (self->repository != NULL);
}

static void
async_initable_iface_init (GAsyncInitableIface *iface)
{
  iface->init_async = rtfm_gir_file_init_async;
  iface->init_finish = rtfm_gir_file_init_finish;
}

/**
 * rtfm_gir_file_get_file:
 *
 * Returns: (transfer none): A #GFile.
 */
GFile *
rtfm_gir_file_get_file (RtfmGirFile *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FILE (self), NULL);

  return self->file;
}

/**
 * rtfm_gir_file_get_repository:
 *
 * Returns: (transfer none): A #GFile.
 */
RtfmGirRepository *
rtfm_gir_file_get_repository (RtfmGirFile *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FILE (self), NULL);

  return self->repository;
}

RtfmGirFile *
rtfm_gir_file_new (GFile *file)
{
  g_return_val_if_fail (G_IS_FILE (file), NULL);

  return g_object_new (RTFM_GIR_TYPE_FILE,
                       "file", file,
                       NULL);
}

static gchar *
get_search_index_filename (GFile *file)
{
  g_autofree gchar *uri = NULL;
  g_autofree gchar *name = NULL;
  g_autoptr(GChecksum) checksum = NULL;
  const gchar *digest;

  g_assert (G_IS_FILE (file));

  uri = g_file_get_uri (file);
  checksum = g_checksum_new (G_CHECKSUM_SHA1);
  g_checksum_update (checksum, (const guint8 *)uri, strlen (uri));
  digest = g_checksum_get_string (checksum);
  name = g_strdup_printf ("%s.gvariant", digest);

  return g_build_filename (g_get_user_cache_dir (),
                           "rtfm",
                           "gobject-introspection",
                           name,
                           NULL);
}

static gboolean
check_index_version (FuzzyIndex  *index,
                     GFile       *file,
                     guint64      mtime,
                     GError     **error)
{
  guint64 metadata_mtime;

  g_assert (FUZZY_IS_INDEX (index));
  g_assert (G_IS_FILE (file));
  g_assert (error != NULL);

  if (INDEX_VERSION != fuzzy_index_get_metadata_uint32 (index, "version"))
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_WRONG_ETAG,
                   "mtime from index is too old, requires index rebuild");
      return FALSE;
    }

  if (mtime != fuzzy_index_get_metadata_uint64 (index, "mtime"))
    {
      g_set_error (error,
                   G_IO_ERROR,
                   G_IO_ERROR_WRONG_ETAG,
                   "mtime from index is too old, requires index rebuild");
      return FALSE;
    }

  return TRUE;
}

static void
rtfm_gir_file_load_index_worker (GTask        *task,
                                 gpointer      source_object,
                                 gpointer      task_data,
                                 GCancellable *cancellable)
{
  g_autoptr(RtfmGirParser) parser = NULL;
  g_autoptr(RtfmGirRepository) repository = NULL;
  g_autoptr(FuzzyIndexBuilder) builder = NULL;
  g_autoptr(FuzzyIndex) new_index = NULL;
  g_autoptr(GFile) index_file = NULL;
  g_autoptr(GFileInfo) file_info = NULL;
  g_autofree gchar *index_path = NULL;
  g_autofree gchar *nsname = NULL;
  RtfmGirFile *self = source_object;
  FuzzyIndex *result = NULL;
  gchar *tmp;
  GSList *list;
  GSList *iter;
  GFile *file = task_data;
  GError *error = NULL;
  guint64 mtime = 0;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (G_IS_TASK (task));
  g_assert (G_IS_FILE (file));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  /*
   * Query information on our .gir file so we have an mtime to
   * validate against the search index.
   */
  file_info = g_file_query_info (file,
                                 G_FILE_ATTRIBUTE_TIME_MODIFIED,
                                 G_FILE_QUERY_INFO_NONE,
                                 cancellable,
                                 &error);
  if (file_info == NULL)
    goto finish;

  mtime = g_file_info_get_attribute_uint64 (file_info, G_FILE_ATTRIBUTE_TIME_MODIFIED);

  /*
   * Open the previous search index if it exists, and see if it is up to
   * date or requires and update.
   */
  index_path = get_search_index_filename (file);
  index_file = g_file_new_for_path (index_path);

  if (g_file_query_exists (index_file, cancellable))
    {
      g_autoptr(FuzzyIndex) prev_index = fuzzy_index_new ();

      if (fuzzy_index_load_file (prev_index, index_file, cancellable, &error) &&
          check_index_version (prev_index, file, mtime, &error))
        {
          result = prev_index;
          goto finish;
        }

      g_message ("%s", error->message);

      g_clear_error (&error);
    }

  /*
   * Parse our own copy of the repository so that we don't need
   * to synchronize with the repository being parsed. We can also
   * through away the parsed tree afterwards to reclaim some memory,
   * which means that building indexes need-not consume extra memory
   * after building, until the user opens the namespace.
   */

  parser = rtfm_gir_parser_new ();

  repository = rtfm_gir_parser_parse_file (parser, file, cancellable, &error);
  if (repository == NULL)
    goto finish;

  /*
   * Translate namespace "Foo-1.0.gir" to "Foo 1.0".
   */
  nsname = g_file_get_basename (file);
  if (NULL != (tmp = strrchr (nsname, '.')))
    *tmp = '\0';
  if (NULL != (tmp = strchr (nsname, '-')))
    *tmp = ' ';

  /*
   * Now build our index from strings in the repository.
   */
  builder = fuzzy_index_builder_new ();
  fuzzy_index_builder_set_metadata_string (builder, "self", index_path);
  fuzzy_index_builder_set_metadata_uint64 (builder, "mtime", mtime);
  fuzzy_index_builder_set_metadata_string (builder, "namespace", nsname);
  fuzzy_index_builder_set_metadata_uint32 (builder, "version", INDEX_VERSION);
  rtfm_gir_file_build_index (self, builder, repository);

  /*
   * Write the search index to disk.
   */
  if (!fuzzy_index_builder_write (builder,
                                  index_file,
                                  g_task_get_priority (task),
                                  cancellable,
                                  &error))
    goto finish;

  /*
   * Now mmap()'d our search index.
   */
  new_index = fuzzy_index_new ();
  if (!fuzzy_index_load_file (new_index, index_file, cancellable, &error))
    goto finish;

  result = new_index;

finish:
  g_mutex_lock (&self->mutex);

  list = self->index_tasks;
  self->index_tasks = NULL;

  for (iter = list; iter != NULL; iter = iter->next)
    {
      GTask *iter_task = iter->data;

      if (result != NULL)
        g_task_return_pointer (iter_task, g_object_ref (result), g_object_unref);
      else
        g_task_return_error (iter_task, g_error_copy (error));
    }

  g_mutex_unlock (&self->mutex);

  g_slist_free_full (list, g_object_unref);
  g_clear_error (&error);
}

void
rtfm_gir_file_load_index_async (RtfmGirFile         *self,
                                GCancellable        *cancellable,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;
  g_autoptr(GMutexLocker) locker = NULL;

  g_return_if_fail (RTFM_GIR_IS_FILE (self));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  locker = g_mutex_locker_new (&self->mutex);

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, FALSE);
  g_task_set_task_data (task, g_object_ref (self->file), g_object_unref);

  if (self->index != NULL)
    {
      g_task_return_pointer (task, g_object_ref (self->index), g_object_unref);
      return;
    }

  /*
   * If there is another build request active, queue the result to
   * be completed by that worker instead of queing duplicated work.
   */
  self->index_tasks = g_slist_prepend (self->index_tasks, g_object_ref (task));
  if (self->index_tasks->next == NULL)
    g_task_run_in_thread (task, rtfm_gir_file_load_index_worker);
}

FuzzyIndex *
rtfm_gir_file_load_index_finish (RtfmGirFile   *self,
                                 GAsyncResult  *result,
                                 GError       **error)
{
  g_return_val_if_fail (RTFM_GIR_IS_FILE (self), NULL);
  g_return_val_if_fail (G_IS_TASK (result), NULL);

  return g_task_propagate_pointer (G_TASK (result), error);
}
