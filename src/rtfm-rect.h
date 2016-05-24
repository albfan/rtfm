/* rtfm-rect.h
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RTFM_RECT_H
#define RTFM_RECT_H

#include <gdk/gdk.h>

/*
 * This is just a helper object for animating rectangles.
 * It allows us to use egg_object_animate() to animate
 * coordinates.
 */

G_BEGIN_DECLS

#define RTFM_TYPE_RECT (rtfm_rect_get_type())

G_DECLARE_FINAL_TYPE (RtfmRect, rtfm_rect, RTFM, RECT, GObject)

void rtfm_rect_get_rect (RtfmRect      *self,
                        GdkRectangle *rect);

G_END_DECLS

#endif /* RTFM_RECT_H */
