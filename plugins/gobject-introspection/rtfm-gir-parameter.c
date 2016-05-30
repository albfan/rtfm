/* rtfm-gir-parameter.c
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

#define G_LOG_DOMAIN "rtfm-gir-parameter"

#include "rtfm-gir-parameter.h"
#include "rtfm-gir-type.h"

struct _RtfmGirParameter
{
  RtfmItem parent_instance;
  gchar *name;
  gchar *transfer_ownership;
  gchar *doc;
  RtfmGirType *type;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_TRANSFER_OWNERSHIP,
  PROP_DOC,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirParameter, rtfm_gir_parameter, RTFM_TYPE_ITEM)

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_parameter_finalize (GObject *object)
{
  RtfmGirParameter *self = (RtfmGirParameter *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->transfer_ownership, g_free);
  g_clear_pointer (&self->doc, g_free);

  G_OBJECT_CLASS (rtfm_gir_parameter_parent_class)->finalize (object);
}

static void
rtfm_gir_parameter_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  RtfmGirParameter *self = (RtfmGirParameter *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_TRANSFER_OWNERSHIP:
      g_value_set_string (value, self->transfer_ownership);
      break;

    case PROP_DOC:
      g_value_set_string (value, self->doc);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_parameter_set_property (GObject       *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  RtfmGirParameter *self = (RtfmGirParameter *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_TRANSFER_OWNERSHIP:
      g_free (self->transfer_ownership);
      self->transfer_ownership = g_value_dup_string (value);
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
rtfm_gir_parameter_class_init (RtfmGirParameterClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_parameter_finalize;
  object_class->get_property = rtfm_gir_parameter_get_property;
  object_class->set_property = rtfm_gir_parameter_set_property;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_TRANSFER_OWNERSHIP] =
    g_param_spec_string ("transfer-ownership",
                         "transfer-ownership",
                         "transfer-ownership",
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
rtfm_gir_parameter_init (RtfmGirParameter *self)
{
}

gboolean
rtfm_gir_parameter_ingest (RtfmGirParameter   *self,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  xmlChar *name;
  xmlChar *transfer_ownership;

  g_assert (RTFM_IS_GIR_PARAMETER (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");
  transfer_ownership = xmlTextReaderGetAttribute (reader, (const xmlChar *)"transfer-ownership");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);
  self->transfer_ownership = g_strdup ((gchar *)transfer_ownership);

  /* Free libxml allocated strings */
  xmlFree (name);
  xmlFree (transfer_ownership);

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

          if (!rtfm_gir_type_ingest (type, reader, error))
            return FALSE;

          g_set_object (&self->type, type);
        }
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
}
