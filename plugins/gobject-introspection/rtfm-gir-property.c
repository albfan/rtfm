/* rtfm-gir-property.c
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

#define G_LOG_DOMAIN "rtfm-gir-property"

#include "rtfm-gir-property.h"
#include "rtfm-gir-type.h"

struct _RtfmGirProperty
{
  RtfmGirBase base;
  gchar *name;
  gchar *version;
  gchar *writable;
  gchar *construct_only;
  gchar *transfer_ownership;
  gchar *doc;
  RtfmGirType *type;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_VERSION,
  PROP_WRITABLE,
  PROP_CONSTRUCT_ONLY,
  PROP_TRANSFER_OWNERSHIP,
  PROP_DOC,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirProperty, rtfm_gir_property, RTFM_TYPE_GIR_BASE)

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_property_ingest (RtfmGirBase       *base,
                          xmlTextReaderPtr   reader,
                          GError           **error);

static void
rtfm_gir_property_finalize (GObject *object)
{
  RtfmGirProperty *self = (RtfmGirProperty *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->writable, g_free);
  g_clear_pointer (&self->construct_only, g_free);
  g_clear_pointer (&self->transfer_ownership, g_free);
  g_clear_pointer (&self->doc, g_free);

  G_OBJECT_CLASS (rtfm_gir_property_parent_class)->finalize (object);
}

static void
rtfm_gir_property_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  RtfmGirProperty *self = (RtfmGirProperty *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_WRITABLE:
      g_value_set_string (value, self->writable);
      break;

    case PROP_CONSTRUCT_ONLY:
      g_value_set_string (value, self->construct_only);
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
rtfm_gir_property_set_property (GObject       *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  RtfmGirProperty *self = (RtfmGirProperty *)object;

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

    case PROP_WRITABLE:
      g_free (self->writable);
      self->writable = g_value_dup_string (value);
      break;

    case PROP_CONSTRUCT_ONLY:
      g_free (self->construct_only);
      self->construct_only = g_value_dup_string (value);
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
rtfm_gir_property_class_init (RtfmGirPropertyClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_property_finalize;
  object_class->get_property = rtfm_gir_property_get_property;
  object_class->set_property = rtfm_gir_property_set_property;

  base_class->ingest = rtfm_gir_property_ingest;

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

  properties [PROP_WRITABLE] =
    g_param_spec_string ("writable",
                         "writable",
                         "writable",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_CONSTRUCT_ONLY] =
    g_param_spec_string ("construct-only",
                         "construct-only",
                         "construct-only",
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
rtfm_gir_property_init (RtfmGirProperty *self)
{
}

static gboolean
rtfm_gir_property_ingest (RtfmGirBase       *base,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  RtfmGirProperty *self = (RtfmGirProperty *)base;
  xmlChar *name;
  xmlChar *version;
  xmlChar *writable;
  xmlChar *construct_only;
  xmlChar *transfer_ownership;

  g_assert (RTFM_IS_GIR_PROPERTY (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");
  version = xmlTextReaderGetAttribute (reader, (const xmlChar *)"version");
  writable = xmlTextReaderGetAttribute (reader, (const xmlChar *)"writable");
  construct_only = xmlTextReaderGetAttribute (reader, (const xmlChar *)"construct-only");
  transfer_ownership = xmlTextReaderGetAttribute (reader, (const xmlChar *)"transfer-ownership");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);
  self->version = g_strdup ((gchar *)version);
  self->writable = g_strdup ((gchar *)writable);
  self->construct_only = g_strdup ((gchar *)construct_only);
  self->transfer_ownership = g_strdup ((gchar *)transfer_ownership);

  /* Free libxml allocated strings */
  xmlFree (name);
  xmlFree (version);
  xmlFree (writable);
  xmlFree (construct_only);
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

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (type), reader, error))
            return FALSE;

          g_set_object (&self->type, type);
        }
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
}

/**
 * rtfm_gir_property_get_referenced:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirType or %NULL.
 */
RtfmGirType *
rtfm_gir_property_get_referenced (RtfmGirProperty *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_PROPERTY (self), NULL);

  return self->type;
}
