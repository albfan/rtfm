/* rtfm-gir-item.c
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

#define G_LOG_DOMAIN "rtfm-gir-item"

#include <glib/gi18n.h>

#include "rtfm-gir-alias.h"
#include "rtfm-gir-bitfield.h"
#include "rtfm-gir-callback.h"
#include "rtfm-gir-class.h"
#include "rtfm-gir-constant.h"
#include "rtfm-gir-enumeration.h"
#include "rtfm-gir-file.h"
#include "rtfm-gir-item.h"
#include "rtfm-gir-namespace.h"
#include "rtfm-gir-record.h"
#include "rtfm-gir-repository.h"
#include "rtfm-gir-repository.h"

typedef struct
{
  GObject *object;
} RtfmGirItemPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (RtfmGirItem, rtfm_gir_item, RTFM_TYPE_ITEM)

enum {
  PROP_0,
  PROP_OBJECT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

#define HAS_CHILD_TYPED(obj, NAME) \
  (rtfm_gir_parser_object_has_child_typed ( \
    RTFM_GIR_PARSER_OBJECT (obj), RTFM_GIR_TYPE_##NAME))
#define GET_CHILDREN_TYPED(obj, NAME) \
  (rtfm_gir_parser_object_get_children_typed ( \
    RTFM_GIR_PARSER_OBJECT (obj), RTFM_GIR_TYPE_##NAME))

RtfmGirItem *
rtfm_gir_item_new (GObject *object)
{
  const gchar *icon_name = NULL;
  g_autofree gchar *id = NULL;
  g_autofree gchar *title = NULL;
  g_autofree gchar *subtitle = NULL;

  if (FALSE) { }
  else if (RTFM_GIR_IS_REPOSITORY (object))
    {
      g_autoptr(GFile) file = NULL;

      g_object_get (object,
                    "file", &file,
                    NULL);

      icon_name = "lang-namespace-symbolic";

      if (G_IS_FILE (file))
        {
          g_autofree gchar *name = NULL;

          if (NULL != (name = g_file_get_basename (file)))
            {
              gchar *tmp = strrchr (name, '.');
              g_auto(GStrv) parts = NULL;

              if (tmp != NULL)
                *tmp = '\0';

              id = g_strdup_printf ("gir:%s", name);
              parts = g_strsplit (name, "-", 0);
              title = g_strdup (parts[0]);
              if (g_strv_length (parts) > 1)
                subtitle = g_strdup_printf ("%s %s", parts[0], parts[1]);
            }
        }
    }
  else if (RTFM_GIR_IS_ALIAS (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
    }
  else if (RTFM_GIR_IS_PROPERTY (object))
    {
      g_object_get (object,
                    "name", &title,
                    NULL);
      icon_name = "lang-property-symbolic";
    }
  else if (RTFM_GIR_IS_FIELD (object))
    {
      g_object_get (object,
                    "name", &title,
                    NULL);
      icon_name = "lang-struct-field-symbolic";
    }
  else if (RTFM_GIR_IS_CLASS (object))
    {
      g_object_get (object,
                    "glib-type-name", &title,
                    NULL);
      icon_name = "lang-class-symbolic";
    }
  else if (RTFM_GIR_IS_RECORD (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
      icon_name = "lang-struct-symbolic";
    }
  else if (RTFM_GIR_IS_UNION (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
      icon_name = "lang-union-symbolic";
    }
  else if (RTFM_GIR_IS_CALLBACK (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
      icon_name = "lang-function-symbolic";
    }
  else if (RTFM_GIR_IS_CONSTANT (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
      icon_name = "lang-define-symbolic";
    }
  else if (RTFM_GIR_IS_CONSTRUCTOR (object))
    {
      g_object_get (object,
                    "c-identifier", &title,
                    NULL);
      icon_name = "lang-function-symbolic";
    }
  else if (RTFM_GIR_IS_BITFIELD (object) || RTFM_GIR_IS_ENUMERATION (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
      icon_name = "lang-enum-symbolic";
    }
  else if (RTFM_GIR_IS_MEMBER (object))
    {
      g_object_get (object,
                    "c-identifier", &title,
                    NULL);
      icon_name = "lang-enum-value-symbolic";
    }
  else if (RTFM_GIR_IS_METHOD (object) || RTFM_GIR_IS_FUNCTION (object))
    {
      g_object_get (object,
                    "c-identifier", &title,
                    NULL);
      icon_name = "lang-function-symbolic";
    }

  return g_object_new (RTFM_GIR_TYPE_ITEM,
                       "icon-name", icon_name,
                       "id", id,
                       "object", object,
                       "subtitle", subtitle,
                       "title", title,
                       NULL);
}

static void
rtfm_gir_item_finalize (GObject *object)
{
  RtfmGirItem *self = (RtfmGirItem *)object;
  RtfmGirItemPrivate *priv = rtfm_gir_item_get_instance_private (self);

  g_clear_object (&object);

  G_OBJECT_CLASS (rtfm_gir_item_parent_class)->finalize (object);
}

static void
rtfm_gir_item_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  RtfmGirItem *self = RTFM_GIR_ITEM (object);
  RtfmGirItemPrivate *priv = rtfm_gir_item_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_OBJECT:
      g_value_set_object (value, priv->object);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_item_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  RtfmGirItem *self = RTFM_GIR_ITEM (object);
  RtfmGirItemPrivate *priv = rtfm_gir_item_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_OBJECT:
      priv->object = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_item_class_init (RtfmGirItemClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_item_finalize;
  object_class->get_property = rtfm_gir_item_get_property;
  object_class->set_property = rtfm_gir_item_set_property;

  properties [PROP_OBJECT] =
    g_param_spec_object ("object",
                         "Object",
                         "Object",
                         G_TYPE_OBJECT,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_item_init (RtfmGirItem *self)
{
}

static RtfmGirNamespace *
get_namespace (RtfmGirRepository *self)
{
  GPtrArray *ar;

  g_assert (RTFM_GIR_IS_REPOSITORY (self));

  ar = rtfm_gir_parser_object_get_children (RTFM_GIR_PARSER_OBJECT (self));

  if (ar != NULL)
    {
      guint i;

      for (i = 0; i < ar->len; i++)
        {
          RtfmGirParserObject *item = g_ptr_array_index (ar, i);

          if (RTFM_GIR_IS_NAMESPACE (item))
            return RTFM_GIR_NAMESPACE (item);
        }
    }

  return NULL;
}

static void
rtfm_gir_item_populate_cb (GObject      *object,
                           GAsyncResult *result,
                           gpointer      user_data)
{
  RtfmGirFile *file = (RtfmGirFile *)object;
  RtfmGirNamespace *namespace;
  RtfmGirRepository *repository;
  RtfmCollection *collection;
  g_autoptr(GTask) task = user_data;
  GError *error = NULL;

  g_assert (RTFM_GIR_IS_FILE (file));
  g_assert (G_IS_TASK (task));

  if (!g_async_initable_init_finish (G_ASYNC_INITABLE (file), result, &error))
    {
      g_task_return_error (task, error);
      return;
    }

  repository = rtfm_gir_file_get_repository (file);
  collection = g_task_get_task_data (task);
  g_assert (RTFM_IS_COLLECTION (collection));
  namespace = get_namespace (repository);
  g_assert (!namespace || RTFM_GIR_IS_NAMESPACE (namespace));

  if (namespace != NULL)
    {
      if (HAS_CHILD_TYPED (namespace, CLASS))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_GIR_TYPE_ITEM,
                               "id", "gir:classes",
                               "object", namespace,
                               "title", _("Classes"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (HAS_CHILD_TYPED (namespace, RECORD))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_GIR_TYPE_ITEM,
                               "id", "gir:records",
                               "object", namespace,
                               "title", _("Structs"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (HAS_CHILD_TYPED (namespace, ENUMERATION))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_GIR_TYPE_ITEM,
                               "id", "gir:enumerations",
                               "object", namespace,
                               "title", _("Enumerations"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (HAS_CHILD_TYPED (namespace, BITFIELD))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_GIR_TYPE_ITEM,
                               "id", "gir:bitfields",
                               "object", namespace,
                               "title", _("Flags"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (HAS_CHILD_TYPED (namespace, ALIAS))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_GIR_TYPE_ITEM,
                               "id", "gir:aliases",
                               "object", namespace,
                               "title", _("Aliases"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (HAS_CHILD_TYPED (namespace, CALLBACK))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_GIR_TYPE_ITEM,
                               "id", "gir:callbacks",
                               "object", namespace,
                               "title", _("Callbacks"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (HAS_CHILD_TYPED (namespace, CONSTANT))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_GIR_TYPE_ITEM,
                               "id", "gir:constants",
                               "object", namespace,
                               "title", _("Constants"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (HAS_CHILD_TYPED (namespace, FUNCTION))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_GIR_TYPE_ITEM,
                               "id", "gir:functions",
                               "object", namespace,
                               "title", _("Global Functions"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }
    }

  g_task_return_boolean (task, TRUE);
}

void
rtfm_gir_item_populate_async (RtfmGirItem         *self,
                              RtfmCollection      *collection,
                              GCancellable        *cancellable,
                              GAsyncReadyCallback  callback,
                              gpointer             user_data)
{
  RtfmGirItemPrivate *priv = rtfm_gir_item_get_instance_private (self);
  g_autoptr(GTask) task = NULL;

  g_assert (RTFM_GIR_IS_ITEM (self));
  g_assert (RTFM_IS_COLLECTION (collection));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);

  if (FALSE) {}
  else if (RTFM_GIR_IS_FILE (priv->object))
    {
      g_task_set_task_data (task, g_object_ref (collection), g_object_unref);
      g_async_initable_init_async (G_ASYNC_INITABLE (priv->object),
                                   G_PRIORITY_DEFAULT,
                                   cancellable,
                                   rtfm_gir_item_populate_cb,
                                   g_object_ref (task));
      return;
    }
  else if (RTFM_GIR_IS_NAMESPACE (priv->object))
    {
      const gchar *id = rtfm_item_get_id (RTFM_ITEM (self));
      g_autoptr(GPtrArray) ar = NULL;

      if (FALSE) {}
      else if (g_strcmp0 ("gir:classes", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, CLASS);
      else if (g_strcmp0 ("gir:aliases", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, ALIAS);
      else if (g_strcmp0 ("gir:records", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, RECORD);
      else if (g_strcmp0 ("gir:enumerations", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, ENUMERATION);
      else if (g_strcmp0 ("gir:bitfields", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, BITFIELD);
      else if (g_strcmp0 ("gir:callbacks", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, CALLBACK);
      else if (g_strcmp0 ("gir:constants", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, CONSTANT);
      else if (g_strcmp0 ("gir:functions", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, FUNCTION);

      if (ar != NULL)
        {
          guint i;

          for (i = 0; i < ar->len; i++)
            {
              GObject *object = g_ptr_array_index (ar, i);
              g_autoptr(RtfmGirItem) item = NULL;

              item = rtfm_gir_item_new (object);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }
        }
    }
  else if (RTFM_GIR_IS_CLASS (priv->object))
    {
      const gchar *id = rtfm_item_get_id (RTFM_ITEM (self));
      g_autoptr(GPtrArray) ar = NULL;

      if (FALSE) {}
      else if (g_strcmp0 ("gir:methods", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, METHOD);
      else if (g_strcmp0 ("gir:virtual", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, VIRTUAL_METHOD);
      else if (g_strcmp0 ("gir:properties", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, PROPERTY);
      else if (g_strcmp0 ("gir:constructors", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, CONSTRUCTOR);
      else if (g_strcmp0 ("gir:fields", id) == 0)
        ar = GET_CHILDREN_TYPED (priv->object, FIELD);

      if (ar != NULL)
        {
          guint i;

          for (i = 0; i < ar->len; i++)
            {
              GObject *object = g_ptr_array_index (ar, i);
              g_autoptr(RtfmGirItem) item = NULL;

              item = rtfm_gir_item_new (object);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }
        }
      else
        {
          RtfmGirClass *klass = RTFM_GIR_CLASS (priv->object);

          if (HAS_CHILD_TYPED (klass, FIELD) || HAS_CHILD_TYPED (klass, UNION))
            {
              g_autoptr(RtfmGirItem) item = NULL;

              item = g_object_new (RTFM_GIR_TYPE_ITEM,
                                   "id", "gir:fields",
                                   "object", priv->object,
                                   "title", _("Fields"),
                                   NULL);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }

          if (HAS_CHILD_TYPED (klass, PROPERTY))
            {
              g_autoptr(RtfmGirItem) item = NULL;

              item = g_object_new (RTFM_GIR_TYPE_ITEM,
                                   "id", "gir:properties",
                                   "object", priv->object,
                                   "title", _("Properties"),
                                   NULL);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }

          if (HAS_CHILD_TYPED (klass, CONSTRUCTOR))
            {
              g_autoptr(RtfmGirItem) item = NULL;

              item = g_object_new (RTFM_GIR_TYPE_ITEM,
                                   "id", "gir:constructors",
                                   "object", priv->object,
                                   "title", _("Constructors"),
                                   NULL);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }

          if (HAS_CHILD_TYPED (klass, METHOD))
            {
              g_autoptr(RtfmGirItem) item = NULL;

              item = g_object_new (RTFM_GIR_TYPE_ITEM,
                                   "id", "gir:methods",
                                   "object", priv->object,
                                   "title", _("Methods"),
                                   NULL);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }
        }
    }
  else if (RTFM_GIR_IS_RECORD (priv->object))
    {
      RtfmGirRecord *record = RTFM_GIR_RECORD (priv->object);
      const gchar *id = rtfm_item_get_id (RTFM_ITEM (self));

      if (g_strcmp0 ("gir:fields", id) == 0)
        {
          GPtrArray *ar;
          guint i;

          ar = GET_CHILDREN_TYPED (record, FIELD);

          if (ar != NULL)
            {
              for (i = 0; i < ar->len; i++)
                {
                  GObject *object = g_ptr_array_index (ar, i);
                  g_autoptr(RtfmGirItem) item = NULL;

                  item = rtfm_gir_item_new (object);
                  rtfm_collection_append (collection, g_steal_pointer (&item));
                }

              g_clear_pointer (&ar, g_ptr_array_unref);
            }

          ar = GET_CHILDREN_TYPED (record, UNION);

          if (ar != NULL)
            {
              for (i = 0; i < ar->len; i++)
                {
                  GObject *object = g_ptr_array_index (ar, i);
                  g_autoptr(RtfmGirItem) item = NULL;

                  item = rtfm_gir_item_new (object);
                  rtfm_collection_append (collection, g_steal_pointer (&item));
                }

              g_clear_pointer (&ar, g_ptr_array_unref);
            }
        }
      else if (HAS_CHILD_TYPED (record, FIELD) || HAS_CHILD_TYPED (record, UNION))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_GIR_TYPE_ITEM,
                               "id", "gir:fields",
                               "object", priv->object,
                               "title", _("Fields"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (HAS_CHILD_TYPED (record, FUNCTION))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_GIR_TYPE_ITEM,
                               "id", "gir:functions",
                               "object", priv->object,
                               "title", _("Functions"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }
    }
  else if (RTFM_GIR_IS_ENUMERATION (priv->object))
    {
      RtfmGirEnumeration *enumeration = RTFM_GIR_ENUMERATION (priv->object);
      g_autoptr(GPtrArray) ar = GET_CHILDREN_TYPED (enumeration, MEMBER);

      if (ar != NULL)
        {
          guint i;

          for (i = 0; i < ar->len; i++)
            {
              GObject *object = g_ptr_array_index (ar, i);
              g_autoptr(RtfmGirItem) item = NULL;

              item = rtfm_gir_item_new (object);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }
        }
    }
  else if (RTFM_GIR_IS_BITFIELD (priv->object))
    {
      RtfmGirBitfield *bitfield = RTFM_GIR_BITFIELD (priv->object);
      g_autoptr(GPtrArray) ar = GET_CHILDREN_TYPED (bitfield, MEMBER);

      if (ar != NULL)
        {
          guint i;

          for (i = 0; i < ar->len; i++)
            {
              GObject *object = g_ptr_array_index (ar, i);
              g_autoptr(RtfmGirItem) item = NULL;

              item = rtfm_gir_item_new (object);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }
        }
    }

  g_task_return_boolean (task, TRUE);
}

gboolean
rtfm_gir_item_populate_finish (RtfmGirItem   *self,
                               GAsyncResult  *result,
                               GError       **error)
{
  g_return_val_if_fail (RTFM_GIR_IS_ITEM (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (result), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

/**
 * rtfm_gir_item_get_object:
 *
 * Returns: (transfer none) (nullable): A #GObject or %NULL.
 */
GObject *
rtfm_gir_item_get_object (RtfmGirItem *self)
{
  RtfmGirItemPrivate *priv = rtfm_gir_item_get_instance_private (self);

  g_return_val_if_fail (RTFM_GIR_IS_ITEM (self), NULL);

  return priv->object;
}
