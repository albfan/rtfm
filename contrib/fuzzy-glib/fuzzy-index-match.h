/* fuzzy-index-match.h
 *
 * Copyright (C) 2016 Christian Hergert <christian@hergert.me>
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

#ifndef FUZZY_INDEX_MATCH_H
#define FUZZY_INDEX_MATCH_H

#include <gio/gio.h>

G_BEGIN_DECLS

#define FUZZY_TYPE_INDEX_MATCH (fuzzy_index_match_get_type())

G_DECLARE_FINAL_TYPE (FuzzyIndexMatch, fuzzy_index_match, FUZZY, INDEX_MATCH, GObject)

const gchar *fuzzy_index_match_get_key      (FuzzyIndexMatch *self);
GVariant    *fuzzy_index_match_get_document (FuzzyIndexMatch *self);
gfloat       fuzzy_index_match_get_score    (FuzzyIndexMatch *self);

G_END_DECLS

#endif /* FUZZY_INDEX_MATCH_H */
