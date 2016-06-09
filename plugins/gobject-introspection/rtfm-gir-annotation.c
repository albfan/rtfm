/* rtfm-gir-annotation.c
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

#define G_LOG_DOMAIN "rtfm-gir-annotation"

#include "rtfm-gir-annotation.h"

struct _RtfmGirAnnotation
{
  GObject parent_instance;
  gchar *key;
  gchar *value;
};

G_DEFINE_TYPE (RtfmGirAnnotation, rtfm_gir_annotation, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_KEY,
  PROP_VALUE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_annotation_ingest (RtfmGirParserObject *object,
                            GMarkupParseContext *context,
                            const gchar *element_name,
                            const gchar **attribute_names,
                            const gchar **attribute_values,
                            GError **error)
{
  RtfmGirAnnotation *self = (RtfmGirAnnotation *)object;

  g_assert (RTFM_GIR_IS_ANNOTATION (self));
  g_assert (g_str_equal (element_name, "annotation"));

  g_clear_pointer (&self->key, g_free);
  g_clear_pointer (&self->value, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "key", &self->key,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "value", &self->value,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  return TRUE;
}

static void
rtfm_gir_annotation_printf (RtfmGirParserObject *object,
                            GString *str,
                            guint depth)
{
  RtfmGirAnnotation *self = (RtfmGirAnnotation *)object;
  guint i;

  g_assert (RTFM_GIR_IS_ANNOTATION (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<annotation");

  if (self->key != NULL)
    g_string_append_printf (str, " key=\"%s\"", self->key);
  if (self->value != NULL)
    g_string_append_printf (str, " value=\"%s\"", self->value);

  g_string_append (str, "/>\n");
}

static void
rtfm_gir_annotation_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  RtfmGirAnnotation *self = (RtfmGirAnnotation *)object;

  switch (prop_id)
    {
    case PROP_KEY:
      g_value_set_string (value, self->key);
      break;

    case PROP_VALUE:
      g_value_set_string (value, self->value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_annotation_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  RtfmGirAnnotation *self = (RtfmGirAnnotation *)object;

  switch (prop_id)
    {
    case PROP_KEY:
      g_free (self->key);
      self->key = g_value_dup_string (value);
      break;

    case PROP_VALUE:
      g_free (self->value);
      self->value = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_annotation_finalize (GObject *object)
{
  RtfmGirAnnotation *self = (RtfmGirAnnotation *)object;

  g_clear_pointer (&self->key, g_free);
  g_clear_pointer (&self->value, g_free);

  G_OBJECT_CLASS (rtfm_gir_annotation_parent_class)->finalize (object);
}

static void
rtfm_gir_annotation_class_init (RtfmGirAnnotationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_annotation_get_property;
  object_class->set_property = rtfm_gir_annotation_set_property;
  object_class->finalize = rtfm_gir_annotation_finalize;

  parent_class->ingest = rtfm_gir_annotation_ingest;
  parent_class->printf = rtfm_gir_annotation_printf;

  properties [PROP_KEY] =
    g_param_spec_string ("key",
                         "key",
                         "key",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VALUE] =
    g_param_spec_string ("value",
                         "value",
                         "value",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
rtfm_gir_annotation_init (RtfmGirAnnotation *self)
{
}

const gchar *
rtfm_gir_annotation_get_key (RtfmGirAnnotation *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_ANNOTATION (self), NULL);

  return self->key;
}

const gchar *
rtfm_gir_annotation_get_value (RtfmGirAnnotation *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_ANNOTATION (self), NULL);

  return self->value;
}

RtfmGirAnnotation *
rtfm_gir_annotation_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_ANNOTATION, NULL);
}
