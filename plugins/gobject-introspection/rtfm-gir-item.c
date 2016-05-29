/* rtfm-gir-item.c
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

#include <glib/gi18n.h>

#include "rtfm-gir-item.h"

typedef struct
{
  void *dummy;
} RtfmGirItemPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (RtfmGirItem, rtfm_gir_item, RTFM_TYPE_ITEM)

enum {
  PROP_0,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_item_finalize (GObject *object)
{
  RtfmGirItem *self = (RtfmGirItem *)object;
  RtfmGirItemPrivate *priv = rtfm_gir_item_get_instance_private (self);

  G_OBJECT_CLASS (rtfm_gir_item_parent_class)->finalize (object);
}

static void
rtfm_gir_item_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  RtfmGirItem *self = RTFM_GIR_ITEM (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_item_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  RtfmGirItem *self = RTFM_GIR_ITEM (object);

  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_item_class_init (RtfmGirItemClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_item_finalize;
  object_class->get_property = rtfm_gir_item_get_property;
  object_class->set_property = rtfm_gir_item_set_property;
}

static void
rtfm_gir_item_init (RtfmGirItem *self)
{
}
