/* rtfm-gir-parameter.h
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

#ifndef RTFM_GIR_PARAMETER_H
#define RTFM_GIR_PARAMETER_H

#include <libxml/xmlreader.h>
#include <rtfm.h>

#include "rtfm-gir-type.h"

G_BEGIN_DECLS

#define RTFM_TYPE_GIR_PARAMETER (rtfm_gir_parameter_get_type())

G_DECLARE_FINAL_TYPE (RtfmGirParameter, rtfm_gir_parameter, RTFM, GIR_PARAMETER, RtfmItem)

gboolean rtfm_gir_parameter_ingest (RtfmGirParameter  *self,
                                    xmlTextReaderPtr    reader,
                                    GError            **error);
RtfmGirType *rtfm_gir_parameter_get_referenced (RtfmGirParameter *self);

G_END_DECLS

#endif /* RTFM_GIR_PARAMETER_H */
