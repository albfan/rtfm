/* rtfm-search-settings.c
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

#define G_LOG_DOMAIN "rtfm-search-settings"

#include "rtfm-search-settings.h"

struct _RtfmSearchSettings
{
  GObject parent_instance;
  gchar  *search_text;
};

G_DEFINE_TYPE (RtfmSearchSettings, rtfm_search_settings, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_SEARCH_TEXT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

RtfmSearchSettings *
rtfm_search_settings_new (void)
{
  return g_object_new (RTFM_TYPE_SEARCH_SETTINGS, NULL);
}

const gchar *
rtfm_search_settings_get_search_text (RtfmSearchSettings *self)
{
  g_return_val_if_fail (RTFM_IS_SEARCH_SETTINGS (self), NULL);

  return self->search_text;
}

void
rtfm_search_settings_set_search_text (RtfmSearchSettings *self,
                                      const gchar        *search_text)
{
  g_return_if_fail (RTFM_IS_SEARCH_SETTINGS (self));

  if (g_strcmp0 (search_text, self->search_text) != 0)
    {
      g_free (self->search_text);
      self->search_text = g_strdup (search_text);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_SEARCH_TEXT]);
    }
}

static void
rtfm_search_settings_finalize (GObject *object)
{
  RtfmSearchSettings *self = (RtfmSearchSettings *)object;

  g_clear_pointer (&self->search_text, g_free);

  G_OBJECT_CLASS (rtfm_search_settings_parent_class)->finalize (object);
}

static void
rtfm_search_settings_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  RtfmSearchSettings *self = RTFM_SEARCH_SETTINGS (object);

  switch (prop_id)
    {
    case PROP_SEARCH_TEXT:
      g_value_set_string (value, rtfm_search_settings_get_search_text (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_settings_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  RtfmSearchSettings *self = RTFM_SEARCH_SETTINGS (object);

  switch (prop_id)
    {
    case PROP_SEARCH_TEXT:
      rtfm_search_settings_set_search_text (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_settings_class_init (RtfmSearchSettingsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_search_settings_finalize;
  object_class->get_property = rtfm_search_settings_get_property;
  object_class->set_property = rtfm_search_settings_set_property;

  properties [PROP_SEARCH_TEXT] =
    g_param_spec_string ("search-text",
                         "Search Text",
                         "Search Text",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_search_settings_init (RtfmSearchSettings *self)
{
}
