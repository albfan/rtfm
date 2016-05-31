/* rtfm-gir-provider.c
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

#define G_LOG_DOMAIN "rtfm-gir-provider"

#include <glib/gi18n.h>
#include <string.h>

#include "rtfm-gir-alias.h"
#include "rtfm-gir-class.h"
#include "rtfm-gir-file.h"
#include "rtfm-gir-namespace.h"
#include "rtfm-gir-provider.h"
#include "rtfm-gir-repository.h"

struct _RtfmGirProvider
{
  GObject     object;

  GPtrArray  *files;
  GHashTable *item_cache;
};

static void provider_iface_init (RtfmProviderInterface *iface);

G_DEFINE_TYPE_EXTENDED (RtfmGirProvider, rtfm_gir_provider, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (RTFM_TYPE_PROVIDER, provider_iface_init))

static void
rtfm_gir_provider_class_init (RtfmGirProviderClass *klass)
{
}

static void
rtfm_gir_provider_init (RtfmGirProvider *self)
{
  self->files = g_ptr_array_new_with_free_func (g_object_unref);
  self->item_cache = g_hash_table_new_full (g_str_hash,
                                            g_str_equal,
                                            g_free,
                                            g_object_unref);
}

static void
rtfm_gir_provider_load_directory (RtfmGirProvider *self,
                                  const gchar     *path)
{
  g_autoptr(GFile) parent = NULL;
  g_autoptr(GFileEnumerator) enumerator = NULL;
  g_autoptr(GError) error = NULL;
  gpointer ptr;

  g_assert (RTFM_IS_GIR_PROVIDER (self));

  if (!g_file_test (path, G_FILE_TEST_IS_DIR))
    return;

  parent = g_file_new_for_path (path);
  enumerator = g_file_enumerate_children (parent,
                                          G_FILE_ATTRIBUTE_STANDARD_NAME,
                                          G_FILE_QUERY_INFO_NONE,
                                          NULL,
                                          &error);

  if (enumerator == NULL)
    {
      g_warning ("%s", error->message);
      return;
    }

  while (NULL != (ptr = g_file_enumerator_next_file (enumerator, NULL, &error)))
    {
      g_autoptr(GFileInfo) file_info = ptr;
      const gchar *name = g_file_info_get_name (file_info);
      g_autoptr(GFile) file = g_file_get_child (parent, name);

      g_ptr_array_add (self->files, g_steal_pointer (&file));
    }

  if (error != NULL)
    {
      g_warning ("%s", error->message);
      return;
    }
}

static void
rtfm_gir_provider_reload (RtfmGirProvider *self)
{
  g_assert (RTFM_IS_GIR_PROVIDER (self));

  rtfm_gir_provider_load_directory (self, "/usr/share/gir-1.0");
}

static void
rtfm_gir_provider_initialize (RtfmProvider *provider,
                              RtfmLibrary  *library)
{
  RtfmGirProvider *self = (RtfmGirProvider *)provider;

  g_assert (RTFM_IS_GIR_PROVIDER (self));
  g_assert (RTFM_IS_LIBRARY (library));

  rtfm_gir_provider_reload (self);
}

static void
rtfm_gir_provider_shutdown (RtfmProvider *provider,
                            RtfmLibrary  *library)
{
  RtfmGirProvider *self = (RtfmGirProvider *)provider;

  g_assert (RTFM_IS_GIR_PROVIDER (self));
  g_assert (RTFM_IS_LIBRARY (library));

}

static void
rtfm_gir_provider_load_file_cb (GObject      *object,
                                GAsyncResult *result,
                                gpointer      user_data)
{
  RtfmGirFile *file = (RtfmGirFile *)object;
  g_autoptr(RtfmGirRepository) repository = NULL;
  g_autoptr(GTask) task = user_data;
  RtfmGirNamespace *namespace;
  RtfmCollection *collection;
  GError *error = NULL;

  g_assert (RTFM_IS_GIR_FILE (file));
  g_assert (G_IS_ASYNC_RESULT (result));
  g_assert (G_IS_TASK (task));

  repository = rtfm_gir_file_load_finish (file, result, &error);
  g_assert (!repository || RTFM_IS_GIR_REPOSITORY (repository));

  if (repository == NULL)
    {
      g_task_return_error (task, error);
      return;
    }

  collection = g_task_get_task_data (task);
  g_assert (RTFM_IS_COLLECTION (collection));

  namespace = rtfm_gir_repository_get_namespace (repository);

  if (namespace != NULL)
    {
      GPtrArray *ar;
      guint i;

      if (NULL != (ar = rtfm_gir_namespace_get_classes (namespace)))
        {
          for (i = 0; i < ar->len; i++)
            {
              RtfmGirClass *klass = g_ptr_array_index (ar, i);

              g_assert (RTFM_IS_GIR_CLASS (klass));

              rtfm_collection_append (collection, RTFM_ITEM (klass));
            }
        }

      if (NULL != (ar = rtfm_gir_namespace_get_aliases (namespace)))
        {
          for (i = 0; i < ar->len; i++)
            {
              RtfmGirAlias *alias = g_ptr_array_index (ar, i);

              g_assert (RTFM_IS_GIR_ALIAS (alias));

              rtfm_collection_append (collection, RTFM_ITEM (alias));
            }
        }
    }

  g_task_return_boolean (task, TRUE);
}

static void
rtfm_gir_provider_populate_async (RtfmProvider        *provider,
                                  RtfmCollection      *collection,
                                  GCancellable        *cancellable,
                                  GAsyncReadyCallback  callback,
                                  gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;
  RtfmGirProvider *self = (RtfmGirProvider *)provider;
  RtfmPath *path;
  RtfmItem *item;
  guint i;

  g_assert (RTFM_IS_GIR_PROVIDER (self));
  g_assert (RTFM_IS_COLLECTION (collection));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_task_data (task, g_object_ref (collection), g_object_unref);

  path = rtfm_collection_get_path (collection);

  if (rtfm_path_is_empty (path))
    {
      for (i = 0; i < self->files->len; i++)
        {
          GFile *file = g_ptr_array_index (self->files, i);
          g_autofree gchar *path = NULL;
          g_autofree gchar *name = NULL;
          g_autofree gchar *id = NULL;
          g_autofree gchar *subtitle = NULL;
          const gchar *version;
          gchar *tmp;

          path = g_file_get_path (file);
          name = g_file_get_basename (file);

          tmp = strrchr (name, '.');
          if (tmp != NULL)
            *tmp = '\0';

          tmp = strrchr (name, '-');
          if (tmp != NULL)
            {
              *tmp = '\0';
              version = ++tmp;
            }

          id = g_strdup_printf ("gir:%s", path);
          subtitle = g_strdup_printf ("%s %s", name, version);
          item = g_object_new (RTFM_TYPE_GIR_FILE,
                               "file", file,
                               "id", id,
                               "title", name,
                               "subtitle", subtitle,
                               "icon-name", "lang-namespace-symbolic",
                               NULL);

          rtfm_collection_append (collection, item);

          g_hash_table_insert (self->item_cache,
                               g_steal_pointer (&id),
                               g_steal_pointer (&item));
        }

      goto complete_task;
    }
  else
    {
      RtfmPathElement *element;
      const gchar *id;
      guint len;

      len = rtfm_path_get_length (path);
      g_assert (len > 0);

      element = rtfm_path_get_element (path, len - 1);
      g_assert (RTFM_IS_PATH_ELEMENT (element));

      id = rtfm_path_element_get_id (element);

      if (id != NULL && g_str_has_prefix (id, "gir:"))
        {
          item = g_hash_table_lookup (self->item_cache, id);

          if (RTFM_IS_GIR_FILE (item))
            {
              rtfm_gir_file_load_async (RTFM_GIR_FILE (item),
                                        cancellable,
                                        rtfm_gir_provider_load_file_cb,
                                        g_object_ref (task));
              return;
            }
        }
    }

complete_task:
  g_task_return_boolean (task, TRUE);
}

static gboolean
rtfm_gir_provider_populate_finish (RtfmProvider  *provider,
                                   GAsyncResult  *result,
                                   GError       **error)
{
  g_assert (RTFM_IS_GIR_PROVIDER (provider));
  g_assert (G_IS_TASK (result));

  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
provider_iface_init (RtfmProviderInterface *iface)
{
  iface->initialize = rtfm_gir_provider_initialize;
  iface->shutdown = rtfm_gir_provider_shutdown;
  iface->populate_async = rtfm_gir_provider_populate_async;
  iface->populate_finish = rtfm_gir_provider_populate_finish;
}
