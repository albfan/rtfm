/* rtfm-search-result.c
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

#define G_LOG_DOMAIN "rtfm-search-result"

#include <glib/gi18n.h>

#include "rtfm-search-result.h"

typedef struct
{
  gchar *category;
  gchar *text;
  gfloat score;
} RtfmSearchResultPrivate;

enum {
  PROP_0,
  PROP_CATEGORY,
  PROP_SCORE,
  PROP_TEXT,
  N_PROPS
};

enum {
  LOAD,
  N_SIGNALS
};

G_DEFINE_TYPE_WITH_PRIVATE (RtfmSearchResult, rtfm_search_result, G_TYPE_OBJECT)

static GParamSpec *properties [N_PROPS];
static guint signals [N_SIGNALS];

static void
rtfm_search_result_finalize (GObject *object)
{
  RtfmSearchResult *self = (RtfmSearchResult *)object;
  RtfmSearchResultPrivate *priv = rtfm_search_result_get_instance_private (self);

  g_clear_pointer (&priv->category, g_free);
  g_clear_pointer (&priv->text, g_free);

  G_OBJECT_CLASS (rtfm_search_result_parent_class)->finalize (object);
}

static void
rtfm_search_result_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  RtfmSearchResult *self = RTFM_SEARCH_RESULT (object);

  switch (prop_id)
    {
    case PROP_CATEGORY:
      g_value_set_string (value, rtfm_search_result_get_category (self));
      break;

    case PROP_SCORE:
      g_value_set_float (value, rtfm_search_result_get_score (self));
      break;

    case PROP_TEXT:
      g_value_set_string (value, rtfm_search_result_get_text (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_result_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  RtfmSearchResult *self = RTFM_SEARCH_RESULT (object);

  switch (prop_id)
    {
    case PROP_CATEGORY:
      rtfm_search_result_set_category (self, g_value_get_string (value));
      break;

    case PROP_SCORE:
      rtfm_search_result_set_score (self, g_value_get_float (value));
      break;

    case PROP_TEXT:
      rtfm_search_result_set_text (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_result_class_init (RtfmSearchResultClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_search_result_finalize;
  object_class->get_property = rtfm_search_result_get_property;
  object_class->set_property = rtfm_search_result_set_property;

  properties [PROP_CATEGORY] =
    g_param_spec_string ("category",
                         "Category",
                         "The search result category",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_SCORE] =
    g_param_spec_float ("score",
                        "Score",
                        "The search result score",
                        0,
                        G_MAXFLOAT,
                        0,
                        (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_TEXT] =
    g_param_spec_string ("text",
                         "Text",
                         "The search result text",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  signals [LOAD] =
    g_signal_new ("load",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (RtfmSearchResultClass, load),
                  NULL, NULL, NULL, G_TYPE_NONE, 0);
}

static void
rtfm_search_result_init (RtfmSearchResult *result)
{
}

const gchar *
rtfm_search_result_get_category (RtfmSearchResult *self)
{
  RtfmSearchResultPrivate *priv = rtfm_search_result_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_SEARCH_RESULT (self), NULL);

  return priv->category;
}

void
rtfm_search_result_set_category (RtfmSearchResult *self,
                                 const gchar      *category)
{
  RtfmSearchResultPrivate *priv = rtfm_search_result_get_instance_private (self);

  g_return_if_fail (RTFM_IS_SEARCH_RESULT (self));

  if (g_strcmp0 (category, priv->category) != 0)
    {
      g_free (priv->category);
      priv->category = g_strdup (category);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_CATEGORY]);
    }
}

gfloat
rtfm_search_result_get_score (RtfmSearchResult *self)
{
  RtfmSearchResultPrivate *priv = rtfm_search_result_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_SEARCH_RESULT (self), 0.0f);

  return priv->score;
}

void
rtfm_search_result_set_score (RtfmSearchResult *self,
                              gfloat            score)
{
  RtfmSearchResultPrivate *priv = rtfm_search_result_get_instance_private (self);

  g_return_if_fail (RTFM_IS_SEARCH_RESULT (self));

  if (score != priv->score)
    {
      priv->score = score;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_SCORE]);
    }
}

const gchar *
rtfm_search_result_get_text (RtfmSearchResult *self)
{
  RtfmSearchResultPrivate *priv = rtfm_search_result_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_SEARCH_RESULT (self), NULL);

  return priv->text;
}

void
rtfm_search_result_set_text (RtfmSearchResult *self,
                             const gchar      *text)
{
  RtfmSearchResultPrivate *priv = rtfm_search_result_get_instance_private (self);

  g_return_if_fail (RTFM_IS_SEARCH_RESULT (self));

  if (g_strcmp0 (text, priv->text) != 0)
    {
      g_free (priv->text);
      priv->text = g_strdup (text);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_TEXT]);
    }
}

/**
 * rtfm_search_result_load:
 * @self: A #RtfmSearchResult
 *
 * Requests that the search result load an item. This allows search
 * results to hold minimum state and only inflate the full item once
 * it has become necessary.
 *
 * Returns: (transfer full): An #RtfmItem.
 */
RtfmItem *
rtfm_search_result_load (RtfmSearchResult *self)
{
  RtfmItem *item = NULL;

  g_return_val_if_fail (RTFM_IS_SEARCH_RESULT (self), NULL);

  g_signal_emit (self, signals [LOAD], 0, &item);

  return item;
}
