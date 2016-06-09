/* rtfm-gir-property.h
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

#ifndef RTFM_GIR_PROPERTY_H
#define RTFM_GIR_PROPERTY_H

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirProperty *rtfm_gir_property_new (RtfmGirParserContext *parser_context);

const gchar *rtfm_gir_property_get_introspectable (RtfmGirProperty *self);

const gchar *rtfm_gir_property_get_deprecated (RtfmGirProperty *self);

const gchar *rtfm_gir_property_get_deprecated_version (RtfmGirProperty *self);

const gchar *rtfm_gir_property_get_version (RtfmGirProperty *self);

const gchar *rtfm_gir_property_get_stability (RtfmGirProperty *self);

const gchar *rtfm_gir_property_get_name (RtfmGirProperty *self);

const gchar *rtfm_gir_property_get_writable (RtfmGirProperty *self);

const gchar *rtfm_gir_property_get_readable (RtfmGirProperty *self);

const gchar *rtfm_gir_property_get_construct (RtfmGirProperty *self);

const gchar *rtfm_gir_property_get_construct_only (RtfmGirProperty *self);

const gchar *rtfm_gir_property_get_transfer_ownership (RtfmGirProperty *self);

G_END_DECLS

#endif /* RTFM_GIR_PROPERTY */
