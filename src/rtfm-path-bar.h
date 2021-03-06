/* rtfm-path-bar.h
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

#ifndef RTFM_PATH_BAR_H
#define RTFM_PATH_BAR_H

#include "rtfm-types.h"

G_BEGIN_DECLS

GtkWidget *rtfm_path_bar_new                (void);
RtfmPath  *rtfm_path_bar_get_path           (RtfmPathBar *self);
void       rtfm_path_bar_set_path           (RtfmPathBar *self,
                                             RtfmPath    *path);
void       rtfm_path_bar_set_selected_index (RtfmPathBar *self,
                                             guint        index);

G_END_DECLS

#endif /* RTFM_PATH_BAR_H */

