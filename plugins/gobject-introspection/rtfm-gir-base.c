/* rtfm-gir-base.c
 *
 * Copyright (C) 2016 Christian Hergert <christian@hergert.me>
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

#define G_LOG_DOMAIN "rtfm-gir-base"

#include "rtfm-gir-base.h"

typedef struct
{
  RtfmGirBase *parent;
} RtfmGirBasePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (RtfmGirBase, rtfm_gir_base, G_TYPE_OBJECT)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_base_finalize (GObject *object)
{
  RtfmGirBase *self = (RtfmGirBase *)object;
  RtfmGirBasePrivate *priv = rtfm_gir_base_get_instance_private (self);

  G_OBJECT_CLASS (rtfm_gir_base_parent_class)->finalize (object);
}

static void
rtfm_gir_base_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  RtfmGirBase *self = RTFM_GIR_BASE (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_base_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  RtfmGirBase *self = RTFM_GIR_BASE (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_base_class_init (RtfmGirBaseClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_base_finalize;
  object_class->get_property = rtfm_gir_base_get_property;
  object_class->set_property = rtfm_gir_base_set_property;
}

static void
rtfm_gir_base_init (RtfmGirBase *self)
{
}

gboolean
rtfm_gir_base_ingest (RtfmGirBase       *self,
                      xmlTextReaderPtr   reader,
                      GError           **error)
{
  g_return_val_if_fail (RTFM_IS_GIR_BASE (self), FALSE);
  g_return_val_if_fail (reader != NULL, FALSE);

  if (RTFM_GIR_BASE_GET_CLASS (self)->ingest)
    return RTFM_GIR_BASE_GET_CLASS (self)->ingest (self, reader, error);

  return TRUE;
}
