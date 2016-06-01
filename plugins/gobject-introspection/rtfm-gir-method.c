/* rtfm-gir-method.c
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

#define G_LOG_DOMAIN "rtfm-gir-method"

#include "rtfm-gir-method.h"
#include "rtfm-gir-return-value.h"
#include "rtfm-gir-parameter.h"

struct _RtfmGirMethod
{
  RtfmGirBase base;
  gchar *name;
  gchar *c_identifier;
  gchar *version;
  gchar *doc;
  RtfmGirReturnValue *return_value;
  GPtrArray *parameters;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_C_IDENTIFIER,
  PROP_VERSION,
  PROP_DOC,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirMethod, rtfm_gir_method, RTFM_TYPE_GIR_BASE)

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_method_ingest (RtfmGirBase       *base,
                        xmlTextReaderPtr   reader,
                        GError           **error);

static void
rtfm_gir_method_finalize (GObject *object)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_identifier, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->doc, g_free);

  G_OBJECT_CLASS (rtfm_gir_method_parent_class)->finalize (object);
}

static void
rtfm_gir_method_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_C_IDENTIFIER:
      g_value_set_string (value, self->c_identifier);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_DOC:
      g_value_set_string (value, self->doc);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_method_set_property (GObject       *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_C_IDENTIFIER:
      g_free (self->c_identifier);
      self->c_identifier = g_value_dup_string (value);
      break;

    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
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
rtfm_gir_method_class_init (RtfmGirMethodClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_method_finalize;
  object_class->get_property = rtfm_gir_method_get_property;
  object_class->set_property = rtfm_gir_method_set_property;

  base_class->ingest = rtfm_gir_method_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_IDENTIFIER] =
    g_param_spec_string ("c-identifier",
                         "c-identifier",
                         "c-identifier",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
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
rtfm_gir_method_init (RtfmGirMethod *self)
{
}

static gboolean
rtfm_gir_method_ingest (RtfmGirBase       *base,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  RtfmGirMethod *self = (RtfmGirMethod *)base;
  xmlChar *name;
  xmlChar *c_identifier;
  xmlChar *version;

  g_assert (RTFM_IS_GIR_METHOD (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");
  c_identifier = xmlTextReaderGetAttribute (reader, (const xmlChar *)"c:identifier");
  version = xmlTextReaderGetAttribute (reader, (const xmlChar *)"version");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);
  self->c_identifier = g_strdup ((gchar *)c_identifier);
  self->version = g_strdup ((gchar *)version);

  /* Free libxml allocated strings */
  xmlFree (name);
  xmlFree (c_identifier);
  xmlFree (version);

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
      else if (g_strcmp0 (element_name, "return-value") == 0)
        {
          g_autoptr(RtfmGirReturnValue) return_value = NULL;

          return_value = g_object_new (RTFM_TYPE_GIR_RETURN_VALUE, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (return_value), reader, error))
            return FALSE;

          g_set_object (&self->return_value, return_value);
        }
      else if (g_strcmp0 (element_name, "parameters") == 0)
        {
          if (self->parameters == NULL)
            self->parameters = g_ptr_array_new_with_free_func (g_object_unref);

          if (xmlTextReaderRead (reader) != 1)
            return FALSE;

          do
            {
              g_autoptr(RtfmGirParameter) parameter = NULL;

              parameter = g_object_new (RTFM_TYPE_GIR_PARAMETER, NULL);

              if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (parameter), reader, error))
                return FALSE;

              g_ptr_array_add (self->parameters, g_steal_pointer (&parameter));
            }
          while (xmlTextReaderNext (reader) == 1);
        }
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
}

/**
 * rtfm_gir_method_get_return_value:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirReturnValue or %NULL.
 */
RtfmGirReturnValue *
rtfm_gir_method_get_return_value (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_METHOD (self), NULL);

  return self->return_value;
}