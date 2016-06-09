/* rtfm-gir-method.c
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

#define G_LOG_DOMAIN "rtfm-gir-method"

#include "rtfm-gir-method.h"

#include "rtfm-gir-doc-version.h"
#include "rtfm-gir-doc-stability.h"
#include "rtfm-gir-doc.h"
#include "rtfm-gir-doc-deprecated.h"
#include "rtfm-gir-annotation.h"
#include "rtfm-gir-parameters.h"
#include "rtfm-gir-return-value.h"

struct _RtfmGirMethod
{
  GObject parent_instance;
  gchar *introspectable;
  gchar *deprecated;
  gchar *deprecated_version;
  gchar *version;
  gchar *stability;
  gchar *name;
  gchar *c_identifier;
  gchar *shadowed_by;
  gchar *shadows;
  gchar *throws;
  gchar *moved_to;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirMethod, rtfm_gir_method, RTFM_GIR_TYPE_PARSER_OBJECT)

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
rtfm_gir_method_get_children (RtfmGirParserObject *object)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

  g_assert (RTFM_GIR_IS_METHOD (self));

  return self->children;
}

static void
rtfm_gir_method_start_element (GMarkupParseContext *context,
                               const gchar *element_name,
                               const gchar **attribute_names,
                               const gchar **attribute_values,
                               gpointer user_data,
                               GError **error)
{
  RtfmGirMethod *self = user_data;

  g_assert (RTFM_GIR_IS_METHOD (self));
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
  else if (g_str_equal (element_name, "parameters"))
    {
      g_autoptr(RtfmGirParameters) child = NULL;

      child = rtfm_gir_parameters_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "return-value"))
    {
      g_autoptr(RtfmGirReturnValue) child = NULL;

      child = rtfm_gir_return_value_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_method_end_element (GMarkupParseContext *context,
                             const gchar *element_name,
                             gpointer user_data,
                             GError **error)
{
  g_assert (RTFM_GIR_IS_METHOD (user_data));
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
  rtfm_gir_method_start_element,
  rtfm_gir_method_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_method_ingest (RtfmGirParserObject *object,
                        GMarkupParseContext *context,
                        const gchar *element_name,
                        const gchar **attribute_names,
                        const gchar **attribute_values,
                        GError **error)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

  g_assert (RTFM_GIR_IS_METHOD (self));
  g_assert (g_str_equal (element_name, "method"));

  g_clear_pointer (&self->introspectable, g_free);
  g_clear_pointer (&self->deprecated, g_free);
  g_clear_pointer (&self->deprecated_version, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->stability, g_free);
  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_identifier, g_free);
  g_clear_pointer (&self->shadowed_by, g_free);
  g_clear_pointer (&self->shadows, g_free);
  g_clear_pointer (&self->throws, g_free);
  g_clear_pointer (&self->moved_to, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "introspectable", &self->introspectable,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "deprecated", &self->deprecated,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "deprecated-version", &self->deprecated_version,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "version", &self->version,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "stability", &self->stability,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:identifier", &self->c_identifier,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "shadowed-by", &self->shadowed_by,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "shadows", &self->shadows,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "throws", &self->throws,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "moved-to", &self->moved_to,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_method_printf (RtfmGirParserObject *object,
                        GString *str,
                        guint depth)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;
  guint i;

  g_assert (RTFM_GIR_IS_METHOD (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<method");

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
      g_string_append (str, "</method>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_method_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

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
rtfm_gir_method_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

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

    case PROP_C_IDENTIFIER:
      g_free (self->c_identifier);
      self->c_identifier = g_value_dup_string (value);
      break;

    case PROP_SHADOWED_BY:
      g_free (self->shadowed_by);
      self->shadowed_by = g_value_dup_string (value);
      break;

    case PROP_SHADOWS:
      g_free (self->shadows);
      self->shadows = g_value_dup_string (value);
      break;

    case PROP_THROWS:
      g_free (self->throws);
      self->throws = g_value_dup_string (value);
      break;

    case PROP_MOVED_TO:
      g_free (self->moved_to);
      self->moved_to = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_method_finalize (GObject *object)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

  g_clear_pointer (&self->introspectable, g_free);
  g_clear_pointer (&self->deprecated, g_free);
  g_clear_pointer (&self->deprecated_version, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->stability, g_free);
  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_identifier, g_free);
  g_clear_pointer (&self->shadowed_by, g_free);
  g_clear_pointer (&self->shadows, g_free);
  g_clear_pointer (&self->throws, g_free);
  g_clear_pointer (&self->moved_to, g_free);
  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_method_parent_class)->finalize (object);
}

static void
rtfm_gir_method_class_init (RtfmGirMethodClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_method_get_property;
  object_class->set_property = rtfm_gir_method_set_property;
  object_class->finalize = rtfm_gir_method_finalize;

  parent_class->ingest = rtfm_gir_method_ingest;
  parent_class->printf = rtfm_gir_method_printf;
  parent_class->get_children = rtfm_gir_method_get_children;

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
rtfm_gir_method_init (RtfmGirMethod *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_method_get_introspectable (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->introspectable;
}

const gchar *
rtfm_gir_method_get_deprecated (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->deprecated;
}

const gchar *
rtfm_gir_method_get_deprecated_version (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->deprecated_version;
}

const gchar *
rtfm_gir_method_get_version (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->version;
}

const gchar *
rtfm_gir_method_get_stability (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->stability;
}

const gchar *
rtfm_gir_method_get_name (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_method_get_c_identifier (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->c_identifier;
}

const gchar *
rtfm_gir_method_get_shadowed_by (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->shadowed_by;
}

const gchar *
rtfm_gir_method_get_shadows (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->shadows;
}

const gchar *
rtfm_gir_method_get_throws (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->throws;
}

const gchar *
rtfm_gir_method_get_moved_to (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_METHOD (self), NULL);

  return self->moved_to;
}

RtfmGirMethod *
rtfm_gir_method_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_METHOD, NULL);
}
