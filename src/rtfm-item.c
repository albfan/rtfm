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
#include "rtfm-path.h"
#include "rtfm-path-element.h"

typedef struct
{
  gchar *id;
  gchar *title;
  gchar *subtitle;
  gchar *icon_name;
  GHashTable *metadata;
} RtfmItemPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (RtfmItem, rtfm_item, G_TYPE_OBJECT)

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
      g_autoptr(RtfmPathElement) element = NULL;

      ret = rtfm_path_new ();
      element = rtfm_path_element_new (priv->id,
                                       priv->icon_name,
                                       priv->title);
      rtfm_path_push_element (ret, element);
    }

  return ret;
}
