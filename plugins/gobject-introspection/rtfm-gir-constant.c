/* rtfm-gir-constant.c
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

#define G_LOG_DOMAIN "rtfm-gir-constant"

#include "rtfm-gir-constant.h"
#include "rtfm-gir-type.h"

struct _RtfmGirConstant
{
  RtfmGirBase base;
  gchar *name;
  gchar *value;
  gchar *c_type;
  gchar *doc;
  RtfmGirType *type;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_VALUE,
  PROP_C_TYPE,
  PROP_DOC,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirConstant, rtfm_gir_constant, RTFM_TYPE_GIR_BASE)

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_constant_ingest (RtfmGirBase       *base,
                          xmlTextReaderPtr   reader,
                          GError           **error);

static void
rtfm_gir_constant_finalize (GObject *object)
{
  RtfmGirConstant *self = (RtfmGirConstant *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->value, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_clear_pointer (&self->doc, g_free);

  G_OBJECT_CLASS (rtfm_gir_constant_parent_class)->finalize (object);
}

static void
rtfm_gir_constant_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  RtfmGirConstant *self = (RtfmGirConstant *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_VALUE:
      g_value_set_string (value, self->value);
      break;

    case PROP_C_TYPE:
      g_value_set_string (value, self->c_type);
      break;

    case PROP_DOC:
      g_value_set_string (value, self->doc);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_constant_set_property (GObject       *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  RtfmGirConstant *self = (RtfmGirConstant *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_VALUE:
      g_free (self->value);
      self->value = g_value_dup_string (value);
      break;

    case PROP_C_TYPE:
      g_free (self->c_type);
      self->c_type = g_value_dup_string (value);
      break;

    case PROP_DOC:
      g_free (self->doc);
      self->doc = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_constant_class_init (RtfmGirConstantClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_constant_finalize;
  object_class->get_property = rtfm_gir_constant_get_property;
  object_class->set_property = rtfm_gir_constant_set_property;

  base_class->ingest = rtfm_gir_constant_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VALUE] =
    g_param_spec_string ("value",
                         "value",
                         "value",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_TYPE] =
    g_param_spec_string ("c-type",
                         "c-type",
                         "c-type",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DOC] =
    g_param_spec_string ("doc",
                         "doc",
                         "doc",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_constant_init (RtfmGirConstant *self)
{
}

static gboolean
rtfm_gir_constant_ingest (RtfmGirBase       *base,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  RtfmGirConstant *self = (RtfmGirConstant *)base;
  xmlChar *name;
  xmlChar *value;
  xmlChar *c_type;

  g_assert (RTFM_IS_GIR_CONSTANT (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");
  value = xmlTextReaderGetAttribute (reader, (const xmlChar *)"value");
  c_type = xmlTextReaderGetAttribute (reader, (const xmlChar *)"c:type");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);
  self->value = g_strdup ((gchar *)value);
  self->c_type = g_strdup ((gchar *)c_type);

  /* Free libxml allocated strings */
  xmlFree (name);
  xmlFree (value);
  xmlFree (c_type);

  if (xmlTextReaderRead (reader) != 1)
    return FALSE;

  while (xmlTextReaderNodeType (reader) != XML_ELEMENT_NODE)
    {
      if (xmlTextReaderNext (reader) != 1)
        return FALSE;
    }

  do
    {
      const gchar *element_name;

      if (xmlTextReaderNodeType (reader) != XML_ELEMENT_NODE)
        continue;

      element_name = (const gchar *)xmlTextReaderConstName (reader);

      if (FALSE) { }
      else if (g_strcmp0 (element_name, "doc") == 0)
        {
          xmlChar *doc;

          doc = xmlTextReaderReadString (reader);

          g_clear_pointer (&self->doc, g_free);
          self->doc = g_strdup ((gchar *)doc);

          xmlFree (doc);
        }
      else if (g_strcmp0 (element_name, "type") == 0)
        {
          g_autoptr(RtfmGirType) type = NULL;

          type = g_object_new (RTFM_TYPE_GIR_TYPE, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (type), reader, error))
            return FALSE;

          g_set_object (&self->type, type);
        }
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
}

/**
 * rtfm_gir_constant_get_referenced:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirType or %NULL.
 */
RtfmGirType *
rtfm_gir_constant_get_referenced (RtfmGirConstant *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CONSTANT (self), NULL);

  return self->type;
}
