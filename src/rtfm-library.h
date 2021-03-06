/* rtfm-library.h
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

#ifndef RTFM_LIBRARY_H
#define RTFM_LIBRARY_H

#include <gio/gio.h>

#include "rtfm-types.h"

G_BEGIN_DECLS

RtfmLibrary  *rtfm_library_new             (void);
RtfmLibrary  *rtfm_library_get_default     (void);
RtfmItem     *rtfm_library_get_item_by_id  (RtfmLibrary           *self,
                                            const gchar           *id);
RtfmProvider *rtfm_library_get_provider    (RtfmLibrary           *self,
                                            const gchar           *id);
RtfmItem     *rtfm_library_get_root        (RtfmLibrary           *self);
void          rtfm_library_populate_async  (RtfmLibrary           *self,
                                            RtfmItem              *item,
                                            GCancellable          *cancellable,
                                            GAsyncReadyCallback    callback,
                                            gpointer               user_data);
gboolean      rtfm_library_populate_finish (RtfmLibrary           *self,
                                            GAsyncResult          *result,
                                            GError               **error);
void          rtfm_library_search_async    (RtfmLibrary           *self,
                                            RtfmSearchSettings    *search_settings,
                                            RtfmSearchResults     *search_results,
                                            GCancellable          *cancellable,
                                            GAsyncReadyCallback    callback,
                                            gpointer               user_data);
gboolean      rtfm_library_search_finish   (RtfmLibrary           *self,
                                            GAsyncResult          *result,
                                            GError               **error);

G_END_DECLS

#endif /* RTFM_LIBRARY_H */

