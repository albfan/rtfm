/* rtfm-sidebar-row.c
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

#define G_LOG_DOMAIN "rtfm-sidebar-row"

#include <glib/gi18n.h>

#include "rtfm-item.h"
#include "rtfm-library.h"
#include "rtfm-sidebar-row.h"

struct _RtfmSidebarRow
{
  GtkListBoxRow  parent_instance;

  GObject       *object;

  GtkImage      *image;
  GtkLabel      *subtitle;
  GtkLabel      *title;
};

G_DEFINE_TYPE (RtfmSidebarRow, rtfm_sidebar_row, GTK_TYPE_LIST_BOX_ROW)

enum {
  PROP_0,
  PROP_OBJECT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
rtfm_sidebar_row_finalize (GObject *object)
{
  RtfmSidebarRow *self = (RtfmSidebarRow *)object;

  g_clear_object (&self->object);

  G_OBJECT_CLASS (rtfm_sidebar_row_parent_class)->finalize (object);
}

static void
rtfm_sidebar_row_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  RtfmSidebarRow *self = RTFM_SIDEBAR_ROW (object);

  switch (prop_id)
    {
    case PROP_OBJECT:
      g_value_set_object (value, rtfm_sidebar_row_get_object (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_sidebar_row_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  RtfmSidebarRow *self = RTFM_SIDEBAR_ROW (object);

  switch (prop_id)
    {
    case PROP_OBJECT:
      rtfm_sidebar_row_set_object (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_sidebar_row_class_init (RtfmSidebarRowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = rtfm_sidebar_row_finalize;
  object_class->get_property = rtfm_sidebar_row_get_property;
  object_class->set_property = rtfm_sidebar_row_set_property;

  properties [PROP_OBJECT] =
    g_param_spec_object ("object",
                         "Object",
                         "Object",
                         G_TYPE_OBJECT,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/rtfm/ui/rtfm-sidebar-row.ui");
  gtk_widget_class_bind_template_child (widget_class, RtfmSidebarRow, image);
  gtk_widget_class_bind_template_child (widget_class, RtfmSidebarRow, subtitle);
  gtk_widget_class_bind_template_child (widget_class, RtfmSidebarRow, title);
}

static void
rtfm_sidebar_row_init (RtfmSidebarRow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

static void
rtfm_sidebar_row_update (RtfmSidebarRow *self,
                         GObject        *object)
{
  g_assert (RTFM_IS_SIDEBAR_ROW (self));
  g_assert (G_IS_OBJECT (object));

  if (RTFM_IS_LIBRARY (object))
    {
      gtk_label_set_label (self->title, _("Developer Library"));
      gtk_label_set_label (self->subtitle, NULL);
      gtk_widget_set_visible (GTK_WIDGET (self->subtitle), FALSE);
      g_object_set (self->image,
                    "icon-name", "help-contents-symbolic",
                    "pixel-size", 16,
                    NULL);
    }
  else if (RTFM_IS_ITEM (object))
    {
      RtfmItem *item = RTFM_ITEM (object);
      const gchar *icon_name = NULL;
      const gchar *subtitle = NULL;
      const gchar *title = NULL;

      icon_name = rtfm_item_get_icon_name (item);
      subtitle = rtfm_item_get_subtitle (item);
      title = rtfm_item_get_title (item);

      gtk_label_set_label (self->title, title);
      gtk_label_set_label (self->subtitle, subtitle);
      g_object_set (self->image,
                    "icon-name", icon_name,
                    "pixel-size", 16,
                    NULL);

      gtk_widget_set_visible (GTK_WIDGET (self->subtitle), subtitle != NULL);
    }
}

GtkWidget *
rtfm_sidebar_row_new (void)
{
  return g_object_new (RTFM_TYPE_SIDEBAR_ROW, NULL);
}

/**
 * rtfm_sidebar_row_get_object:
 *
 * Returns: (transfer none) (nullable): A #GObject or %NULL.
 */
GObject *
rtfm_sidebar_row_get_object (RtfmSidebarRow *self)
{
  g_return_val_if_fail (RTFM_IS_SIDEBAR_ROW (self), NULL);

  return self->object;
}

void
rtfm_sidebar_row_set_object (RtfmSidebarRow *self,
                             GObject        *object)
{
  g_return_if_fail (RTFM_IS_SIDEBAR_ROW (self));
  g_return_if_fail (!object || G_IS_OBJECT (object));

  if (g_set_object (&self->object, object))
    {
      if (object != NULL)
        rtfm_sidebar_row_update (self, object);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_OBJECT]);
    }
}
