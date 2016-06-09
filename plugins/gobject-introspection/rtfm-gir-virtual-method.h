/* rtfm-gir-virtual-method.h
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

#ifndef RTFM_GIR_VIRTUAL_METHOD_H
#define RTFM_GIR_VIRTUAL_METHOD_H

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirVirtualMethod *rtfm_gir_virtual_method_new (RtfmGirParserContext *parser_context);

const gchar *rtfm_gir_virtual_method_get_introspectable (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_deprecated (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_deprecated_version (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_version (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_stability (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_name (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_c_identifier (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_shadowed_by (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_shadows (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_throws (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_moved_to (RtfmGirVirtualMethod *self);

const gchar *rtfm_gir_virtual_method_get_invoker (RtfmGirVirtualMethod *self);

G_END_DECLS

#endif /* RTFM_GIR_VIRTUAL_METHOD */
