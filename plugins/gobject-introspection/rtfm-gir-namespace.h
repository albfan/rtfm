/* rtfm-gir-namespace.h
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

#ifndef RTFM_GIR_NAMESPACE_H
#define RTFM_GIR_NAMESPACE_H

#include "rtfm-gir-base.h"
#include "rtfm-gir-alias.h"
#include "rtfm-gir-bitfield.h"
#include "rtfm-gir-callback.h"
#include "rtfm-gir-class.h"
#include "rtfm-gir-constant.h"
#include "rtfm-gir-enumeration.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-record.h"

G_BEGIN_DECLS

#define RTFM_TYPE_GIR_NAMESPACE (rtfm_gir_namespace_get_type())

G_DECLARE_FINAL_TYPE (RtfmGirNamespace, rtfm_gir_namespace, RTFM, GIR_NAMESPACE, RtfmGirBase)

gboolean   rtfm_gir_namespace_has_aliases (RtfmGirNamespace *self);
GPtrArray *rtfm_gir_namespace_get_aliases (RtfmGirNamespace *self);
gboolean   rtfm_gir_namespace_has_bitfields (RtfmGirNamespace *self);
GPtrArray *rtfm_gir_namespace_get_bitfields (RtfmGirNamespace *self);
gboolean   rtfm_gir_namespace_has_callbacks (RtfmGirNamespace *self);
GPtrArray *rtfm_gir_namespace_get_callbacks (RtfmGirNamespace *self);
gboolean   rtfm_gir_namespace_has_classes (RtfmGirNamespace *self);
GPtrArray *rtfm_gir_namespace_get_classes (RtfmGirNamespace *self);
gboolean   rtfm_gir_namespace_has_constants (RtfmGirNamespace *self);
GPtrArray *rtfm_gir_namespace_get_constants (RtfmGirNamespace *self);
gboolean   rtfm_gir_namespace_has_enumerations (RtfmGirNamespace *self);
GPtrArray *rtfm_gir_namespace_get_enumerations (RtfmGirNamespace *self);
gboolean   rtfm_gir_namespace_has_functions (RtfmGirNamespace *self);
GPtrArray *rtfm_gir_namespace_get_functions (RtfmGirNamespace *self);
gboolean   rtfm_gir_namespace_has_records (RtfmGirNamespace *self);
GPtrArray *rtfm_gir_namespace_get_records (RtfmGirNamespace *self);

G_END_DECLS

#endif /* RTFM_GIR_NAMESPACE_H */
