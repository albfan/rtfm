/* rtfm-search-view.c
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

#define G_LOG_DOMAIN "rtfm-search-view"

#include "rtfm-search-results.h"
#include "rtfm-search-view.h"

struct _RtfmSearchView
{
  GtkBin             parent_instance;
  RtfmSearchResults *search_results;
};

G_DEFINE_TYPE (RtfmSearchView, rtfm_search_view, GTK_TYPE_BIN)

enum {
  PROP_0,
  PROP_SEARCH_RESULTS,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
rtfm_search_view_connect (RtfmSearchView *self)
{
  g_return_if_fail (RTFM_IS_SEARCH_VIEW (self));

}

static void
rtfm_search_view_disconnect (RtfmSearchView *self)
{
  g_return_if_fail (RTFM_IS_SEARCH_VIEW (self));

}

static void
rtfm_search_view_finalize (GObject *object)
{
  RtfmSearchView *self = (RtfmSearchView *)object;

  g_clear_object (&self->search_results);

  G_OBJECT_CLASS (rtfm_search_view_parent_class)->finalize (object);
}

static void
rtfm_search_view_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  RtfmSearchView *self = RTFM_SEARCH_VIEW (object);

  switch (prop_id)
    {
    case PROP_SEARCH_RESULTS:
      g_value_set_object (value, rtfm_search_view_get_search_results (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_view_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  RtfmSearchView *self = RTFM_SEARCH_VIEW (object);

  switch (prop_id)
    {
    case PROP_SEARCH_RESULTS:
      rtfm_search_view_set_search_results (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_view_class_init (RtfmSearchViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = rtfm_search_view_finalize;
  object_class->get_property = rtfm_search_view_get_property;
  object_class->set_property = rtfm_search_view_set_property;

  properties [PROP_SEARCH_RESULTS] =
    g_param_spec_object ("search-results",
                         "Search Results",
                         "Search Results",
                         RTFM_TYPE_SEARCH_RESULTS,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/rtfm/ui/rtfm-search-view.ui");
}

static void
rtfm_search_view_init (RtfmSearchView *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

GtkWidget *
rtfm_search_view_new (void)
{
  return g_object_new (RTFM_TYPE_SEARCH_VIEW, NULL);
}

/**
 * rtfm_search_view_get_search_results:
 * @self: A #RtfmSearchResults
 *
 * Gets the search results being viewed.
 *
 * Returns: (nullable) (transfer none): An #RtfmSearchResults or %NULL.
 */
RtfmSearchResults *
rtfm_search_view_get_search_results (RtfmSearchView *self)
{
  g_return_val_if_fail (RTFM_IS_SEARCH_VIEW (self), NULL);

  return self->search_results;
}

void
rtfm_search_view_set_search_results (RtfmSearchView    *self,
                                     RtfmSearchResults *search_results)
{
  g_return_if_fail (RTFM_IS_SEARCH_VIEW (self));
  g_return_if_fail (RTFM_IS_SEARCH_RESULTS (search_results));

  if (search_results != self->search_results)
    {
      if (self->search_results != NULL)
        {
          rtfm_search_view_disconnect (self);
          g_clear_object (&self->search_results);
        }

      if (search_results != NULL)
        {
          self->search_results = g_object_ref (search_results);
          rtfm_search_view_connect (self);
        }

      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_SEARCH_RESULTS]);
    }
}