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

#include "rtfm-gir-parser-types.h"

G_BEGIN_DECLS

RtfmGirClass *rtfm_gir_class_new (RtfmGirParserContext *parser_context);

const gchar *rtfm_gir_class_get_introspectable (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_deprecated (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_deprecated_version (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_version (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_stability (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_name (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_glib_type_name (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_glib_get_type (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_parent (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_glib_type_struct (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_glib_ref_func (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_glib_unref_func (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_glib_set_value_func (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_glib_get_value_func (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_c_type (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_c_symbol_prefix (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_abstract (RtfmGirClass *self);

const gchar *rtfm_gir_class_get_glib_fundamental (RtfmGirClass *self);

G_END_DECLS

#endif /* RTFM_GIR_CLASS */
