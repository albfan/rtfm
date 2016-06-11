/* rtfm-gir-parameters.c
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

#define G_LOG_DOMAIN "rtfm-gir-parameters"

#include "rtfm-gir-parameters.h"

#include "rtfm-gir-parameter.h"
#include "rtfm-gir-instance-parameter.h"

struct _RtfmGirParameters
{
  GObject parent_instance;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirParameters, rtfm_gir_parameters, RTFM_GIR_TYPE_PARSER_OBJECT)

static GPtrArray *
rtfm_gir_parameters_get_children (RtfmGirParserObject *object)
{
  RtfmGirParameters *self = (RtfmGirParameters *)object;

  g_assert (RTFM_GIR_IS_PARAMETERS (self));

  return self->children;
}

static void
rtfm_gir_parameters_start_element (GMarkupParseContext *context,
                                   const gchar *element_name,
                                   const gchar **attribute_names,
                                   const gchar **attribute_values,
                                   gpointer user_data,
                                   GError **error)
{
  RtfmGirParameters *self = user_data;
  RtfmGirParserContext *parser_context;

  g_assert (RTFM_GIR_IS_PARAMETERS (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  parser_context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));

  if (FALSE) {}
  else if (g_str_equal (element_name, "parameter"))
    {
      g_autoptr(RtfmGirParameter) child = NULL;

      child = rtfm_gir_parameter_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "instance-parameter"))
    {
      g_autoptr(RtfmGirInstanceParameter) child = NULL;

      child = rtfm_gir_instance_parameter_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_parameters_end_element (GMarkupParseContext *context,
                                 const gchar *element_name,
                                 gpointer user_data,
                                 GError **error)
{
  g_assert (RTFM_GIR_IS_PARAMETERS (user_data));
  g_assert (context != NULL);
  g_assert (element_name != NULL);

  if (FALSE) {}
  else if (g_str_equal (element_name, "parameter"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "instance-parameter"))
    {
      g_markup_parse_context_pop (context);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_parameters_start_element,
  rtfm_gir_parameters_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_parameters_ingest (RtfmGirParserObject *object,
                            GMarkupParseContext *context,
                            const gchar *element_name,
                            const gchar **attribute_names,
                            const gchar **attribute_values,
                            GError **error)
{
  RtfmGirParameters *self = (RtfmGirParameters *)object;

  g_assert (RTFM_GIR_IS_PARAMETERS (self));
  g_assert (g_str_equal (element_name, "parameters"));


  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_parameters_printf (RtfmGirParserObject *object,
                            GString *str,
                            guint depth)
{
  RtfmGirParameters *self = (RtfmGirParameters *)object;
  guint i;

  g_assert (RTFM_GIR_IS_PARAMETERS (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<parameters");


  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</parameters>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_parameters_finalize (GObject *object)
{
  RtfmGirParameters *self = (RtfmGirParameters *)object;

  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_parameters_parent_class)->finalize (object);
}

static void
rtfm_gir_parameters_class_init (RtfmGirParametersClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_parameters_finalize;

  parent_class->ingest = rtfm_gir_parameters_ingest;
  parent_class->printf = rtfm_gir_parameters_printf;
  parent_class->get_children = rtfm_gir_parameters_get_children;
}

static void
rtfm_gir_parameters_init (RtfmGirParameters *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

RtfmGirParameters *
rtfm_gir_parameters_new (RtfmGirParserContext *parser_context)
{
  return g_object_new (RTFM_GIR_TYPE_PARAMETERS,
                       "parser-context", parser_context,
                       NULL);
}
