/* rtfm-gir-bitfield.h
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

#ifndef RTFM_GIR_BITFIELD_H
#define RTFM_GIR_BITFIELD_H

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirBitfield *rtfm_gir_bitfield_new (RtfmGirParserContext *parser_context);

const gchar *rtfm_gir_bitfield_get_introspectable (RtfmGirBitfield *self);

const gchar *rtfm_gir_bitfield_get_deprecated (RtfmGirBitfield *self);

const gchar *rtfm_gir_bitfield_get_deprecated_version (RtfmGirBitfield *self);

const gchar *rtfm_gir_bitfield_get_version (RtfmGirBitfield *self);

const gchar *rtfm_gir_bitfield_get_stability (RtfmGirBitfield *self);

const gchar *rtfm_gir_bitfield_get_name (RtfmGirBitfield *self);

const gchar *rtfm_gir_bitfield_get_c_type (RtfmGirBitfield *self);

const gchar *rtfm_gir_bitfield_get_glib_type_name (RtfmGirBitfield *self);

const gchar *rtfm_gir_bitfield_get_glib_get_type (RtfmGirBitfield *self);

G_END_DECLS

#endif /* RTFM_GIR_BITFIELD */
