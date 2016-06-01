/* rtfm-gir-class.h
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

#ifndef RTFM_GIR_CLASS_H
#define RTFM_GIR_CLASS_H

#include "rtfm-gir-base.h"
#include "rtfm-gir-method.h"
#include "rtfm-gir-property.h"
#include "rtfm-gir-bitfield.h"

G_BEGIN_DECLS

#define RTFM_TYPE_GIR_CLASS (rtfm_gir_class_get_type())

G_DECLARE_FINAL_TYPE (RtfmGirClass, rtfm_gir_class, RTFM, GIR_CLASS, RtfmGirBase)

gboolean   rtfm_gir_class_has_methods (RtfmGirClass *self);
GPtrArray *rtfm_gir_class_get_methods (RtfmGirClass *self);
gboolean   rtfm_gir_class_has_propertys (RtfmGirClass *self);
GPtrArray *rtfm_gir_class_get_propertys (RtfmGirClass *self);
gboolean   rtfm_gir_class_has_bitfields (RtfmGirClass *self);
GPtrArray *rtfm_gir_class_get_bitfields (RtfmGirClass *self);

G_END_DECLS

#endif /* RTFM_GIR_CLASS_H */