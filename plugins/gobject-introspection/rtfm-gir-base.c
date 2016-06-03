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
  RtfmGirBase  *parent;
  GStringChunk *string_chunk;
} RtfmGirBasePrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (RtfmGirBase, rtfm_gir_base, G_TYPE_OBJECT)

static void
rtfm_gir_base_finalize (GObject *object)
{
  RtfmGirBase *self = (RtfmGirBase *)object;
  RtfmGirBasePrivate *priv = rtfm_gir_base_get_instance_private (self);

  g_clear_pointer (&priv->string_chunk, g_string_chunk_free);

  G_OBJECT_CLASS (rtfm_gir_base_parent_class)->finalize (object);
}

static void
rtfm_gir_base_class_init (RtfmGirBaseClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_base_finalize;
}

static void
rtfm_gir_base_init (RtfmGirBase *self)
{
}

gboolean
rtfm_gir_base_ingest (RtfmGirBase          *self,
                      GMarkupParseContext  *context,
                      const gchar          *element_name,
                      const gchar         **attribute_names,
                      const gchar         **attribute_values,
                      GError              **error)
{
  g_return_val_if_fail (RTFM_IS_GIR_BASE (self), FALSE);
  g_return_val_if_fail (context != NULL, FALSE);
  g_return_val_if_fail (element_name != NULL, FALSE);
  g_return_val_if_fail (attribute_names != NULL, FALSE);
  g_return_val_if_fail (attribute_values != NULL, FALSE);

  if (RTFM_GIR_BASE_GET_CLASS (self)->ingest)
    return RTFM_GIR_BASE_GET_CLASS (self)->ingest (self,
                                                   context,
                                                   element_name,
                                                   attribute_names,
                                                   attribute_values,
                                                   error);

  return TRUE;
}

/**
 * rtfm_gir_base_get_toplevel:
 *
 * Returns: (transfer none): An #RtfmGirBase.
 */
RtfmGirBase *
rtfm_gir_base_get_toplevel (RtfmGirBase *self)
{
  RtfmGirBase *iter;
  RtfmGirBase *ret = NULL;

  g_return_val_if_fail (RTFM_IS_GIR_BASE (self), NULL);

  for (iter = self; iter != NULL; iter = rtfm_gir_base_get_parent (iter))
    ret = iter;

  return ret;
}

/**
 * rtfm_gir_base_get_parent:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirBase or %NULL.
 */
RtfmGirBase *
rtfm_gir_base_get_parent (RtfmGirBase *self)
{
  RtfmGirBasePrivate *priv = rtfm_gir_base_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_GIR_BASE (self), NULL);

  return priv->parent;
}

void
rtfm_gir_base_set_parent (RtfmGirBase *self,
                          RtfmGirBase *parent)
{
  RtfmGirBasePrivate *priv = rtfm_gir_base_get_instance_private (self);

  g_return_if_fail (RTFM_IS_GIR_BASE (self));

  priv->parent = parent;
}

const gchar *
rtfm_gir_base_intern_string (RtfmGirBase *self,
                             const gchar *string)
{
  g_return_val_if_fail (RTFM_IS_GIR_BASE (self), NULL);

  if (string != NULL)
    {
      RtfmGirBase *toplevel;
      RtfmGirBasePrivate *priv;

      toplevel = rtfm_gir_base_get_toplevel (self);
      priv = rtfm_gir_base_get_instance_private (toplevel);

      if (priv->string_chunk == NULL)
        priv->string_chunk = g_string_chunk_new (4096);

      return g_string_chunk_insert_const (priv->string_chunk, string);
    }

  return NULL;
}
