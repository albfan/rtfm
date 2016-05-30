/* rtfm-gir-bitfield.c
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

#define G_LOG_DOMAIN "rtfm-gir-bitfield"

#include "rtfm-gir-bitfield.h"
#include "rtfm-gir-member.h"

struct _RtfmGirBitfield
{
  RtfmItem parent_instance;
  gchar *name;
  gchar *version;
  gchar *glib_type_name;
  gchar *glib_get_type;
  gchar *c_type;
  gchar *doc;
  GPtrArray *member;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_VERSION,
  PROP_GLIB_TYPE_NAME,
  PROP_GLIB_GET_TYPE,
  PROP_C_TYPE,
  PROP_DOC,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirBitfield, rtfm_gir_bitfield, RTFM_TYPE_ITEM)

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_bitfield_finalize (GObject *object)
{
  RtfmGirBitfield *self = (RtfmGirBitfield *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->glib_type_name, g_free);
  g_clear_pointer (&self->glib_get_type, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_clear_pointer (&self->doc, g_free);
  g_clear_pointer (&self->doc, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_bitfield_parent_class)->finalize (object);
}

static void
rtfm_gir_bitfield_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  RtfmGirBitfield *self = (RtfmGirBitfield *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_GLIB_TYPE_NAME:
      g_value_set_string (value, self->glib_type_name);
      break;

    case PROP_GLIB_GET_TYPE:
      g_value_set_string (value, self->glib_get_type);
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
rtfm_gir_bitfield_set_property (GObject       *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  RtfmGirBitfield *self = (RtfmGirBitfield *)object;

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

    case PROP_GLIB_TYPE_NAME:
      g_free (self->glib_type_name);
      self->glib_type_name = g_value_dup_string (value);
      break;

    case PROP_GLIB_GET_TYPE:
      g_free (self->glib_get_type);
      self->glib_get_type = g_value_dup_string (value);
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
rtfm_gir_bitfield_class_init (RtfmGirBitfieldClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_bitfield_finalize;
  object_class->get_property = rtfm_gir_bitfield_get_property;
  object_class->set_property = rtfm_gir_bitfield_set_property;

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

  properties [PROP_GLIB_TYPE_NAME] =
    g_param_spec_string ("glib-type-name",
                         "glib-type-name",
                         "glib-type-name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_GET_TYPE] =
    g_param_spec_string ("glib-get-type",
                         "glib-get-type",
                         "glib-get-type",
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
rtfm_gir_bitfield_init (RtfmGirBitfield *self)
{
}

gboolean
rtfm_gir_bitfield_ingest (RtfmGirBitfield   *self,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  xmlChar *name;
  xmlChar *version;
  xmlChar *glib_type_name;
  xmlChar *glib_get_type;
  xmlChar *c_type;

  g_assert (RTFM_IS_GIR_BITFIELD (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");
  version = xmlTextReaderGetAttribute (reader, (const xmlChar *)"version");
  glib_type_name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"glib:type-name");
  glib_get_type = xmlTextReaderGetAttribute (reader, (const xmlChar *)"glib:get-type");
  c_type = xmlTextReaderGetAttribute (reader, (const xmlChar *)"c:type");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);
  self->version = g_strdup ((gchar *)version);
  self->glib_type_name = g_strdup ((gchar *)glib_type_name);
  self->glib_get_type = g_strdup ((gchar *)glib_get_type);
  self->c_type = g_strdup ((gchar *)c_type);

  /* Free libxml allocated strings */
  xmlFree (name);
  xmlFree (version);
  xmlFree (glib_type_name);
  xmlFree (glib_get_type);
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
      else if (g_strcmp0 (element_name, "member") == 0)
        {
          g_autoptr(RtfmGirMember) member = NULL;

          member = g_object_new (RTFM_TYPE_GIR_MEMBER, NULL);

          if (!rtfm_gir_member_ingest (member, reader, error))
            return FALSE;

          if (self->member == NULL)
            self->member = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->member, g_steal_pointer (&member));
        }
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
}
