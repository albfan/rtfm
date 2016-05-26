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
#include "rtfm-path.h"

struct _RtfmCollection
{
  GObject    parent_instance;
  RtfmPath  *path;
  GSequence *items;
};

static void list_model_iface_init (GListModelInterface *iface);

G_DEFINE_TYPE_EXTENDED (RtfmCollection, rtfm_collection, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, list_model_iface_init))

enum {
  PROP_0,
  PROP_PATH,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

RtfmCollection *
rtfm_collection_new (RtfmPath *path)
{
  g_return_val_if_fail (!path || RTFM_IS_PATH (path), NULL);

  return g_object_new (RTFM_TYPE_COLLECTION,
                       "path", path,
                       NULL);
}

/**
 * rtfm_collection_get_path:
 * @self: An #RtfmCollection
 *
 * Gets the path for the collection of items.
 *
 * Returns: (nullable) (transfer none): An #RtfmPath or %NULL.
 */
RtfmPath *
rtfm_collection_get_path (RtfmCollection *self)
{
  g_return_val_if_fail (RTFM_IS_COLLECTION (self), NULL);

  return self->path;
}

static void
rtfm_collection_finalize (GObject *object)
{
  RtfmCollection *self = (RtfmCollection *)object;

  g_clear_pointer (&self->items, g_sequence_free);
  g_clear_object (&self->path);

  G_OBJECT_CLASS (rtfm_collection_parent_class)->finalize (object);
}

static void
rtfm_collection_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  RtfmCollection *self = RTFM_COLLECTION (object);

  switch (prop_id)
    {
    case PROP_PATH:
      g_value_set_object (value, rtfm_collection_get_path (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_collection_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RtfmCollection *self = RTFM_COLLECTION (object);

  switch (prop_id)
    {
    case PROP_PATH:
      g_clear_object (&self->path);
      self->path = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_collection_class_init (RtfmCollectionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_collection_finalize;
  object_class->get_property = rtfm_collection_get_property;
  object_class->set_property = rtfm_collection_set_property;

  properties [PROP_PATH] =
    g_param_spec_object ("path",
                         "Path",
                         "Path",
                         RTFM_TYPE_PATH,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_collection_init (RtfmCollection *self)
{
  self->items = g_sequence_new (g_object_unref);
  self->path = rtfm_path_new ();
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
rtfm_collection_append (RtfmCollection *self,
                        RtfmItem       *item)
{
  guint position;

  g_return_if_fail (RTFM_IS_COLLECTION (self));
  g_return_if_fail (RTFM_IS_ITEM (item));

  position = g_sequence_get_length (self->items);
  g_sequence_append (self->items, g_object_ref (item));
  g_list_model_items_changed (G_LIST_MODEL (self), position, 0, 1);
}

void
rtfm_collection_prepend (RtfmCollection *self,
                         RtfmItem       *item)
{
  g_return_if_fail (RTFM_IS_COLLECTION (self));
  g_return_if_fail (RTFM_IS_ITEM (item));

  g_sequence_prepend (self->items, g_object_ref (item));
  g_list_model_items_changed (G_LIST_MODEL (self), 0, 0, 1);
}

void
rtfm_collection_remove_item (RtfmCollection *self,
                             RtfmItem       *item)
{
  GSequenceIter *iter;

  g_return_if_fail (RTFM_IS_COLLECTION (self));
  g_return_if_fail (RTFM_IS_ITEM (item));

  /*
   * TODO: If we know the collection is sorted, we can take shortcuts
   *       here. We should introduce simple sorting anyway.
   */

  for (iter = g_sequence_get_begin_iter (self->items);
       !g_sequence_iter_is_end (iter);
       iter = g_sequence_iter_next (iter))
    {
      RtfmItem *cursor = g_sequence_get (iter);

      if (cursor == item)
        {
          guint position = g_sequence_iter_get_position (iter);

          g_sequence_remove (iter);
          g_list_model_items_changed (G_LIST_MODEL (self), position, 1, 0);

          return;
        }
    }
}

/**
 * rtfm_collection_foreach:
 * @self: A #RtfmCollection.
 * @callback: (scope call): A callback for each item
 * @user_data: user data for @callback
 *
 * Calls @callback for each item in @self.
 */
void
rtfm_collection_foreach (RtfmCollection         *self,
                         RtfmCollectionCallback  callback,
                         gpointer                user_data)
{
  GSequenceIter *iter;
  GSequenceIter *next = NULL;

  g_return_if_fail (RTFM_IS_COLLECTION (self));
  g_return_if_fail (callback != NULL);

  for (iter = g_sequence_get_begin_iter (self->items);
       !g_sequence_iter_is_end (iter);
       iter = next)
    {
      next = g_sequence_iter_next (iter);
      callback (g_sequence_get (iter), user_data);
    }
}

/**
 * rtfm_collection_to_array:
 *
 * This function is mostly useful for language bindings and likely should
 * not be used from C code.
 *
 * Returns: (transfer container) (element-type Rtfm.Item): A #GPtrArray of
 *   #RtfmItem instances.
 */
GPtrArray *
rtfm_collection_to_array (RtfmCollection *self)
{
  GSequenceIter *iter;
  GPtrArray *ar;

  g_return_val_if_fail (RTFM_IS_COLLECTION (self), NULL);

  ar = g_ptr_array_new ();

  for (iter = g_sequence_get_begin_iter (self->items);
       !g_sequence_iter_is_end (iter);
       iter = g_sequence_iter_next (iter))
    g_ptr_array_add (ar, g_sequence_get (iter));

  return ar;
}
