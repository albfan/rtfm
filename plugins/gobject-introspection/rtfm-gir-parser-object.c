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

G_DEFINE_TYPE (RtfmGirParserObject, rtfm_gir_parser_object, G_TYPE_OBJECT)

static void
rtfm_gir_parser_object_class_init (RtfmGirParserObjectClass *klass)
{
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
