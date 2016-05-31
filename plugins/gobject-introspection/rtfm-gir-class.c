/* rtfm-gir-class.c
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

#define G_LOG_DOMAIN "rtfm-gir-class"

#include "rtfm-gir-class.h"
#include "rtfm-gir-method.h"
#include "rtfm-gir-property.h"
#include "rtfm-gir-bitfield.h"

struct _RtfmGirClass
{
  RtfmItem parent_instance;
  gchar *name;
  gchar *c_symbol_prefix;
  gchar *c_type;
  gchar *version;
  gchar *parent;
  gchar *glib_tpye_name;
  gchar *glib_get_type;
  gchar *doc;
  GPtrArray *method;
  GPtrArray *property;
  GPtrArray *bitfield;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_C_SYMBOL_PREFIX,
  PROP_C_TYPE,
  PROP_VERSION,
  PROP_PARENT,
  PROP_GLIB_TPYE_NAME,
  PROP_GLIB_GET_TYPE,
  PROP_DOC,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirClass, rtfm_gir_class, RTFM_TYPE_ITEM)

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_class_finalize (GObject *object)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_symbol_prefix, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->parent, g_free);
  g_clear_pointer (&self->glib_tpye_name, g_free);
  g_clear_pointer (&self->glib_get_type, g_free);
  g_clear_pointer (&self->doc, g_free);
  g_clear_pointer (&self->doc, g_ptr_array_unref);
  g_clear_pointer (&self->doc, g_ptr_array_unref);
  g_clear_pointer (&self->doc, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_class_parent_class)->finalize (object);
}

static void
rtfm_gir_class_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_C_SYMBOL_PREFIX:
      g_value_set_string (value, self->c_symbol_prefix);
      break;

    case PROP_C_TYPE:
      g_value_set_string (value, self->c_type);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_PARENT:
      g_value_set_string (value, self->parent);
      break;

    case PROP_GLIB_TPYE_NAME:
      g_value_set_string (value, self->glib_tpye_name);
      break;

    case PROP_GLIB_GET_TYPE:
      g_value_set_string (value, self->glib_get_type);
      break;

    case PROP_DOC:
      g_value_set_string (value, self->doc);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_class_set_property (GObject       *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_C_SYMBOL_PREFIX:
      g_free (self->c_symbol_prefix);
      self->c_symbol_prefix = g_value_dup_string (value);
      break;

    case PROP_C_TYPE:
      g_free (self->c_type);
      self->c_type = g_value_dup_string (value);
      break;

    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
      break;

    case PROP_PARENT:
      g_free (self->parent);
      self->parent = g_value_dup_string (value);
      break;

    case PROP_GLIB_TPYE_NAME:
      g_free (self->glib_tpye_name);
      self->glib_tpye_name = g_value_dup_string (value);
      break;

    case PROP_GLIB_GET_TYPE:
      g_free (self->glib_get_type);
      self->glib_get_type = g_value_dup_string (value);
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
rtfm_gir_class_class_init (RtfmGirClassClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_class_finalize;
  object_class->get_property = rtfm_gir_class_get_property;
  object_class->set_property = rtfm_gir_class_set_property;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_SYMBOL_PREFIX] =
    g_param_spec_string ("c-symbol-prefix",
                         "c-symbol-prefix",
                         "c-symbol-prefix",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_TYPE] =
    g_param_spec_string ("c-type",
                         "c-type",
                         "c-type",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_PARENT] =
    g_param_spec_string ("parent",
                         "parent",
                         "parent",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_TPYE_NAME] =
    g_param_spec_string ("glib-tpye-name",
                         "glib-tpye-name",
                         "glib-tpye-name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_GET_TYPE] =
    g_param_spec_string ("glib-get-type",
                         "glib-get-type",
                         "glib-get-type",
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
rtfm_gir_class_init (RtfmGirClass *self)
{
}

gboolean
rtfm_gir_class_ingest (RtfmGirClass      *self,
                       xmlTextReaderPtr   reader,
                       GError           **error)
{
  xmlChar *name;
  xmlChar *c_symbol_prefix;
  xmlChar *c_type;
  xmlChar *version;
  xmlChar *parent;
  xmlChar *glib_tpye_name;
  xmlChar *glib_get_type;

  g_assert (RTFM_IS_GIR_CLASS (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");
  c_symbol_prefix = xmlTextReaderGetAttribute (reader, (const xmlChar *)"c:symbol-prefix");
  c_type = xmlTextReaderGetAttribute (reader, (const xmlChar *)"c:type");
  version = xmlTextReaderGetAttribute (reader, (const xmlChar *)"version");
  parent = xmlTextReaderGetAttribute (reader, (const xmlChar *)"parent");
  glib_tpye_name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"glib:tpye-name");
  glib_get_type = xmlTextReaderGetAttribute (reader, (const xmlChar *)"glib:get-type");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);
  self->c_symbol_prefix = g_strdup ((gchar *)c_symbol_prefix);
  self->c_type = g_strdup ((gchar *)c_type);
  self->version = g_strdup ((gchar *)version);
  self->parent = g_strdup ((gchar *)parent);
  self->glib_tpye_name = g_strdup ((gchar *)glib_tpye_name);
  self->glib_get_type = g_strdup ((gchar *)glib_get_type);

  /* Free libxml allocated strings */
  xmlFree (name);
  xmlFree (c_symbol_prefix);
  xmlFree (c_type);
  xmlFree (version);
  xmlFree (parent);
  xmlFree (glib_tpye_name);
  xmlFree (glib_get_type);

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
      else if (g_strcmp0 (element_name, "method") == 0)
        {
          g_autoptr(RtfmGirMethod) method = NULL;

          method = g_object_new (RTFM_TYPE_GIR_METHOD, NULL);

          if (!rtfm_gir_method_ingest (method, reader, error))
            return FALSE;

          if (self->method == NULL)
            self->method = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->method, g_steal_pointer (&method));
        }
      else if (g_strcmp0 (element_name, "property") == 0)
        {
          g_autoptr(RtfmGirProperty) property = NULL;

          property = g_object_new (RTFM_TYPE_GIR_PROPERTY, NULL);

          if (!rtfm_gir_property_ingest (property, reader, error))
            return FALSE;

          if (self->property == NULL)
            self->property = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->property, g_steal_pointer (&property));
        }
      else if (g_strcmp0 (element_name, "bitfield") == 0)
        {
          g_autoptr(RtfmGirBitfield) bitfield = NULL;

          bitfield = g_object_new (RTFM_TYPE_GIR_BITFIELD, NULL);

          if (!rtfm_gir_bitfield_ingest (bitfield, reader, error))
            return FALSE;

          if (self->bitfield == NULL)
            self->bitfield = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->bitfield, g_steal_pointer (&bitfield));
        }
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
}
