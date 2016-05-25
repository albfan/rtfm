/* rtfm-sidebar.c
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

#define G_LOG_DOMAIN "rtfm-sidebar"

#include "rtfm-collection.h"
#include "rtfm-item.h"
#include "rtfm-library.h"
#include "rtfm-path.h"
#include "rtfm-sidebar.h"
#include "rtfm-sidebar-row.h"
#include "rtfm-stack-list.h"
#include "rtfm-widget.h"

typedef struct
{
  RtfmLibrary   *library;
  GCancellable  *cancellable;

  RtfmStackList *browse;
  RtfmStackList *search;
} RtfmSidebarPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (RtfmSidebar, rtfm_sidebar, GTK_TYPE_BOX)

enum {
  PROP_0,
  PROP_LIBRARY,
  N_PROPS
};

enum {
  ITEM_ACTIVATED,
  N_SIGNALS
};

static GParamSpec *properties [N_PROPS];
static guint signals [N_SIGNALS];

static GtkWidget *
rtfm_sidebar_create_row (gpointer item,
                         gpointer user_data)
{
  g_assert (G_IS_OBJECT (item));
  g_assert (RTFM_IS_SIDEBAR (user_data));

  return g_object_new (RTFM_TYPE_SIDEBAR_ROW,
                       "object", item,
                       "visible", TRUE,
                       NULL);
}

static void
rtfm_sidebar_load_children_cb (GObject      *object,
                               GAsyncResult *result,
                               gpointer      user_data)
{
  g_autoptr(RtfmSidebar) self = user_data;
  RtfmLibrary *library = (RtfmLibrary *)object;

  g_assert (RTFM_IS_SIDEBAR (self));
  g_assert (RTFM_IS_LIBRARY (library));

  rtfm_library_load_children_finish (library, result, NULL);
}

static void
rtfm_sidebar_connect (RtfmSidebar *self,
                      RtfmLibrary *library)
{
  RtfmSidebarPrivate *priv = rtfm_sidebar_get_instance_private (self);
  g_autoptr(RtfmCollection) collection = NULL;
  g_autoptr(RtfmPath) path = NULL;
  RtfmSidebarRow *row;

  g_assert (RTFM_IS_SIDEBAR (self));
  g_assert (RTFM_IS_LIBRARY (library));

  priv->library = g_object_ref (library);
  priv->cancellable = g_cancellable_new ();

  row = g_object_new (RTFM_TYPE_SIDEBAR_ROW,
                      "object", library,
                      "visible", TRUE,
                      NULL);

  path = rtfm_path_new ();
  collection = rtfm_collection_new (path);

  rtfm_library_load_children_async (priv->library,
                                    path,
                                    collection,
                                    priv->cancellable,
                                    rtfm_sidebar_load_children_cb,
                                    g_object_ref (self));

  rtfm_stack_list_push (RTFM_STACK_LIST (priv->browse),
                        GTK_WIDGET (row),
                        G_LIST_MODEL (collection),
                        rtfm_sidebar_create_row,
                        self,
                        NULL);
}

static void
rtfm_sidebar_disconnect (RtfmSidebar *self)
{
  RtfmSidebarPrivate *priv = rtfm_sidebar_get_instance_private (self);

  g_assert (RTFM_IS_SIDEBAR (self));
  g_assert (RTFM_IS_LIBRARY (priv->library));

  if (priv->cancellable != NULL)
    {
      if (!g_cancellable_is_cancelled (priv->cancellable))
        g_cancellable_cancel (priv->cancellable);
      g_clear_object (&priv->library);
    }

  rtfm_stack_list_clear (priv->browse);
  rtfm_stack_list_clear (priv->search);
}

static void
rtfm_sidebar_browse_row_activated (RtfmSidebar    *self,
                                   RtfmSidebarRow *row,
                                   RtfmStackList  *stack_list)
{
  RtfmSidebarPrivate *priv = rtfm_sidebar_get_instance_private (self);
  g_autoptr(RtfmCollection) collection = NULL;
  g_autoptr(RtfmPath) path = NULL;
  GtkWidget *header;
  GObject *object;

  g_assert (RTFM_IS_SIDEBAR (self));
  g_assert (RTFM_IS_SIDEBAR_ROW (row));
  g_assert (RTFM_IS_STACK_LIST (stack_list));

  object = rtfm_sidebar_row_get_object (row);

  if (!RTFM_IS_ITEM (object))
    return;

  header = g_object_new (RTFM_TYPE_SIDEBAR_ROW,
                         "object", object,
                         "visible", TRUE,
                         NULL);

  path = rtfm_item_get_path (RTFM_ITEM (object));
  collection = rtfm_collection_new (path);

  rtfm_stack_list_push (priv->browse,
                        header,
                        G_LIST_MODEL (collection),
                        rtfm_sidebar_create_row,
                        self, NULL);

  rtfm_library_load_children_async (priv->library,
                                    path,
                                    collection,
                                    priv->cancellable,
                                    rtfm_sidebar_load_children_cb,
                                    g_object_ref (self));

  if (RTFM_IS_ITEM (object))
    g_signal_emit (self, signals [ITEM_ACTIVATED], 0, object);
}

static void
rtfm_sidebar_browse_header_activated (RtfmSidebar    *self,
                                      RtfmSidebarRow *row,
                                      RtfmStackList  *stack_list)
{
  GObject *object;

  g_assert (RTFM_IS_SIDEBAR (self));
  g_assert (RTFM_IS_SIDEBAR_ROW (row));
  g_assert (RTFM_IS_STACK_LIST (stack_list));

  object = rtfm_sidebar_row_get_object (row);

  if (!RTFM_IS_ITEM (object))
    return;

  if (RTFM_IS_ITEM (object))
    g_signal_emit (self, signals [ITEM_ACTIVATED], 0, object);
}

static void
rtfm_sidebar_finalize (GObject *object)
{
  RtfmSidebar *self = (RtfmSidebar *)object;
  RtfmSidebarPrivate *priv = rtfm_sidebar_get_instance_private (self);

  g_clear_object (&priv->library);

  G_OBJECT_CLASS (rtfm_sidebar_parent_class)->finalize (object);
}

static void
rtfm_sidebar_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  RtfmSidebar *self = RTFM_SIDEBAR (object);

  switch (prop_id)
    {
    case PROP_LIBRARY:
      g_value_set_object (value, rtfm_sidebar_get_library (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_sidebar_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  RtfmSidebar *self = RTFM_SIDEBAR (object);

  switch (prop_id)
    {
    case PROP_LIBRARY:
      rtfm_sidebar_set_library (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_sidebar_class_init (RtfmSidebarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = rtfm_sidebar_finalize;
  object_class->get_property = rtfm_sidebar_get_property;
  object_class->set_property = rtfm_sidebar_set_property;

  properties [PROP_LIBRARY] =
    g_param_spec_object ("library",
                         "Library",
                         "Library",
                         RTFM_TYPE_LIBRARY,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/rtfm/ui/rtfm-sidebar.ui");
  gtk_widget_class_set_css_name (widget_class, "rtfmsidebar");
  gtk_widget_class_bind_template_child_private (widget_class, RtfmSidebar, browse);
  gtk_widget_class_bind_template_child_private (widget_class, RtfmSidebar, search);

  signals [ITEM_ACTIVATED] =
    g_signal_new ("item-activated",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (RtfmSidebarClass, item_activated),
                  NULL, NULL, NULL,
                  G_TYPE_NONE, 1, RTFM_TYPE_ITEM);

  rtfm_gtk_widget_class_set_css_from_resource (widget_class,
                                               NULL,
                                               "/org/gnome/rtfm/css/rtfm-sidebar.css");
}

static void
rtfm_sidebar_init (RtfmSidebar *self)
{
  RtfmSidebarPrivate *priv = rtfm_sidebar_get_instance_private (self);

  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect_object (priv->browse,
                           "row-activated",
                           G_CALLBACK (rtfm_sidebar_browse_row_activated),
                           self,
                           G_CONNECT_SWAPPED);

  g_signal_connect_object (priv->browse,
                           "header-activated",
                           G_CALLBACK (rtfm_sidebar_browse_header_activated),
                           self,
                           G_CONNECT_SWAPPED);
}

GtkWidget *
rtfm_sidebar_new (void)
{
  return g_object_new (RTFM_TYPE_SIDEBAR, NULL);
}

/**
 * rtfm_sidebar_get_library:
 *
 * Gets the library associated with the sidebar.
 *
 * Returns: (transfer none): An #RtfmLibrary.
 */
RtfmLibrary *
rtfm_sidebar_get_library (RtfmSidebar *self)
{
  RtfmSidebarPrivate *priv = rtfm_sidebar_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_SIDEBAR (self), NULL);

  return priv->library;
}

void
rtfm_sidebar_set_library (RtfmSidebar *self,
                          RtfmLibrary *library)
{
  RtfmSidebarPrivate *priv = rtfm_sidebar_get_instance_private (self);

  g_return_if_fail (RTFM_IS_SIDEBAR (self));
  g_return_if_fail (RTFM_IS_LIBRARY (library));

  if (library != priv->library)
    {
      if (priv->library)
        rtfm_sidebar_disconnect (self);

      if (library)
        rtfm_sidebar_connect (self, library);

      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_LIBRARY]);
    }
}
