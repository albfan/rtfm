/* rtfm-gir-type.c
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

#define G_LOG_DOMAIN "rtfm-gir-type"

#include "rtfm-gir-type.h"

struct _RtfmGirType
{
  RtfmGirBase base;
  gchar *name;
  gchar *c_type;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_C_TYPE,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirType, rtfm_gir_type, RTFM_TYPE_GIR_BASE)

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_type_ingest (RtfmGirBase       *base,
                      xmlTextReaderPtr   reader,
                      GError           **error);

static void
rtfm_gir_type_finalize (GObject *object)
{
  RtfmGirType *self = (RtfmGirType *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_type, g_free);

  G_OBJECT_CLASS (rtfm_gir_type_parent_class)->finalize (object);
}

static void
rtfm_gir_type_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  RtfmGirType *self = (RtfmGirType *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_C_TYPE:
      g_value_set_string (value, self->c_type);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_type_set_property (GObject       *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  RtfmGirType *self = (RtfmGirType *)object;

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

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_type_class_init (RtfmGirTypeClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_type_finalize;
  object_class->get_property = rtfm_gir_type_get_property;
  object_class->set_property = rtfm_gir_type_set_property;

  base_class->ingest = rtfm_gir_type_ingest;

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

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_type_init (RtfmGirType *self)
{
}

static gboolean
rtfm_gir_type_ingest (RtfmGirBase       *base,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  RtfmGirType *self = (RtfmGirType *)base;
  xmlChar *name;
  xmlChar *c_type;

  g_assert (RTFM_IS_GIR_TYPE (self));
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

  return TRUE;
}
