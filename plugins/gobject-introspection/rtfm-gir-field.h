/* rtfm-gir-field.h
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

#ifndef RTFM_GIR_FIELD_H
#define RTFM_GIR_FIELD_H

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirField *rtfm_gir_field_new (RtfmGirParserContext *parser_context);

const gchar *rtfm_gir_field_get_introspectable (RtfmGirField *self);

const gchar *rtfm_gir_field_get_deprecated (RtfmGirField *self);

const gchar *rtfm_gir_field_get_deprecated_version (RtfmGirField *self);

const gchar *rtfm_gir_field_get_version (RtfmGirField *self);

const gchar *rtfm_gir_field_get_stability (RtfmGirField *self);

const gchar *rtfm_gir_field_get_name (RtfmGirField *self);

const gchar *rtfm_gir_field_get_writable (RtfmGirField *self);

const gchar *rtfm_gir_field_get_readable (RtfmGirField *self);

const gchar *rtfm_gir_field_get_private (RtfmGirField *self);

const gchar *rtfm_gir_field_get_bits (RtfmGirField *self);

G_END_DECLS

#endif /* RTFM_GIR_FIELD */
