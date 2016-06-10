/* fuzzy-index.h
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

#ifndef FUZZY_INDEX_H
#define FUZZY_INDEX_H

#include <gio/gio.h>

G_BEGIN_DECLS

#define FUZZY_TYPE_INDEX (fuzzy_index_get_type())

G_DECLARE_FINAL_TYPE (FuzzyIndex, fuzzy_index, FUZZY, INDEX, GObject)

FuzzyIndex  *fuzzy_index_new                 (void);
gboolean     fuzzy_index_load_file           (FuzzyIndex           *self,
                                              GFile                *file,
                                              GCancellable         *cancellable,
                                              GError              **error);
void         fuzzy_index_load_file_async     (FuzzyIndex           *self,
                                              GFile                *file,
                                              GCancellable         *cancellable,
                                              GAsyncReadyCallback   callback,
                                              gpointer              user_data);
gboolean     fuzzy_index_load_file_finish    (FuzzyIndex           *self,
                                              GAsyncResult         *result,
                                              GError              **error);
void         fuzzy_index_query_async         (FuzzyIndex           *self,
                                              const gchar          *query,
                                              guint                 max_matches,
                                              GCancellable         *cancellable,
                                              GAsyncReadyCallback   callback,
                                              gpointer              user_data);
GListModel  *fuzzy_index_query_finish        (FuzzyIndex           *self,
                                              GAsyncResult         *result,
                                              GError              **error);
GVariant    *fuzzy_index_get_metadata        (FuzzyIndex           *self,
                                              const gchar          *key);
guint64      fuzzy_index_get_metadata_uint64 (FuzzyIndex           *self,
                                              const gchar          *key);
const gchar *fuzzy_index_get_metadata_string (FuzzyIndex           *self,
                                              const gchar          *key);

G_END_DECLS

#endif /* FUZZY_INDEX_H */
