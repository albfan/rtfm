/* rtfm-gir-record.c
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

#define G_LOG_DOMAIN "rtfm-gir-record"

#include "rtfm-gir-record.h"
#include "rtfm-gir-field.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-parameter.h"

struct _RtfmGirRecord
{
  RtfmGirBase base;
  gchar *name;
  gchar *c_type;
  gchar *doc;
  GPtrArray *field;
  GPtrArray *function;
  GPtrArray *parameters;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_C_TYPE,
  PROP_DOC,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirRecord, rtfm_gir_record, RTFM_TYPE_GIR_BASE)

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_record_ingest (RtfmGirBase       *base,
                        xmlTextReaderPtr   reader,
                        GError           **error);

static void
rtfm_gir_record_finalize (GObject *object)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_clear_pointer (&self->doc, g_free);
  g_clear_pointer (&self->doc, g_ptr_array_unref);
  g_clear_pointer (&self->doc, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_record_parent_class)->finalize (object);
}

static void
rtfm_gir_record_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
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
rtfm_gir_record_set_property (GObject       *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
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
rtfm_gir_record_class_init (RtfmGirRecordClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_record_finalize;
  object_class->get_property = rtfm_gir_record_get_property;
  object_class->set_property = rtfm_gir_record_set_property;

  base_class->ingest = rtfm_gir_record_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
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
rtfm_gir_record_init (RtfmGirRecord *self)
{
}

static gboolean
rtfm_gir_record_ingest (RtfmGirBase       *base,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  RtfmGirRecord *self = (RtfmGirRecord *)base;
  xmlChar *name;
  xmlChar *c_type;

  g_assert (RTFM_IS_GIR_RECORD (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");
  c_type = xmlTextReaderGetAttribute (reader, (const xmlChar *)"c:type");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);
  self->c_type = g_strdup ((gchar *)c_type);

  /* Free libxml allocated strings */
  xmlFree (name);
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
      else if (g_strcmp0 (element_name, "field") == 0)
        {
          g_autoptr(RtfmGirField) field = NULL;

          field = g_object_new (RTFM_TYPE_GIR_FIELD, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (field), reader, error))
            return FALSE;

          if (self->field == NULL)
            self->field = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->field, g_steal_pointer (&field));
        }
      else if (g_strcmp0 (element_name, "function") == 0)
        {
          g_autoptr(RtfmGirFunction) function = NULL;

          function = g_object_new (RTFM_TYPE_GIR_FUNCTION, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (function), reader, error))
            return FALSE;

          if (self->function == NULL)
            self->function = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->function, g_steal_pointer (&function));
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

gboolean
rtfm_gir_record_has_fields (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_RECORD (self), FALSE);

  return self->field != NULL && self->field->len > 0;
}

/**
 * rtfm_gir_record_get_fields:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirField):
 *  An array of #RtfmGirField or %NULL.
 */
GPtrArray *
rtfm_gir_record_get_fields (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_RECORD (self), NULL);

  return self->field;
}

gboolean
rtfm_gir_record_has_functions (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_RECORD (self), FALSE);

  return self->function != NULL && self->function->len > 0;
}

/**
 * rtfm_gir_record_get_functions:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirFunction):
 *  An array of #RtfmGirFunction or %NULL.
 */
GPtrArray *
rtfm_gir_record_get_functions (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_RECORD (self), NULL);

  return self->function;
}
