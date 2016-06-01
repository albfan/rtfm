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
#include "rtfm-gir-item.h"
#include "rtfm-gir-namespace.h"
#include "rtfm-gir-provider.h"
#include "rtfm-gir-repository.h"

struct _RtfmGirProvider
{
  GObject     object;

  GPtrArray  *repositories;
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
  self->repositories = g_ptr_array_new_with_free_func (g_object_unref);
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
      g_autoptr(RtfmGirRepository) repository = NULL;
      g_autoptr(GFileInfo) file_info = ptr;
      g_autoptr(GFile) file = NULL;
      const gchar *name;

      name = g_file_info_get_name (file_info);
      file = g_file_get_child (parent, name);
      repository = rtfm_gir_repository_new (file);

      g_ptr_array_add (self->repositories, g_steal_pointer (&repository));
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
rtfm_gir_provider_populate_cb (GObject      *object,
                               GAsyncResult *result,
                               gpointer      user_data)
{
  g_autoptr(GTask) task = user_data;
  RtfmGirItem *item = (RtfmGirItem *)object;
  GError *error = NULL;

  g_assert (RTFM_IS_GIR_ITEM (item));
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
      for (i = 0; i < self->repositories->len; i++)
        {
          RtfmGirRepository *repository = g_ptr_array_index (self->repositories, i);
          g_autoptr(RtfmGirItem) item = NULL;

          item = rtfm_gir_item_new (G_OBJECT (repository));
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      g_task_return_boolean (task, TRUE);

      return;
    }
  else if (RTFM_IS_GIR_ITEM (parent))
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
