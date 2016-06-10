/* rtfm-search-result.h
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

#ifndef RTFM_SEARCH_RESULT_H
#define RTFM_SEARCH_RESULT_H

#include "rtfm-types.h"

G_BEGIN_DECLS

struct _RtfmSearchResultClass
{
  GObjectClass parent_class;

  RtfmItem (*load) (RtfmSearchResult *self);

  gpointer _reserved1;
  gpointer _reserved2;
  gpointer _reserved3;
  gpointer _reserved4;
  gpointer _reserved5;
  gpointer _reserved6;
  gpointer _reserved7;
  gpointer _reserved8;
};

RtfmSearchResult *rtfm_search_result_new          (void);
RtfmItem         *rtfm_search_result_load         (RtfmSearchResult *self);
const gchar      *rtfm_search_result_get_category (RtfmSearchResult *self);
void              rtfm_search_result_set_category (RtfmSearchResult *self,
                                                   const gchar      *category);
gfloat            rtfm_search_result_get_score    (RtfmSearchResult *self);
void              rtfm_search_result_set_score    (RtfmSearchResult *self,
                                                   gfloat            score);
const gchar      *rtfm_search_result_get_text     (RtfmSearchResult *self);
void              rtfm_search_result_set_text     (RtfmSearchResult *self,
                                                   const gchar      *text);

G_END_DECLS

#endif /* RTFM_SEARCH_RESULT_H */
