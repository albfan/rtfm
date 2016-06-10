/* fuzzy-index-builder.h
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

#ifndef FUZZY_INDEX_BUILDER_H
#define FUZZY_INDEX_BUILDER_H

#include <gio/gio.h>

G_BEGIN_DECLS

#define FUZZY_TYPE_INDEX_BUILDER (fuzzy_index_builder_get_type())

G_DECLARE_FINAL_TYPE (FuzzyIndexBuilder, fuzzy_index_builder, FUZZY, INDEX_BUILDER, GObject)

FuzzyIndexBuilder *fuzzy_index_builder_new          (void);
guint64            fuzzy_index_builder_insert       (FuzzyIndexBuilder    *self,
                                                     const gchar          *key,
                                                     GVariant             *document);
gboolean           fuzzy_index_builder_write        (FuzzyIndexBuilder    *self,
                                                     GFile                *file,
                                                     gint                  io_priority,
                                                     GCancellable         *cancellable,
                                                     GError              **error);
void               fuzzy_index_builder_write_async  (FuzzyIndexBuilder    *self,
                                                     GFile                *file,
                                                     gint                  io_priority,
                                                     GCancellable         *cancellable,
                                                     GAsyncReadyCallback   callback,
                                                     gpointer              user_data);
gboolean           fuzzy_index_builder_write_finish (FuzzyIndexBuilder    *self,
                                                     GAsyncResult         *result,
                                                     GError              **error);
const GVariant    *fuzzy_index_builder_get_document (FuzzyIndexBuilder    *self,
                                                     guint64               document_id);
void               fuzzy_index_builder_set_metadata (FuzzyIndexBuilder    *self,
                                                     const gchar          *key,
                                                     GVariant             *value);
void               fuzzy_index_builder_set_metadata_string
                                                    (FuzzyIndexBuilder    *self,
                                                     const gchar          *key,
                                                     const gchar          *value);
void               fuzzy_index_builder_set_metadata_uint64
                                                    (FuzzyIndexBuilder    *self,
                                                     const gchar          *key,
                                                     guint64               value);

G_END_DECLS

#endif /* FUZZY_INDEX_BUILDER_H */
