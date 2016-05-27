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

#include "rtfm-types.h"

G_BEGIN_DECLS

struct _RtfmViewClass
{
  GtkBoxClass parent;

  gpointer padding[8];
};

GtkWidget   *rtfm_view_new         (void);
RtfmItem    *rtfm_view_get_item    (RtfmView      *self);
void         rtfm_view_set_item    (RtfmView      *self,
                                    RtfmItem      *item);
RtfmLibrary *rtfm_view_get_library (RtfmView      *self);
void         rtfm_view_set_library (RtfmView      *self,
                                    RtfmLibrary   *library);

G_END_DECLS

#endif /* RTFM_VIEW_H */

