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

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirParameter *rtfm_gir_parameter_new (RtfmGirParserContext *parser_context);

const gchar *rtfm_gir_parameter_get_name (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_nullable (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_allow_none (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_introspectable (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_closure (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_destroy (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_scope (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_direction (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_caller_allocates (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_optional (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_skip (RtfmGirParameter *self);

const gchar *rtfm_gir_parameter_get_transfer_ownership (RtfmGirParameter *self);

G_END_DECLS

#endif /* RTFM_GIR_PARAMETER */
