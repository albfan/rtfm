/* rtfm-search-settings.h
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

#ifndef RTFM_SEARCH_SETTINGS_H
#define RTFM_SEARCH_SETTINGS_H

#include "rtfm-types.h"

G_BEGIN_DECLS

RtfmSearchSettings *rtfm_search_settings_new             (void);
const gchar        *rtfm_search_settings_get_search_text (RtfmSearchSettings *self);
void                rtfm_search_settings_set_search_text (RtfmSearchSettings *self,
                                                          const gchar        *search_text);

G_END_DECLS

#endif /* RTFM_SEARCH_SETTINGS_H */

