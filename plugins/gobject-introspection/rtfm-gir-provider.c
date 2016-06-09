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
#include "rtfm-gir-item.h"
#include "rtfm-gir-namespace.h"
#include "rtfm-gir-provider.h"
#include "rtfm-gir-repository.h"

struct _RtfmGirProvider
{
  GObject     object;

  GPtrArray  *files;
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
}

static void
rtfm_gir_provider_load_directory (RtfmGirProvider *self,
                                  const gchar     *path,
                                  GCancellable    *cancellable)
{
  g_autoptr(GFile) parent = NULL;
  g_autoptr(GFileEnumerator) enumerator = NULL;
  g_autoptr(GError) error = NULL;
  g_autofree gchar *index_dir = NULL;
  gpointer ptr;
  guint i;

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

  while (NULL != (ptr = g_file_enumerator_next_file (enumerator, cancellable, &error)))
    {
      g_autoptr(GFileInfo) file_info = ptr;
      g_autoptr(GFile) file = NULL;
      const gchar *name;

      name = g_file_info_get_name (file_info);
      file = g_file_get_child (parent, name);

      g_ptr_array_add (self->files, rtfm_gir_file_new (file));
    }

  if (error != NULL)
    {
      g_warning ("%s", error->message);
      return;
    }

  index_dir = g_build_filename (g_get_user_cache_dir (),
                                "rtfm",
                                "gobject-introspection",
                                NULL);

  if (!g_file_test (index_dir, G_FILE_TEST_IS_DIR))
    g_mkdir_with_parents (index_dir, 0750);

  for (i = 0; i < self->files->len; i++)
    {
      RtfmGirFile *item = g_ptr_array_index (self->files, i);
      g_autoptr(GFile) file = NULL;
      g_autofree gchar *gir_path = NULL;
      g_autofree gchar *index_path = NULL;
      g_autofree gchar *name = NULL;
      g_autoptr(GChecksum) checksum = NULL;
      const gchar *hex;
      GFile *gir_file = NULL;


      gir_file = rtfm_gir_file_get_file (item);
      gir_path = g_file_get_path (gir_file);

      checksum = g_checksum_new (G_CHECKSUM_SHA1);
      g_checksum_update (checksum, gir_path, strlen (gir_path));
      hex = g_checksum_get_string (checksum);
      name = g_strdup_printf ("%s.gvariant", hex);
      index_path = g_build_filename (g_get_user_cache_dir (),
                                     "rtfm",
                                     "gobject-introspection",
                                     name,
                                     NULL);
      file = g_file_new_for_path (index_path);

#if 0
      rtfm_gir_repository_build_index_async (repository,
                                             file,
                                             cancellable,
                                             NULL,
                                             NULL);
#endif
    }
}

static void
rtfm_gir_provider_reload (RtfmGirProvider *self)
{
  g_assert (RTFM_IS_GIR_PROVIDER (self));

  rtfm_gir_provider_load_directory (self, "/usr/share/gir-1.0", NULL);
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
rtfm_gir_provider_populate_cb (GObject      *object,
                               GAsyncResult *result,
                               gpointer      user_data)
{
  g_autoptr(GTask) task = user_data;
  RtfmGirItem *item = (RtfmGirItem *)object;
  GError *error = NULL;

  g_assert (RTFM_GIR_IS_ITEM (item));
  g_assert (G_IS_ASYNC_RESULT (result));
  g_assert (G_IS_TASK (task));

  if (!rtfm_gir_item_populate_finish (item, result, &error))
    g_task_return_error (task, error);
  else
    g_task_return_boolean (task, TRUE);
}

static void
rtfm_gir_provider_populate_async (RtfmProvider        *provider,
                                  RtfmItem            *parent,
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
  g_assert (!parent || RTFM_IS_ITEM (parent));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_task_data (task, g_object_ref (collection), g_object_unref);

  path = rtfm_collection_get_path (collection);

  if (rtfm_path_is_empty (path))
    {
      for (i = 0; i < self->files->len; i++)
        {
          RtfmGirFile *file = g_ptr_array_index (self->files, i);
          g_autoptr(RtfmGirItem) item = NULL;

          item = rtfm_gir_item_new (G_OBJECT (file));
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      g_task_return_boolean (task, TRUE);

      return;
    }
  else if (RTFM_GIR_IS_ITEM (parent))
    {
      rtfm_gir_item_populate_async (RTFM_GIR_ITEM (parent),
                                    collection,
                                    cancellable,
                                    rtfm_gir_provider_populate_cb,
                                    g_object_ref (task));
    }
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
