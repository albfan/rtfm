/* rtfm-view.c
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

#define G_LOG_DOMAIN "rtfm-view"

#include <libpeas/peas.h>

#include "rtfm-item.h"
#include "rtfm-library-private.h"
#include "rtfm-view.h"
#include "rtfm-view-addin.h"
#include "rtfm-widget.h"

typedef struct
{
  PeasExtensionSet *addins;
  RtfmItem         *item;
  RtfmLibrary      *library;
} RtfmViewPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (RtfmView, rtfm_view, GTK_TYPE_BOX)

enum {
  PROP_0,
  PROP_ITEM,
  PROP_LIBRARY,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
rtfm_view_addins_extension_added (PeasExtensionSet *set,
                                  PeasPluginInfo   *plugin_info,
                                  PeasExtension    *exten,
                                  gpointer          user_data)
{
  RtfmViewAddin *addin = (RtfmViewAddin *)exten;
  RtfmView *self = user_data;

  g_assert (PEAS_IS_EXTENSION_SET (set));
  g_assert (plugin_info != NULL);
  g_assert (RTFM_IS_VIEW_ADDIN (addin));

  rtfm_view_addin_load (addin, self);
}

static void
rtfm_view_addins_extension_removed (PeasExtensionSet *set,
                                    PeasPluginInfo   *plugin_info,
                                    PeasExtension    *exten,
                                    gpointer          user_data)
{
  RtfmViewAddin *addin = (RtfmViewAddin *)exten;
  RtfmView *self = user_data;

  g_assert (PEAS_IS_EXTENSION_SET (set));
  g_assert (plugin_info != NULL);
  g_assert (RTFM_IS_VIEW_ADDIN (addin));

  rtfm_view_addin_set_item (addin, NULL);
  rtfm_view_addin_unload (addin, self);
}

static void
rtfm_view_realize (GtkWidget *widget)
{
  RtfmView *self = (RtfmView *)widget;
  RtfmViewPrivate *priv = rtfm_view_get_instance_private (self);
  RtfmLibrary *library;
  PeasEngine *engine;

  g_assert (RTFM_IS_VIEW (self));

  GTK_WIDGET_CLASS (rtfm_view_parent_class)->realize (widget);

  library = rtfm_view_get_library (self);
  engine = rtfm_library_get_engine (library);

  priv->addins = peas_extension_set_new (engine,
                                         RTFM_TYPE_VIEW_ADDIN,
                                         NULL);

  g_signal_connect (priv->addins,
                    "extension-added",
                    G_CALLBACK (rtfm_view_addins_extension_added),
                    self);

  g_signal_connect (priv->addins,
                    "extension-removed",
                    G_CALLBACK (rtfm_view_addins_extension_removed),
                    self);

  peas_extension_set_foreach (priv->addins,
                              rtfm_view_addins_extension_added,
                              self);
}

static void
rtfm_view_unrealize (GtkWidget *widget)
{
  RtfmView *self = (RtfmView *)widget;
  RtfmViewPrivate *priv = rtfm_view_get_instance_private (self);

  g_assert (RTFM_IS_VIEW (self));
  g_assert (PEAS_IS_EXTENSION_SET (priv->addins));

  g_clear_object (&priv->addins);

  GTK_WIDGET_CLASS (rtfm_view_parent_class)->unrealize (widget);
}

static void
rtfm_view_finalize (GObject *object)
{
  RtfmView *self = (RtfmView *)object;
  RtfmViewPrivate *priv = rtfm_view_get_instance_private (self);

  g_clear_object (&priv->item);

  G_OBJECT_CLASS (rtfm_view_parent_class)->finalize (object);
}

static void
rtfm_view_get_property (GObject    *object,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
  RtfmView *self = RTFM_VIEW (object);

  switch (prop_id)
    {
    case PROP_ITEM:
      g_value_set_object (value, rtfm_view_get_item (self));
      break;

    case PROP_LIBRARY:
      g_value_set_object (value, rtfm_view_get_library (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_view_set_property (GObject      *object,
                        guint         prop_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
  RtfmView *self = RTFM_VIEW (object);

  switch (prop_id)
    {
    case PROP_ITEM:
      rtfm_view_set_item (self, g_value_get_object (value));
      break;

    case PROP_LIBRARY:
      rtfm_view_set_library (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_view_class_init (RtfmViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = rtfm_view_finalize;
  object_class->get_property = rtfm_view_get_property;
  object_class->set_property = rtfm_view_set_property;

  widget_class->realize = rtfm_view_realize;
  widget_class->unrealize = rtfm_view_unrealize;

  properties [PROP_ITEM] =
    g_param_spec_object ("item",
                         "Item",
                         "Item",
                         RTFM_TYPE_ITEM,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_LIBRARY] =
    g_param_spec_object ("library",
                         "Library",
                         "Library",
                         RTFM_TYPE_LIBRARY,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/rtfm/ui/rtfm-view.ui");
  gtk_widget_class_set_css_name (widget_class, "rtfmview");

  rtfm_gtk_widget_class_set_css_from_resource (widget_class,
                                               NULL,
                                               "/org/gnome/rtfm/css/rtfm-view.css");
}

static void
rtfm_view_init (RtfmView *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
  gtk_orientable_set_orientation (GTK_ORIENTABLE (self),
                                  GTK_ORIENTATION_VERTICAL);
}

static void
rtfm_view_propagate_item (PeasExtensionSet *set,
                          PeasPluginInfo   *plugin_info,
                          PeasExtension    *exten,
                          gpointer          user_data)
{
  RtfmViewAddin *addin = (RtfmViewAddin *)exten;
  RtfmItem *item = user_data;

  g_assert (RTFM_IS_VIEW_ADDIN (addin));
  g_assert (plugin_info != NULL);
  g_assert (PEAS_IS_EXTENSION_SET (set));

  rtfm_view_addin_set_item (addin, item);
}

static void
rtfm_view_disconnect (RtfmView *self)
{
  RtfmViewPrivate *priv = rtfm_view_get_instance_private (self);

  g_assert (RTFM_IS_VIEW (self));
  g_assert (RTFM_IS_ITEM (priv->item));

  peas_extension_set_foreach (priv->addins,
                              rtfm_view_propagate_item,
                              NULL);

  g_clear_object (&priv->item);
}

static void
rtfm_view_connect (RtfmView *self,
                   RtfmItem *item)
{
  RtfmViewPrivate *priv = rtfm_view_get_instance_private (self);

  g_assert (RTFM_IS_VIEW (self));
  g_assert (RTFM_IS_ITEM (item));
  g_assert (priv->item == NULL);

  priv->item = g_object_ref (item);
  peas_extension_set_foreach (priv->addins, rtfm_view_propagate_item, item);
}

GtkWidget *
rtfm_view_new (void)
{
  return g_object_new (RTFM_TYPE_VIEW, NULL);
}

/**
 * rtfm_view_get_item:
 *
 * Returns: (transfer none): An #RtfmItem.
 */
RtfmItem *
rtfm_view_get_item (RtfmView *self)
{
  RtfmViewPrivate *priv = rtfm_view_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_VIEW (self), NULL);

  return priv->item;
}

void
rtfm_view_set_item (RtfmView *self,
                    RtfmItem *item)
{
  RtfmViewPrivate *priv = rtfm_view_get_instance_private (self);

  g_return_if_fail (RTFM_IS_VIEW (self));
  g_return_if_fail (!item || RTFM_IS_ITEM (item));

  if (priv->item != item)
    {
      if (priv->item != NULL)
        rtfm_view_disconnect (self);

      if (item != NULL)
        rtfm_view_connect (self, item);

      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ITEM]);
    }
}

/**
 * rtfm_view_get_library:
 *
 * Retrieves the #RtfmLibrary that is being used.
 *
 * Returns: (transfer none): An #RtfmLibrary.
 */
RtfmLibrary *
rtfm_view_get_library (RtfmView *self)
{
  RtfmViewPrivate *priv = rtfm_view_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_VIEW (self), NULL);

  if (priv->library != NULL)
    return priv->library;
  else
    return rtfm_library_get_default ();
}

void
rtfm_view_set_library (RtfmView    *self,
                       RtfmLibrary *library)
{
  RtfmViewPrivate *priv = rtfm_view_get_instance_private (self);

  g_return_if_fail (RTFM_IS_VIEW (self));
  g_return_if_fail (!library || RTFM_IS_LIBRARY (library));

  if (g_set_object (&priv->library, library))
    g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_LIBRARY]);
}
