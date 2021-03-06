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

#include "rtfm-application.h"

gint
main (gint   argc,
      gchar *argv[])
{
  static const gchar *focus_search_accels[] = { "<ctrl>k", NULL };
  RtfmApplication *app;
  gint ret;

  g_set_prgname ("rtfm");
  g_set_application_name ("Rtfm");

  app = rtfm_application_new ();
  gtk_application_set_accels_for_action (GTK_APPLICATION (app),
                                         "win.focus-search",
                                         focus_search_accels);
  ret = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return ret;
}
