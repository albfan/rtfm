/* rtfm-gir-class.c
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

#define G_LOG_DOMAIN "rtfm-gir-class"

#include "rtfm-gir-class.h"

#include "rtfm-gir-doc-version.h"
#include "rtfm-gir-doc-stability.h"
#include "rtfm-gir-doc.h"
#include "rtfm-gir-doc-deprecated.h"
#include "rtfm-gir-annotation.h"
#include "rtfm-gir-implements.h"
#include "rtfm-gir-constructor.h"
#include "rtfm-gir-method.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-virtual-method.h"
#include "rtfm-gir-field.h"
#include "rtfm-gir-property.h"
#include "rtfm-gir-glib-signal.h"
#include "rtfm-gir-union.h"
#include "rtfm-gir-constant.h"
#include "rtfm-gir-record.h"
#include "rtfm-gir-callback.h"

struct _RtfmGirClass
{
  GObject parent_instance;
  gchar *introspectable;
  gchar *deprecated;
  gchar *deprecated_version;
  gchar *version;
  gchar *stability;
  gchar *name;
  gchar *glib_type_name;
  gchar *glib_get_type;
  gchar *parent;
  gchar *glib_type_struct;
  gchar *glib_ref_func;
  gchar *glib_unref_func;
  gchar *glib_set_value_func;
  gchar *glib_get_value_func;
  gchar *c_type;
  gchar *c_symbol_prefix;
  gchar *abstract;
  gchar *glib_fundamental;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirClass, rtfm_gir_class, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_INTROSPECTABLE,
  PROP_DEPRECATED,
  PROP_DEPRECATED_VERSION,
  PROP_VERSION,
  PROP_STABILITY,
  PROP_NAME,
  PROP_GLIB_TYPE_NAME,
  PROP_GLIB_GET_TYPE,
  PROP_PARENT,
  PROP_GLIB_TYPE_STRUCT,
  PROP_GLIB_REF_FUNC,
  PROP_GLIB_UNREF_FUNC,
  PROP_GLIB_SET_VALUE_FUNC,
  PROP_GLIB_GET_VALUE_FUNC,
  PROP_C_TYPE,
  PROP_C_SYMBOL_PREFIX,
  PROP_ABSTRACT,
  PROP_GLIB_FUNDAMENTAL,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_class_get_children (RtfmGirParserObject *object)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  g_assert (RTFM_GIR_IS_CLASS (self));

  return self->children;
}

static void
rtfm_gir_class_start_element (GMarkupParseContext *context,
                              const gchar *element_name,
                              const gchar **attribute_names,
                              const gchar **attribute_values,
                              gpointer user_data,
                              GError **error)
{
  RtfmGirClass *self = user_data;

  g_assert (RTFM_GIR_IS_CLASS (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  if (FALSE) {}
  else if (g_str_equal (element_name, "doc-version"))
    {
      g_autoptr(RtfmGirDocVersion) child = NULL;

      child = rtfm_gir_doc_version_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc-stability"))
    {
      g_autoptr(RtfmGirDocStability) child = NULL;

      child = rtfm_gir_doc_stability_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc"))
    {
      g_autoptr(RtfmGirDoc) child = NULL;

      child = rtfm_gir_doc_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc-deprecated"))
    {
      g_autoptr(RtfmGirDocDeprecated) child = NULL;

      child = rtfm_gir_doc_deprecated_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "annotation"))
    {
      g_autoptr(RtfmGirAnnotation) child = NULL;

      child = rtfm_gir_annotation_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "implements"))
    {
      g_autoptr(RtfmGirImplements) child = NULL;

      child = rtfm_gir_implements_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "constructor"))
    {
      g_autoptr(RtfmGirConstructor) child = NULL;

      child = rtfm_gir_constructor_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "method"))
    {
      g_autoptr(RtfmGirMethod) child = NULL;

      child = rtfm_gir_method_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "function"))
    {
      g_autoptr(RtfmGirFunction) child = NULL;

      child = rtfm_gir_function_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "virtual-method"))
    {
      g_autoptr(RtfmGirVirtualMethod) child = NULL;

      child = rtfm_gir_virtual_method_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "field"))
    {
      g_autoptr(RtfmGirField) child = NULL;

      child = rtfm_gir_field_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "property"))
    {
      g_autoptr(RtfmGirProperty) child = NULL;

      child = rtfm_gir_property_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "glib:signal"))
    {
      g_autoptr(RtfmGirGlibSignal) child = NULL;

      child = rtfm_gir_glib_signal_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "union"))
    {
      g_autoptr(RtfmGirUnion) child = NULL;

      child = rtfm_gir_union_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "constant"))
    {
      g_autoptr(RtfmGirConstant) child = NULL;

      child = rtfm_gir_constant_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "record"))
    {
      g_autoptr(RtfmGirRecord) child = NULL;

      child = rtfm_gir_record_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "callback"))
    {
      g_autoptr(RtfmGirCallback) child = NULL;

      child = rtfm_gir_callback_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_class_end_element (GMarkupParseContext *context,
                            const gchar *element_name,
                            gpointer user_data,
                            GError **error)
{
  g_assert (RTFM_GIR_IS_CLASS (user_data));
  g_assert (context != NULL);
  g_assert (element_name != NULL);

  if (FALSE) {}
  else if (g_str_equal (element_name, "doc-version"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "doc-stability"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "doc"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "doc-deprecated"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "constructor"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "method"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "function"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "virtual-method"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "field"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "property"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "glib:signal"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "union"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "constant"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "record"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "callback"))
    {
      g_markup_parse_context_pop (context);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_class_start_element,
  rtfm_gir_class_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_class_ingest (RtfmGirParserObject *object,
                       GMarkupParseContext *context,
                       const gchar *element_name,
                       const gchar **attribute_names,
                       const gchar **attribute_values,
                       GError **error)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  g_assert (RTFM_GIR_IS_CLASS (self));
  g_assert (g_str_equal (element_name, "class"));

  g_clear_pointer (&self->introspectable, g_free);
  g_clear_pointer (&self->deprecated, g_free);
  g_clear_pointer (&self->deprecated_version, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->stability, g_free);
  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->glib_type_name, g_free);
  g_clear_pointer (&self->glib_get_type, g_free);
  g_clear_pointer (&self->parent, g_free);
  g_clear_pointer (&self->glib_type_struct, g_free);
  g_clear_pointer (&self->glib_ref_func, g_free);
  g_clear_pointer (&self->glib_unref_func, g_free);
  g_clear_pointer (&self->glib_set_value_func, g_free);
  g_clear_pointer (&self->glib_get_value_func, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_clear_pointer (&self->c_symbol_prefix, g_free);
  g_clear_pointer (&self->abstract, g_free);
  g_clear_pointer (&self->glib_fundamental, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "introspectable", &self->introspectable,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "deprecated", &self->deprecated,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "deprecated-version", &self->deprecated_version,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "version", &self->version,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "stability", &self->stability,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:type-name", &self->glib_type_name,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:get-type", &self->glib_get_type,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "parent", &self->parent,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:type-struct", &self->glib_type_struct,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:ref-func", &self->glib_ref_func,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:unref-func", &self->glib_unref_func,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:set-value-func", &self->glib_set_value_func,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:get-value-func", &self->glib_get_value_func,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:type", &self->c_type,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:symbol-prefix", &self->c_symbol_prefix,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "abstract", &self->abstract,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:fundamental", &self->glib_fundamental,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_class_printf (RtfmGirParserObject *object,
                       GString *str,
                       guint depth)
{
  RtfmGirClass *self = (RtfmGirClass *)object;
  guint i;

  g_assert (RTFM_GIR_IS_CLASS (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<class");

  if (self->introspectable != NULL)
    g_string_append_printf (str, " introspectable=\"%s\"", self->introspectable);
  if (self->deprecated != NULL)
    g_string_append_printf (str, " deprecated=\"%s\"", self->deprecated);
  if (self->deprecated_version != NULL)
    g_string_append_printf (str, " deprecated-version=\"%s\"", self->deprecated_version);
  if (self->version != NULL)
    g_string_append_printf (str, " version=\"%s\"", self->version);
  if (self->stability != NULL)
    g_string_append_printf (str, " stability=\"%s\"", self->stability);
  if (self->name != NULL)
    g_string_append_printf (str, " name=\"%s\"", self->name);
  if (self->glib_type_name != NULL)
    g_string_append_printf (str, " glib:type-name=\"%s\"", self->glib_type_name);
  if (self->glib_get_type != NULL)
    g_string_append_printf (str, " glib:get-type=\"%s\"", self->glib_get_type);
  if (self->parent != NULL)
    g_string_append_printf (str, " parent=\"%s\"", self->parent);
  if (self->glib_type_struct != NULL)
    g_string_append_printf (str, " glib:type-struct=\"%s\"", self->glib_type_struct);
  if (self->glib_ref_func != NULL)
    g_string_append_printf (str, " glib:ref-func=\"%s\"", self->glib_ref_func);
  if (self->glib_unref_func != NULL)
    g_string_append_printf (str, " glib:unref-func=\"%s\"", self->glib_unref_func);
  if (self->glib_set_value_func != NULL)
    g_string_append_printf (str, " glib:set-value-func=\"%s\"", self->glib_set_value_func);
  if (self->glib_get_value_func != NULL)
    g_string_append_printf (str, " glib:get-value-func=\"%s\"", self->glib_get_value_func);
  if (self->c_type != NULL)
    g_string_append_printf (str, " c:type=\"%s\"", self->c_type);
  if (self->c_symbol_prefix != NULL)
    g_string_append_printf (str, " c:symbol-prefix=\"%s\"", self->c_symbol_prefix);
  if (self->abstract != NULL)
    g_string_append_printf (str, " abstract=\"%s\"", self->abstract);
  if (self->glib_fundamental != NULL)
    g_string_append_printf (str, " glib:fundamental=\"%s\"", self->glib_fundamental);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</class>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_class_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  switch (prop_id)
    {
    case PROP_INTROSPECTABLE:
      g_value_set_string (value, self->introspectable);
      break;

    case PROP_DEPRECATED:
      g_value_set_string (value, self->deprecated);
      break;

    case PROP_DEPRECATED_VERSION:
      g_value_set_string (value, self->deprecated_version);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_STABILITY:
      g_value_set_string (value, self->stability);
      break;

    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_GLIB_TYPE_NAME:
      g_value_set_string (value, self->glib_type_name);
      break;

    case PROP_GLIB_GET_TYPE:
      g_value_set_string (value, self->glib_get_type);
      break;

    case PROP_PARENT:
      g_value_set_string (value, self->parent);
      break;

    case PROP_GLIB_TYPE_STRUCT:
      g_value_set_string (value, self->glib_type_struct);
      break;

    case PROP_GLIB_REF_FUNC:
      g_value_set_string (value, self->glib_ref_func);
      break;

    case PROP_GLIB_UNREF_FUNC:
      g_value_set_string (value, self->glib_unref_func);
      break;

    case PROP_GLIB_SET_VALUE_FUNC:
      g_value_set_string (value, self->glib_set_value_func);
      break;

    case PROP_GLIB_GET_VALUE_FUNC:
      g_value_set_string (value, self->glib_get_value_func);
      break;

    case PROP_C_TYPE:
      g_value_set_string (value, self->c_type);
      break;

    case PROP_C_SYMBOL_PREFIX:
      g_value_set_string (value, self->c_symbol_prefix);
      break;

    case PROP_ABSTRACT:
      g_value_set_string (value, self->abstract);
      break;

    case PROP_GLIB_FUNDAMENTAL:
      g_value_set_string (value, self->glib_fundamental);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_class_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  switch (prop_id)
    {
    case PROP_INTROSPECTABLE:
      g_free (self->introspectable);
      self->introspectable = g_value_dup_string (value);
      break;

    case PROP_DEPRECATED:
      g_free (self->deprecated);
      self->deprecated = g_value_dup_string (value);
      break;

    case PROP_DEPRECATED_VERSION:
      g_free (self->deprecated_version);
      self->deprecated_version = g_value_dup_string (value);
      break;

    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
      break;

    case PROP_STABILITY:
      g_free (self->stability);
      self->stability = g_value_dup_string (value);
      break;

    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_GLIB_TYPE_NAME:
      g_free (self->glib_type_name);
      self->glib_type_name = g_value_dup_string (value);
      break;

    case PROP_GLIB_GET_TYPE:
      g_free (self->glib_get_type);
      self->glib_get_type = g_value_dup_string (value);
      break;

    case PROP_PARENT:
      g_free (self->parent);
      self->parent = g_value_dup_string (value);
      break;

    case PROP_GLIB_TYPE_STRUCT:
      g_free (self->glib_type_struct);
      self->glib_type_struct = g_value_dup_string (value);
      break;

    case PROP_GLIB_REF_FUNC:
      g_free (self->glib_ref_func);
      self->glib_ref_func = g_value_dup_string (value);
      break;

    case PROP_GLIB_UNREF_FUNC:
      g_free (self->glib_unref_func);
      self->glib_unref_func = g_value_dup_string (value);
      break;

    case PROP_GLIB_SET_VALUE_FUNC:
      g_free (self->glib_set_value_func);
      self->glib_set_value_func = g_value_dup_string (value);
      break;

    case PROP_GLIB_GET_VALUE_FUNC:
      g_free (self->glib_get_value_func);
      self->glib_get_value_func = g_value_dup_string (value);
      break;

    case PROP_C_TYPE:
      g_free (self->c_type);
      self->c_type = g_value_dup_string (value);
      break;

    case PROP_C_SYMBOL_PREFIX:
      g_free (self->c_symbol_prefix);
      self->c_symbol_prefix = g_value_dup_string (value);
      break;

    case PROP_ABSTRACT:
      g_free (self->abstract);
      self->abstract = g_value_dup_string (value);
      break;

    case PROP_GLIB_FUNDAMENTAL:
      g_free (self->glib_fundamental);
      self->glib_fundamental = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_class_finalize (GObject *object)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  g_clear_pointer (&self->introspectable, g_free);
  g_clear_pointer (&self->deprecated, g_free);
  g_clear_pointer (&self->deprecated_version, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->stability, g_free);
  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->glib_type_name, g_free);
  g_clear_pointer (&self->glib_get_type, g_free);
  g_clear_pointer (&self->parent, g_free);
  g_clear_pointer (&self->glib_type_struct, g_free);
  g_clear_pointer (&self->glib_ref_func, g_free);
  g_clear_pointer (&self->glib_unref_func, g_free);
  g_clear_pointer (&self->glib_set_value_func, g_free);
  g_clear_pointer (&self->glib_get_value_func, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_clear_pointer (&self->c_symbol_prefix, g_free);
  g_clear_pointer (&self->abstract, g_free);
  g_clear_pointer (&self->glib_fundamental, g_free);
  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_class_parent_class)->finalize (object);
}

static void
rtfm_gir_class_class_init (RtfmGirClassClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_class_get_property;
  object_class->set_property = rtfm_gir_class_set_property;
  object_class->finalize = rtfm_gir_class_finalize;

  parent_class->ingest = rtfm_gir_class_ingest;
  parent_class->printf = rtfm_gir_class_printf;
  parent_class->get_children = rtfm_gir_class_get_children;

  properties [PROP_INTROSPECTABLE] =
    g_param_spec_string ("introspectable",
                         "introspectable",
                         "introspectable",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DEPRECATED] =
    g_param_spec_string ("deprecated",
                         "deprecated",
                         "deprecated",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DEPRECATED_VERSION] =
    g_param_spec_string ("deprecated-version",
                         "deprecated-version",
                         "deprecated-version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_STABILITY] =
    g_param_spec_string ("stability",
                         "stability",
                         "stability",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_TYPE_NAME] =
    g_param_spec_string ("glib-type-name",
                         "glib-type-name",
                         "glib-type-name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_GET_TYPE] =
    g_param_spec_string ("glib-get-type",
                         "glib-get-type",
                         "glib-get-type",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_PARENT] =
    g_param_spec_string ("parent",
                         "parent",
                         "parent",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_TYPE_STRUCT] =
    g_param_spec_string ("glib-type-struct",
                         "glib-type-struct",
                         "glib-type-struct",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_REF_FUNC] =
    g_param_spec_string ("glib-ref-func",
                         "glib-ref-func",
                         "glib-ref-func",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_UNREF_FUNC] =
    g_param_spec_string ("glib-unref-func",
                         "glib-unref-func",
                         "glib-unref-func",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_SET_VALUE_FUNC] =
    g_param_spec_string ("glib-set-value-func",
                         "glib-set-value-func",
                         "glib-set-value-func",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_GET_VALUE_FUNC] =
    g_param_spec_string ("glib-get-value-func",
                         "glib-get-value-func",
                         "glib-get-value-func",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_TYPE] =
    g_param_spec_string ("c-type",
                         "c-type",
                         "c-type",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_SYMBOL_PREFIX] =
    g_param_spec_string ("c-symbol-prefix",
                         "c-symbol-prefix",
                         "c-symbol-prefix",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_ABSTRACT] =
    g_param_spec_string ("abstract",
                         "abstract",
                         "abstract",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_FUNDAMENTAL] =
    g_param_spec_string ("glib-fundamental",
                         "glib-fundamental",
                         "glib-fundamental",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_class_init (RtfmGirClass *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_class_get_introspectable (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->introspectable;
}

const gchar *
rtfm_gir_class_get_deprecated (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->deprecated;
}

const gchar *
rtfm_gir_class_get_deprecated_version (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->deprecated_version;
}

const gchar *
rtfm_gir_class_get_version (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->version;
}

const gchar *
rtfm_gir_class_get_stability (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->stability;
}

const gchar *
rtfm_gir_class_get_name (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_class_get_glib_type_name (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->glib_type_name;
}

const gchar *
rtfm_gir_class_get_glib_get_type (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->glib_get_type;
}

const gchar *
rtfm_gir_class_get_parent (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->parent;
}

const gchar *
rtfm_gir_class_get_glib_type_struct (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->glib_type_struct;
}

const gchar *
rtfm_gir_class_get_glib_ref_func (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->glib_ref_func;
}

const gchar *
rtfm_gir_class_get_glib_unref_func (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->glib_unref_func;
}

const gchar *
rtfm_gir_class_get_glib_set_value_func (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->glib_set_value_func;
}

const gchar *
rtfm_gir_class_get_glib_get_value_func (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->glib_get_value_func;
}

const gchar *
rtfm_gir_class_get_c_type (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->c_type;
}

const gchar *
rtfm_gir_class_get_c_symbol_prefix (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->c_symbol_prefix;
}

const gchar *
rtfm_gir_class_get_abstract (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->abstract;
}

const gchar *
rtfm_gir_class_get_glib_fundamental (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CLASS (self), NULL);

  return self->glib_fundamental;
}

RtfmGirClass *
rtfm_gir_class_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_CLASS, NULL);
}
