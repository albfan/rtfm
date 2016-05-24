/* rtfm-widget.c
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

#define G_LOG_DOMAIN "rtfm-widget"

#include <string.h>

#include "rtfm-widget.h"

static GHashTable *class_to_themes;
static guint queued_reload;

static gboolean
theme_matches (GtkSettings *settings,
               const gchar *theme_name)
{
  g_autofree gchar *name = NULL;
  gboolean dark_theme = FALSE;

  g_assert (GTK_IS_SETTINGS (settings));
  g_assert (theme_name != NULL);

  g_object_get (settings,
                "gtk-theme-name", &name,
                "gtk-application-prefer-dark-theme", &dark_theme,
                NULL);

  if (g_strcmp0 (theme_name, "shared") == 0)
    return TRUE;

  if (dark_theme && g_str_has_suffix (name, "-dark"))
    return TRUE;
  else if (dark_theme)
    return FALSE;

  return g_str_has_prefix (name, theme_name);
}

static gboolean
do_reload (gpointer user_data)
{
  GtkSettings *settings;
  GdkScreen *screen;
  GHashTableIter iter;
  gpointer key, value;

  queued_reload = 0;

  settings = gtk_settings_get_default ();
  screen = gdk_screen_get_default ();

  g_hash_table_iter_init (&iter, class_to_themes);

  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      GHashTableIter theme_iter;
      GHashTable *themes = value;

      g_hash_table_iter_init (&theme_iter, themes);

      while (g_hash_table_iter_next (&theme_iter, &key, &value))
        {
          const gchar *theme_name = key;
          GtkStyleProvider *provider = value;

          if (theme_matches (settings, theme_name))
            gtk_style_context_add_provider_for_screen (screen, provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
          else
            gtk_style_context_remove_provider_for_screen (screen, provider);
        }
    }

  return G_SOURCE_REMOVE;
}

static void
queue_reload (void)
{
  if (queued_reload == 0)
    queued_reload = g_timeout_add (0, do_reload, NULL);
}

void
rtfm_gtk_widget_class_set_css_from_resource (GtkWidgetClass *widget_class,
                                             const gchar    *theme_name,
                                             const gchar    *resource_path)
{
  GtkSettings *settings;
  GdkScreen *screen;
  GHashTable *by_theme;
  GtkCssProvider *provider;

  g_return_if_fail (GTK_IS_WIDGET_CLASS (widget_class));
  g_return_if_fail (resource_path != NULL);

  if (theme_name == NULL)
    theme_name = "shared";

  /*
   * If screen is NULL, then we are probably doing GIR introspection
   * and we have not initialized the display. We don't need to do
   * anything in that case.
   */
  if (NULL == (screen = gdk_screen_get_default ()) ||
      NULL == (settings = gtk_settings_get_default ()))
    return;

  if (class_to_themes == NULL)
    {
      class_to_themes = g_hash_table_new_full (NULL, NULL, NULL,
                                               (GDestroyNotify)g_hash_table_unref);
      g_signal_connect (settings,
                        "notify::gtk-theme-name",
                        queue_reload,
                        NULL);
      g_signal_connect (settings,
                        "notify::gtk-application-prefer-dark-theme",
                        queue_reload,
                        NULL);
    }

  by_theme = g_hash_table_lookup (class_to_themes, theme_name);

  if (by_theme == NULL)
    {
      by_theme = g_hash_table_new_full (NULL, NULL, g_free, g_object_unref);
      g_hash_table_insert (class_to_themes, widget_class, by_theme);
    }

  provider = gtk_css_provider_new ();
  gtk_css_provider_load_from_resource (provider, resource_path);

  g_hash_table_insert (by_theme, g_strdup (theme_name), provider);

  queue_reload ();
}
