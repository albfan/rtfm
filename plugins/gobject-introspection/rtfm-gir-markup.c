/* rtfm-gir-markup.c
 *
 * Copyright (C) 2016 Christian Hergert <christian@hergert.me>
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "rtfm-gir-markup.h"

gboolean
find_pair (const gchar **attribute_names,
           const gchar **attribute_values,
           const gchar  *name,
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
rtfm_g_markup_collect_some_attributes (const gchar         *element_name,
                                       const gchar        **attribute_names,
                                       const gchar        **attribute_values,
                                       GError             **error,
                                       GMarkupCollectType   first_type,
                                       const gchar         *first_attr,
                                       ...)
{
  GMarkupCollectType type = first_type;
  const gchar *attr = first_attr;
  const gchar *value;
  gpointer *data;
  gboolean ret = FALSE;
  va_list args;
  guint i;

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
                       "The attribute \"%s\" is missing",
                       attr);
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
