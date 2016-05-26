/* rtfm-collection.h
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

#ifndef RTFM_COLLECTION_H
#define RTFM_COLLECTION_H

#include <gio/gio.h>

#include "rtfm-types.h"

G_BEGIN_DECLS

typedef void (*RtfmCollectionCallback) (RtfmItem *item,
                                        gpointer  user_data);

RtfmCollection *rtfm_collection_new         (RtfmPath               *path);
RtfmPath       *rtfm_collection_get_path    (RtfmCollection         *self);
void            rtfm_collection_foreach     (RtfmCollection         *self,
                                             RtfmCollectionCallback  callback,
                                             gpointer                user_data);
void            rtfm_collection_prepend     (RtfmCollection         *self,
                                             RtfmItem               *item);
void            rtfm_collection_append      (RtfmCollection         *self,
                                             RtfmItem               *item);
void            rtfm_collection_remove_item (RtfmCollection         *self,
                                             RtfmItem               *item);
GPtrArray      *rtfm_collection_to_array    (RtfmCollection         *self);

G_END_DECLS

#endif /* RTFM_COLLECTION_H */

