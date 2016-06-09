/* rtfm-gir-constructor.h
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

#ifndef RTFM_GIR_CONSTRUCTOR_H
#define RTFM_GIR_CONSTRUCTOR_H

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirConstructor *rtfm_gir_constructor_new (void);

const gchar *rtfm_gir_constructor_get_introspectable (RtfmGirConstructor *self);

const gchar *rtfm_gir_constructor_get_deprecated (RtfmGirConstructor *self);

const gchar *rtfm_gir_constructor_get_deprecated_version (RtfmGirConstructor *self);

const gchar *rtfm_gir_constructor_get_version (RtfmGirConstructor *self);

const gchar *rtfm_gir_constructor_get_stability (RtfmGirConstructor *self);

const gchar *rtfm_gir_constructor_get_name (RtfmGirConstructor *self);

const gchar *rtfm_gir_constructor_get_c_identifier (RtfmGirConstructor *self);

const gchar *rtfm_gir_constructor_get_shadowed_by (RtfmGirConstructor *self);

const gchar *rtfm_gir_constructor_get_shadows (RtfmGirConstructor *self);

const gchar *rtfm_gir_constructor_get_throws (RtfmGirConstructor *self);

const gchar *rtfm_gir_constructor_get_moved_to (RtfmGirConstructor *self);

G_END_DECLS

#endif /* RTFM_GIR_CONSTRUCTOR */
