/* rtfm-application.c
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

#define G_LOG_DOMAIN "rtfm-application"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <glib/gi18n.h>

#include "rtfm-application.h"
#include "rtfm-library.h"
#include "rtfm-search-result.h"
#include "rtfm-search-results.h"
#include "rtfm-search-settings.h"
#include "rtfm-window.h"

struct _RtfmApplication
{
  GtkApplication parent;
};

G_DEFINE_TYPE (RtfmApplication, rtfm_application, GTK_TYPE_APPLICATION)

static void
rtfm_application_activate (GApplication *app)
{
  GtkApplication *application = (GtkApplication *)app;
  GtkWindow *window;
  GList *windows;

  g_assert (GTK_IS_APPLICATION (application));

  windows = gtk_application_get_windows (application);

  for (; windows; windows = windows->next)
    {
      window = windows->data;

      if (RTFM_IS_WINDOW (window))
        {
          gtk_window_present (window);
          return;
        }
    }

  window = g_object_new (RTFM_TYPE_WINDOW,
                         "application", application,
                         "default-width", 1000,
                         "default-height", 600,
                         NULL);

  gtk_window_present (window);
}

static void
rtfm_application_search_by_keyword_cb (GObject      *object,
                                       GAsyncResult *result,
                                       gpointer      user_data)
{
  RtfmLibrary *library = (RtfmLibrary *)object;
  GMainLoop *main_loop = user_data;
  g_autoptr(GError) error = NULL;

  g_assert (RTFM_IS_LIBRARY (library));

  if (!rtfm_library_search_finish (library, result, &error))
    g_printerr ("%s\n", error->message);

  g_main_loop_quit (main_loop);
}

static void
rtfm_application_search_by_keyword (RtfmApplication *self,
                                    const gchar     *keyword)
{
  g_autoptr(GMainLoop) main_loop = NULL;
  g_autoptr(RtfmSearchResults) search_results = NULL;
  g_autoptr(RtfmSearchSettings) search_settings = NULL;
  g_autoptr(GTimer) timer = NULL;
  RtfmLibrary *library;
  guint i;
  guint n_items;

  g_assert (RTFM_IS_APPLICATION (self));
  g_assert (keyword != NULL);

  main_loop = g_main_loop_new (NULL, FALSE);
  library = rtfm_library_get_default ();

  search_settings = rtfm_search_settings_new ();
  rtfm_search_settings_set_search_text (search_settings, keyword);

  search_results = rtfm_search_results_new (0);
  timer = g_timer_new ();

  rtfm_library_search_async (library,
                             search_settings,
                             search_results,
                             NULL,
                             rtfm_application_search_by_keyword_cb,
                             main_loop);

  g_main_loop_run (main_loop);

  n_items = g_list_model_get_n_items (G_LIST_MODEL (search_results));

  for (i = 0; i < n_items; i++)
    {
      g_autoptr(RtfmSearchResult) result = NULL;
      const gchar *text;
      gfloat score;

      result = g_list_model_get_item (G_LIST_MODEL (search_results), i);
      text = rtfm_search_result_get_text (result);
      score = rtfm_search_result_get_score (result);

      g_print ("%s (%f)\n", text, score);
    }

  g_print ("%u results in %lf seconds.\n",
           n_items, g_timer_elapsed (timer, NULL));
}

static gboolean
rtfm_application_local_command_line (GApplication   *application,
                                     gchar        ***arguments,
                                     gint           *exit_status)
{
  g_autoptr(GOptionContext) context = NULL;
  g_autoptr(GError) error = NULL;
  g_autofree gchar *keyword = NULL;
  gint argc;

  const GOptionEntry entries[] = {
    { "keyword", 'k', 0, G_OPTION_ARG_STRING, &keyword },
    { NULL }
  };

  g_assert (G_IS_APPLICATION (application));
  g_assert (arguments != NULL);
  g_assert (exit_status != NULL);

  argc = g_strv_length (*arguments);

  context = g_option_context_new (_("- Read the Fine Manual"));
  g_option_context_set_help_enabled (context, FALSE);
  g_option_context_set_ignore_unknown_options (context, TRUE);
  g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);

  if (!g_option_context_parse (context, &argc, arguments, &error))
    {
      g_printerr ("%s\n", error->message);
      return TRUE;
    }

  if (keyword != NULL)
    {
      rtfm_application_search_by_keyword (RTFM_APPLICATION (application), keyword);
      *exit_status = 0;
      return TRUE;
    }

  return G_APPLICATION_CLASS (rtfm_application_parent_class)->local_command_line (application, arguments, exit_status);
}

static void
rtfm_application_class_init (RtfmApplicationClass *klass)
{
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

  app_class->activate = rtfm_application_activate;
  app_class->local_command_line = rtfm_application_local_command_line;
}

static void
rtfm_application_init (RtfmApplication *self)
{
}

RtfmApplication *
rtfm_application_new (void)
{
  return g_object_new (RTFM_TYPE_APPLICATION,
                       "application-id", "org.gnome.Rtfm",
                       "resource-base-path", "/org/gnome/rtfm",
                       NULL);
}
