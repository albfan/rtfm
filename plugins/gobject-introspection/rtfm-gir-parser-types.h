/* rtfm-gir-parser-types.h
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

#ifndef RTFM_GIR_PARSER_TYPES_H
#define RTFM_GIR_PARSER_TYPES_H

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _RtfmGirParserContext RtfmGirParserContext;

#define RTFM_GIR_TYPE_PARSER_OBJECT (rtfm_gir_parser_object_get_type())
#define RTFM_GIR_TYPE_PARSER_CONTEXT (rtfm_gir_parser_context_get_type())

G_DECLARE_DERIVABLE_TYPE (RtfmGirParserObject, rtfm_gir_parser_object, RTFM_GIR, PARSER_OBJECT, GObject)

struct _RtfmGirParserObjectClass
{
  GObjectClass parent_class;

  GPtrArray *(*get_children) (RtfmGirParserObject *self);
  gboolean   (*ingest)       (RtfmGirParserObject *self,
                              GMarkupParseContext *context,
                              const gchar *element_name,
                              const gchar **attribute_names,
                              const gchar **attribute_values,
                              GError **error);
  void       (*printf)       (RtfmGirParserObject *self,
                              GString *str,
                              guint depth);
};

#define RTFM_GIR_TYPE_CONSTANT (rtfm_gir_constant_get_type())
#define RTFM_GIR_TYPE_CONSTRUCTOR (rtfm_gir_constructor_get_type())
#define RTFM_GIR_TYPE_DOC (rtfm_gir_doc_get_type())
#define RTFM_GIR_TYPE_MEMBER (rtfm_gir_member_get_type())
#define RTFM_GIR_TYPE_VIRTUAL_METHOD (rtfm_gir_virtual_method_get_type())
#define RTFM_GIR_TYPE_INSTANCE_PARAMETER (rtfm_gir_instance_parameter_get_type())
#define RTFM_GIR_TYPE_CALLBACK (rtfm_gir_callback_get_type())
#define RTFM_GIR_TYPE_INCLUDE (rtfm_gir_include_get_type())
#define RTFM_GIR_TYPE_METHOD (rtfm_gir_method_get_type())
#define RTFM_GIR_TYPE_UNION (rtfm_gir_union_get_type())
#define RTFM_GIR_TYPE_GLIB_SIGNAL (rtfm_gir_glib_signal_get_type())
#define RTFM_GIR_TYPE_BITFIELD (rtfm_gir_bitfield_get_type())
#define RTFM_GIR_TYPE_GLIB_BOXED (rtfm_gir_glib_boxed_get_type())
#define RTFM_GIR_TYPE_PARAMETERS (rtfm_gir_parameters_get_type())
#define RTFM_GIR_TYPE_INTERFACE (rtfm_gir_interface_get_type())
#define RTFM_GIR_TYPE_IMPLEMENTS (rtfm_gir_implements_get_type())
#define RTFM_GIR_TYPE_ENUMERATION (rtfm_gir_enumeration_get_type())
#define RTFM_GIR_TYPE_ALIAS (rtfm_gir_alias_get_type())
#define RTFM_GIR_TYPE_PROPERTY (rtfm_gir_property_get_type())
#define RTFM_GIR_TYPE_ANNOTATION (rtfm_gir_annotation_get_type())
#define RTFM_GIR_TYPE_REPOSITORY (rtfm_gir_repository_get_type())
#define RTFM_GIR_TYPE_DOC_DEPRECATED (rtfm_gir_doc_deprecated_get_type())
#define RTFM_GIR_TYPE_PACKAGE (rtfm_gir_package_get_type())
#define RTFM_GIR_TYPE_RECORD (rtfm_gir_record_get_type())
#define RTFM_GIR_TYPE_PARAMETER (rtfm_gir_parameter_get_type())
#define RTFM_GIR_TYPE_NAMESPACE (rtfm_gir_namespace_get_type())
#define RTFM_GIR_TYPE_C_INCLUDE (rtfm_gir_c_include_get_type())
#define RTFM_GIR_TYPE_FUNCTION (rtfm_gir_function_get_type())
#define RTFM_GIR_TYPE_FIELD (rtfm_gir_field_get_type())
#define RTFM_GIR_TYPE_PREREQUISITE (rtfm_gir_prerequisite_get_type())
#define RTFM_GIR_TYPE_DOC_VERSION (rtfm_gir_doc_version_get_type())
#define RTFM_GIR_TYPE_RETURN_VALUE (rtfm_gir_return_value_get_type())
#define RTFM_GIR_TYPE_DOC_STABILITY (rtfm_gir_doc_stability_get_type())
#define RTFM_GIR_TYPE_CLASS (rtfm_gir_class_get_type())
#define RTFM_GIR_TYPE_ARRAY (rtfm_gir_array_get_type())
#define RTFM_GIR_TYPE_TYPE (rtfm_gir_type_get_type())
#define RTFM_GIR_TYPE_VARARGS (rtfm_gir_varargs_get_type())
G_DECLARE_FINAL_TYPE (RtfmGirConstant, rtfm_gir_constant, RTFM_GIR, CONSTANT, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirConstructor, rtfm_gir_constructor, RTFM_GIR, CONSTRUCTOR, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirDoc, rtfm_gir_doc, RTFM_GIR, DOC, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirMember, rtfm_gir_member, RTFM_GIR, MEMBER, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirVirtualMethod, rtfm_gir_virtual_method, RTFM_GIR, VIRTUAL_METHOD, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirInstanceParameter, rtfm_gir_instance_parameter, RTFM_GIR, INSTANCE_PARAMETER, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirCallback, rtfm_gir_callback, RTFM_GIR, CALLBACK, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirInclude, rtfm_gir_include, RTFM_GIR, INCLUDE, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirMethod, rtfm_gir_method, RTFM_GIR, METHOD, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirUnion, rtfm_gir_union, RTFM_GIR, UNION, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirGlibSignal, rtfm_gir_glib_signal, RTFM_GIR, GLIB_SIGNAL, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirBitfield, rtfm_gir_bitfield, RTFM_GIR, BITFIELD, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirGlibBoxed, rtfm_gir_glib_boxed, RTFM_GIR, GLIB_BOXED, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirParameters, rtfm_gir_parameters, RTFM_GIR, PARAMETERS, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirInterface, rtfm_gir_interface, RTFM_GIR, INTERFACE, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirImplements, rtfm_gir_implements, RTFM_GIR, IMPLEMENTS, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirEnumeration, rtfm_gir_enumeration, RTFM_GIR, ENUMERATION, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirAlias, rtfm_gir_alias, RTFM_GIR, ALIAS, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirProperty, rtfm_gir_property, RTFM_GIR, PROPERTY, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirAnnotation, rtfm_gir_annotation, RTFM_GIR, ANNOTATION, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirRepository, rtfm_gir_repository, RTFM_GIR, REPOSITORY, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirDocDeprecated, rtfm_gir_doc_deprecated, RTFM_GIR, DOC_DEPRECATED, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirPackage, rtfm_gir_package, RTFM_GIR, PACKAGE, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirRecord, rtfm_gir_record, RTFM_GIR, RECORD, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirParameter, rtfm_gir_parameter, RTFM_GIR, PARAMETER, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirNamespace, rtfm_gir_namespace, RTFM_GIR, NAMESPACE, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirCInclude, rtfm_gir_c_include, RTFM_GIR, C_INCLUDE, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirFunction, rtfm_gir_function, RTFM_GIR, FUNCTION, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirField, rtfm_gir_field, RTFM_GIR, FIELD, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirPrerequisite, rtfm_gir_prerequisite, RTFM_GIR, PREREQUISITE, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirDocVersion, rtfm_gir_doc_version, RTFM_GIR, DOC_VERSION, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirReturnValue, rtfm_gir_return_value, RTFM_GIR, RETURN_VALUE, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirDocStability, rtfm_gir_doc_stability, RTFM_GIR, DOC_STABILITY, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirClass, rtfm_gir_class, RTFM_GIR, CLASS, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirArray, rtfm_gir_array, RTFM_GIR, ARRAY, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirType, rtfm_gir_type, RTFM_GIR, TYPE, RtfmGirParserObject)
G_DECLARE_FINAL_TYPE (RtfmGirVarargs, rtfm_gir_varargs, RTFM_GIR, VARARGS, RtfmGirParserObject)

GPtrArray *rtfm_gir_parser_object_get_children       (RtfmGirParserObject *self);
gboolean   rtfm_gir_parser_object_ingest             (RtfmGirParserObject *self,
                                                      GMarkupParseContext *context,
                                                      const gchar *element_name,
                                                      const gchar **attribute_names,
                                                      const gchar **attribute_values,
                                                      GError **error);
void       rtfm_gir_parser_object_printf             (RtfmGirParserObject *self,
                                                      GString *str,
                                                      guint depth);
GPtrArray *rtfm_gir_parser_object_get_children_typed (RtfmGirParserObject *self,
                                                       GType type);
gboolean   rtfm_gir_parser_object_has_child_typed    (RtfmGirParserObject *self,
                                                      GType type);

RtfmGirParserContext *rtfm_gir_parser_object_get_parser_context (RtfmGirParserObject *self);

gboolean   rtfm_gir_g_markup_collect_attributes (const gchar *element_name,
                                                 const gchar **attribute_names,
                                                 const gchar **attribute_values,
                                                 GError **error,
                                                 GMarkupCollectType first_type,
                                                 const gchar *first_attr,
                                                 ...);

GType rtfm_gir_parser_context_get_type (void);
RtfmGirParserContext *rtfm_gir_parser_context_new (void);
RtfmGirParserContext *rtfm_gir_parser_context_ref (RtfmGirParserContext *self);
void rtfm_gir_parser_context_unref (RtfmGirParserContext *self);
const gchar *rtfm_gir_parser_context_intern_string (RtfmGirParserContext *self, const gchar *string);

G_DEFINE_AUTOPTR_CLEANUP_FUNC (RtfmGirParserContext, rtfm_gir_parser_context_unref)

G_END_DECLS

#endif /* RTFM_GIR_PARSER_TYPES_H */
