/* rtfm-gir-plugin.c
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

#include <libpeas/peas.h>
#include <rtfm.h>

#include "rtfm-gir-provider.h"
#include "rtfm-gir-view-addin.h"

static GStringChunk *strings;

const gchar *
rtfm_gir_plugin_intern_string (const gchar *string)
{
  return g_string_chunk_insert_const (strings, string);
}

void
peas_register_types (PeasObjectModule *module)
{
  strings = g_string_chunk_new (4096);

  peas_object_module_register_extension_type (module,
                                              RTFM_TYPE_PROVIDER,
                                              RTFM_TYPE_GIR_PROVIDER);
  peas_object_module_register_extension_type (module,
                                              RTFM_TYPE_VIEW_ADDIN,
                                              RTFM_TYPE_GIR_VIEW_ADDIN);
}
