/* rtfm-gir-function.h
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

#ifndef RTFM_GIR_FUNCTION_H
#define RTFM_GIR_FUNCTION_H

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirFunction *rtfm_gir_function_new (RtfmGirParserContext *parser_context);

const gchar *rtfm_gir_function_get_introspectable (RtfmGirFunction *self);

const gchar *rtfm_gir_function_get_deprecated (RtfmGirFunction *self);

const gchar *rtfm_gir_function_get_deprecated_version (RtfmGirFunction *self);

const gchar *rtfm_gir_function_get_version (RtfmGirFunction *self);

const gchar *rtfm_gir_function_get_stability (RtfmGirFunction *self);

const gchar *rtfm_gir_function_get_name (RtfmGirFunction *self);

const gchar *rtfm_gir_function_get_c_identifier (RtfmGirFunction *self);

const gchar *rtfm_gir_function_get_shadowed_by (RtfmGirFunction *self);

const gchar *rtfm_gir_function_get_shadows (RtfmGirFunction *self);

const gchar *rtfm_gir_function_get_throws (RtfmGirFunction *self);

const gchar *rtfm_gir_function_get_moved_to (RtfmGirFunction *self);

G_END_DECLS

#endif /* RTFM_GIR_FUNCTION */
