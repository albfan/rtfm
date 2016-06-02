/* rtfm-gir-markup.h
 *
 * Copyright (C) 2016 Christian Hergert <chergert@redhat.com>
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

#ifndef RTFM_GIR_MARKUP_H
#define RTFM_GIR_MARKUP_H

#include <glib.h>

G_BEGIN_DECLS

gboolean rtfm_g_markup_collect_some_attributes (const gchar         *element_name,
                                                const gchar        **attribute_names,
                                                const gchar        **attribute_values,
                                                GError             **error,
                                                GMarkupCollectType   first_type,
                                                const gchar         *first_attr,
                                                ...);

G_END_DECLS

#endif /* RTFM_GIR_MARKUP_H */
