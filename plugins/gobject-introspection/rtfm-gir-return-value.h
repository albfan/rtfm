/* rtfm-gir-return-value.h
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

#ifndef RTFM_GIR_RETURN_VALUE_H
#define RTFM_GIR_RETURN_VALUE_H

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirReturnValue *rtfm_gir_return_value_new (void);

const gchar *rtfm_gir_return_value_get_introspectable (RtfmGirReturnValue *self);

const gchar *rtfm_gir_return_value_get_nullable (RtfmGirReturnValue *self);

const gchar *rtfm_gir_return_value_get_closure (RtfmGirReturnValue *self);

const gchar *rtfm_gir_return_value_get_scope (RtfmGirReturnValue *self);

const gchar *rtfm_gir_return_value_get_destroy (RtfmGirReturnValue *self);

const gchar *rtfm_gir_return_value_get_skip (RtfmGirReturnValue *self);

const gchar *rtfm_gir_return_value_get_allow_none (RtfmGirReturnValue *self);

const gchar *rtfm_gir_return_value_get_transfer_ownership (RtfmGirReturnValue *self);

G_END_DECLS

#endif /* RTFM_GIR_RETURN_VALUE */
