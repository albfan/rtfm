/* rtfm-window.c
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

#define G_LOG_DOMAIN "rtfm-library"

#include "rtfm-library.h"
#include "rtfm-path-bar.h"
#include "rtfm-sidebar.h"
#include "rtfm-view.h"
#include "rtfm-widget.h"
#include "rtfm-window.h"

struct _RtfmWindow
{
  GtkApplicationWindow  parent_instance;

  RtfmLibrary          *library;

  RtfmPathBar          *path_bar;
  RtfmSidebar          *sidebar;
  RtfmView             *view;
};

G_DEFINE_TYPE (RtfmWindow, rtfm_window, GTK_TYPE_APPLICATION_WINDOW)

enum {
  PROP_0,
  PROP_LIBRARY,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * rtfm_window_get_library:
 * @self: A #RtfmWindow.
 *
 * Gets the library used by @window.
 *
 * Returns: (transfer none): An #RtfmWindow.
 */
RtfmLibrary *
rtfm_window_get_library (RtfmWindow *self)
{
  g_return_val_if_fail (RTFM_IS_WINDOW (self), NULL);

  return self->library;
}

void
rtfm_window_set_library (RtfmWindow  *self,
                         RtfmLibrary *library)
{
  g_return_if_fail (RTFM_IS_WINDOW (self));
  g_return_if_fail (RTFM_IS_LIBRARY (library));

  if (g_set_object (&self->library, library))
    {
      rtfm_sidebar_set_library (self->sidebar, library);
      rtfm_path_bar_set_path (self->path_bar, NULL);
      rtfm_view_set_item (self->view, NULL);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_LIBRARY]);
    }
}

static void
rtfm_window_sidebar_item_activated (RtfmWindow  *self,
                                    RtfmItem    *item,
                                    RtfmSidebar *sidebar)
{
  g_autoptr(RtfmPath) path = NULL;
  RtfmPath *old_path;

  g_assert (RTFM_IS_WINDOW (self));
  g_assert (RTFM_IS_ITEM (item));
  g_assert (RTFM_IS_SIDEBAR (sidebar));

  rtfm_view_set_item (self->view, item);

  old_path = rtfm_path_bar_get_path (self->path_bar);
  path = rtfm_item_get_path (item);

  if (path == NULL || old_path == NULL || !rtfm_path_has_prefix (old_path, path))
    {
      rtfm_path_bar_set_path (self->path_bar, path);
    }
  else
    {
      guint index = rtfm_path_get_length (path);

      if (index > 0)
        index--;

      rtfm_path_bar_set_selected_index (self->path_bar, index);
    }
}

static void
rtfm_window_path_bar_element_selected (RtfmWindow      *self,
                                       RtfmPath        *path,
                                       RtfmPathElement *element,
                                       RtfmPathBar     *path_bar)
{
  const gchar *id;

  g_assert (RTFM_IS_WINDOW (self));
  g_assert (RTFM_IS_PATH (path));
  g_assert (RTFM_IS_PATH_ELEMENT (element));
  g_assert (RTFM_IS_PATH_BAR (path_bar));

  if (NULL != (id = rtfm_path_element_get_id (element)))
    {
      g_autoptr(RtfmItem) item = NULL;

      item = rtfm_library_get_item_by_id (self->library, id);

      if (item != NULL)
        rtfm_view_set_item (self->view, item);
    }
}

static void
rtfm_window_constructed (GObject *object)
{
  RtfmWindow *self = (RtfmWindow *)object;

  G_OBJECT_CLASS (rtfm_window_parent_class)->constructed (object);

  if (self->library == NULL)
    rtfm_window_set_library (self, rtfm_library_get_default ());
}

static void
rtfm_window_finalize (GObject *object)
{
  RtfmWindow *self = (RtfmWindow *)object;

  g_clear_object (&self->library);

  G_OBJECT_CLASS (rtfm_window_parent_class)->finalize (object);
}

static void
rtfm_window_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  RtfmWindow *self = RTFM_WINDOW (object);

  switch (prop_id)
    {
    case PROP_LIBRARY:
      g_value_set_object (value, rtfm_window_get_library (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_window_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  RtfmWindow *self = RTFM_WINDOW (object);

  switch (prop_id)
    {
    case PROP_LIBRARY:
      rtfm_window_set_library (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_window_class_init (RtfmWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->constructed = rtfm_window_constructed;
  object_class->finalize = rtfm_window_finalize;
  object_class->get_property = rtfm_window_get_property;
  object_class->set_property = rtfm_window_set_property;

  properties [PROP_LIBRARY] =
    g_param_spec_object ("library",
                         "Library",
                         "Library",
                         RTFM_TYPE_LIBRARY,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/rtfm/ui/rtfm-window.ui");
  gtk_widget_class_bind_template_child (widget_class, RtfmWindow, path_bar);
  gtk_widget_class_bind_template_child (widget_class, RtfmWindow, sidebar);
  gtk_widget_class_bind_template_child (widget_class, RtfmWindow, view);

  rtfm_gtk_widget_class_set_css_from_resource (widget_class,
                                               NULL,
                                               "/org/gnome/rtfm/css/rtfm-window.css");
}

static void
rtfm_window_init (RtfmWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  g_signal_connect_object (self->sidebar,
                           "item-activated",
                           G_CALLBACK (rtfm_window_sidebar_item_activated),
                           self,
                           G_CONNECT_SWAPPED);

  g_signal_connect_object (self->path_bar,
                           "element-selected",
                           G_CALLBACK (rtfm_window_path_bar_element_selected),
                           self,
                           G_CONNECT_SWAPPED);
}

GtkWidget *
rtfm_window_new (void)
{
  return g_object_new (RTFM_TYPE_WINDOW, NULL);
}
