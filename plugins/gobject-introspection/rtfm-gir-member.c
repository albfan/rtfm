/* rtfm-gir-member.c
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

#define G_LOG_DOMAIN "rtfm-gir-member"

#include "rtfm-gir-member.h"

struct _RtfmGirMember
{
  RtfmItem parent_instance;
  gchar *name;
  gchar *value;
  gchar *c_identifier;
  gchar *doc;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_VALUE,
  PROP_C_IDENTIFIER,
  PROP_DOC,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirMember, rtfm_gir_member, RTFM_TYPE_ITEM)

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_member_finalize (GObject *object)
{
  RtfmGirMember *self = (RtfmGirMember *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->value, g_free);
  g_clear_pointer (&self->c_identifier, g_free);
  g_clear_pointer (&self->doc, g_free);

  G_OBJECT_CLASS (rtfm_gir_member_parent_class)->finalize (object);
}

static void
rtfm_gir_member_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  RtfmGirMember *self = (RtfmGirMember *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_VALUE:
      g_value_set_string (value, self->value);
      break;

    case PROP_C_IDENTIFIER:
      g_value_set_string (value, self->c_identifier);
      break;

    case PROP_DOC:
      g_value_set_string (value, self->doc);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_member_set_property (GObject       *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RtfmGirMember *self = (RtfmGirMember *)object;

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

    case PROP_C_IDENTIFIER:
      g_free (self->c_identifier);
      self->c_identifier = g_value_dup_string (value);
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
rtfm_gir_member_class_init (RtfmGirMemberClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_member_finalize;
  object_class->get_property = rtfm_gir_member_get_property;
  object_class->set_property = rtfm_gir_member_set_property;

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

  properties [PROP_C_IDENTIFIER] =
    g_param_spec_string ("c-identifier",
                         "c-identifier",
                         "c-identifier",
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
rtfm_gir_member_init (RtfmGirMember *self)
{
}

gboolean
rtfm_gir_member_ingest (RtfmGirMember   *self,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  xmlChar *name;
  xmlChar *value;
  xmlChar *c_identifier;

  g_assert (RTFM_IS_GIR_MEMBER (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");
  value = xmlTextReaderGetAttribute (reader, (const xmlChar *)"value");
  c_identifier = xmlTextReaderGetAttribute (reader, (const xmlChar *)"c:identifier");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);
  self->value = g_strdup ((gchar *)value);
  self->c_identifier = g_strdup ((gchar *)c_identifier);

  /* Free libxml allocated strings */
  xmlFree (name);
  xmlFree (value);
  xmlFree (c_identifier);

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
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
}
