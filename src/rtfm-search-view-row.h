/* rtfm-search-view-row.h
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

#ifndef RTFM_SEARCH_VIEW_ROW_H
#define RTFM_SEARCH_VIEW_ROW_H

#include <gtk/gtk.h>

#include "rtfm-search-result.h"

G_BEGIN_DECLS

#define RTFM_TYPE_SEARCH_VIEW_ROW (rtfm_search_view_row_get_type())

G_DECLARE_FINAL_TYPE (RtfmSearchViewRow, rtfm_search_view_row, RTFM, SEARCH_VIEW_ROW, GtkListBoxRow)

RtfmSearchResult *rtfm_search_view_row_get_result (RtfmSearchViewRow *self);
void              rtfm_search_view_row_set_result (RtfmSearchViewRow *self,
                                                   RtfmSearchResult  *result);

G_END_DECLS

#endif /* RTFM_SEARCH_VIEW_ROW_H */

