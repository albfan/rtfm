/* rtfm-gir-c-include.c
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

#define G_LOG_DOMAIN "rtfm-gir-c_include"

#include "rtfm-gir-c-include.h"

struct _RtfmGirCInclude
{
  RtfmItem parent_instance;
  gchar *name;
};

enum {
  PROP_0,
  PROP_NAME,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirCInclude, rtfm_gir_c_include, RTFM_TYPE_ITEM)

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_c_include_finalize (GObject *object)
{
  RtfmGirCInclude *self = (RtfmGirCInclude *)object;

  g_clear_pointer (&self->name, g_free);

  G_OBJECT_CLASS (rtfm_gir_c_include_parent_class)->finalize (object);
}

static void
rtfm_gir_c_include_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  RtfmGirCInclude *self = (RtfmGirCInclude *)object;

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
rtfm_gir_c_include_set_property (GObject       *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  RtfmGirCInclude *self = (RtfmGirCInclude *)object;

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
rtfm_gir_c_include_class_init (RtfmGirCIncludeClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_c_include_finalize;
  object_class->get_property = rtfm_gir_c_include_get_property;
  object_class->set_property = rtfm_gir_c_include_set_property;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_c_include_init (RtfmGirCInclude *self)
{
}

gboolean
rtfm_gir_c_include_ingest (RtfmGirCInclude   *self,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  xmlChar *name;

  g_assert (RTFM_IS_GIR_C_INCLUDE (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);

  /* Free libxml allocated strings */
  xmlFree (name);

  return TRUE;
}
