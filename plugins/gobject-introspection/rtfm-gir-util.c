/* rtfm-gir-util.c
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

#define G_LOG_DOMAIN "rtfm-gir-util"

#include "rtfm-gir-parser-types.h"
#include "rtfm-gir-util.h"

#include "rtfm-gir-class.h"
#include "rtfm-gir-constructor.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-method.h"
#include "rtfm-gir-namespace.h"
#include "rtfm-gir-record.h"

static void
build_path_to_instance (gpointer  instance,
                        GString  *string)
{
  g_assert (RTFM_GIR_IS_PARSER_OBJECT (instance));
  g_assert (string->len > 0);

  if (string->str[string->len-1] == ']')
    g_string_append_c (string, '/');

  if (FALSE) {}
  else if (RTFM_GIR_IS_NAMESPACE (instance))
    {
      g_autofree gchar *name = NULL;
      g_autofree gchar *version = NULL;

      g_object_get (instance,
                    "name", &name,
                    "version", &version,
                    NULL);

      g_string_append_printf (string, "namespace[%s-%s]",
                              rtfm_gir_namespace_get_name (instance),
                              rtfm_gir_namespace_get_version (instance));
    }
  else if (RTFM_GIR_IS_CLASS (instance))
    {
      g_string_append_printf (string, "class[%s]",
                              rtfm_gir_class_get_name (instance));
    }
  else if (RTFM_GIR_IS_RECORD (instance))
    {
      g_string_append_printf (string, "record[%s]",
                              rtfm_gir_record_get_name (instance));
    }
  else if (RTFM_GIR_IS_FUNCTION (instance))
    {
      g_string_append_printf (string, "function[%s]",
                              rtfm_gir_function_get_name (instance));
    }
  else if (RTFM_GIR_IS_METHOD (instance))
    {
      g_string_append_printf (string, "method[%s]",
                              rtfm_gir_method_get_name (instance));
    }
  else if (RTFM_GIR_IS_CONSTRUCTOR (instance))
    {
      g_string_append_printf (string, "ctor[%s]",
                              rtfm_gir_constructor_get_name (instance));
    }
}

gchar *
rtfm_gir_generate_id (gpointer instance)
{
  g_autoptr(GPtrArray) ar = NULL;
  GString *str;
  guint i;

  g_return_val_if_fail (G_IS_OBJECT (instance), NULL);

  str = g_string_new ("gir:");

  ar = g_ptr_array_new ();
  for (; instance != NULL; instance = rtfm_gir_parser_object_get_parent (instance))
    g_ptr_array_add (ar, instance);

  for (i = ar->len; i > 0; i--)
    build_path_to_instance (g_ptr_array_index (ar, i - 1), str);

  return g_string_free (str, FALSE);
}
