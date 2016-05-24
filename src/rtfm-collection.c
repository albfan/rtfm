/* rtfm-collection.c
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

#define G_LOG_DOMAIN "rtfm-collection"

#include "rtfm-collection.h"

struct _RtfmCollection
{
  GObject    parent_instance;
  GSequence *items;
};

static void list_model_iface_init (GListModelInterface *iface);

G_DEFINE_TYPE_EXTENDED (RtfmCollection, rtfm_collection, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, list_model_iface_init))

RtfmCollection *
rtfm_collection_new (void)
{
  return g_object_new (RTFM_TYPE_COLLECTION, NULL);
}

static void
rtfm_collection_finalize (GObject *object)
{
  RtfmCollection *self = (RtfmCollection *)object;

  g_clear_pointer (&self->items, g_sequence_free);

  G_OBJECT_CLASS (rtfm_collection_parent_class)->finalize (object);
}

static void
rtfm_collection_class_init (RtfmCollectionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_collection_finalize;
}

static void
rtfm_collection_init (RtfmCollection *self)
{
  self->items = g_sequence_new (g_object_unref);
}

static GType
rtfm_collection_get_item_type (GListModel *model)
{
  return RTFM_TYPE_ITEM;
}

static guint
rtfm_collection_get_n_items (GListModel *model)
{
  RtfmCollection *self = (RtfmCollection *)model;

  g_assert (RTFM_IS_COLLECTION (self));

  return g_sequence_get_length (self->items);
}

static gpointer
rtfm_collection_get_item (GListModel *model,
                          guint       position)
{
  RtfmCollection *self = (RtfmCollection *)model;
  GSequenceIter *iter;

  g_assert (RTFM_IS_COLLECTION (self));

  iter = g_sequence_get_iter_at_pos (self->items, position);

  if G_LIKELY (!g_sequence_iter_is_end (iter))
    return g_object_ref (g_sequence_get (iter));

  g_critical ("Implausable result from RtfmCollection");

  return NULL;
}

static void
list_model_iface_init (GListModelInterface *iface)
{
  iface->get_item_type = rtfm_collection_get_item_type;
  iface->get_n_items = rtfm_collection_get_n_items;
  iface->get_item = rtfm_collection_get_item;
}

void
rtfm_collection_add (RtfmCollection *self,
                     RtfmItem       *item)
{
  guint position;

  g_return_if_fail (RTFM_IS_COLLECTION (self));
  g_return_if_fail (RTFM_IS_ITEM (item));

  position = g_sequence_get_length (self->items);
  g_sequence_append (self->items, g_object_ref (item));
  g_list_model_items_changed (G_LIST_MODEL (self), position, 0, 1);
}
