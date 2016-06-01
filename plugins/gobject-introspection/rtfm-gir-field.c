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
#include "rtfm-gir-type.h"

struct _RtfmGirField
{
  RtfmGirBase base;
  gchar *name;
  gchar *writable;
  gchar *doc;
  RtfmGirType *type;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_WRITABLE,
  PROP_DOC,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirField, rtfm_gir_field, RTFM_TYPE_GIR_BASE)

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_field_ingest (RtfmGirBase       *base,
                       xmlTextReaderPtr   reader,
                       GError           **error);

static void
rtfm_gir_field_finalize (GObject *object)
{
  RtfmGirField *self = (RtfmGirField *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->writable, g_free);
  g_clear_pointer (&self->doc, g_free);

  G_OBJECT_CLASS (rtfm_gir_field_parent_class)->finalize (object);
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
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_WRITABLE:
      g_value_set_string (value, self->writable);
      break;

    case PROP_DOC:
      g_value_set_string (value, self->doc);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_field_set_property (GObject       *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  RtfmGirField *self = (RtfmGirField *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_WRITABLE:
      g_free (self->writable);
      self->writable = g_value_dup_string (value);
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
rtfm_gir_field_class_init (RtfmGirFieldClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_field_finalize;
  object_class->get_property = rtfm_gir_field_get_property;
  object_class->set_property = rtfm_gir_field_set_property;

  base_class->ingest = rtfm_gir_field_ingest;

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

  properties [PROP_DOC] =
    g_param_spec_string ("doc",
                         "doc",
                         "doc",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_field_init (RtfmGirField *self)
{
}

static gboolean
rtfm_gir_field_ingest (RtfmGirBase       *base,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  RtfmGirField *self = (RtfmGirField *)base;
  xmlChar *name;
  xmlChar *writable;

  g_assert (RTFM_IS_GIR_FIELD (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");
  writable = xmlTextReaderGetAttribute (reader, (const xmlChar *)"writable");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);
  self->writable = g_strdup ((gchar *)writable);

  /* Free libxml allocated strings */
  xmlFree (name);
  xmlFree (writable);

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
 * rtfm_gir_field_get_referenced:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirType or %NULL.
 */
RtfmGirType *
rtfm_gir_field_get_referenced (RtfmGirField *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_FIELD (self), NULL);

  return self->type;
}
