/* rtfm-provider.c
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

#define G_LOG_DOMAIN "rtfm-provider"

#include "rtfm-provider.h"

G_DEFINE_INTERFACE (RtfmProvider, rtfm_provider, G_TYPE_OBJECT)

static void
rtfm_provider_real_populate_async (RtfmProvider        *self,
                                   RtfmCollection      *collection,
                                   GCancellable        *cancellable,
                                   GAsyncReadyCallback  callback,
                                   gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  g_assert (RTFM_IS_PROVIDER (self));
  g_assert (RTFM_IS_COLLECTION (collection));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_return_boolean (task, TRUE);
}

static gboolean
rtfm_provider_real_populate_finish (RtfmProvider  *self,
                                    GAsyncResult  *result,
                                    GError       **error)
{
  g_return_val_if_fail (RTFM_IS_PROVIDER (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (result), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
rtfm_provider_real_search_async (RtfmProvider        *self,
                                 RtfmSearchSettings  *search_settings,
                                 RtfmCollection      *collection,
                                 GCancellable        *cancellable,
                                 GAsyncReadyCallback  callback,
                                 gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  g_return_if_fail (RTFM_IS_PROVIDER (self));
  g_return_if_fail (RTFM_IS_SEARCH_SETTINGS (search_settings));
  g_return_if_fail (RTFM_IS_COLLECTION (collection));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_return_boolean (task, TRUE);
}

static gboolean
rtfm_provider_real_search_finish (RtfmProvider  *self,
                                  GAsyncResult  *result,
                                  GError       **error)
{
  g_return_val_if_fail (RTFM_IS_PROVIDER (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (result), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
rtfm_provider_real_annotate_async (RtfmProvider        *self,
                                   RtfmItem            *item,
                                   GCancellable        *cancellable,
                                   GAsyncReadyCallback  callback,
                                   gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  g_return_if_fail (RTFM_IS_PROVIDER (self));
  g_return_if_fail (RTFM_IS_ITEM (item));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_return_boolean (task, TRUE);
}

static gboolean
rtfm_provider_real_annotate_finish (RtfmProvider  *self,
                                       GAsyncResult  *result,
                                       GError       **error)
{
  g_return_val_if_fail (RTFM_IS_PROVIDER (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (result), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
rtfm_provider_default_init (RtfmProviderInterface *iface)
{
  iface->populate_async = rtfm_provider_real_populate_async;
  iface->populate_finish = rtfm_provider_real_populate_finish;
  iface->search_async = rtfm_provider_real_search_async;
  iface->search_finish = rtfm_provider_real_search_finish;
  iface->annotate_async = rtfm_provider_real_annotate_async;
  iface->annotate_finish = rtfm_provider_real_annotate_finish;
}

void
rtfm_provider_populate_async (RtfmProvider        *self,
                              RtfmCollection      *collection,
                              GCancellable        *cancellable,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data)
{
  g_return_if_fail (RTFM_IS_PROVIDER (self));
  g_return_if_fail (RTFM_IS_COLLECTION (collection));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  RTFM_PROVIDER_GET_IFACE (self)->populate_async (self, collection, cancellable, callback, user_data);
}

gboolean
rtfm_provider_populate_finish (RtfmProvider  *self,
                               GAsyncResult  *result,
                               GError       **error)
{
  g_return_val_if_fail (RTFM_IS_PROVIDER (self), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  return RTFM_PROVIDER_GET_IFACE (self)->populate_finish (self, result, error);
}

void
rtfm_provider_search_async (RtfmProvider        *self,
                            RtfmSearchSettings  *search_settings,
                            RtfmCollection      *collection,
                            GCancellable        *cancellable,
                            GAsyncReadyCallback  callback,
                            gpointer             user_data)
{
  g_return_if_fail (RTFM_IS_PROVIDER (self));
  g_return_if_fail (RTFM_IS_SEARCH_SETTINGS (search_settings));
  g_return_if_fail (RTFM_IS_COLLECTION (collection));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  RTFM_PROVIDER_GET_IFACE (self)->search_async (self, search_settings, collection, cancellable, callback, user_data);
}

gboolean
rtfm_provider_search_finish (RtfmProvider  *self,
                             GAsyncResult  *result,
                             GError       **error)
{
  g_return_val_if_fail (RTFM_IS_PROVIDER (self), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  return RTFM_PROVIDER_GET_IFACE (self)->search_finish (self, result, error);
}

void
rtfm_provider_annotate_async (RtfmProvider        *self,
                                 RtfmItem            *item,
                                 GCancellable        *cancellable,
                                 GAsyncReadyCallback  callback,
                                 gpointer             user_data)
{
  g_return_if_fail (RTFM_IS_PROVIDER (self));
  g_return_if_fail (RTFM_IS_ITEM (item));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  RTFM_PROVIDER_GET_IFACE (self)->annotate_async (self, item, cancellable, callback, user_data);
}

gboolean
rtfm_provider_annotate_finish (RtfmProvider  *self,
                               GAsyncResult  *result,
                               GError       **error)
{
  g_return_val_if_fail (RTFM_IS_PROVIDER (self), FALSE);
  g_return_val_if_fail (G_IS_ASYNC_RESULT (result), FALSE);

  return RTFM_PROVIDER_GET_IFACE (self)->annotate_finish (self, result, error);
}

void
rtfm_provider_initialize (RtfmProvider *self,
                          RtfmLibrary  *library)
{
  g_return_if_fail (RTFM_IS_PROVIDER (self));
  g_return_if_fail (RTFM_IS_LIBRARY (library));

  if (RTFM_PROVIDER_GET_IFACE (self)->initialize)
    return RTFM_PROVIDER_GET_IFACE (self)->initialize (self, library);
}

void
rtfm_provider_shutdown (RtfmProvider *self,
                        RtfmLibrary  *library)
{
  g_return_if_fail (RTFM_IS_PROVIDER (self));
  g_return_if_fail (RTFM_IS_LIBRARY (library));

  if (RTFM_PROVIDER_GET_IFACE (self)->shutdown)
    return RTFM_PROVIDER_GET_IFACE (self)->shutdown (self, library);
}

/**
 * rtfm_provider_load_item:
 * @self: An #RtfmProvider
 * @id: the identifier of the item
 *
 * This function should load an #RtfmItem for a given @id if the provider
 * knows how to create the item.
 *
 * Providers generally prefix their items with a keyword like "foo:" so that
 * they know which items they can decode by id.
 *
 * Identifiers are stored in various places like the path bar, so that we can
 * avoid holding onto #RtfmItem instances any more than necessary. It can also
 * be used to reload a previous item at next application startup.
 *
 * Returns: (transfer full) (nullable): An #RtfmItem or %NULL.
 */
RtfmItem *
rtfm_provider_load_item (RtfmProvider *self,
                         const gchar  *id)
{
  g_return_val_if_fail (RTFM_IS_PROVIDER (self), NULL);
  g_return_val_if_fail (id != NULL, NULL);

  if (RTFM_PROVIDER_GET_IFACE (self)->load_item)
    return RTFM_PROVIDER_GET_IFACE (self)->load_item (self, id);

  return NULL;
}

void
rtfm_provider_postprocess (RtfmProvider   *self,
                           RtfmCollection *collection)
{
  g_return_if_fail (RTFM_IS_PROVIDER (self));
  g_return_if_fail (RTFM_IS_COLLECTION (collection));

  if (RTFM_PROVIDER_GET_IFACE (self)->postprocess)
    RTFM_PROVIDER_GET_IFACE (self)->postprocess (self, collection);
}
