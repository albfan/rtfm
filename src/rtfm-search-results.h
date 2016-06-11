/* rtfm-search-results.h
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

#ifndef RTFM_SEARCH_RESULTS_H
#define RTFM_SEARCH_RESULTS_H

#include <gio/gio.h>

#include "rtfm-types.h"

G_BEGIN_DECLS

RtfmSearchResults *rtfm_search_results_new                (guint              max_results);
guint              rtfm_search_results_get_max_results    (RtfmSearchResults *self);
void               rtfm_search_results_set_max_results    (RtfmSearchResults *self,
                                                           guint              max_results);
void               rtfm_search_results_add                (RtfmSearchResults *self,
                                                           RtfmSearchResult  *result);
gboolean           rtfm_search_results_accepts_with_score (RtfmSearchResults *self,
                                                           gfloat             score);

G_END_DECLS

#endif /* RTFM_SEARCH_RESULTS_H */
