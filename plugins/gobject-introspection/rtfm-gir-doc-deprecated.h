/* rtfm-gir-doc-deprecated.h
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

#ifndef RTFM_GIR_DOC_DEPRECATED_H
#define RTFM_GIR_DOC_DEPRECATED_H

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirDocDeprecated *rtfm_gir_doc_deprecated_new (RtfmGirParserContext *parser_context);

const gchar *rtfm_gir_doc_deprecated_get_xml_space (RtfmGirDocDeprecated *self);

const gchar *rtfm_gir_doc_deprecated_get_xml_whitespace (RtfmGirDocDeprecated *self);

G_END_DECLS

#endif /* RTFM_GIR_DOC_DEPRECATED */
