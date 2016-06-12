/* fuzzy-index-private.h
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

#ifndef FUZZY_INDEX_PRIVATE_H
#define FUZZY_INDEX_PRIVATE_H

#include "fuzzy-index.h"

G_BEGIN_DECLS

GVariant    *_fuzzy_index_lookup_document (FuzzyIndex *self,
                                           guint       document_id);
gboolean     _fuzzy_index_resolve         (FuzzyIndex   *self,
                                           guint         lookaside_id,
                                           guint        *document_id,
                                           const gchar **key);

G_END_DECLS

#endif /* FUZZY_INDEX_PRIVATE_H */
