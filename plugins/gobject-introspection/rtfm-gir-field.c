/* rtfm-gir-field.c
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

#define G_LOG_DOMAIN "rtfm-gir-field"

#include "rtfm-gir-field.h"

#include "rtfm-gir-doc-version.h"
#include "rtfm-gir-doc-stability.h"
#include "rtfm-gir-doc.h"
#include "rtfm-gir-doc-deprecated.h"
#include "rtfm-gir-annotation.h"
#include "rtfm-gir-callback.h"
#include "rtfm-gir-type.h"
#include "rtfm-gir-array.h"

struct _RtfmGirField
{
  GObject parent_instance;
  const gchar *introspectable;
  const gchar *deprecated;
  const gchar *deprecated_version;
  const gchar *version;
  const gchar *stability;
  const gchar *name;
  const gchar *writable;
  const gchar *readable;
  const gchar *private;
  const gchar *bits;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirField, rtfm_gir_field, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_INTROSPECTABLE,
  PROP_DEPRECATED,
  PROP_DEPRECATED_VERSION,
  PROP_VERSION,
  PROP_STABILITY,
  PROP_NAME,
  PROP_WRITABLE,
  PROP_READABLE,
  PROP_PRIVATE,
  PROP_BITS,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_field_get_children (RtfmGirParserObject *object)
{
  RtfmGirField *self = (RtfmGirField *)object;

  g_assert (RTFM_GIR_IS_FIELD (self));

  return self->children;
}

static void
rtfm_gir_field_start_element (GMarkupParseContext *context,
                              const gchar *element_name,
                              const gchar **attribute_names,
                              const gchar **attribute_values,
                              gpointer user_data,
                              GError **error)
{
  RtfmGirField *self = user_data;
  RtfmGirParserContext *parser_context;

  g_assert (RTFM_GIR_IS_FIELD (self));
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
  else if (g_str_equal (element_name, "callback"))
    {
      g_autoptr(RtfmGirCallback) child = NULL;

      child = rtfm_gir_callback_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "type"))
    {
      g_autoptr(RtfmGirType) child = NULL;

      child = rtfm_gir_type_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "array"))
    {
      g_autoptr(RtfmGirArray) child = NULL;

      child = rtfm_gir_array_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_field_end_element (GMarkupParseContext *context,
                            const gchar *element_name,
                            gpointer user_data,
                            GError **error)
{
  g_assert (RTFM_GIR_IS_FIELD (user_data));
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
  else if (g_str_equal (element_name, "callback"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "type"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "array"))
    {
      g_markup_parse_context_pop (context);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_field_start_element,
  rtfm_gir_field_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_field_ingest (RtfmGirParserObject *object,
                       GMarkupParseContext *context,
                       const gchar *element_name,
                       const gchar **attribute_names,
                       const gchar **attribute_values,
                       GError **error)
{
  RtfmGirField *self = (RtfmGirField *)object;
  RtfmGirParserContext *parser_context;
  const gchar *introspectable = NULL;
  const gchar *deprecated = NULL;
  const gchar *deprecated_version = NULL;
  const gchar *version = NULL;
  const gchar *stability = NULL;
  const gchar *name = NULL;
  const gchar *writable = NULL;
  const gchar *readable = NULL;
  const gchar *private = NULL;
  const gchar *bits = NULL;

  g_assert (RTFM_GIR_IS_FIELD (self));
  g_assert (g_str_equal (element_name, "field"));

  parser_context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));


  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "introspectable", &introspectable,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "deprecated", &deprecated,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "deprecated-version", &deprecated_version,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "version", &version,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "stability", &stability,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "writable", &writable,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "readable", &readable,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "private", &private,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "bits", &bits,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  self->introspectable = rtfm_gir_parser_context_intern_string (parser_context, introspectable);
  self->deprecated = rtfm_gir_parser_context_intern_string (parser_context, deprecated);
  self->deprecated_version = rtfm_gir_parser_context_intern_string (parser_context, deprecated_version);
  self->version = rtfm_gir_parser_context_intern_string (parser_context, version);
  self->stability = rtfm_gir_parser_context_intern_string (parser_context, stability);
  self->name = rtfm_gir_parser_context_intern_string (parser_context, name);
  self->writable = rtfm_gir_parser_context_intern_string (parser_context, writable);
  self->readable = rtfm_gir_parser_context_intern_string (parser_context, readable);
  self->private = rtfm_gir_parser_context_intern_string (parser_context, private);
  self->bits = rtfm_gir_parser_context_intern_string (parser_context, bits);

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_field_printf (RtfmGirParserObject *object,
                       GString *str,
                       guint depth)
{
  RtfmGirField *self = (RtfmGirField *)object;
  guint i;

  g_assert (RTFM_GIR_IS_FIELD (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<field");

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
  if (self->writable != NULL)
    g_string_append_printf (str, " writable=\"%s\"", self->writable);
  if (self->readable != NULL)
    g_string_append_printf (str, " readable=\"%s\"", self->readable);
  if (self->private != NULL)
    g_string_append_printf (str, " private=\"%s\"", self->private);
  if (self->bits != NULL)
    g_string_append_printf (str, " bits=\"%s\"", self->bits);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</field>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_field_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  RtfmGirField *self = (RtfmGirField *)object;

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

    case PROP_WRITABLE:
      g_value_set_string (value, self->writable);
      break;

    case PROP_READABLE:
      g_value_set_string (value, self->readable);
      break;

    case PROP_PRIVATE:
      g_value_set_string (value, self->private);
      break;

    case PROP_BITS:
      g_value_set_string (value, self->bits);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_field_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  RtfmGirField *self = (RtfmGirField *)object;
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

    case PROP_WRITABLE:
      self->writable = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_READABLE:
      self->readable = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_PRIVATE:
      self->private = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_BITS:
      self->bits = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_field_finalize (GObject *object)
{
  RtfmGirField *self = (RtfmGirField *)object;

  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_field_parent_class)->finalize (object);
}

static void
rtfm_gir_field_class_init (RtfmGirFieldClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_field_get_property;
  object_class->set_property = rtfm_gir_field_set_property;
  object_class->finalize = rtfm_gir_field_finalize;

  parent_class->ingest = rtfm_gir_field_ingest;
  parent_class->printf = rtfm_gir_field_printf;
  parent_class->get_children = rtfm_gir_field_get_children;

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

  properties [PROP_WRITABLE] =
    g_param_spec_string ("writable",
                         "writable",
                         "writable",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_READABLE] =
    g_param_spec_string ("readable",
                         "readable",
                         "readable",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_PRIVATE] =
    g_param_spec_string ("private",
                         "private",
                         "private",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_BITS] =
    g_param_spec_string ("bits",
                         "bits",
                         "bits",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_field_init (RtfmGirField *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_field_get_introspectable (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FIELD (self), NULL);

  return self->introspectable;
}

const gchar *
rtfm_gir_field_get_deprecated (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FIELD (self), NULL);

  return self->deprecated;
}

const gchar *
rtfm_gir_field_get_deprecated_version (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FIELD (self), NULL);

  return self->deprecated_version;
}

const gchar *
rtfm_gir_field_get_version (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FIELD (self), NULL);

  return self->version;
}

const gchar *
rtfm_gir_field_get_stability (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FIELD (self), NULL);

  return self->stability;
}

const gchar *
rtfm_gir_field_get_name (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FIELD (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_field_get_writable (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FIELD (self), NULL);

  return self->writable;
}

const gchar *
rtfm_gir_field_get_readable (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FIELD (self), NULL);

  return self->readable;
}

const gchar *
rtfm_gir_field_get_private (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FIELD (self), NULL);

  return self->private;
}

const gchar *
rtfm_gir_field_get_bits (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FIELD (self), NULL);

  return self->bits;
}

RtfmGirField *
rtfm_gir_field_new (RtfmGirParserContext *parser_context)
{
  return g_object_new (RTFM_GIR_TYPE_FIELD,
                       "parser-context", parser_context,
                       NULL);
}
