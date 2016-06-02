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
#include "rtfm-gir-markup.h"
#include "rtfm-gir-namespace.h"

#if 1
# define ENTRY     do { g_printerr ("ENTRY: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); } while (0)
# define EXIT      do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return; } while (0)
# define RETURN(r) do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return r; } while (0)
#else
# define ENTRY
# define EXIT return
# define RETURN(r) do { return r; } while (0)
#endif

struct _RtfmGirRepository
{
  RtfmGirBase       base;

  GMutex            mutex;
  GSList           *loading_tasks;
  GError           *loading_error;

  GFile            *file;
  gchar            *version;
  RtfmGirNamespace *namespace;
  GPtrArray        *include;
  GPtrArray        *package;
  GPtrArray        *c_include;

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

G_DEFINE_TYPE_EXTENDED (RtfmGirRepository, rtfm_gir_repository, RTFM_TYPE_GIR_BASE, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_ASYNC_INITABLE,
                                               async_initable_iface_init))

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_repository_ingest (RtfmGirBase          *base,
                            GMarkupParseContext  *context,
                            const gchar          *element_name,
                            const gchar         **attribute_names,
                            const gchar         **attribute_values,
                            GError              **error);

static void
rtfm_gir_repository_finalize (GObject *object)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  g_clear_pointer (&self->version, g_free);

  g_clear_object (&self->file);
  g_clear_object (&self->namespace);
  g_clear_pointer (&self->include, g_ptr_array_unref);
  g_clear_pointer (&self->package, g_ptr_array_unref);
  g_clear_pointer (&self->c_include, g_ptr_array_unref);

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
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_repository_finalize;
  object_class->get_property = rtfm_gir_repository_get_property;
  object_class->set_property = rtfm_gir_repository_set_property;

  base_class->ingest = rtfm_gir_repository_ingest;

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

static void
rtfm_gir_repository_start_element (GMarkupParseContext  *context,
                                   const gchar          *element_name,
                                   const gchar         **attribute_names,
                                   const gchar         **attribute_values,
                                   gpointer              user_data,
                                   GError              **error)
{
  RtfmGirRepository *self = user_data;

  ENTRY;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_REPOSITORY (self));
  g_assert (error != NULL);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "namespace") == 0)
    {
      g_autoptr(RtfmGirNamespace) namespace = NULL;

      namespace = g_object_new (RTFM_TYPE_GIR_NAMESPACE, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (namespace),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      g_set_object (&self->namespace, namespace);
    }
  else if (g_strcmp0 (element_name, "include") == 0)
    {
      g_autoptr(RtfmGirInclude) include = NULL;

      include = g_object_new (RTFM_TYPE_GIR_INCLUDE, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (include),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->include == NULL)
        self->include = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->include, g_steal_pointer (&include));
    }
  else if (g_strcmp0 (element_name, "package") == 0)
    {
      g_autoptr(RtfmGirPackage) package = NULL;

      package = g_object_new (RTFM_TYPE_GIR_PACKAGE, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (package),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->package == NULL)
        self->package = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->package, g_steal_pointer (&package));
    }
  else if (g_strcmp0 (element_name, "c:include") == 0)
    {
      g_autoptr(RtfmGirCInclude) c_include = NULL;

      c_include = g_object_new (RTFM_TYPE_GIR_C_INCLUDE, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (c_include),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->c_include == NULL)
        self->c_include = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->c_include, g_steal_pointer (&c_include));
    }

  EXIT;
}

static void
rtfm_gir_repository_end_element (GMarkupParseContext  *context,
                                 const gchar          *element_name,
                                 gpointer              user_data,
                                 GError              **error)
{
  RtfmGirRepository *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_REPOSITORY (self));
  g_assert (error != NULL);

  if (g_strcmp0 (element_name, "repository") == 0)
    g_markup_parse_context_pop (context);
}

static void
rtfm_gir_repository_text (GMarkupParseContext  *context,
                          const gchar          *text,
                          gsize                 text_len,
                          gpointer              user_data,
                          GError              **error)
{
  RtfmGirRepository *self = user_data;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_IS_GIR_REPOSITORY (self));
  g_assert (error != NULL);

}

static void
rtfm_gir_repository_error (GMarkupParseContext *context,
                           GError              *error,
                           gpointer             user_data)
{
  RtfmGirRepository *self = user_data;

  g_assert (context != NULL);
  g_assert (RTFM_IS_GIR_REPOSITORY (self));
  g_assert (error != NULL);

  if (error != self->loading_error)
    {
      g_clear_error (&self->loading_error);
      self->loading_error = g_error_copy (error);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_repository_start_element,
  rtfm_gir_repository_end_element,
  rtfm_gir_repository_text,
  NULL,
  rtfm_gir_repository_error,
};

static gboolean
rtfm_gir_repository_ingest (RtfmGirBase          *base,
                            GMarkupParseContext  *context,
                            const gchar          *element_name,
                            const gchar         **attribute_names,
                            const gchar         **attribute_values,
                            GError              **error)
{
  RtfmGirRepository *self = (RtfmGirRepository *)base;

  ENTRY;

  g_assert (RTFM_IS_GIR_REPOSITORY (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (error != NULL);

  g_clear_pointer (&self->version, g_free);

  if (!rtfm_g_markup_collect_some_attributes (element_name,
                                              attribute_names,
                                              attribute_values,
                                              error,
                                              G_MARKUP_COLLECT_STRDUP, "version", &self->version,
                                              G_MARKUP_COLLECT_INVALID))
    RETURN (FALSE);

  g_markup_parse_context_push (context, &markup_parser, self);

  RETURN (TRUE);
}

/**
 * rtfm_gir_repository_get_include:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirInclude):
 *   A #GPtrArray of #RtfmGirInclude or %NULL.
 */
GPtrArray *
rtfm_gir_repository_get_include (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_REPOSITORY (self), NULL);

  return self->include;
}

/**
 * rtfm_gir_repository_get_package:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirPackage):
 *   A #GPtrArray of #RtfmGirPackage or %NULL.
 */
GPtrArray *
rtfm_gir_repository_get_package (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_REPOSITORY (self), NULL);

  return self->package;
}

/**
 * rtfm_gir_repository_get_c_include:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirCInclude):
 *   A #GPtrArray of #RtfmGirCInclude or %NULL.
 */
GPtrArray *
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
rtfm_gir_repository_base_start_element (GMarkupParseContext  *context,
                                        const gchar          *element_name,
                                        const gchar         **attribute_names,
                                        const gchar         **attribute_values,
                                        gpointer              user_data,
                                        GError              **error)
{
  RtfmGirRepository *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_REPOSITORY (self));

  if (g_strcmp0 (element_name, "repository") == 0)
    rtfm_gir_base_ingest (RTFM_GIR_BASE (self),
                          context,
                          element_name,
                          attribute_names,
                          attribute_values,
                          error);
}

static void
rtfm_gir_repository_base_end_element (GMarkupParseContext  *context,
                                      const gchar          *element_name,
                                      gpointer              user_data,
                                      GError              **error)
{
  RtfmGirRepository *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_REPOSITORY (self));

  g_markup_parse_context_pop (context);
}

static void
rtfm_gir_repository_base_error (GMarkupParseContext *context,
                                GError              *error,
                                gpointer             user_data)
{
  RtfmGirRepository *self = user_data;

  g_assert (RTFM_IS_GIR_REPOSITORY (self));
  g_assert (context != NULL);
  g_assert (error != NULL);

  g_clear_error (&self->loading_error);
  self->loading_error = g_error_copy (error);
}

static const GMarkupParser base_parser = {
  rtfm_gir_repository_base_start_element,
  rtfm_gir_repository_base_end_element,
  NULL,
  NULL,
  rtfm_gir_repository_base_error,
};

static void
rtfm_gir_repository_init_worker (GTask        *task,
                                 gpointer      source_object,
                                 gpointer      task_data,
                                 GCancellable *cancellable)
{
  RtfmGirRepository *self = source_object;
  g_autoptr(GMarkupParseContext) context = NULL;
  g_autoptr(GMutexLocker) locker = NULL;
  g_autofree gchar *contents = NULL;
  GError *error = NULL;
  GSList *list;
  GSList *iter;
  gsize contents_len = 0;

  g_assert (G_IS_TASK (task));
  g_assert (RTFM_IS_GIR_REPOSITORY (self));

  if (self->file == NULL)
    {
      error = g_error_new (G_IO_ERROR,
                           G_IO_ERROR_INVALID_FILENAME,
                           "RtfmGirRepository:file must be set before initializing");
      goto failure;
    }

  if (!g_file_load_contents (self->file,
                             cancellable,
                             &contents,
                             &contents_len,
                             NULL,
                             &error))
    goto failure;

  context = g_markup_parse_context_new (&base_parser, 0, self, NULL);

  if (!g_markup_parse_context_parse (context, contents, contents_len, &error))
    goto failure;

  if (self->loading_error != NULL)
    {
      error = g_error_copy (self->loading_error);
      goto failure;
    }

  if (!g_markup_parse_context_end_parse (context, &error))
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
      if (self->loading_error != NULL)
        g_task_return_error (task, g_error_copy (self->loading_error));
      else
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
