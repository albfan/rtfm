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
#include "rtfm-gir-item.h"
#include "rtfm-gir-record.h"
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

RtfmGirItem *
rtfm_gir_item_new (GObject *object)
{
  const gchar *icon_name = NULL;
  g_autofree gchar *id = NULL;
  g_autofree gchar *title = NULL;
  g_autofree gchar *subtitle = NULL;

  if (FALSE) { }
  else if (RTFM_IS_GIR_REPOSITORY (object))
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
  else if (RTFM_IS_GIR_ALIAS (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
    }
  else if (RTFM_IS_GIR_PROPERTY (object))
    {
      g_object_get (object,
                    "name", &title,
                    NULL);
      icon_name = "lang-property-symbolic";
    }
  else if (RTFM_IS_GIR_FIELD (object))
    {
      g_object_get (object,
                    "name", &title,
                    NULL);
      icon_name = "lang-struct-field-symbolic";
    }
  else if (RTFM_IS_GIR_CLASS (object))
    {
      g_object_get (object,
                    "glib-type-name", &title,
                    NULL);
      icon_name = "lang-class-symbolic";
    }
  else if (RTFM_IS_GIR_RECORD (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
      icon_name = "lang-struct-symbolic";
    }
  else if (RTFM_IS_GIR_UNION (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
      icon_name = "lang-union-symbolic";
    }
  else if (RTFM_IS_GIR_CALLBACK (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
      icon_name = "lang-function-symbolic";
    }
  else if (RTFM_IS_GIR_CONSTANT (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
      icon_name = "lang-define-symbolic";
    }
  else if (RTFM_IS_GIR_CONSTRUCTOR (object))
    {
      g_object_get (object,
                    "c-identifier", &title,
                    NULL);
      icon_name = "lang-function-symbolic";
    }
  else if (RTFM_IS_GIR_BITFIELD (object) || RTFM_IS_GIR_ENUMERATION (object))
    {
      g_object_get (object,
                    "c-type", &title,
                    NULL);
      icon_name = "lang-enum-symbolic";
    }
  else if (RTFM_IS_GIR_MEMBER (object))
    {
      g_object_get (object,
                    "c-identifier", &title,
                    NULL);
      icon_name = "lang-enum-value-symbolic";
    }
  else if (RTFM_IS_GIR_METHOD (object) || RTFM_IS_GIR_FUNCTION (object))
    {
      g_object_get (object,
                    "c-identifier", &title,
                    NULL);
      icon_name = "lang-function-symbolic";
    }

  return g_object_new (RTFM_TYPE_GIR_ITEM,
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

static void
rtfm_gir_item_populate_repository_cb (GObject      *object,
                                      GAsyncResult *result,
                                      gpointer      user_data)
{
  RtfmGirRepository *repository = (RtfmGirRepository *)object;
  RtfmGirNamespace *namespace;
  RtfmCollection *collection;
  g_autoptr(GTask) task = user_data;
  GError *error = NULL;

  g_assert (RTFM_IS_GIR_REPOSITORY (repository));
  g_assert (G_IS_TASK (task));

  if (!g_async_initable_init_finish (G_ASYNC_INITABLE (repository), result, &error))
    {
      g_task_return_error (task, error);
      return;
    }

  collection = g_task_get_task_data (task);
  g_assert (RTFM_IS_COLLECTION (collection));

  namespace = rtfm_gir_repository_get_namespace (repository);
  g_assert (!namespace || RTFM_IS_GIR_NAMESPACE (namespace));

  if (namespace != NULL)
    {
      if (rtfm_gir_namespace_has_classes (namespace))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_TYPE_GIR_ITEM,
                               "id", "gir:classes",
                               "object", namespace,
                               "title", _("Classes"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (rtfm_gir_namespace_has_records (namespace))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_TYPE_GIR_ITEM,
                               "id", "gir:records",
                               "object", namespace,
                               "title", _("Structs"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (rtfm_gir_namespace_has_enumerations (namespace))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_TYPE_GIR_ITEM,
                               "id", "gir:enumerations",
                               "object", namespace,
                               "title", _("Enumerations"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (rtfm_gir_namespace_has_bitfields (namespace))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_TYPE_GIR_ITEM,
                               "id", "gir:bitfields",
                               "object", namespace,
                               "title", _("Flags"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (rtfm_gir_namespace_has_aliases (namespace))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_TYPE_GIR_ITEM,
                               "id", "gir:aliases",
                               "object", namespace,
                               "title", _("Aliases"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (rtfm_gir_namespace_has_callbacks (namespace))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_TYPE_GIR_ITEM,
                               "id", "gir:callbacks",
                               "object", namespace,
                               "title", _("Callbacks"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (rtfm_gir_namespace_has_constants (namespace))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_TYPE_GIR_ITEM,
                               "id", "gir:constants",
                               "object", namespace,
                               "title", _("Constants"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (rtfm_gir_namespace_has_functions (namespace))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_TYPE_GIR_ITEM,
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

  g_assert (RTFM_IS_GIR_ITEM (self));
  g_assert (RTFM_IS_COLLECTION (collection));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);

  if (FALSE) {}
  else if (RTFM_IS_GIR_REPOSITORY (priv->object))
    {
      g_task_set_task_data (task, g_object_ref (collection), g_object_unref);
      g_async_initable_init_async (G_ASYNC_INITABLE (priv->object),
                                   G_PRIORITY_DEFAULT,
                                   cancellable,
                                   rtfm_gir_item_populate_repository_cb,
                                   g_object_ref (task));
      return;
    }
  else if (RTFM_IS_GIR_NAMESPACE (priv->object))
    {
      const gchar *id = rtfm_item_get_id (RTFM_ITEM (self));
      GPtrArray *ar = NULL;

      if (FALSE) {}
      else if (g_strcmp0 ("gir:classes", id) == 0)
        ar = rtfm_gir_namespace_get_classes (RTFM_GIR_NAMESPACE (priv->object));
      else if (g_strcmp0 ("gir:aliases", id) == 0)
        ar = rtfm_gir_namespace_get_aliases (RTFM_GIR_NAMESPACE (priv->object));
      else if (g_strcmp0 ("gir:records", id) == 0)
        ar = rtfm_gir_namespace_get_records (RTFM_GIR_NAMESPACE (priv->object));
      else if (g_strcmp0 ("gir:enumerations", id) == 0)
        ar = rtfm_gir_namespace_get_enumerations (RTFM_GIR_NAMESPACE (priv->object));
      else if (g_strcmp0 ("gir:bitfields", id) == 0)
        ar = rtfm_gir_namespace_get_bitfields (RTFM_GIR_NAMESPACE (priv->object));
      else if (g_strcmp0 ("gir:callbacks", id) == 0)
        ar = rtfm_gir_namespace_get_callbacks (RTFM_GIR_NAMESPACE (priv->object));
      else if (g_strcmp0 ("gir:constants", id) == 0)
        ar = rtfm_gir_namespace_get_constants (RTFM_GIR_NAMESPACE (priv->object));
      else if (g_strcmp0 ("gir:functions", id) == 0)
        ar = rtfm_gir_namespace_get_functions (RTFM_GIR_NAMESPACE (priv->object));

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
  else if (RTFM_IS_GIR_CLASS (priv->object))
    {
      const gchar *id = rtfm_item_get_id (RTFM_ITEM (self));
      GPtrArray *ar = NULL;

      if (FALSE) {}
      else if (g_strcmp0 ("gir:methods", id) == 0)
        ar = rtfm_gir_class_get_methods (RTFM_GIR_CLASS (priv->object));
      else if (g_strcmp0 ("gir:virtual", id) == 0)
        ar = rtfm_gir_class_get_virtual_methods (RTFM_GIR_CLASS (priv->object));
      else if (g_strcmp0 ("gir:properties", id) == 0)
        ar = rtfm_gir_class_get_properties (RTFM_GIR_CLASS (priv->object));
      else if (g_strcmp0 ("gir:constructors", id) == 0)
        ar = rtfm_gir_class_get_constructors (RTFM_GIR_CLASS (priv->object));
      else if (g_strcmp0 ("gir:fields", id) == 0)
        ar = rtfm_gir_class_get_fields (RTFM_GIR_CLASS (priv->object));

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
          if (rtfm_gir_class_has_fields (RTFM_GIR_CLASS (priv->object)) ||
              rtfm_gir_class_has_unions (RTFM_GIR_CLASS (priv->object)))
            {
              g_autoptr(RtfmGirItem) item = NULL;

              item = g_object_new (RTFM_TYPE_GIR_ITEM,
                                   "id", "gir:fields",
                                   "object", priv->object,
                                   "title", _("Fields"),
                                   NULL);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }

          if (rtfm_gir_class_has_properties (RTFM_GIR_CLASS (priv->object)))
            {
              g_autoptr(RtfmGirItem) item = NULL;

              item = g_object_new (RTFM_TYPE_GIR_ITEM,
                                   "id", "gir:properties",
                                   "object", priv->object,
                                   "title", _("Properties"),
                                   NULL);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }

          if (rtfm_gir_class_has_constructors (RTFM_GIR_CLASS (priv->object)))
            {
              g_autoptr(RtfmGirItem) item = NULL;

              item = g_object_new (RTFM_TYPE_GIR_ITEM,
                                   "id", "gir:constructors",
                                   "object", priv->object,
                                   "title", _("Constructors"),
                                   NULL);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }

          if (rtfm_gir_class_has_methods (RTFM_GIR_CLASS (priv->object)))
            {
              g_autoptr(RtfmGirItem) item = NULL;

              item = g_object_new (RTFM_TYPE_GIR_ITEM,
                                   "id", "gir:methods",
                                   "object", priv->object,
                                   "title", _("Methods"),
                                   NULL);
              rtfm_collection_append (collection, g_steal_pointer (&item));
            }
        }
    }
  else if (RTFM_IS_GIR_RECORD (priv->object))
    {
      RtfmGirRecord *record = RTFM_GIR_RECORD (priv->object);
      const gchar *id = rtfm_item_get_id (RTFM_ITEM (self));

      if (g_strcmp0 ("gir:fields", id) == 0)
        {
          GPtrArray *ar = NULL;
          guint i;

          ar = rtfm_gir_record_get_fields (record);

          if (ar != NULL)
            {
              for (i = 0; i < ar->len; i++)
                {
                  GObject *object = g_ptr_array_index (ar, i);
                  g_autoptr(RtfmGirItem) item = NULL;

                  item = rtfm_gir_item_new (object);
                  rtfm_collection_append (collection, g_steal_pointer (&item));
                }
            }

          ar = rtfm_gir_record_get_unions (record);

          if (ar != NULL)
            {
              for (i = 0; i < ar->len; i++)
                {
                  GObject *object = g_ptr_array_index (ar, i);
                  g_autoptr(RtfmGirItem) item = NULL;

                  item = rtfm_gir_item_new (object);
                  rtfm_collection_append (collection, g_steal_pointer (&item));
                }
            }
        }
      else if (rtfm_gir_record_has_fields (record) ||
               rtfm_gir_record_has_unions (record))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_TYPE_GIR_ITEM,
                               "id", "gir:fields",
                               "object", priv->object,
                               "title", _("Fields"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }

      if (rtfm_gir_record_has_functions (record))
        {
          g_autoptr(RtfmGirItem) item = NULL;

          item = g_object_new (RTFM_TYPE_GIR_ITEM,
                               "id", "gir:functions",
                               "object", priv->object,
                               "title", _("Functions"),
                               NULL);
          rtfm_collection_append (collection, g_steal_pointer (&item));
        }
    }
  else if (RTFM_IS_GIR_ENUMERATION (priv->object))
    {
      RtfmGirEnumeration *enumeration = RTFM_GIR_ENUMERATION (priv->object);

      if (rtfm_gir_enumeration_has_members (enumeration))
        {
          GPtrArray *ar = rtfm_gir_enumeration_get_members (enumeration);
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
  else if (RTFM_IS_GIR_BITFIELD (priv->object))
    {
      RtfmGirBitfield *bitfield = RTFM_GIR_BITFIELD (priv->object);

      if (rtfm_gir_bitfield_has_members (bitfield))
        {
          GPtrArray *ar = rtfm_gir_bitfield_get_members (bitfield);
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
  g_return_val_if_fail (RTFM_IS_GIR_ITEM (self), FALSE);
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

  g_return_val_if_fail (RTFM_IS_GIR_ITEM (self), NULL);

  return priv->object;
}
