/* rtfm-gir-implements.c
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

#define G_LOG_DOMAIN "rtfm-gir-implements"

#include "rtfm-gir-implements.h"

struct _RtfmGirImplements
{
  GObject parent_instance;
  const gchar *name;
};

G_DEFINE_TYPE (RtfmGirImplements, rtfm_gir_implements, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_NAME,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_implements_ingest (RtfmGirParserObject *object,
                            GMarkupParseContext *context,
                            const gchar *element_name,
                            const gchar **attribute_names,
                            const gchar **attribute_values,
                            GError **error)
{
  RtfmGirImplements *self = (RtfmGirImplements *)object;
  RtfmGirParserContext *parser_context;
  const gchar *name = NULL;

  g_assert (RTFM_GIR_IS_IMPLEMENTS (self));
  g_assert (g_str_equal (element_name, "implements"));

  parser_context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));


  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  self->name = rtfm_gir_parser_context_intern_string (parser_context, name);

  return TRUE;
}

static void
rtfm_gir_implements_printf (RtfmGirParserObject *object,
                            GString *str,
                            guint depth)
{
  RtfmGirImplements *self = (RtfmGirImplements *)object;
  guint i;

  g_assert (RTFM_GIR_IS_IMPLEMENTS (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<implements");

  if (self->name != NULL)
    g_string_append_printf (str, " name=\"%s\"", self->name);

  g_string_append (str, "/>\n");
}

static void
rtfm_gir_implements_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  RtfmGirImplements *self = (RtfmGirImplements *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_implements_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  RtfmGirImplements *self = (RtfmGirImplements *)object;
  RtfmGirParserContext *context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));

  switch (prop_id)
    {
    case PROP_NAME:
      self->name = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_implements_finalize (GObject *object)
{

  G_OBJECT_CLASS (rtfm_gir_implements_parent_class)->finalize (object);
}

static void
rtfm_gir_implements_class_init (RtfmGirImplementsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_implements_get_property;
  object_class->set_property = rtfm_gir_implements_set_property;
  object_class->finalize = rtfm_gir_implements_finalize;

  parent_class->ingest = rtfm_gir_implements_ingest;
  parent_class->printf = rtfm_gir_implements_printf;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_implements_init (RtfmGirImplements *self)
{
}

const gchar *
rtfm_gir_implements_get_name (RtfmGirImplements *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_IMPLEMENTS (self), NULL);

  return self->name;
}

RtfmGirImplements *
rtfm_gir_implements_new (RtfmGirParserContext *parser_context)
{
  return g_object_new (RTFM_GIR_TYPE_IMPLEMENTS,
                       "parser-context", parser_context,
                       NULL);
}
