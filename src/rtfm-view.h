/* rtfm-view.h
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

#ifndef RTFM_VIEW_H
#define RTFM_VIEW_H

#include <gtk/gtk.h>

#include "rtfm-item.h"
#include "rtfm-library.h"
#include "rtfm-view-group.h"

G_BEGIN_DECLS

#define RTFM_TYPE_VIEW (rtfm_view_get_type())

G_DECLARE_DERIVABLE_TYPE (RtfmView, rtfm_view, RTFM, VIEW, GtkBin)

struct _RtfmViewClass
{
  GtkBinClass parent;
};

GtkWidget   *rtfm_view_new         (void);
RtfmItem    *rtfm_view_get_item    (RtfmView      *self);
void         rtfm_view_set_item    (RtfmView      *self,
                                    RtfmItem      *item);
RtfmLibrary *rtfm_view_get_library (RtfmView      *self);
void         rtfm_view_set_library (RtfmView      *self,
                                    RtfmLibrary   *library);
void         rtfm_view_add_group   (RtfmView      *self,
                                    RtfmViewGroup *group);

G_END_DECLS

#endif /* RTFM_VIEW_H */

