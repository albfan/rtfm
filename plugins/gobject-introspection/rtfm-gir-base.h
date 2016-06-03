/* rtfm-gir-base.h
 *
 * Copyright (C) 2016 Christian Hergert <christian@hergert.me>
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

#ifndef RTFM_GIR_BASE_H
#define RTFM_GIR_BASE_H

#include <rtfm.h>

G_BEGIN_DECLS

#define RTFM_TYPE_GIR_BASE (rtfm_gir_base_get_type())

G_DECLARE_DERIVABLE_TYPE (RtfmGirBase, rtfm_gir_base, RTFM, GIR_BASE, GObject)

struct _RtfmGirBaseClass
{
  GObjectClass parent;

  gboolean (*ingest) (RtfmGirBase          *self,
                      GMarkupParseContext  *context,
                      const gchar          *element_name,
                      const gchar         **attribute_names,
                      const gchar         **attribute_values,
                      GError              **error);

  gpointer padding[7];
};

RtfmGirBase *rtfm_gir_base_get_toplevel  (RtfmGirBase          *self);
RtfmGirBase *rtfm_gir_base_get_parent    (RtfmGirBase          *self);
void         rtfm_gir_base_set_parent    (RtfmGirBase          *self,
                                          RtfmGirBase          *parent);
gboolean     rtfm_gir_base_ingest        (RtfmGirBase          *self,
                                          GMarkupParseContext  *context,
                                          const gchar          *element_name,
                                          const gchar         **attribute_names,
                                          const gchar         **attribute_values,
                                          GError              **error);
const gchar *rtfm_gir_base_intern_string (RtfmGirBase          *self,
                                          const gchar          *string);

G_END_DECLS

#endif /* RTFM_GIR_BASE_H */
