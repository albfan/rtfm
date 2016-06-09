/* rtfm-gir-varargs.c
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

#define G_LOG_DOMAIN "rtfm-gir-varargs"

#include "rtfm-gir-varargs.h"

struct _RtfmGirVarargs
{
  GObject parent_instance;
};

G_DEFINE_TYPE (RtfmGirVarargs, rtfm_gir_varargs, RTFM_GIR_TYPE_PARSER_OBJECT)

static gboolean
rtfm_gir_varargs_ingest (RtfmGirParserObject *object,
                         GMarkupParseContext *context,
                         const gchar *element_name,
                         const gchar **attribute_names,
                         const gchar **attribute_values,
                         GError **error)
{
  RtfmGirVarargs *self = (RtfmGirVarargs *)object;

  g_assert (RTFM_GIR_IS_VARARGS (self));
  g_assert (g_str_equal (element_name, "varargs"));


  return TRUE;
}

static void
rtfm_gir_varargs_printf (RtfmGirParserObject *object,
                         GString *str,
                         guint depth)
{
  RtfmGirVarargs *self = (RtfmGirVarargs *)object;
  guint i;

  g_assert (RTFM_GIR_IS_VARARGS (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<varargs");


  g_string_append (str, "/>\n");
}

static void
rtfm_gir_varargs_finalize (GObject *object)
{

  G_OBJECT_CLASS (rtfm_gir_varargs_parent_class)->finalize (object);
}

static void
rtfm_gir_varargs_class_init (RtfmGirVarargsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_varargs_finalize;

  parent_class->ingest = rtfm_gir_varargs_ingest;
  parent_class->printf = rtfm_gir_varargs_printf;
}

static void
rtfm_gir_varargs_init (RtfmGirVarargs *self)
{
}

RtfmGirVarargs *
rtfm_gir_varargs_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_VARARGS, NULL);
}
