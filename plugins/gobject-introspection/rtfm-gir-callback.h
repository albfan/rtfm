/* rtfm-gir-callback.h
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

#ifndef RTFM_GIR_CALLBACK_H
#define RTFM_GIR_CALLBACK_H

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirCallback *rtfm_gir_callback_new (RtfmGirParserContext *parser_context);

const gchar *rtfm_gir_callback_get_introspectable (RtfmGirCallback *self);

const gchar *rtfm_gir_callback_get_deprecated (RtfmGirCallback *self);

const gchar *rtfm_gir_callback_get_deprecated_version (RtfmGirCallback *self);

const gchar *rtfm_gir_callback_get_version (RtfmGirCallback *self);

const gchar *rtfm_gir_callback_get_stability (RtfmGirCallback *self);

const gchar *rtfm_gir_callback_get_name (RtfmGirCallback *self);

const gchar *rtfm_gir_callback_get_c_type (RtfmGirCallback *self);

const gchar *rtfm_gir_callback_get_throws (RtfmGirCallback *self);

G_END_DECLS

#endif /* RTFM_GIR_CALLBACK */
