/* rtfm-gir-array.c
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

#define G_LOG_DOMAIN "rtfm-gir-array"

#include "rtfm-gir-array.h"

#include "rtfm-gir-type.h"
#include "rtfm-gir-array.h"

struct _RtfmGirArray
{
  GObject parent_instance;
  const gchar *name;
  const gchar *zero_terminated;
  const gchar *fixed_size;
  const gchar *introspectable;
  const gchar *length;
  const gchar *c_type;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirArray, rtfm_gir_array, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_NAME,
  PROP_ZERO_TERMINATED,
  PROP_FIXED_SIZE,
  PROP_INTROSPECTABLE,
  PROP_LENGTH,
  PROP_C_TYPE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_array_get_children (RtfmGirParserObject *object)
{
  RtfmGirArray *self = (RtfmGirArray *)object;

  g_assert (RTFM_GIR_IS_ARRAY (self));

  return self->children;
}

static void
rtfm_gir_array_start_element (GMarkupParseContext *context,
                              const gchar *element_name,
                              const gchar **attribute_names,
                              const gchar **attribute_values,
                              gpointer user_data,
                              GError **error)
{
  RtfmGirArray *self = user_data;
  RtfmGirParserContext *parser_context;

  g_assert (RTFM_GIR_IS_ARRAY (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  parser_context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));

  if (FALSE) {}
  else if (g_str_equal (element_name, "type"))
    {
      g_autoptr(RtfmGirType) child = NULL;

      child = rtfm_gir_type_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "array"))
    {
      g_autoptr(RtfmGirArray) child = NULL;

      child = rtfm_gir_array_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_array_end_element (GMarkupParseContext *context,
                            const gchar *element_name,
                            gpointer user_data,
                            GError **error)
{
  g_assert (RTFM_GIR_IS_ARRAY (user_data));
  g_assert (context != NULL);
  g_assert (element_name != NULL);

  if (FALSE) {}
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
  rtfm_gir_array_start_element,
  rtfm_gir_array_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_array_ingest (RtfmGirParserObject *object,
                       GMarkupParseContext *context,
                       const gchar *element_name,
                       const gchar **attribute_names,
                       const gchar **attribute_values,
                       GError **error)
{
  RtfmGirArray *self = (RtfmGirArray *)object;
  RtfmGirParserContext *parser_context;
  const gchar *name = NULL;
  const gchar *zero_terminated = NULL;
  const gchar *fixed_size = NULL;
  const gchar *introspectable = NULL;
  const gchar *length = NULL;
  const gchar *c_type = NULL;

  g_assert (RTFM_GIR_IS_ARRAY (self));
  g_assert (g_str_equal (element_name, "array"));

  parser_context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));


  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "zero-terminated", &zero_terminated,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "fixed-size", &fixed_size,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "introspectable", &introspectable,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "length", &length,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "c:type", &c_type,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  self->name = rtfm_gir_parser_context_intern_string (parser_context, name);
  self->zero_terminated = rtfm_gir_parser_context_intern_string (parser_context, zero_terminated);
  self->fixed_size = rtfm_gir_parser_context_intern_string (parser_context, fixed_size);
  self->introspectable = rtfm_gir_parser_context_intern_string (parser_context, introspectable);
  self->length = rtfm_gir_parser_context_intern_string (parser_context, length);
  self->c_type = rtfm_gir_parser_context_intern_string (parser_context, c_type);

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_array_printf (RtfmGirParserObject *object,
                       GString *str,
                       guint depth)
{
  RtfmGirArray *self = (RtfmGirArray *)object;
  guint i;

  g_assert (RTFM_GIR_IS_ARRAY (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<array");

  if (self->name != NULL)
    g_string_append_printf (str, " name=\"%s\"", self->name);
  if (self->zero_terminated != NULL)
    g_string_append_printf (str, " zero-terminated=\"%s\"", self->zero_terminated);
  if (self->fixed_size != NULL)
    g_string_append_printf (str, " fixed-size=\"%s\"", self->fixed_size);
  if (self->introspectable != NULL)
    g_string_append_printf (str, " introspectable=\"%s\"", self->introspectable);
  if (self->length != NULL)
    g_string_append_printf (str, " length=\"%s\"", self->length);
  if (self->c_type != NULL)
    g_string_append_printf (str, " c:type=\"%s\"", self->c_type);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</array>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_array_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  RtfmGirArray *self = (RtfmGirArray *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_ZERO_TERMINATED:
      g_value_set_string (value, self->zero_terminated);
      break;

    case PROP_FIXED_SIZE:
      g_value_set_string (value, self->fixed_size);
      break;

    case PROP_INTROSPECTABLE:
      g_value_set_string (value, self->introspectable);
      break;

    case PROP_LENGTH:
      g_value_set_string (value, self->length);
      break;

    case PROP_C_TYPE:
      g_value_set_string (value, self->c_type);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_array_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  RtfmGirArray *self = (RtfmGirArray *)object;
  RtfmGirParserContext *context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));

  switch (prop_id)
    {
    case PROP_NAME:
      self->name = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_ZERO_TERMINATED:
      self->zero_terminated = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_FIXED_SIZE:
      self->fixed_size = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_INTROSPECTABLE:
      self->introspectable = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_LENGTH:
      self->length = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_C_TYPE:
      self->c_type = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_array_finalize (GObject *object)
{
  RtfmGirArray *self = (RtfmGirArray *)object;

  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_array_parent_class)->finalize (object);
}

static void
rtfm_gir_array_class_init (RtfmGirArrayClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_array_get_property;
  object_class->set_property = rtfm_gir_array_set_property;
  object_class->finalize = rtfm_gir_array_finalize;

  parent_class->ingest = rtfm_gir_array_ingest;
  parent_class->printf = rtfm_gir_array_printf;
  parent_class->get_children = rtfm_gir_array_get_children;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_ZERO_TERMINATED] =
    g_param_spec_string ("zero-terminated",
                         "zero-terminated",
                         "zero-terminated",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_FIXED_SIZE] =
    g_param_spec_string ("fixed-size",
                         "fixed-size",
                         "fixed-size",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_INTROSPECTABLE] =
    g_param_spec_string ("introspectable",
                         "introspectable",
                         "introspectable",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_LENGTH] =
    g_param_spec_string ("length",
                         "length",
                         "length",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_TYPE] =
    g_param_spec_string ("c-type",
                         "c-type",
                         "c-type",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_array_init (RtfmGirArray *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_array_get_name (RtfmGirArray *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_ARRAY (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_array_get_zero_terminated (RtfmGirArray *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_ARRAY (self), NULL);

  return self->zero_terminated;
}

const gchar *
rtfm_gir_array_get_fixed_size (RtfmGirArray *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_ARRAY (self), NULL);

  return self->fixed_size;
}

const gchar *
rtfm_gir_array_get_introspectable (RtfmGirArray *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_ARRAY (self), NULL);

  return self->introspectable;
}

const gchar *
rtfm_gir_array_get_length (RtfmGirArray *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_ARRAY (self), NULL);

  return self->length;
}

const gchar *
rtfm_gir_array_get_c_type (RtfmGirArray *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_ARRAY (self), NULL);

  return self->c_type;
}

RtfmGirArray *
rtfm_gir_array_new (RtfmGirParserContext *parser_context)
{
  return g_object_new (RTFM_GIR_TYPE_ARRAY,
                       "parser-context", parser_context,
                       NULL);
}
