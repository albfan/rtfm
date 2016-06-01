/* rtfm-gir-implements.c
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

#define G_LOG_DOMAIN "rtfm-gir-implements"

#include "rtfm-gir-implements.h"

struct _RtfmGirImplements
{
  RtfmGirBase base;
  gchar *name;
};

enum {
  PROP_0,
  PROP_NAME,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirImplements, rtfm_gir_implements, RTFM_TYPE_GIR_BASE)

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_implements_ingest (RtfmGirBase       *base,
                            xmlTextReaderPtr   reader,
                            GError           **error);

static void
rtfm_gir_implements_finalize (GObject *object)
{
  RtfmGirImplements *self = (RtfmGirImplements *)object;

  g_clear_pointer (&self->name, g_free);

  G_OBJECT_CLASS (rtfm_gir_implements_parent_class)->finalize (object);
}

static void
rtfm_gir_implements_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  RtfmGirImplements *self = (RtfmGirImplements *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_implements_set_property (GObject       *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  RtfmGirImplements *self = (RtfmGirImplements *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_implements_class_init (RtfmGirImplementsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_implements_finalize;
  object_class->get_property = rtfm_gir_implements_get_property;
  object_class->set_property = rtfm_gir_implements_set_property;

  base_class->ingest = rtfm_gir_implements_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_implements_init (RtfmGirImplements *self)
{
}

static gboolean
rtfm_gir_implements_ingest (RtfmGirBase       *base,
                            xmlTextReaderPtr   reader,
                            GError           **error)
{
  RtfmGirImplements *self = (RtfmGirImplements *)base;
  xmlChar *name;

  g_assert (RTFM_IS_GIR_IMPLEMENTS (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);

  /* Free libxml allocated strings */
  xmlFree (name);

  return TRUE;
}
