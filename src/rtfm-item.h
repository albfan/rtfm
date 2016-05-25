/* rtfm-item.h
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

#ifndef RTFM_ITEM_H
#define RTFM_ITEM_H

#include "rtfm-types.h"

G_BEGIN_DECLS

struct _RtfmItemClass
{
  GObjectClass parent;

  RtfmPath *(*get_path) (RtfmItem *self);

  gpointer padding[8];
};

RtfmItem    *rtfm_item_new                 (void);
const gchar *rtfm_item_get_id              (RtfmItem    *self);
void         rtfm_item_set_id              (RtfmItem    *self,
                                            const gchar *id);
const gchar *rtfm_item_get_icon_name       (RtfmItem    *self);
void         rtfm_item_set_icon_name       (RtfmItem    *self,
                                            const gchar *icon_name);
const gchar *rtfm_item_get_title           (RtfmItem    *self);
void         rtfm_item_set_title           (RtfmItem    *self,
                                            const gchar *title);
const gchar *rtfm_item_get_subtitle        (RtfmItem    *self);
void         rtfm_item_set_subtitle        (RtfmItem    *self,
                                            const gchar *subtitle);
const gchar *rtfm_item_get_metadata_string (RtfmItem    *self,
                                            const gchar *key);
void         rtfm_item_set_metadata_string (RtfmItem    *self,
                                            const gchar *key,
                                            const gchar *value);
GVariant    *rtfm_item_get_metadata        (RtfmItem    *self,
                                            const gchar *key);
void         rtfm_item_set_metadata        (RtfmItem    *self,
                                            const gchar *key,
                                            GVariant    *value);
RtfmPath    *rtfm_item_get_path            (RtfmItem    *self);


GList       *rtfm_item_get_children        (RtfmItem    *self);
RtfmItem    *rtfm_item_get_parent          (RtfmItem    *self);
void         rtfm_item_append              (RtfmItem    *self,
                                            RtfmItem    *child);
void         rtfm_item_prepend             (RtfmItem    *self,
                                            RtfmItem    *child);
void         rtfm_item_insert_after        (RtfmItem    *self,
                                            RtfmItem    *sibling,
                                            RtfmItem    *child);
void         rtfm_item_insert_before       (RtfmItem    *self,
                                            RtfmItem    *sibling,
                                            RtfmItem    *child);
void         rtfm_item_remove_all          (RtfmItem    *self);

G_END_DECLS

#endif /* RTFM_ITEM_H */

