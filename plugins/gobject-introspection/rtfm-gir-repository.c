/* rtfm-gir-repository.c
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

#define G_LOG_DOMAIN "rtfm-gir-repository"

#include "rtfm-gir-repository.h"
#include "rtfm-gir-include.h"
#include "rtfm-gir-package.h"
#include "rtfm-gir-c-include.h"
#include "rtfm-gir-namespace.h"

G_DEFINE_AUTOPTR_CLEANUP_FUNC (xmlTextReader, xmlFreeTextReader)

#define IS_ELEMENT_NAMED(r,name) \
  ((xmlTextReaderNodeType(r) == XML_ELEMENT_NODE) && \
   (0 == g_strcmp0 (name, (gchar *)xmlTextReaderConstName(r))))

struct _RtfmGirRepository
{
  RtfmItem          parent_instance;

  GMutex            mutex;
  GSList           *loading_tasks;

  GFile            *file;
  gchar            *version;
  RtfmGirInclude   *include;
  RtfmGirPackage   *package;
  RtfmGirCInclude  *c_include;
  RtfmGirNamespace *namespace;

  guint             loaded : 1;
  guint             loading : 1;
};

enum {
  PROP_0,
  PROP_FILE,
  PROP_VERSION,
  N_PROPS
};

static void async_initable_iface_init (GAsyncInitableIface *iface);

G_DEFINE_TYPE_EXTENDED (RtfmGirRepository, rtfm_gir_repository, RTFM_TYPE_ITEM, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_ASYNC_INITABLE,
                                               async_initable_iface_init))

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_repository_finalize (GObject *object)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  g_clear_pointer (&self->version, g_free);

  g_clear_object (&self->file);
  g_clear_object (&self->include);
  g_clear_object (&self->package);
  g_clear_object (&self->c_include);
  g_clear_object (&self->namespace);

  g_slist_free_full (self->loading_tasks, g_object_unref);
  self->loading_tasks = NULL;

  g_mutex_clear (&self->mutex);

  G_OBJECT_CLASS (rtfm_gir_repository_parent_class)->finalize (object);
}

static void
rtfm_gir_repository_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, self->file);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_repository_set_property (GObject       *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  switch (prop_id)
    {
    case PROP_FILE:
      self->file = g_value_dup_object (value);
      break;

    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_repository_class_init (RtfmGirRepositoryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_repository_finalize;
  object_class->get_property = rtfm_gir_repository_get_property;
  object_class->set_property = rtfm_gir_repository_set_property;

  properties [PROP_FILE] =
    g_param_spec_object ("file",
                         "File",
                         "The .gir file to be read",
                         G_TYPE_FILE,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_repository_init (RtfmGirRepository *self)
{
  g_mutex_init (&self->mutex);
}

RtfmGirRepository *
rtfm_gir_repository_new (GFile *file)
{
  return g_object_new (RTFM_TYPE_GIR_REPOSITORY,
                       "file", file,
                       NULL);
}

gboolean
rtfm_gir_repository_ingest (RtfmGirRepository   *self,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  xmlChar *version;

  g_assert (RTFM_IS_GIR_REPOSITORY (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  version = xmlTextReaderGetAttribute (reader, (const xmlChar *)"version");

  /* Copy properties to object */
  self->version = g_strdup ((gchar *)version);

  /* Free libxml allocated strings */
  xmlFree (version);

  if (xmlTextReaderRead (reader) != 1)
    return FALSE;

  while (xmlTextReaderNodeType (reader) != XML_ELEMENT_NODE)
    {
      if (xmlTextReaderNext (reader) != 1)
        return FALSE;
    }

  do
    {
      const gchar *element_name;

      if (xmlTextReaderNodeType (reader) != XML_ELEMENT_NODE)
        continue;

      element_name = (const gchar *)xmlTextReaderConstName (reader);

      if (FALSE) { }
      else if (g_strcmp0 (element_name, "include") == 0)
        {
          g_autoptr(RtfmGirInclude) include = NULL;

          include = g_object_new (RTFM_TYPE_GIR_INCLUDE, NULL);

          if (!rtfm_gir_include_ingest (include, reader, error))
            return FALSE;

          g_set_object (&self->include, include);
        }
      else if (g_strcmp0 (element_name, "package") == 0)
        {
          g_autoptr(RtfmGirPackage) package = NULL;

          package = g_object_new (RTFM_TYPE_GIR_PACKAGE, NULL);

          if (!rtfm_gir_package_ingest (package, reader, error))
            return FALSE;

          g_set_object (&self->package, package);
        }
      else if (g_strcmp0 (element_name, "c:include") == 0)
        {
          g_autoptr(RtfmGirCInclude) c_include = NULL;

          c_include = g_object_new (RTFM_TYPE_GIR_C_INCLUDE, NULL);

          if (!rtfm_gir_c_include_ingest (c_include, reader, error))
            return FALSE;

          g_set_object (&self->c_include, c_include);
        }
      else if (g_strcmp0 (element_name, "namespace") == 0)
        {
          g_autoptr(RtfmGirNamespace) namespace = NULL;

          namespace = g_object_new (RTFM_TYPE_GIR_NAMESPACE, NULL);

          if (!rtfm_gir_namespace_ingest (namespace, reader, error))
            return FALSE;

          g_set_object (&self->namespace, namespace);
        }
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
}

/**
 * rtfm_gir_repository_get_include:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirInclude or %NULL.
 */
RtfmGirInclude *
rtfm_gir_repository_get_include (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_REPOSITORY (self), NULL);

  return self->include;
}

/**
 * rtfm_gir_repository_get_package:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirPackage or %NULL.
 */
RtfmGirPackage *
rtfm_gir_repository_get_package (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_REPOSITORY (self), NULL);

  return self->package;
}

/**
 * rtfm_gir_repository_get_c_include:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirCInclude or %NULL.
 */
RtfmGirCInclude *
rtfm_gir_repository_get_c_include (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_REPOSITORY (self), NULL);

  return self->c_include;
}

/**
 * rtfm_gir_repository_get_namespace:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirNamespace or %NULL.
 */
RtfmGirNamespace *
rtfm_gir_repository_get_namespace (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_REPOSITORY (self), NULL);

  return self->namespace;
}

static void
rtfm_gir_repository_init_worker (GTask        *task,
                                 gpointer      source_object,
                                 gpointer      task_data,
                                 GCancellable *cancellable)
{
  RtfmGirRepository *self = source_object;
  g_autoptr(GMutexLocker) locker = NULL;
  g_autofree gchar *path = NULL;
  g_autofree gchar *error_string = NULL;
  g_autoptr(xmlTextReader) reader = NULL;
  GError *error = NULL;
  GSList *list;
  GSList *iter;

  g_assert (G_IS_TASK (task));
  g_assert (RTFM_IS_GIR_REPOSITORY (self));

  if (!G_IS_FILE (self->file) ||
      !g_file_is_native (self->file) ||
      NULL == (path = g_file_get_path (self->file)))
    {
      error = g_error_new (G_IO_ERROR,
                           G_IO_ERROR_INVALID_FILENAME,
                           "Missing filename or not a local file");
      goto failure;
    }

  reader = xmlNewTextReaderFilename (path);

  if (reader == NULL)
    {
      error = g_error_new (G_IO_ERROR,
                           G_IO_ERROR_INVALID_FILENAME,
                           "Failed to create xmlTextReader");
      goto failure;
    }

skip_node:
  if (xmlTextReaderNext (reader) != 1)
    {
      error = g_error_new (G_IO_ERROR,
                           G_IO_ERROR_INVALID_FILENAME,
                           "Failed to read from xmlTextReader");
      goto failure;
    }

  if (xmlTextReaderNodeType (reader) == XML_COMMENT_NODE)
    goto skip_node;

  if (!IS_ELEMENT_NAMED (reader, "repository"))
    {
      error = g_error_new (G_IO_ERROR,
                           G_IO_ERROR_FAILED,
                           "Failed to locate repository element");
      goto failure;
    }

  if (!rtfm_gir_repository_ingest (self, reader, &error))
    goto failure;

  g_assert_no_error (error);

  locker = g_mutex_locker_new (&self->mutex);

  self->loading = FALSE;
  self->loaded = TRUE;

  list = g_steal_pointer (&self->loading_tasks);

  for (iter = list; iter != NULL; iter = iter->next)
    {
      GTask *iter_task = iter->data;

      g_task_return_boolean (iter_task, TRUE);
    }

  g_slist_free_full (list, g_object_unref);

  g_task_return_boolean (task, TRUE);

  return;

failure:
  locker = g_mutex_locker_new (&self->mutex);

  self->loading = FALSE;
  self->loaded = TRUE;

  list = g_steal_pointer (&self->loading_tasks);

  for (iter = list; iter != NULL; iter = iter->next)
    {
      GTask *iter_task = iter->data;

      g_task_return_boolean (iter_task, FALSE);
    }

  g_slist_free_full (list, g_object_unref);

  if (error != NULL)
    g_task_return_error (task, error);
  else if (error_string != NULL)
    g_task_return_new_error (task,
                             G_IO_ERROR,
                             G_IO_ERROR_FAILED,
                             "%s", error_string);
  else
    g_task_return_new_error (task,
                             G_IO_ERROR,
                             G_IO_ERROR_FAILED,
                             "An unknown error occurred while parsing the gir file");
}

static void
rtfm_gir_repository_init_async (GAsyncInitable      *initable,
                                gint                 priority,
                                GCancellable        *cancellable,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  RtfmGirRepository *self = (RtfmGirRepository *)initable;
  g_autoptr(GTask) task = NULL;
  g_autoptr(GMutexLocker) locker = NULL;

  g_assert (G_IS_ASYNC_INITABLE (initable));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  locker = g_mutex_locker_new (&self->mutex);

  if (self->loaded)
    {
      g_task_return_boolean (task, TRUE);
      return;
    }

  if (self->loading)
    {
      self->loading_tasks = g_slist_prepend (self->loading_tasks, g_object_ref (task));
      return;
    }

  self->loading = TRUE;

  g_task_run_in_thread (task, rtfm_gir_repository_init_worker);
}

static gboolean
rtfm_gir_repository_init_finish (GAsyncInitable  *initable,
                                 GAsyncResult    *result,
                                 GError         **error)
{
  g_assert (RTFM_IS_GIR_REPOSITORY (initable));
  g_assert (G_IS_TASK (result));

  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
async_initable_iface_init (GAsyncInitableIface *iface)
{
  iface->init_async = rtfm_gir_repository_init_async;
  iface->init_finish = rtfm_gir_repository_init_finish;
}
