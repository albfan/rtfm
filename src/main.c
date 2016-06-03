/* main.c
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

#include <gtk/gtk.h>

#include "rtfm-window.h"

static void
activate_cb (GtkApplication *application,
             gpointer        user_data)
{
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

gint
main (gint   argc,
      gchar *argv[])
{
  static const gchar *focus_search_accels[] = { "<ctrl>k", NULL };
  GApplication *app;
  gint ret;

  g_set_prgname ("rtfm");
  g_set_application_name ("Rtfm");

  app = g_object_new (GTK_TYPE_APPLICATION,
                      "application-id", "org.gnome.Rtfm",
                      NULL);
  gtk_application_set_accels_for_action (GTK_APPLICATION (app),
                                         "win.focus-search",
                                         focus_search_accels);
  g_signal_connect (app, "activate", G_CALLBACK (activate_cb), NULL);
  ret = g_application_run (app, argc, argv);
  g_object_unref (app);

  return ret;
}
