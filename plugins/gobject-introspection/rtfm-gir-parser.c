/* rtfm-gir-parser.c
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

#define G_LOG_DOMAIN "rtfm-gir-parser"

#include "rtfm-gir-parser.h"

struct _RtfmGirParser
{
  GObject parent_instance;
};

G_DEFINE_TYPE (RtfmGirParser, rtfm_gir_parser, G_TYPE_OBJECT)

static void
rtfm_gir_parser_class_init (RtfmGirParserClass *klass)
{
}

static void
rtfm_gir_parser_init (RtfmGirParser *self)
{
}

static void
rtfm_gir_start_element (GMarkupParseContext *context,
                        const gchar *element_name,
                        const gchar **attribute_names,
                        const gchar **attribute_values,
                        gpointer user_data,
                        GError **error)
{
  RtfmGirRepository **result = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (result != NULL);

  if (g_str_equal (element_name, "repository"))
    {
      g_autoptr(RtfmGirRepository) child = NULL;
      g_autoptr(RtfmGirParserContext) parser_context = NULL;

      parser_context = rtfm_gir_parser_context_new ();
      child = rtfm_gir_repository_new (parser_context);

      if (rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child),
                                         context,
                                         element_name,
                                         attribute_names,
                                         attribute_values,
                                         error))
        {
          g_clear_object (result);
          *result = g_steal_pointer (&child);
        }
    }
}

static void
rtfm_gir_end_element (GMarkupParseContext *context,
                      const gchar *element_name,
                      gpointer user_data,
                      GError **error)
{
  g_assert (context != NULL);
  g_assert (element_name != NULL);

  if (g_str_equal (element_name, "repository"))
    {
      g_markup_parse_context_pop (context);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_start_element,
  rtfm_gir_end_element,
  NULL,
  NULL,
  NULL,
};

/**
 * rtfm_gir_parser_parse_file:
 * @self: A #RtfmGirParser
 * @file: A #GFile
 * @cancellable: (nullable): A #GCancellable or %NULL.
 * @error: A location for a #GError or %NULL.
 *
 * Parses the contents of @file and returns the toplevel object.
 *
 * Returns: (transfer full): An #RtfmGirRepository or %NULL upon failure.
 */
RtfmGirRepository *
rtfm_gir_parser_parse_file (RtfmGirParser *self,
                            GFile *file,
                            GCancellable *cancellable,
                            GError **error)
{
  g_autoptr(GMarkupParseContext) context = NULL;
  g_autoptr(RtfmGirRepository) ret = NULL;
  g_autofree gchar *content = NULL;
  gsize content_len = 0;

  g_return_val_if_fail (RTFM_GIR_IS_PARSER (self), NULL);
  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable), NULL);

  if (!g_file_load_contents (file, cancellable, &content, &content_len, NULL, error))
    return NULL;

  context = g_markup_parse_context_new (&markup_parser, 0, &ret, NULL);

  if (!g_markup_parse_context_parse (context, content, content_len, error))
    return NULL;

  if (!g_markup_parse_context_end_parse (context, error))
    return NULL;

  if (ret == NULL)
    {
      g_set_error (error,
                   G_MARKUP_ERROR,
                   G_MARKUP_ERROR_INVALID_CONTENT,
                   "Failed to locate \"repository\" element");
      return NULL;
    }

  return g_steal_pointer (&ret);
}

RtfmGirParser *
rtfm_gir_parser_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_PARSER, NULL);
}

static gboolean
find_pair (const gchar **attribute_names,
           const gchar **attribute_values,
           const gchar *name,
           const gchar **value)
{
  guint i;

  *value = NULL;

  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (name != NULL);
  g_assert (value != NULL);

  for (i = 0; attribute_names[i]; i++)
    {
      if (g_strcmp0 (attribute_names[i], name) == 0)
        {
          *value = attribute_values[i];
          return TRUE;
        }
    }

  return FALSE;
}

gboolean
rtfm_gir_g_markup_collect_attributes (const gchar *element_name,
                                      const gchar **attribute_names,
                                      const gchar **attribute_values,
                                      GError **error,
                                      GMarkupCollectType first_type,
                                      const gchar *first_attr,
                                      ...)
{
  GMarkupCollectType type = first_type;
  const gchar *attr = first_attr;
  const gchar *value;
  gpointer *data;
  gboolean ret = FALSE;
  va_list args;

  g_return_val_if_fail (element_name != NULL, FALSE);
  g_return_val_if_fail (attribute_names != NULL, FALSE);
  g_return_val_if_fail (attribute_values != NULL, FALSE);

  if (first_type == G_MARKUP_COLLECT_INVALID)
    return TRUE;

  va_start (args, first_attr);

  for (;;)
    {
      data = va_arg (args, gpointer *);

      if (data != NULL)
        *data = NULL;

      if (find_pair (attribute_names, attribute_values, attr, &value))
        {
          const gchar *names[] = { attr, NULL };
          const gchar *values[] = { value, NULL };

          if (!g_markup_collect_attributes (element_name,
                                            names,
                                            values,
                                            error,
                                            type, attr, data,
                                            G_MARKUP_COLLECT_INVALID))
            goto failure;

        }
      else if (!(type & G_MARKUP_COLLECT_OPTIONAL))
        {
          g_set_error (error,
                       G_MARKUP_ERROR,
                       G_MARKUP_ERROR_MISSING_ATTRIBUTE,
                       "The attribute \"%s\" of element \"%s\" is missing",
                       attr, element_name);
          goto failure;
        }

      type = va_arg (args, GMarkupCollectType);
      if (type == G_MARKUP_COLLECT_INVALID)
        break;

      attr = va_arg (args, const gchar *);
    }

  ret = TRUE;

failure:
  va_end (args);

  return ret;
}
