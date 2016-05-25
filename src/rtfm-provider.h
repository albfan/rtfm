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

#include "rtfm-types.h"

G_BEGIN_DECLS

struct _RtfmProviderInterface
{
  GTypeInterface parent;

  /* Plugin initialization and shutdown */
  void       (*load)                 (RtfmProvider         *self,
                                      RtfmLibrary          *library);
  void       (*unload)               (RtfmProvider         *self,
                                      RtfmLibrary          *library);

  /* Load a given item by id */
  RtfmItem  *(*load_item)            (RtfmProvider         *self,
                                      const gchar          *id);

  /* Load children from a given path */
  void       (*load_children_async)  (RtfmProvider         *self,
                                      RtfmPath             *path,
                                      RtfmCollection       *collection,
                                      GCancellable         *cancellable,
                                      GAsyncReadyCallback   callback,
                                      gpointer              user_data);
  gboolean   (*load_children_finish) (RtfmProvider         *self,
                                      GAsyncResult         *result,
                                      GError              **error);

  /* Perform search across the provider data set */
  void       (*search_async)         (RtfmProvider         *self,
                                      RtfmSearchSettings   *search_settings,
                                      RtfmCollection       *collection,
                                      GCancellable         *cancellable,
                                      GAsyncReadyCallback   callback,
                                      gpointer              user_data);
  gboolean   (*search_finish)        (RtfmProvider         *self,
                                      GAsyncResult         *result,
                                      GError              **error);

  /* Extend an item for ancillary information */
  void       (*extend_item_async)    (RtfmProvider         *self,
                                      RtfmItem             *item,
                                      GCancellable         *cancellable,
                                      GAsyncReadyCallback   callback,
                                      gpointer              user_data);
  gboolean   (*extend_item_finish)   (RtfmProvider         *self,
                                      GAsyncResult         *result,
                                      GError              **error);
};

void      rtfm_provider_load                 (RtfmProvider         *self,
                                              RtfmLibrary          *library);
void      rtfm_provider_unload               (RtfmProvider         *self,
                                              RtfmLibrary          *library);
RtfmItem *rtfm_provider_load_item            (RtfmProvider         *self,
                                              const gchar          *id);
void      rtfm_provider_load_children_async  (RtfmProvider         *self,
                                              RtfmPath             *path,
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
