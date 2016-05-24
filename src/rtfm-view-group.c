/* rtfm-view-group.c
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

#define G_LOG_DOMAIN "rtfm-view-group"

#include "rtfm-view-group.h"
#include "rtfm-widget.h"

typedef struct
{
  GtkLabel *title;
  GtkListBox *list_box;
} RtfmViewGroupPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (RtfmViewGroup, rtfm_view_group, GTK_TYPE_BOX)

enum {
  PROP_0,
  PROP_TITLE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
rtfm_view_group_finalize (GObject *object)
{
  G_OBJECT_CLASS (rtfm_view_group_parent_class)->finalize (object);
}

static void
rtfm_view_group_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  RtfmViewGroup *self = RTFM_VIEW_GROUP (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      g_value_set_string (value, rtfm_view_group_get_title (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_view_group_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RtfmViewGroup *self = RTFM_VIEW_GROUP (object);

  switch (prop_id)
    {
    case PROP_TITLE:
      rtfm_view_group_set_title (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_view_group_class_init (RtfmViewGroupClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = rtfm_view_group_finalize;
  object_class->get_property = rtfm_view_group_get_property;
  object_class->set_property = rtfm_view_group_set_property;

  properties [PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "Title",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/rtfm/ui/rtfm-view-group.ui");
  gtk_widget_class_set_css_name (widget_class, "rtfmviewgroup");
  gtk_widget_class_bind_template_child_private (widget_class, RtfmViewGroup, list_box);
  gtk_widget_class_bind_template_child_private (widget_class, RtfmViewGroup, title);

  rtfm_gtk_widget_class_set_css_from_resource (widget_class,
                                               NULL,
                                               "/org/gnome/rtfm/css/rtfm-view-group.css");
}

static void
rtfm_view_group_init (RtfmViewGroup *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  gtk_orientable_set_orientation (GTK_ORIENTABLE (self), GTK_ORIENTATION_VERTICAL);
}

GtkWidget *
rtfm_view_group_new (void)
{
  return g_object_new (RTFM_TYPE_VIEW_GROUP, NULL);
}

const gchar *
rtfm_view_group_get_title (RtfmViewGroup *self)
{
  RtfmViewGroupPrivate *priv = rtfm_view_group_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_VIEW_GROUP (self), NULL);

  return gtk_label_get_label (priv->title);
}

void
rtfm_view_group_set_title (RtfmViewGroup *self,
                           const gchar   *title)
{
  RtfmViewGroupPrivate *priv = rtfm_view_group_get_instance_private (self);

  g_return_if_fail (RTFM_IS_VIEW_GROUP (self));

  gtk_label_set_label (priv->title, title);
  g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_TITLE]);
}

void
rtfm_view_group_add_row (RtfmViewGroup *self,
                         GtkWidget     *row)
{
  RtfmViewGroupPrivate *priv = rtfm_view_group_get_instance_private (self);

  g_return_if_fail (RTFM_IS_VIEW_GROUP (self));
  g_return_if_fail (GTK_IS_WIDGET (row));

  gtk_container_add (GTK_CONTAINER (priv->list_box), row);
}

void
rtfm_view_group_clear (RtfmViewGroup *self)
{
  RtfmViewGroupPrivate *priv = rtfm_view_group_get_instance_private (self);

  g_return_if_fail (RTFM_IS_VIEW_GROUP (self));

  gtk_container_foreach (GTK_CONTAINER (priv->list_box),
                         (GtkCallback)gtk_widget_destroy,
                         NULL);
}
