/* rtfm-path-element.h
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

#ifndef RTFM_PATH_ELEMENT_H
#define RTFM_PATH_ELEMENT_H

#include <glib-object.h>

G_BEGIN_DECLS

#define RTFM_TYPE_PATH_ELEMENT (rtfm_path_element_get_type())

G_DECLARE_FINAL_TYPE (RtfmPathElement, rtfm_path_element, RTFM, PATH_ELEMENT, GObject)

RtfmPathElement *rtfm_path_element_new           (const gchar     *id,
                                                  const gchar     *icon_name,
                                                  const gchar     *title);
const gchar     *rtfm_path_element_get_title     (RtfmPathElement *self);
const gchar     *rtfm_path_element_get_id        (RtfmPathElement *self);
const gchar     *rtfm_path_element_get_icon_name (RtfmPathElement *self);

G_END_DECLS

#endif /* RTFM_PATH_ELEMENT_H */

