/* rtfm-gir-include.c
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

#define G_LOG_DOMAIN "rtfm-gir-include"

#include "rtfm-gir-include.h"

struct _RtfmGirInclude
{
  GObject parent_instance;
  gchar *name;
  gchar *version;
};

G_DEFINE_TYPE (RtfmGirInclude, rtfm_gir_include, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_NAME,
  PROP_VERSION,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_include_ingest (RtfmGirParserObject *object,
                         GMarkupParseContext *context,
                         const gchar *element_name,
                         const gchar **attribute_names,
                         const gchar **attribute_values,
                         GError **error)
{
  RtfmGirInclude *self = (RtfmGirInclude *)object;

  g_assert (RTFM_GIR_IS_INCLUDE (self));
  g_assert (g_str_equal (element_name, "include"));

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->version, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "version", &self->version,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  return TRUE;
}

static void
rtfm_gir_include_printf (RtfmGirParserObject *object,
                         GString *str,
                         guint depth)
{
  RtfmGirInclude *self = (RtfmGirInclude *)object;
  guint i;

  g_assert (RTFM_GIR_IS_INCLUDE (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<include");

  if (self->name != NULL)
    g_string_append_printf (str, " name=\"%s\"", self->name);
  if (self->version != NULL)
    g_string_append_printf (str, " version=\"%s\"", self->version);

  g_string_append (str, "/>\n");
}

static void
rtfm_gir_include_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  RtfmGirInclude *self = (RtfmGirInclude *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_include_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  RtfmGirInclude *self = (RtfmGirInclude *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_include_finalize (GObject *object)
{
  RtfmGirInclude *self = (RtfmGirInclude *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->version, g_free);

  G_OBJECT_CLASS (rtfm_gir_include_parent_class)->finalize (object);
}

static void
rtfm_gir_include_class_init (RtfmGirIncludeClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_include_get_property;
  object_class->set_property = rtfm_gir_include_set_property;
  object_class->finalize = rtfm_gir_include_finalize;

  parent_class->ingest = rtfm_gir_include_ingest;
  parent_class->printf = rtfm_gir_include_printf;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_include_init (RtfmGirInclude *self)
{
}

const gchar *
rtfm_gir_include_get_name (RtfmGirInclude *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_INCLUDE (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_include_get_version (RtfmGirInclude *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_INCLUDE (self), NULL);

  return self->version;
}

RtfmGirInclude *
rtfm_gir_include_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_INCLUDE, NULL);
}
