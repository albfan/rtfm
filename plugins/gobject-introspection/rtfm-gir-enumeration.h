/* rtfm-gir-enumeration.h
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

#ifndef RTFM_GIR_ENUMERATION_H
#define RTFM_GIR_ENUMERATION_H

#include "rtfm-gir-base.h"
#include "rtfm-gir-member.h"

G_BEGIN_DECLS

#define RTFM_TYPE_GIR_ENUMERATION (rtfm_gir_enumeration_get_type())

G_DECLARE_FINAL_TYPE (RtfmGirEnumeration, rtfm_gir_enumeration, RTFM, GIR_ENUMERATION, RtfmGirBase)

gboolean   rtfm_gir_enumeration_has_members (RtfmGirEnumeration *self);
GPtrArray *rtfm_gir_enumeration_get_members (RtfmGirEnumeration *self);

G_END_DECLS

#endif /* RTFM_GIR_ENUMERATION_H */