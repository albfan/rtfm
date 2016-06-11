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

#include "rtfm-item.h"
#include "rtfm-library.h"
#include "rtfm-path.h"
#include "rtfm-path-element.h"
#include "rtfm-search-results.h"
#include "rtfm-search-settings.h"
#include "rtfm-search-view.h"
#include "rtfm-sidebar.h"
#include "rtfm-view.h"
#include "rtfm-widget.h"
#include "rtfm-window.h"

#define SEARCH_TIMEOUT_DELAY 50

struct _RtfmWindow
{
  GtkApplicationWindow  parent_instance;

  RtfmLibrary          *library;

  /* Active Search */
  RtfmSearchResults    *search_results;
  RtfmSearchSettings   *search_settings;
  GCancellable         *search_cancellable;

  GSimpleAction        *focus_search;
  GtkPaned             *paned;
  GtkSearchEntry       *search_entry;
  RtfmSearchView       *search_view;
  RtfmSidebar          *sidebar;
  GtkStack             *stack;
  RtfmView             *view;

  guint                 active_search_count;
  guint                 queued_search_timeout;
};

typedef enum
{
  RTFM_WINDOW_MODE_BROWSE,
  RTFM_WINDOW_MODE_SEARCH,
} RtfmWindowMode;

G_DEFINE_TYPE (RtfmWindow, rtfm_window, GTK_TYPE_APPLICATION_WINDOW)

enum {
  PROP_0,
  PROP_LIBRARY,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
rtfm_window_cancel_search (RtfmWindow *self)
{
  g_assert (RTFM_IS_WINDOW (self));

  if (self->search_cancellable != NULL)
    {
      if (!g_cancellable_is_cancelled (self->search_cancellable))
        g_cancellable_cancel (self->search_cancellable);
    }
}

static void
rtfm_window_set_mode (RtfmWindow     *self,
                      RtfmWindowMode  mode)
{
  g_return_if_fail (RTFM_IS_WINDOW (self));
  g_return_if_fail (mode == RTFM_WINDOW_MODE_BROWSE ||
                    mode == RTFM_WINDOW_MODE_SEARCH);

  switch (mode)
    {
    case RTFM_WINDOW_MODE_BROWSE:
      if (self->active_search_count > 0)
        rtfm_window_cancel_search (self);
      gtk_stack_set_visible_child (self->stack, GTK_WIDGET (self->paned));
      break;

    case RTFM_WINDOW_MODE_SEARCH:
      gtk_stack_set_visible_child (self->stack, GTK_WIDGET (self->search_view));
      break;

    default:
      g_assert_not_reached ();
      break;
    }
}

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
      rtfm_view_set_item (self->view, NULL);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_LIBRARY]);
    }
}

static void
rtfm_window_sidebar_item_activated (RtfmWindow  *self,
                                    RtfmItem    *item,
                                    RtfmSidebar *sidebar)
{
  g_assert (RTFM_IS_WINDOW (self));
  g_assert (RTFM_IS_ITEM (item));
  g_assert (RTFM_IS_SIDEBAR (sidebar));

  rtfm_view_set_item (self->view, item);
}

static void
rtfm_window_action_focus_search (GSimpleAction *action,
                                 GVariant      *param,
                                 gpointer       user_data)
{
  RtfmWindow *self = user_data;

  g_assert (RTFM_IS_WINDOW (self));
  g_assert (G_IS_SIMPLE_ACTION (action));

  gtk_widget_grab_focus (GTK_WIDGET (self->search_entry));
}

static void
rtfm_window_search_cb (GObject      *object,
                       GAsyncResult *result,
                       gpointer      user_data)
{
  g_autoptr(RtfmWindow) self = user_data;
  g_autoptr(GError) error = NULL;
  RtfmLibrary *library = (RtfmLibrary *)object;

  g_assert (RTFM_IS_LIBRARY (library));
  g_assert (RTFM_IS_WINDOW (self));
  g_assert (G_IS_ASYNC_RESULT (result));

  self->active_search_count--;

  if (!rtfm_library_search_finish (library, result, &error))
    {
      if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        g_message ("%s", error->message);
      return;
    }
}

static gboolean
rtfm_window_do_search (gpointer user_data)
{
  RtfmWindow *self = user_data;
  const gchar *text;

  g_assert (RTFM_IS_WINDOW (self));

  self->queued_search_timeout = 0;

  /* Check if we are searching for something new */
  text = gtk_entry_get_text (GTK_ENTRY (self->search_entry));
  if (text == NULL || *text == '\0')
    goto change_page;

  self->search_cancellable = g_cancellable_new ();
  self->search_results = rtfm_search_results_new (25);

  rtfm_search_settings_set_search_text (self->search_settings, text);

  self->active_search_count++;

  rtfm_library_search_async (self->library,
                             self->search_settings,
                             self->search_results,
                             self->search_cancellable,
                             rtfm_window_search_cb,
                             g_object_ref (self));

  rtfm_search_view_set_search_results (self->search_view, self->search_results);

change_page:
  if (text == NULL || *text == '\0')
    rtfm_window_set_mode (self, RTFM_WINDOW_MODE_BROWSE);
  else
    rtfm_window_set_mode (self, RTFM_WINDOW_MODE_SEARCH);

  return G_SOURCE_REMOVE;
}

static void
rtfm_window_queue_search_query (RtfmWindow *self)
{
  g_assert (RTFM_IS_WINDOW (self));

  if (self->queued_search_timeout == 0)
    self->queued_search_timeout =
      g_timeout_add (SEARCH_TIMEOUT_DELAY, rtfm_window_do_search, self);
}

static void
rtfm_window_search_entry_changed (RtfmWindow     *self,
                                  GtkSearchEntry *search_entry)
{
  g_assert (RTFM_IS_WINDOW (self));
  g_assert (GTK_IS_SEARCH_ENTRY (search_entry));

  /* Cancel any active search request */
  rtfm_window_cancel_search (self);

  /* Clear state from previous search requests */
  g_clear_object (&self->search_results);
  g_clear_object (&self->search_cancellable);

  /* Queue a request to begin the search so we can coalesce
   * searches within a short period of time.
   */
  rtfm_window_queue_search_query (self);
}

static void
rtfm_window_search_entry_stop_search (RtfmWindow     *self,
                                      GtkSearchEntry *search_entry)
{
  g_assert (RTFM_IS_WINDOW (self));
  g_assert (GTK_IS_SEARCH_ENTRY (search_entry));

  if (self->active_search_count == 0)
    rtfm_window_set_mode (self, RTFM_WINDOW_MODE_BROWSE);
  else
    rtfm_window_cancel_search (self);
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

  if (self->queued_search_timeout != 0)
    {
      g_source_remove (self->queued_search_timeout);
      self->queued_search_timeout = 0;
    }

  g_clear_object (&self->library);

  g_clear_object (&self->search_cancellable);
  g_clear_object (&self->search_results);
  g_clear_object (&self->search_settings);

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

  gtk_widget_class_bind_template_child (widget_class, RtfmWindow, paned);
  gtk_widget_class_bind_template_child (widget_class, RtfmWindow, search_entry);
  gtk_widget_class_bind_template_child (widget_class, RtfmWindow, search_view);
  gtk_widget_class_bind_template_child (widget_class, RtfmWindow, sidebar);
  gtk_widget_class_bind_template_child (widget_class, RtfmWindow, stack);
  gtk_widget_class_bind_template_child (widget_class, RtfmWindow, view);

  gtk_widget_class_bind_template_callback (widget_class, rtfm_window_sidebar_item_activated);
  gtk_widget_class_bind_template_callback (widget_class, rtfm_window_search_entry_changed);
  gtk_widget_class_bind_template_callback (widget_class, rtfm_window_search_entry_stop_search);

  rtfm_gtk_widget_class_set_css_from_resource (widget_class,
                                               NULL,
                                               "/org/gnome/rtfm/css/rtfm-window.css");
}

static void
rtfm_window_init (RtfmWindow *self)
{
  static GActionEntry actions[] = {
    { "focus-search", rtfm_window_action_focus_search },
  };

  gtk_widget_init_template (GTK_WIDGET (self));

  self->search_settings = rtfm_search_settings_new ();

  g_action_map_add_action_entries (G_ACTION_MAP (self),
                                   actions,
                                   G_N_ELEMENTS (actions),
                                   self);
}

GtkWidget *
rtfm_window_new (void)
{
  return g_object_new (RTFM_TYPE_WINDOW, NULL);
}
