/* rtfm-search-results.c
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

#define G_LOG_DOMAIN "rtfm-search-results"

#include "rtfm-search-result.h"
#include "rtfm-search-results.h"

struct _RtfmSearchResults
{
  GObject    parent_instance;
  guint      max_results;
  guint      n_items;
  GSequence *results;
};

static void list_model_iface_init (GListModelInterface *iface);

G_DEFINE_TYPE_EXTENDED (RtfmSearchResults, rtfm_search_results, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, list_model_iface_init))

enum {
  PROP_0,
  PROP_MAX_RESULTS,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static gint
compare_by_score (gconstpointer a,
                  gconstpointer b,
                  gpointer      user_data)
{
  RtfmSearchResult *result1 = (RtfmSearchResult *)a;
  RtfmSearchResult *result2 = (RtfmSearchResult *)b;
  gfloat score1 = rtfm_search_result_get_score (result1);
  gfloat score2 = rtfm_search_result_get_score (result2);

  /* Sort high to low */

  if (score1 < score2)
    return 1;
  else if (score1 > score2)
    return -1;
  else
    return 0;
}

static void
rtfm_search_results_do_add (RtfmSearchResults *self,
                            RtfmSearchResult  *search_result)
{
  GSequenceIter *iter;
  guint position;

  g_assert (RTFM_IS_SEARCH_RESULTS (self));
  g_assert (RTFM_IS_SEARCH_RESULT (search_result));

  /*
   * If we are already at our maximum number of items, we might be able to
   * just evict the lowest scoring item and replace it with this one.
   */
  if (self->max_results != 0 && self->n_items == self->max_results)
    {
      GSequenceIter *last_iter;
      RtfmSearchResult *lowest;
      gfloat score;
      gfloat lowest_score;

      last_iter = g_sequence_iter_prev (g_sequence_get_end_iter (self->results));
      lowest = g_sequence_get (last_iter);

      score = rtfm_search_result_get_score (search_result);
      lowest_score = rtfm_search_result_get_score (lowest);

      if (score > lowest_score)
        {
          g_sequence_set (last_iter, g_object_ref (search_result));
          g_list_model_items_changed (G_LIST_MODEL (self), self->n_items - 1, 1, 1);
        }

      return;
    }

  self->n_items++;

  iter = g_sequence_insert_sorted (self->results,
                                   g_object_ref (search_result),
                                   compare_by_score,
                                   NULL);

  position = g_sequence_iter_get_position (iter);

  g_list_model_items_changed (G_LIST_MODEL (self), position, 0, 1);
}

static void
rtfm_search_results_finalize (GObject *object)
{
  RtfmSearchResults *self = (RtfmSearchResults *)object;

  g_clear_pointer (&self->results, g_sequence_free);

  G_OBJECT_CLASS (rtfm_search_results_parent_class)->finalize (object);
}

static void
rtfm_search_results_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  RtfmSearchResults *self = RTFM_SEARCH_RESULTS (object);

  switch (prop_id)
    {
    case PROP_MAX_RESULTS:
      g_value_set_uint (value, rtfm_search_results_get_max_results (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_results_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  RtfmSearchResults *self = RTFM_SEARCH_RESULTS (object);

  switch (prop_id)
    {
    case PROP_MAX_RESULTS:
      rtfm_search_results_set_max_results (self, g_value_get_uint (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_results_class_init (RtfmSearchResultsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_search_results_finalize;
  object_class->get_property = rtfm_search_results_get_property;
  object_class->set_property = rtfm_search_results_set_property;

  properties [PROP_MAX_RESULTS] =
    g_param_spec_uint ("max-results",
                       "Max Results",
                       "The maximum number of results in the collection",
                       0,
                       G_MAXUINT,
                       0,
                       (G_PARAM_READWRITE |
                        G_PARAM_CONSTRUCT_ONLY |
                        G_PARAM_EXPLICIT_NOTIFY |
                        G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_search_results_init (RtfmSearchResults *self)
{
  self->results = g_sequence_new (g_object_unref);
}

RtfmSearchResults *
rtfm_search_results_new (guint max_results)
{
  return g_object_new (RTFM_TYPE_SEARCH_RESULTS,
                       "max-results", max_results,
                       NULL);
}

static GType
rtfm_search_results_get_item_type (GListModel *model)
{
  return RTFM_TYPE_SEARCH_RESULT;
}

static guint
rtfm_search_results_get_n_items (GListModel *model)
{
  RtfmSearchResults *self = (RtfmSearchResults *)model;

  g_assert (RTFM_IS_SEARCH_RESULTS (self));

  return self->n_items;
}

static gpointer
rtfm_search_results_get_item (GListModel *model,
                              guint       position)
{
  RtfmSearchResults *self = (RtfmSearchResults *)model;
  GSequenceIter *iter;

  g_assert (RTFM_IS_SEARCH_RESULTS (self));
  g_assert (position < (guint)g_sequence_get_length (self->results));

  iter = g_sequence_get_iter_at_pos (self->results, position);

  return g_object_ref (g_sequence_get (iter));
}

static void
list_model_iface_init (GListModelInterface *iface)
{
  iface->get_item_type = rtfm_search_results_get_item_type;
  iface->get_n_items = rtfm_search_results_get_n_items;
  iface->get_item = rtfm_search_results_get_item;
}

void
rtfm_search_results_add (RtfmSearchResults *self,
                         RtfmSearchResult  *search_result)
{
  g_return_if_fail (RTFM_IS_SEARCH_RESULTS (self));
  g_return_if_fail (RTFM_IS_SEARCH_RESULT (search_result));

  /*
   * TODO: To make it easier for threaded workers to add items, we
   *       need to defer the add into the main loop when the thread
   *       default context does not match our main context.
   */

  rtfm_search_results_do_add (self, search_result);
}

guint
rtfm_search_results_get_max_results (RtfmSearchResults *self)
{
  g_return_val_if_fail (RTFM_IS_SEARCH_RESULTS (self), 0);

  return self->max_results;
}

void
rtfm_search_results_set_max_results (RtfmSearchResults *self,
                                     guint              max_results)
{
  g_return_if_fail (RTFM_IS_SEARCH_RESULTS (self));

  if (max_results != self->max_results)
    {
      self->max_results = max_results;
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_MAX_RESULTS]);
    }
}
