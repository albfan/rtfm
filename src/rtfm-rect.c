/* rtfm-rect.c
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib/gi18n.h>

#include "rtfm-rect.h"

struct _RtfmRect
{
  GObject parent_instance;

  gint x;
  gint y;
  gint width;
  gint height;
};

enum {
  PROP_0,
  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT,
  LAST_PROP
};

G_DEFINE_TYPE (RtfmRect, rtfm_rect, G_TYPE_OBJECT)

static GParamSpec *gParamSpecs [LAST_PROP];

static void
rtfm_rect_get_property (GObject    *object,
                       guint       prop_id,
                       GValue     *value,
                       GParamSpec *pspec)
{
  RtfmRect *self = RTFM_RECT (object);

  switch (prop_id)
    {
    case PROP_X:
      g_value_set_int (value, self->x);
      break;

    case PROP_Y:
      g_value_set_int (value, self->y);
      break;

    case PROP_WIDTH:
      g_value_set_int (value, self->width);
      break;

    case PROP_HEIGHT:
      g_value_set_int (value, self->height);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_rect_set_property (GObject      *object,
                       guint         prop_id,
                       const GValue *value,
                       GParamSpec   *pspec)
{
  RtfmRect *self = RTFM_RECT (object);

  switch (prop_id)
    {
    case PROP_X:
      self->x = g_value_get_int (value);
      break;

    case PROP_Y:
      self->y = g_value_get_int (value);
      break;

    case PROP_WIDTH:
      self->width = g_value_get_int (value);
      break;

    case PROP_HEIGHT:
      self->height = g_value_get_int (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_rect_class_init (RtfmRectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_rect_get_property;
  object_class->set_property = rtfm_rect_set_property;

  gParamSpecs [PROP_X] =
    g_param_spec_int ("x",
                      "X",
                      "X",
                      G_MININT,
                      G_MAXINT,
                      0,
                      (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gParamSpecs [PROP_Y] =
    g_param_spec_int ("y",
                      "Y",
                      "Y",
                      G_MININT,
                      G_MAXINT,
                      0,
                      (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gParamSpecs [PROP_WIDTH] =
    g_param_spec_int ("width",
                      "Width",
                      "Width",
                      G_MININT,
                      G_MAXINT,
                      0,
                      (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gParamSpecs [PROP_HEIGHT] =
    g_param_spec_int ("height",
                      "Height",
                      "Height",
                      G_MININT,
                      G_MAXINT,
                      0,
                      (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, LAST_PROP, gParamSpecs);
}

static void
rtfm_rect_init (RtfmRect *rect)
{
}

void
rtfm_rect_get_rect (RtfmRect      *self,
                   GdkRectangle *rect)
{
  g_return_if_fail (RTFM_IS_RECT (self));
  g_return_if_fail (rect != NULL);

  rect->x = self->x;
  rect->y = self->y;
  rect->width = self->width;
  rect->height = self->height;
}
