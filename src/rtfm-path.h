/* rtfm-path.h
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

#ifndef RTFM_PATH_H
#define RTFM_PATH_H

#include <gio/gio.h>

#include "rtfm-path-element.h"

G_BEGIN_DECLS

#define RTFM_TYPE_PATH (rtfm_path_get_type())

G_DECLARE_FINAL_TYPE (RtfmPath, rtfm_path, RTFM, PATH, GObject)

RtfmPath *rtfm_path_new          (void);
void      rtfm_path_push_element (RtfmPath        *self,
                                  RtfmPathElement *element);
GList    *rtfm_path_get_elements (RtfmPath        *self);
gboolean  rtfm_path_has_prefix   (RtfmPath        *self,
                                  RtfmPath        *prefix);
guint     rtfm_path_get_length   (RtfmPath        *self);
gchar    *rtfm_path_printf       (RtfmPath        *self);
gboolean  rtfm_path_is_empty     (RtfmPath        *self);

G_END_DECLS

#endif /* RTFM_PATH_H */

