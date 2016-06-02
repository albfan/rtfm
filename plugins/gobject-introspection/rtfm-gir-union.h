/* rtfm-gir-union.h
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

#ifndef RTFM_GIR_UNION_H
#define RTFM_GIR_UNION_H

#include "rtfm-gir-base.h"
#include "rtfm-gir-record.h"
#include "rtfm-gir-field.h"

G_BEGIN_DECLS

#define RTFM_TYPE_GIR_UNION (rtfm_gir_union_get_type())

G_DECLARE_FINAL_TYPE (RtfmGirUnion, rtfm_gir_union, RTFM, GIR_UNION, RtfmGirBase)

gboolean   rtfm_gir_union_has_records (RtfmGirUnion *self);
GPtrArray *rtfm_gir_union_get_records (RtfmGirUnion *self);
gboolean   rtfm_gir_union_has_fields (RtfmGirUnion *self);
GPtrArray *rtfm_gir_union_get_fields (RtfmGirUnion *self);

G_END_DECLS

#endif /* RTFM_GIR_UNION_H */
