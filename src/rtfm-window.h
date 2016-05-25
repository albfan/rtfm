/* rtfm-window.h
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

#ifndef RTFM_WINDOW_H
#define RTFM_WINDOW_H

#include "rtfm-types.h"

G_BEGIN_DECLS

GtkWidget   *rtfm_window_new         (void);
RtfmLibrary *rtfm_window_get_library (RtfmWindow  *self);
void         rtfm_window_set_library (RtfmWindow  *self,
                                      RtfmLibrary *library);

G_END_DECLS

#endif /* RTFM_WINDOW_H */
