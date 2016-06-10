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
  GSequence *results;
};

static void list_model_iface_init (GListModelInterface *iface);

G_DEFINE_TYPE_EXTENDED (RtfmSearchResults, rtfm_search_results, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, list_model_iface_init))

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

  g_clear_pointer (&self->results, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_search_results_parent_class)->finalize (object);
}

static void
rtfm_search_results_class_init (RtfmSearchResultsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_search_results_finalize;
}

static void
rtfm_search_results_init (RtfmSearchResults *self)
{
  self->results = g_sequence_new (g_object_unref);
}

RtfmSearchResults *
rtfm_search_results_new (void)
{
  return g_object_new (RTFM_TYPE_SEARCH_RESULTS, NULL);
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

  return g_sequence_get_length (self->results);
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
