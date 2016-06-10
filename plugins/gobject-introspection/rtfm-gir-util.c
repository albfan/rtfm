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

gchar *
rtfm_gir_generate_id (gpointer instance)
{
  g_return_val_if_fail (G_IS_OBJECT (instance), NULL);

  if (RTFM_GIR_IS_NAMESPACE (instance))
    {
      g_autofree gchar *name = NULL;
      g_autofree gchar *version = NULL;

      g_object_get (instance,
                    "name", &name,
                    "version", &version,
                    NULL);

      return g_strdup_printf ("gir:%s-%s", name, version);
    }

  return NULL;
}
