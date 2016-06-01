/* rtfm-gir-parameters.c
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

#define G_LOG_DOMAIN "rtfm-gir-parameters"

#include "rtfm-gir-parameters.h"
#include "rtfm-gir-parameter.h"

struct _RtfmGirParameters
{
  RtfmGirBase base;
  GPtrArray *parameter;
};

enum {
  PROP_0,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirParameters, rtfm_gir_parameters, RTFM_TYPE_GIR_BASE)

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_parameters_ingest (RtfmGirBase       *base,
                            xmlTextReaderPtr   reader,
                            GError           **error);

static void
rtfm_gir_parameters_finalize (GObject *object)
{
  RtfmGirParameters *self = (RtfmGirParameters *)object;

  g_clear_pointer (&self->parameter, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_parameters_parent_class)->finalize (object);
}

static void
rtfm_gir_parameters_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  RtfmGirParameters *self = (RtfmGirParameters *)object;

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_parameters_set_property (GObject       *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  RtfmGirParameters *self = (RtfmGirParameters *)object;

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_parameters_class_init (RtfmGirParametersClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_parameters_finalize;
  object_class->get_property = rtfm_gir_parameters_get_property;
  object_class->set_property = rtfm_gir_parameters_set_property;

  base_class->ingest = rtfm_gir_parameters_ingest;

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_parameters_init (RtfmGirParameters *self)
{
}

static gboolean
rtfm_gir_parameters_ingest (RtfmGirBase       *base,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  RtfmGirParameters *self = (RtfmGirParameters *)base;

  g_assert (RTFM_IS_GIR_PARAMETERS (self));
  g_assert (reader != NULL);

  /* Read properties from element */

  /* Copy properties to object */

  /* Free libxml allocated strings */

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
      else if ((g_strcmp0 (element_name, "parameter") == 0) || (g_strcmp0 (element_name, "instance-parameter") == 0))
        {
          g_autoptr(RtfmGirParameter) parameter = NULL;

          parameter = g_object_new (RTFM_TYPE_GIR_PARAMETER, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (parameter), reader, error))
            return FALSE;

          if (self->parameter == NULL)
            self->parameter = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->parameter, g_steal_pointer (&parameter));
        }
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
}

gboolean
rtfm_gir_parameters_has_parameters (RtfmGirParameters *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_PARAMETERS (self), FALSE);

  return self->parameter != NULL && self->parameter->len > 0;
}

/**
 * rtfm_gir_parameters_get_parameters:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirParameter):
 *  An array of #RtfmGirParameter or %NULL.
 */
GPtrArray *
rtfm_gir_parameters_get_parameters (RtfmGirParameters *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_PARAMETERS (self), NULL);

  return self->parameter;
}
