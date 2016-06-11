/* rtfm-gir-constructor.c
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

#define G_LOG_DOMAIN "rtfm-gir-constructor"

#include "rtfm-gir-constructor.h"

#include "rtfm-gir-doc-version.h"
#include "rtfm-gir-doc-stability.h"
#include "rtfm-gir-doc.h"
#include "rtfm-gir-doc-deprecated.h"
#include "rtfm-gir-annotation.h"
#include "rtfm-gir-parameters.h"
#include "rtfm-gir-return-value.h"

struct _RtfmGirConstructor
{
  GObject parent_instance;
  const gchar *introspectable;
  const gchar *deprecated;
  const gchar *deprecated_version;
  const gchar *version;
  const gchar *stability;
  const gchar *name;
  const gchar *c_identifier;
  const gchar *shadowed_by;
  const gchar *shadows;
  const gchar *throws;
  const gchar *moved_to;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirConstructor, rtfm_gir_constructor, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_INTROSPECTABLE,
  PROP_DEPRECATED,
  PROP_DEPRECATED_VERSION,
  PROP_VERSION,
  PROP_STABILITY,
  PROP_NAME,
  PROP_C_IDENTIFIER,
  PROP_SHADOWED_BY,
  PROP_SHADOWS,
  PROP_THROWS,
  PROP_MOVED_TO,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_constructor_get_children (RtfmGirParserObject *object)
{
  RtfmGirConstructor *self = (RtfmGirConstructor *)object;

  g_assert (RTFM_GIR_IS_CONSTRUCTOR (self));

  return self->children;
}

static void
rtfm_gir_constructor_start_element (GMarkupParseContext *context,
                                    const gchar *element_name,
                                    const gchar **attribute_names,
                                    const gchar **attribute_values,
                                    gpointer user_data,
                                    GError **error)
{
  RtfmGirConstructor *self = user_data;
  RtfmGirParserContext *parser_context;

  g_assert (RTFM_GIR_IS_CONSTRUCTOR (self));
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

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc-stability"))
    {
      g_autoptr(RtfmGirDocStability) child = NULL;

      child = rtfm_gir_doc_stability_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc"))
    {
      g_autoptr(RtfmGirDoc) child = NULL;

      child = rtfm_gir_doc_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc-deprecated"))
    {
      g_autoptr(RtfmGirDocDeprecated) child = NULL;

      child = rtfm_gir_doc_deprecated_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "annotation"))
    {
      g_autoptr(RtfmGirAnnotation) child = NULL;

      child = rtfm_gir_annotation_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "parameters"))
    {
      g_autoptr(RtfmGirParameters) child = NULL;

      child = rtfm_gir_parameters_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "return-value"))
    {
      g_autoptr(RtfmGirReturnValue) child = NULL;

      child = rtfm_gir_return_value_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_constructor_end_element (GMarkupParseContext *context,
                                  const gchar *element_name,
                                  gpointer user_data,
                                  GError **error)
{
  g_assert (RTFM_GIR_IS_CONSTRUCTOR (user_data));
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
  else if (g_str_equal (element_name, "parameters"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "return-value"))
    {
      g_markup_parse_context_pop (context);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_constructor_start_element,
  rtfm_gir_constructor_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_constructor_ingest (RtfmGirParserObject *object,
                             GMarkupParseContext *context,
                             const gchar *element_name,
                             const gchar **attribute_names,
                             const gchar **attribute_values,
                             GError **error)
{
  RtfmGirConstructor *self = (RtfmGirConstructor *)object;
  RtfmGirParserContext *parser_context;
  const gchar *introspectable = NULL;
  const gchar *deprecated = NULL;
  const gchar *deprecated_version = NULL;
  const gchar *version = NULL;
  const gchar *stability = NULL;
  const gchar *name = NULL;
  const gchar *c_identifier = NULL;
  const gchar *shadowed_by = NULL;
  const gchar *shadows = NULL;
  const gchar *throws = NULL;
  const gchar *moved_to = NULL;

  g_assert (RTFM_GIR_IS_CONSTRUCTOR (self));
  g_assert (g_str_equal (element_name, "constructor"));

  parser_context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));


  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "introspectable", &introspectable,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "deprecated", &deprecated,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "deprecated-version", &deprecated_version,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "version", &version,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "stability", &stability,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "c:identifier", &c_identifier,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "shadowed-by", &shadowed_by,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "shadows", &shadows,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "throws", &throws,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "moved-to", &moved_to,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  self->introspectable = rtfm_gir_parser_context_intern_string (parser_context, introspectable);
  self->deprecated = rtfm_gir_parser_context_intern_string (parser_context, deprecated);
  self->deprecated_version = rtfm_gir_parser_context_intern_string (parser_context, deprecated_version);
  self->version = rtfm_gir_parser_context_intern_string (parser_context, version);
  self->stability = rtfm_gir_parser_context_intern_string (parser_context, stability);
  self->name = rtfm_gir_parser_context_intern_string (parser_context, name);
  self->c_identifier = rtfm_gir_parser_context_intern_string (parser_context, c_identifier);
  self->shadowed_by = rtfm_gir_parser_context_intern_string (parser_context, shadowed_by);
  self->shadows = rtfm_gir_parser_context_intern_string (parser_context, shadows);
  self->throws = rtfm_gir_parser_context_intern_string (parser_context, throws);
  self->moved_to = rtfm_gir_parser_context_intern_string (parser_context, moved_to);

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_constructor_printf (RtfmGirParserObject *object,
                             GString *str,
                             guint depth)
{
  RtfmGirConstructor *self = (RtfmGirConstructor *)object;
  guint i;

  g_assert (RTFM_GIR_IS_CONSTRUCTOR (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<constructor");

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
  if (self->c_identifier != NULL)
    g_string_append_printf (str, " c:identifier=\"%s\"", self->c_identifier);
  if (self->shadowed_by != NULL)
    g_string_append_printf (str, " shadowed-by=\"%s\"", self->shadowed_by);
  if (self->shadows != NULL)
    g_string_append_printf (str, " shadows=\"%s\"", self->shadows);
  if (self->throws != NULL)
    g_string_append_printf (str, " throws=\"%s\"", self->throws);
  if (self->moved_to != NULL)
    g_string_append_printf (str, " moved-to=\"%s\"", self->moved_to);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</constructor>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_constructor_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  RtfmGirConstructor *self = (RtfmGirConstructor *)object;

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

    case PROP_C_IDENTIFIER:
      g_value_set_string (value, self->c_identifier);
      break;

    case PROP_SHADOWED_BY:
      g_value_set_string (value, self->shadowed_by);
      break;

    case PROP_SHADOWS:
      g_value_set_string (value, self->shadows);
      break;

    case PROP_THROWS:
      g_value_set_string (value, self->throws);
      break;

    case PROP_MOVED_TO:
      g_value_set_string (value, self->moved_to);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_constructor_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  RtfmGirConstructor *self = (RtfmGirConstructor *)object;
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

    case PROP_C_IDENTIFIER:
      self->c_identifier = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_SHADOWED_BY:
      self->shadowed_by = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_SHADOWS:
      self->shadows = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_THROWS:
      self->throws = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_MOVED_TO:
      self->moved_to = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_constructor_finalize (GObject *object)
{
  RtfmGirConstructor *self = (RtfmGirConstructor *)object;

  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_constructor_parent_class)->finalize (object);
}

static void
rtfm_gir_constructor_class_init (RtfmGirConstructorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_constructor_get_property;
  object_class->set_property = rtfm_gir_constructor_set_property;
  object_class->finalize = rtfm_gir_constructor_finalize;

  parent_class->ingest = rtfm_gir_constructor_ingest;
  parent_class->printf = rtfm_gir_constructor_printf;
  parent_class->get_children = rtfm_gir_constructor_get_children;

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

  properties [PROP_C_IDENTIFIER] =
    g_param_spec_string ("c-identifier",
                         "c-identifier",
                         "c-identifier",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_SHADOWED_BY] =
    g_param_spec_string ("shadowed-by",
                         "shadowed-by",
                         "shadowed-by",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_SHADOWS] =
    g_param_spec_string ("shadows",
                         "shadows",
                         "shadows",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_THROWS] =
    g_param_spec_string ("throws",
                         "throws",
                         "throws",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_MOVED_TO] =
    g_param_spec_string ("moved-to",
                         "moved-to",
                         "moved-to",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_constructor_init (RtfmGirConstructor *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_constructor_get_introspectable (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->introspectable;
}

const gchar *
rtfm_gir_constructor_get_deprecated (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->deprecated;
}

const gchar *
rtfm_gir_constructor_get_deprecated_version (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->deprecated_version;
}

const gchar *
rtfm_gir_constructor_get_version (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->version;
}

const gchar *
rtfm_gir_constructor_get_stability (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->stability;
}

const gchar *
rtfm_gir_constructor_get_name (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_constructor_get_c_identifier (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->c_identifier;
}

const gchar *
rtfm_gir_constructor_get_shadowed_by (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->shadowed_by;
}

const gchar *
rtfm_gir_constructor_get_shadows (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->shadows;
}

const gchar *
rtfm_gir_constructor_get_throws (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->throws;
}

const gchar *
rtfm_gir_constructor_get_moved_to (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_CONSTRUCTOR (self), NULL);

  return self->moved_to;
}

RtfmGirConstructor *
rtfm_gir_constructor_new (RtfmGirParserContext *parser_context)
{
  return g_object_new (RTFM_GIR_TYPE_CONSTRUCTOR,
                       "parser-context", parser_context,
                       NULL);
}
