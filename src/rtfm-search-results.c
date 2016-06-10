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

#include "rtfm-search-results.h"

struct _RtfmSearchResults
{
  GObject    parent_instance;
  GPtrArray *results;
};

G_DEFINE_TYPE (RtfmSearchResults, rtfm_search_results, G_TYPE_OBJECT)

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
}

RtfmSearchResults *
rtfm_search_results_new (void)
{
  return g_object_new (RTFM_TYPE_SEARCH_RESULTS, NULL);
}
