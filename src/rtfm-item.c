/* rtfm-item.c
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

#define G_LOG_DOMAIN "rtfm-item"

#include "rtfm-item.h"
#include "rtfm-item-private.h"
#include "rtfm-path.h"
#include "rtfm-path-element.h"

#define GET_PRIVATE(t) ((RtfmItemPrivate *)rtfm_item_get_instance_private(t))

typedef struct
{
  /*
   * Basic information about the item.
   */
  gchar      *id;
  gchar      *title;
  gchar      *subtitle;
  gchar      *icon_name;
  GHashTable *metadata;

  /* Item tree pointers. */
  RtfmItem   *parent;
  RtfmItem   *next;
  RtfmItem   *prev;
  RtfmItem   *first_child;
  RtfmItem   *last_child;

  /* Cached number of children */
  guint       n_items;

  guint       populated : 1;
  guint       is_root : 1;
} RtfmItemPrivate;

static void list_model_iface_init (GListModelInterface *iface);
static void rtfm_item_unparent    (RtfmItem            *self);
static void rtfm_item_foreach     (RtfmItem            *self,
                                   GFunc                callback,
                                   gpointer             user_data);

G_DEFINE_TYPE_EXTENDED (RtfmItem, rtfm_item, G_TYPE_OBJECT, 0,
                        G_ADD_PRIVATE (RtfmItem)
                        G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, list_model_iface_init))

enum {
  PROP_0,
  PROP_ICON_NAME,
  PROP_ID,
  PROP_SUBTITLE,
  PROP_TITLE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
rtfm_item_dispose (GObject *object)
{
  RtfmItem *self = (RtfmItem *)object;
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  if (priv->first_child != NULL)
    rtfm_item_remove_all (self);

  G_OBJECT_CLASS (rtfm_item_parent_class)->dispose (object);
}

static void
rtfm_item_finalize (GObject *object)
{
  RtfmItem *self = (RtfmItem *)object;
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_clear_pointer (&priv->icon_name, g_free);
  g_clear_pointer (&priv->id, g_free);
  g_clear_pointer (&priv->subtitle, g_free);
  g_clear_pointer (&priv->title, g_free);

  G_OBJECT_CLASS (rtfm_item_parent_class)->finalize (object);
}

static void
rtfm_item_get_property (GObject    *object,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
  RtfmItem *self = RTFM_ITEM (object);

  switch (prop_id)
    {
    case PROP_ICON_NAME:
      g_value_set_string (value, rtfm_item_get_icon_name (self));
      break;

    case PROP_ID:
      g_value_set_string (value, rtfm_item_get_id (self));
      break;

    case PROP_SUBTITLE:
      g_value_set_string (value, rtfm_item_get_subtitle (self));
      break;

    case PROP_TITLE:
      g_value_set_string (value, rtfm_item_get_title (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_item_set_property (GObject      *object,
                        guint         prop_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
  RtfmItem *self = RTFM_ITEM (object);

  switch (prop_id)
    {
    case PROP_ICON_NAME:
      rtfm_item_set_icon_name (self, g_value_get_string (value));
      break;

    case PROP_ID:
      rtfm_item_set_id (self, g_value_get_string (value));
      break;

    case PROP_SUBTITLE:
      rtfm_item_set_subtitle (self, g_value_get_string (value));
      break;

    case PROP_TITLE:
      rtfm_item_set_title (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_item_class_init (RtfmItemClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = rtfm_item_dispose;
  object_class->finalize = rtfm_item_finalize;
  object_class->get_property = rtfm_item_get_property;
  object_class->set_property = rtfm_item_set_property;

  properties [PROP_ICON_NAME] =
    g_param_spec_string ("icon-name",
                         "Icon Name",
                         "Icon Name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_ID] =
    g_param_spec_string ("id",
                         "Identifier",
                         "The unique item identifier.",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_SUBTITLE] =
    g_param_spec_string ("subtitle",
                         "Subtitle",
                         "The subtitle for the item.",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  properties [PROP_TITLE] =
    g_param_spec_string ("title",
                         "Title",
                         "The title for the item.",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_item_init (RtfmItem *self)
{
}

RtfmItem *
rtfm_item_new (void)
{
  return g_object_new (RTFM_TYPE_ITEM, NULL);
}

const gchar *
rtfm_item_get_icon_name (RtfmItem *self)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_ITEM (self), NULL);

  return priv->icon_name;
}

const gchar *
rtfm_item_get_id (RtfmItem *self)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_ITEM (self), NULL);

  return priv->id;
}

const gchar *
rtfm_item_get_subtitle (RtfmItem *self)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_ITEM (self), NULL);

  return priv->subtitle;
}

const gchar *
rtfm_item_get_title (RtfmItem *self)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_ITEM (self), NULL);

  return priv->title;
}

void
rtfm_item_set_icon_name (RtfmItem    *self,
                         const gchar *icon_name)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_if_fail (RTFM_IS_ITEM (self));

  if (g_strcmp0 (icon_name, priv->icon_name) != 0)
    {
      g_free (priv->icon_name);
      priv->icon_name = g_strdup (icon_name);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ICON_NAME]);
    }
}

void
rtfm_item_set_id (RtfmItem    *self,
                  const gchar *id)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_if_fail (RTFM_IS_ITEM (self));

  if (g_strcmp0 (id, priv->id) != 0)
    {
      g_free (priv->id);
      priv->id = g_strdup (id);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ID]);
    }
}

void
rtfm_item_set_subtitle (RtfmItem    *self,
                        const gchar *subtitle)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_if_fail (RTFM_IS_ITEM (self));

  if (g_strcmp0 (subtitle, priv->subtitle) != 0)
    {
      g_free (priv->subtitle);
      priv->subtitle = g_strdup (subtitle);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_SUBTITLE]);
    }
}

void
rtfm_item_set_title (RtfmItem    *self,
                     const gchar *title)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_if_fail (RTFM_IS_ITEM (self));

  if (g_strcmp0 (title, priv->title) != 0)
    {
      g_free (priv->title);
      priv->title = g_strdup (title);
      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_TITLE]);
    }
}

/**
 * rtfm_item_get_metadata:
 * @self: A #RtfmItem.
 * @key: The metadata key.
 *
 * Looks up the metadata by key.
 *
 * Returns: (transfer none) (nullable): A #GVariant or %NULL.
 */
GVariant *
rtfm_item_get_metadata (RtfmItem    *self,
                        const gchar *key)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_ITEM (self), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  if (priv->metadata != NULL)
    return g_hash_table_lookup (priv->metadata, key);

  return NULL;
}

void
rtfm_item_set_metadata (RtfmItem    *self,
                        const gchar *key,
                        GVariant    *value)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_if_fail (RTFM_IS_ITEM (self));
  g_return_if_fail (key != NULL);

  if (priv->metadata == NULL)
    priv->metadata = g_hash_table_new_full (g_str_hash,
                                            g_str_equal,
                                            g_free,
                                            (GDestroyNotify)g_variant_unref);

  g_hash_table_insert (priv->metadata, g_strdup (key), g_variant_ref_sink (value));
}

void
rtfm_item_set_metadata_string (RtfmItem    *self,
                               const gchar *key,
                               const gchar *value)
{
  g_return_if_fail (RTFM_IS_ITEM (self));
  g_return_if_fail (key != NULL);

  if (value == NULL)
    rtfm_item_set_metadata (self, key, NULL);
  else
    rtfm_item_set_metadata (self, key, g_variant_new_string (value));
}

const gchar *
rtfm_item_get_metadata_string (RtfmItem    *self,
                               const gchar *key)
{
  GVariant *value;

  g_return_val_if_fail (RTFM_IS_ITEM (self), NULL);
  g_return_val_if_fail (key != NULL, NULL);

  value = rtfm_item_get_metadata (self, key);

  if (g_variant_is_of_type (value, G_VARIANT_TYPE_STRING))
    return g_variant_get_string (value, NULL);

  return NULL;
}

/**
 * rtfm_item_get_path:
 * @self: A #RtfmItem.
 *
 * Gets the path for the item. This is used to display the path to the
 * item in the top of the window.
 *
 * Returns: (transfer full): An #RtfmPath or %NULL.
 */
RtfmPath *
rtfm_item_get_path (RtfmItem *self)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);
  RtfmPath *ret = NULL;

  g_return_val_if_fail (RTFM_IS_ITEM (self), NULL);

  if (RTFM_ITEM_GET_CLASS (self)->get_path)
    ret = RTFM_ITEM_GET_CLASS (self)->get_path (self);

  if (ret == NULL)
    {
      ret = rtfm_path_new ();

      if (!priv->is_root)
        {
          g_autoptr(RtfmPathElement) element = NULL;

          element = rtfm_path_element_new (priv->id, priv->icon_name, priv->title);
          rtfm_path_push_element (ret, element);
        }
    }

  return ret;
}

/**
 * rtfm_item_get_parent:
 *
 * Returns: (transfer none) (nullable): An #RtfmItem or %NULL.
 */
RtfmItem *
rtfm_item_get_parent (RtfmItem *self)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_ITEM (self), NULL);

  return priv->parent;
}

static void
rtfm_item_unparent (RtfmItem *self)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_assert (RTFM_IS_ITEM (self));
  g_assert (priv->prev == NULL || priv->parent != NULL);
  g_assert (priv->next == NULL || priv->parent != NULL);

  if (priv->parent != NULL)
    {
      RtfmItem *parent = priv->parent;
      RtfmItem *iter;
      guint position = 0;

      if (priv->prev != NULL)
        GET_PRIVATE (priv->prev)->next = priv->next;
      else
        GET_PRIVATE (parent)->first_child = priv->next;

      if (priv->next != NULL)
        GET_PRIVATE (priv->next)->prev = priv->prev;
      else
        GET_PRIVATE (parent)->last_child = priv->prev;

      GET_PRIVATE (parent)->n_items--;

      priv->prev = NULL;
      priv->next = NULL;
      priv->parent = NULL;

      for (iter = GET_PRIVATE (parent)->first_child;
           iter != NULL && iter != self;
           iter = GET_PRIVATE (iter)->next)
        position++;

      g_list_model_items_changed (G_LIST_MODEL (parent), position, 1, 0);

      /* The parent holds a reference to the child, so now we can release it
       * since we've cleaned up the parent.
       */
      g_object_unref (self);
    }
}

void
rtfm_item_append (RtfmItem *self,
                  RtfmItem *child)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);
  RtfmItemPrivate *childpriv = rtfm_item_get_instance_private (child);
  guint position;

  g_return_if_fail (RTFM_IS_ITEM (self));
  g_return_if_fail (RTFM_IS_ITEM (child));

  g_object_ref (child);
  rtfm_item_unparent (child);
  childpriv->parent = self;

  childpriv->prev = priv->last_child;
  priv->last_child = child;
  if (priv->first_child == NULL)
    priv->first_child = child;

  position = priv->n_items;
  priv->n_items++;
  g_list_model_items_changed (G_LIST_MODEL (self), position, 0, 1);
}

void
rtfm_item_prepend (RtfmItem *self,
                   RtfmItem *child)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);
  RtfmItemPrivate *childpriv = rtfm_item_get_instance_private (child);

  g_return_if_fail (RTFM_IS_ITEM (self));
  g_return_if_fail (RTFM_IS_ITEM (child));

  g_object_ref (child);
  rtfm_item_unparent (child);
  childpriv->parent = self;

  childpriv->next = priv->first_child;
  priv->first_child = child;
  if (priv->last_child == NULL)
    priv->last_child = child;

  priv->n_items++;
  g_list_model_items_changed (G_LIST_MODEL (self), 0, 0, 1);
}

void
rtfm_item_insert_after (RtfmItem *self,
                        RtfmItem *sibling,
                        RtfmItem *child)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);
  RtfmItemPrivate *siblingpriv = rtfm_item_get_instance_private (sibling);
  RtfmItemPrivate *childpriv = rtfm_item_get_instance_private (child);
  RtfmItem *iter;
  guint position = 0;

  g_return_if_fail (RTFM_IS_ITEM (self));
  g_return_if_fail (RTFM_IS_ITEM (sibling));
  g_return_if_fail (RTFM_IS_ITEM (child));
  g_return_if_fail (siblingpriv->parent == self);

  g_object_ref (child);
  rtfm_item_unparent (child);
  childpriv->parent = self;

  childpriv->prev = sibling;
  childpriv->next = siblingpriv->next;
  siblingpriv->next = child;

  if (childpriv->next == NULL)
    priv->last_child = child;

  priv->n_items++;

  for (iter = priv->first_child;
       iter != NULL && iter != child;
       iter = GET_PRIVATE (iter)->next)
    position++;

  g_list_model_items_changed (G_LIST_MODEL (self), position, 0, 1);
}

void
rtfm_item_insert_before (RtfmItem *self,
                         RtfmItem *sibling,
                         RtfmItem *child)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);
  RtfmItemPrivate *siblingpriv = rtfm_item_get_instance_private (sibling);
  RtfmItemPrivate *childpriv = rtfm_item_get_instance_private (child);
  RtfmItem *iter;
  guint position = 0;

  g_return_if_fail (RTFM_IS_ITEM (self));
  g_return_if_fail (RTFM_IS_ITEM (sibling));
  g_return_if_fail (RTFM_IS_ITEM (child));
  g_return_if_fail (siblingpriv->parent == self);

  g_object_ref (child);
  rtfm_item_unparent (child);
  childpriv->parent = self;

  childpriv->next = sibling;
  childpriv->prev = siblingpriv->prev;
  siblingpriv->prev = child;

  if (childpriv->prev == NULL)
    priv->first_child = child;

  priv->n_items++;

  for (iter = priv->first_child;
       iter != NULL && iter != child;
       iter = GET_PRIVATE (iter)->next)
    position++;

  g_list_model_items_changed (G_LIST_MODEL (self), position, 0, 1);
}

/**
 * rtfm_item_get_children:
 * @self: An #RtfmItem
 *
 * Gets the children of an item.
 *
 * Returns: (transfer container) (element-type Rtfm.Item): Retreives the
 *   children of the item as a #GList.
 */
GList *
rtfm_item_get_children (RtfmItem *self)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);
  RtfmItem *iter;
  GList *list = NULL;

  g_return_val_if_fail (RTFM_IS_ITEM (self), NULL);

  for (iter = priv->last_child; iter != NULL; iter = GET_PRIVATE (iter)->prev)
    list = g_list_prepend (list, iter);

  return list;
}

static void
rtfm_item_foreach (RtfmItem *self,
                   GFunc     callback,
                   gpointer  user_data)
{
  RtfmItem *iter;

  g_return_if_fail (RTFM_IS_ITEM (self));
  g_return_if_fail (callback != NULL);

  for (iter = GET_PRIVATE (self)->first_child;
       iter != NULL;
       iter = GET_PRIVATE (iter)->next)
    callback (iter, user_data);
}

static GType
rtfm_item_get_item_type (GListModel *model)
{
  return RTFM_TYPE_ITEM;
}

static guint
rtfm_item_get_n_items (GListModel *model)
{
  RtfmItem *self = (RtfmItem *)model;
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_ITEM (self), 0);

  return priv->n_items;
}

static void
lookup_at_index (gpointer data,
                 gpointer user_data)
{
  RtfmItem *child = data;
  struct {
    RtfmItem *child;
    guint     offset;
  } *lookup = user_data;

  g_assert (RTFM_IS_ITEM (child));
  g_assert (lookup != NULL);

  if (lookup->child == NULL)
    {
      if (lookup->offset == 0)
        lookup->child = child;
      else
        lookup->offset--;
    }
}

static gpointer
rtfm_item_get_item (GListModel *model,
                    guint       index)
{
  RtfmItem *self = (RtfmItem *)model;
  struct {
    RtfmItem *child;
    guint     offset;
  } lookup = { NULL, index };

  g_return_val_if_fail (RTFM_IS_ITEM (self), NULL);

  rtfm_item_foreach (self, lookup_at_index, &lookup);

  return g_object_ref (lookup.child);
}

static void
list_model_iface_init (GListModelInterface *iface)
{
  iface->get_item_type = rtfm_item_get_item_type;
  iface->get_n_items = rtfm_item_get_n_items;
  iface->get_item = rtfm_item_get_item;
}

void
rtfm_item_remove_all (RtfmItem *self)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);
  RtfmItem *chain;
  RtfmItem *iter;
  RtfmItem *next = NULL;
  GList *list = NULL;
  guint count = 0;

  g_assert (RTFM_IS_ITEM (self));

  if (priv->first_child == NULL)
    return;

  chain = priv->first_child;

  priv->first_child = NULL;
  priv->last_child = NULL;

  for (iter = chain; iter != NULL; iter = next)
    {
      RtfmItemPrivate *iterpriv = rtfm_item_get_instance_private (iter);

      next = iterpriv->next;

      iterpriv->parent = NULL;
      iterpriv->prev = NULL;
      iterpriv->next = NULL;

      list = g_list_prepend (list, iter);

      count++;
    }

  g_list_model_items_changed (G_LIST_MODEL (self), 0, count, 0);

  g_list_free_full (list, g_object_unref);
}

gboolean
_rtfm_item_get_populated (RtfmItem *self)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_val_if_fail (RTFM_IS_ITEM (self), FALSE);

  return priv->populated;
}

void
_rtfm_item_set_populated (RtfmItem *self,
                          gboolean  populated)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_if_fail (RTFM_IS_ITEM (self));

  priv->populated = !!populated;
}

void
_rtfm_item_set_is_root (RtfmItem *self,
                        gboolean  is_root)
{
  RtfmItemPrivate *priv = rtfm_item_get_instance_private (self);

  g_return_if_fail (RTFM_IS_ITEM (self));

  priv->is_root = !!is_root;
}
