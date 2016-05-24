/* rtfm-provider.h
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

#ifndef RTFM_PROVIDER_H
#define RTFM_PROVIDER_H

#include <gio/gio.h>

#include "rtfm-collection.h"
#include "rtfm-item.h"
#include "rtfm-library.h"
#include "rtfm-search-settings.h"

G_BEGIN_DECLS

#define RTFM_TYPE_PROVIDER (rtfm_provider_get_type ())

G_DECLARE_INTERFACE (RtfmProvider, rtfm_provider, RTFM, PROVIDER, GObject)

struct _RtfmProviderInterface
{
  GTypeInterface parent;

  void       (*load)                 (RtfmProvider         *self,
                                      RtfmLibrary          *library);
  void       (*unload)               (RtfmProvider         *self,
                                      RtfmLibrary          *library);
  void       (*load_children_async)  (RtfmProvider         *self,
                                      RtfmItem             *item,
                                      RtfmCollection       *collection,
                                      GCancellable         *cancellable,
                                      GAsyncReadyCallback   callback,
                                      gpointer              user_data);
  gboolean   (*load_children_finish) (RtfmProvider         *self,
                                      GAsyncResult         *result,
                                      GError              **error);
  void       (*search_async)         (RtfmProvider         *self,
                                      RtfmSearchSettings   *search_settings,
                                      RtfmCollection       *collection,
                                      GCancellable         *cancellable,
                                      GAsyncReadyCallback   callback,
                                      gpointer              user_data);
  gboolean   (*search_finish)        (RtfmProvider         *self,
                                      GAsyncResult         *result,
                                      GError              **error);
  void       (*extend_item_async)    (RtfmProvider         *self,
                                      RtfmItem             *item,
                                      GCancellable         *cancellable,
                                      GAsyncReadyCallback   callback,
                                      gpointer              user_data);
  gboolean   (*extend_item_finish)   (RtfmProvider         *self,
                                      GAsyncResult         *result,
                                      GError              **error);
  RtfmItem  *(*load_item)            (RtfmProvider         *self,
                                      const gchar          *id);
};

void      rtfm_provider_load                 (RtfmProvider         *self,
                                              RtfmLibrary          *library);
void      rtfm_provider_unload               (RtfmProvider         *self,
                                              RtfmLibrary          *library);
RtfmItem *rtfm_provider_load_item            (RtfmProvider         *self,
                                              const gchar          *id);
void      rtfm_provider_load_children_async  (RtfmProvider         *self,
                                              RtfmItem             *item,
                                              RtfmCollection       *collection,
                                              GCancellable         *cancellable,
                                              GAsyncReadyCallback   callback,
                                              gpointer              user_data);
gboolean  rtfm_provider_load_children_finish (RtfmProvider         *self,
                                              GAsyncResult         *result,
                                              GError              **error);
void      rtfm_provider_search_async         (RtfmProvider         *self,
                                              RtfmSearchSettings   *search_settings,
                                              RtfmCollection       *collection,
                                              GCancellable         *cancellable,
                                              GAsyncReadyCallback   callback,
                                              gpointer              user_data);
gboolean  rtfm_provider_search_finish        (RtfmProvider         *self,
                                              GAsyncResult         *result,
                                              GError              **error);
void      rtfm_provider_extend_item_async    (RtfmProvider         *self,
                                              RtfmItem             *item,
                                              GCancellable         *cancellable,
                                              GAsyncReadyCallback   callback,
                                              gpointer              user_data);
gboolean  rtfm_provider_extend_item_finish   (RtfmProvider         *self,
                                              GAsyncResult         *result,
                                              GError              **error);

G_END_DECLS

#endif /* RTFM_PROVIDER_H */
