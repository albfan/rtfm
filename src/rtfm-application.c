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

#include "rtfm-application.h"
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
rtfm_application_class_init (RtfmApplicationClass *klass)
{
  GApplicationClass *app_class = G_APPLICATION_CLASS (klass);

  app_class->activate = rtfm_application_activate;
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
