/* rtfm-widget.h
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

#ifndef RTFM_WIDGET_H
#define RTFM_WIDGET_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

void rtfm_g_object_unref_in_main                 (gpointer        instance);
void rtfm_gtk_widget_class_set_css_from_resource (GtkWidgetClass *widget_class,
                                                  const gchar    *theme_name,
                                                  const gchar    *resource_path);

G_END_DECLS

#endif /* RTFM_WIDGET_H */
