/* rtfm-gir-record.h
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

#ifndef RTFM_GIR_RECORD_H
#define RTFM_GIR_RECORD_H

#include <libxml/xmlreader.h>
#include <rtfm.h>

G_BEGIN_DECLS

#define RTFM_TYPE_GIR_RECORD (rtfm_gir_record_get_type())

G_DECLARE_FINAL_TYPE (RtfmGirRecord, rtfm_gir_record, RTFM, GIR_RECORD, RtfmItem)

gboolean rtfm_gir_record_ingest (RtfmGirRecord     *self,
                                 xmlTextReaderPtr   reader,
                                 GError           **error);

G_END_DECLS

#endif /* RTFM_GIR_RECORD_H */
