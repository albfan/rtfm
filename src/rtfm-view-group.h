/* rtfm-view-group.h
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

#ifndef RTFM_VIEW_GROUP_H
#define RTFM_VIEW_GROUP_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RTFM_TYPE_VIEW_GROUP (rtfm_view_group_get_type())

G_DECLARE_DERIVABLE_TYPE (RtfmViewGroup, rtfm_view_group, RTFM, VIEW_GROUP, GtkBox)

struct _RtfmViewGroupClass
{
  GtkBoxClass parent;
};

GtkWidget   *rtfm_view_group_new       (void);
const gchar *rtfm_view_group_get_title (RtfmViewGroup *self);
void         rtfm_view_group_set_title (RtfmViewGroup *self,
                                        const gchar   *title);
void         rtfm_view_group_clear     (RtfmViewGroup *self);
void         rtfm_view_group_add_row   (RtfmViewGroup *self,
                                        GtkWidget     *row);

G_END_DECLS

#endif /* RTFM_VIEW_GROUP_H */

