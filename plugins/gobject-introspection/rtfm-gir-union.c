/* rtfm-gir-union.c
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

#define G_LOG_DOMAIN "rtfm-gir-union"

#include "rtfm-gir-union.h"

#include "rtfm-gir-doc-version.h"
#include "rtfm-gir-doc-stability.h"
#include "rtfm-gir-doc.h"
#include "rtfm-gir-doc-deprecated.h"
#include "rtfm-gir-annotation.h"
#include "rtfm-gir-field.h"
#include "rtfm-gir-constructor.h"
#include "rtfm-gir-method.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-record.h"

struct _RtfmGirUnion
{
  GObject parent_instance;
  const gchar *introspectable;
  const gchar *deprecated;
  const gchar *deprecated_version;
  const gchar *version;
  const gchar *stability;
  const gchar *name;
  const gchar *c_type;
  const gchar *c_symbol_prefix;
  const gchar *glib_get_type;
  const gchar *glib_type_name;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirUnion, rtfm_gir_union, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_INTROSPECTABLE,
  PROP_DEPRECATED,
  PROP_DEPRECATED_VERSION,
  PROP_VERSION,
  PROP_STABILITY,
  PROP_NAME,
  PROP_C_TYPE,
  PROP_C_SYMBOL_PREFIX,
  PROP_GLIB_GET_TYPE,
  PROP_GLIB_TYPE_NAME,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_union_get_children (RtfmGirParserObject *object)
{
  RtfmGirUnion *self = (RtfmGirUnion *)object;

  g_assert (RTFM_GIR_IS_UNION (self));

  return self->children;
}

static void
rtfm_gir_union_start_element (GMarkupParseContext *context,
                              const gchar *element_name,
                              const gchar **attribute_names,
                              const gchar **attribute_values,
                              gpointer user_data,
                              GError **error)
{
  RtfmGirUnion *self = user_data;
  RtfmGirParserContext *parser_context;

  g_assert (RTFM_GIR_IS_UNION (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  parser_context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));

  if (FALSE) {}
  else if (g_str_equal (element_name, "doc-version"))
    {
      g_autoptr(RtfmGirDocVersion) child = NULL;

      child = rtfm_gir_doc_version_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc-stability"))
    {
      g_autoptr(RtfmGirDocStability) child = NULL;

      child = rtfm_gir_doc_stability_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc"))
    {
      g_autoptr(RtfmGirDoc) child = NULL;

      child = rtfm_gir_doc_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc-deprecated"))
    {
      g_autoptr(RtfmGirDocDeprecated) child = NULL;

      child = rtfm_gir_doc_deprecated_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "annotation"))
    {
      g_autoptr(RtfmGirAnnotation) child = NULL;

      child = rtfm_gir_annotation_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "field"))
    {
      g_autoptr(RtfmGirField) child = NULL;

      child = rtfm_gir_field_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "constructor"))
    {
      g_autoptr(RtfmGirConstructor) child = NULL;

      child = rtfm_gir_constructor_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "method"))
    {
      g_autoptr(RtfmGirMethod) child = NULL;

      child = rtfm_gir_method_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "function"))
    {
      g_autoptr(RtfmGirFunction) child = NULL;

      child = rtfm_gir_function_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "record"))
    {
      g_autoptr(RtfmGirRecord) child = NULL;

      child = rtfm_gir_record_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_union_end_element (GMarkupParseContext *context,
                            const gchar *element_name,
                            gpointer user_data,
                            GError **error)
{
  g_assert (RTFM_GIR_IS_UNION (user_data));
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
  else if (g_str_equal (element_name, "field"))
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
  else if (g_str_equal (element_name, "record"))
    {
      g_markup_parse_context_pop (context);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_union_start_element,
  rtfm_gir_union_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_union_ingest (RtfmGirParserObject *object,
                       GMarkupParseContext *context,
                       const gchar *element_name,
                       const gchar **attribute_names,
                       const gchar **attribute_values,
                       GError **error)
{
  RtfmGirUnion *self = (RtfmGirUnion *)object;
  RtfmGirParserContext *parser_context;
  const gchar *introspectable = NULL;
  const gchar *deprecated = NULL;
  const gchar *deprecated_version = NULL;
  const gchar *version = NULL;
  const gchar *stability = NULL;
  const gchar *name = NULL;
  const gchar *c_type = NULL;
  const gchar *c_symbol_prefix = NULL;
  const gchar *glib_get_type = NULL;
  const gchar *glib_type_name = NULL;

  g_assert (RTFM_GIR_IS_UNION (self));
  g_assert (g_str_equal (element_name, "union"));

  parser_context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));


  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "introspectable", &introspectable,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "deprecated", &deprecated,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "deprecated-version", &deprecated_version,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "version", &version,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "stability", &stability,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "c:type", &c_type,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "c:symbol-prefix", &c_symbol_prefix,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "glib:get-type", &glib_get_type,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "glib:type-name", &glib_type_name,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  self->introspectable = rtfm_gir_parser_context_intern_string (parser_context, introspectable);
  self->deprecated = rtfm_gir_parser_context_intern_string (parser_context, deprecated);
  self->deprecated_version = rtfm_gir_parser_context_intern_string (parser_context, deprecated_version);
  self->version = rtfm_gir_parser_context_intern_string (parser_context, version);
  self->stability = rtfm_gir_parser_context_intern_string (parser_context, stability);
  self->name = rtfm_gir_parser_context_intern_string (parser_context, name);
  self->c_type = rtfm_gir_parser_context_intern_string (parser_context, c_type);
  self->c_symbol_prefix = rtfm_gir_parser_context_intern_string (parser_context, c_symbol_prefix);
  self->glib_get_type = rtfm_gir_parser_context_intern_string (parser_context, glib_get_type);
  self->glib_type_name = rtfm_gir_parser_context_intern_string (parser_context, glib_type_name);

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_union_printf (RtfmGirParserObject *object,
                       GString *str,
                       guint depth)
{
  RtfmGirUnion *self = (RtfmGirUnion *)object;
  guint i;

  g_assert (RTFM_GIR_IS_UNION (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<union");

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
  if (self->c_type != NULL)
    g_string_append_printf (str, " c:type=\"%s\"", self->c_type);
  if (self->c_symbol_prefix != NULL)
    g_string_append_printf (str, " c:symbol-prefix=\"%s\"", self->c_symbol_prefix);
  if (self->glib_get_type != NULL)
    g_string_append_printf (str, " glib:get-type=\"%s\"", self->glib_get_type);
  if (self->glib_type_name != NULL)
    g_string_append_printf (str, " glib:type-name=\"%s\"", self->glib_type_name);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</union>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_union_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  RtfmGirUnion *self = (RtfmGirUnion *)object;

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

    case PROP_C_TYPE:
      g_value_set_string (value, self->c_type);
      break;

    case PROP_C_SYMBOL_PREFIX:
      g_value_set_string (value, self->c_symbol_prefix);
      break;

    case PROP_GLIB_GET_TYPE:
      g_value_set_string (value, self->glib_get_type);
      break;

    case PROP_GLIB_TYPE_NAME:
      g_value_set_string (value, self->glib_type_name);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_union_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  RtfmGirUnion *self = (RtfmGirUnion *)object;
  RtfmGirParserContext *context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));

  switch (prop_id)
    {
    case PROP_INTROSPECTABLE:
      self->introspectable = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_DEPRECATED:
      self->deprecated = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_DEPRECATED_VERSION:
      self->deprecated_version = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_VERSION:
      self->version = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_STABILITY:
      self->stability = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_NAME:
      self->name = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_C_TYPE:
      self->c_type = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_C_SYMBOL_PREFIX:
      self->c_symbol_prefix = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_GLIB_GET_TYPE:
      self->glib_get_type = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_GLIB_TYPE_NAME:
      self->glib_type_name = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_union_finalize (GObject *object)
{
  RtfmGirUnion *self = (RtfmGirUnion *)object;

  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_union_parent_class)->finalize (object);
}

static void
rtfm_gir_union_class_init (RtfmGirUnionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_union_get_property;
  object_class->set_property = rtfm_gir_union_set_property;
  object_class->finalize = rtfm_gir_union_finalize;

  parent_class->ingest = rtfm_gir_union_ingest;
  parent_class->printf = rtfm_gir_union_printf;
  parent_class->get_children = rtfm_gir_union_get_children;

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

  properties [PROP_GLIB_GET_TYPE] =
    g_param_spec_string ("glib-get-type",
                         "glib-get-type",
                         "glib-get-type",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_TYPE_NAME] =
    g_param_spec_string ("glib-type-name",
                         "glib-type-name",
                         "glib-type-name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_union_init (RtfmGirUnion *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_union_get_introspectable (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_UNION (self), NULL);

  return self->introspectable;
}

const gchar *
rtfm_gir_union_get_deprecated (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_UNION (self), NULL);

  return self->deprecated;
}

const gchar *
rtfm_gir_union_get_deprecated_version (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_UNION (self), NULL);

  return self->deprecated_version;
}

const gchar *
rtfm_gir_union_get_version (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_UNION (self), NULL);

  return self->version;
}

const gchar *
rtfm_gir_union_get_stability (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_UNION (self), NULL);

  return self->stability;
}

const gchar *
rtfm_gir_union_get_name (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_UNION (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_union_get_c_type (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_UNION (self), NULL);

  return self->c_type;
}

const gchar *
rtfm_gir_union_get_c_symbol_prefix (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_UNION (self), NULL);

  return self->c_symbol_prefix;
}

const gchar *
rtfm_gir_union_get_glib_get_type (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_UNION (self), NULL);

  return self->glib_get_type;
}

const gchar *
rtfm_gir_union_get_glib_type_name (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_UNION (self), NULL);

  return self->glib_type_name;
}

RtfmGirUnion *
rtfm_gir_union_new (RtfmGirParserContext *parser_context)
{
  return g_object_new (RTFM_GIR_TYPE_UNION,
                       "parser-context", parser_context,
                       NULL);
}
