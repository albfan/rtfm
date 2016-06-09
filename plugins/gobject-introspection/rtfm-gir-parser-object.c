/* rtfm-gir-parser-object.c
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

#define G_LOG_DOMAIN "rtfm-gir-parser-object"

#include "rtfm-gir-parser-types.h"

typedef struct
{
  RtfmGirParserContext *parser_context;
} RtfmGirParserObjectPrivate;

struct _RtfmGirParserContext
{
  volatile gint ref_count;
  GStringChunk *strings;
};

enum {
  PROP_0,
  PROP_PARSER_CONTEXT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

G_DEFINE_TYPE_WITH_PRIVATE (RtfmGirParserObject, rtfm_gir_parser_object, G_TYPE_OBJECT)
G_DEFINE_BOXED_TYPE (RtfmGirParserContext, rtfm_gir_parser_context, rtfm_gir_parser_context_ref, rtfm_gir_parser_context_unref)

RtfmGirParserContext *
rtfm_gir_parser_context_new (void)
{
  RtfmGirParserContext *ret;

  ret = g_slice_new0 (RtfmGirParserContext);
  ret->ref_count = 1;
  ret->strings = g_string_chunk_new (4096);

  return ret;
}

RtfmGirParserContext *
rtfm_gir_parser_context_ref (RtfmGirParserContext *self)
{
  g_return_val_if_fail (self != NULL, NULL);
  g_return_val_if_fail (self->ref_count > 0, NULL);

  g_atomic_int_inc (&self->ref_count);

  return self;
}

void
rtfm_gir_parser_context_unref (RtfmGirParserContext *self)
{
  g_return_if_fail (self != NULL);
  g_return_if_fail (self->ref_count > 0);

  if (g_atomic_int_dec_and_test (&self->ref_count))
    {
      g_string_chunk_free (self->strings);
      g_slice_free (RtfmGirParserContext, self);
    }
}

const gchar *
rtfm_gir_parser_context_intern_string (RtfmGirParserContext *self,
                                       const gchar *string)
{
  g_return_val_if_fail (self != NULL, NULL);

  if (string == NULL)
    return NULL;
  else
    return g_string_chunk_insert_const (self->strings, string);
}

/**
 * rtfm_gir_parser_object_get_parser_context:
 * @self: A #RtfmGirParserObject
 *
 * Gets the parser context associated with this object.
 *
 * Returns: (transfer none): A #RtfmGirParserContext
 */
RtfmGirParserContext *
rtfm_gir_parser_object_get_parser_context (RtfmGirParserObject *self)
{
  RtfmGirParserObjectPrivate *priv = rtfm_gir_parser_object_get_instance_private (self);

  g_return_val_if_fail (RTFM_GIR_IS_PARSER_OBJECT (self), NULL);

  return priv->parser_context;
}

static void
rtfm_gir_parser_object_finalize (GObject *object)
{
  RtfmGirParserObject *self = (RtfmGirParserObject *)object;
  RtfmGirParserObjectPrivate *priv = rtfm_gir_parser_object_get_instance_private (self);

  g_clear_pointer (&priv->parser_context, rtfm_gir_parser_context_unref);

  G_OBJECT_CLASS (rtfm_gir_parser_object_parent_class)->finalize (object);
}

static void
rtfm_gir_parser_object_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  RtfmGirParserObject *self = (RtfmGirParserObject *)object;

  switch (prop_id)
    {
    case PROP_PARSER_CONTEXT:
      g_value_set_boxed (value, rtfm_gir_parser_object_get_parser_context (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_parser_object_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  RtfmGirParserObject *self = (RtfmGirParserObject *)object;
  RtfmGirParserObjectPrivate *priv = rtfm_gir_parser_object_get_instance_private (self);

  switch (prop_id)
    {
    case PROP_PARSER_CONTEXT:
      priv->parser_context = g_value_dup_boxed (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_parser_object_class_init (RtfmGirParserObjectClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_parser_object_finalize;
  object_class->get_property = rtfm_gir_parser_object_get_property;
  object_class->set_property = rtfm_gir_parser_object_set_property;

  properties [PROP_PARSER_CONTEXT] =
    g_param_spec_boxed ("parser-context",
                        "Parser Context",
                        "The parser context used for shared allocations",
                        RTFM_GIR_TYPE_PARSER_CONTEXT,
                        (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_parser_object_init (RtfmGirParserObject *self)
{
}

/**
 * rtfm_gir_parser_object_get_children:
 * @self: An #RtfmGirParserObject
 *
 * Gets all of the children of @self in the order they were parsed.
 *
 * Returns: (nullable) (transfer none) (element-type RtfmGir.ParserObject):
 *   An array of RtfmGirParserObject or %NULL.
 */
GPtrArray *
rtfm_gir_parser_object_get_children (RtfmGirParserObject *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARSER_OBJECT (self), NULL);

  if (RTFM_GIR_PARSER_OBJECT_GET_CLASS (self)->get_children)
    return RTFM_GIR_PARSER_OBJECT_GET_CLASS (self)->get_children (self);

  return NULL;
}

/**
 * rtfm_gir_parser_object_get_children_typed:
 * @self: An #RtfmGirParserObject
 * @type: A #GType
 *
 * Gets all children of @self which are of type @type.
 *
 * Returns: (transfer container) (element-type RtfmGir.ParserObject): An
 *   array of RtfmGirParserObject instances matching @type.
 */
GPtrArray *
rtfm_gir_parser_object_get_children_typed (RtfmGirParserObject *self,
                                           GType type)
{
  GPtrArray *ar;
  GPtrArray *ret;

  g_return_val_if_fail (RTFM_GIR_IS_PARSER_OBJECT (self), NULL);

  ret = g_ptr_array_new ();

  ar = rtfm_gir_parser_object_get_children (self);

  if (ar != NULL)
    {
      guint i;

      for (i = 0; i < ar->len; i++)
        {
          RtfmGirParserObject *item = g_ptr_array_index (ar, i);

          if (g_type_is_a (G_OBJECT_TYPE (item), type))
            g_ptr_array_add (ret, item);
        }
    }

  return ret;
}

gboolean
rtfm_gir_parser_object_has_child_typed (RtfmGirParserObject *self,
                                        GType type)
{
  GPtrArray *ar;
  guint i;

  g_return_val_if_fail (RTFM_GIR_IS_PARSER_OBJECT (self), FALSE);

  ar = rtfm_gir_parser_object_get_children (self);

  if (ar != NULL)
    {
      for (i = 0; i < ar->len; i++)
        {
          GObject *object = g_ptr_array_index (ar, i);

          if (g_type_is_a (G_TYPE_FROM_INSTANCE (object), type))
            return TRUE;
        }
    }

  return FALSE;
}

gboolean
rtfm_gir_parser_object_ingest (RtfmGirParserObject *self,
                               GMarkupParseContext *context,
                               const gchar *element_name,
                               const gchar **attribute_names,
                               const gchar **attribute_values,
                               GError **error)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARSER_OBJECT (self), FALSE);
  g_return_val_if_fail (context != NULL, FALSE);
  g_return_val_if_fail (element_name != NULL, FALSE);
  g_return_val_if_fail (attribute_names != NULL, FALSE);
  g_return_val_if_fail (attribute_values != NULL, FALSE);

  if (RTFM_GIR_PARSER_OBJECT_GET_CLASS (self)->ingest)
    return RTFM_GIR_PARSER_OBJECT_GET_CLASS (self)->ingest (self, context, element_name, attribute_names, attribute_values, error);

  return TRUE;
}

void
rtfm_gir_parser_object_printf (RtfmGirParserObject *self,
                               GString *str,
                               guint depth)
{
  g_return_if_fail (RTFM_GIR_IS_PARSER_OBJECT (self));
  g_return_if_fail (str != NULL);

  if (RTFM_GIR_PARSER_OBJECT_GET_CLASS (self)->printf)
    RTFM_GIR_PARSER_OBJECT_GET_CLASS (self)->printf (self, str, depth);
}
