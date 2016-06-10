/* rtfm-gir-search-result.c
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

#include "rtfm-gir-search-result.h"

struct _RtfmGirSearchResult
{
  GObject   parent_instance;

  GVariant *document;
};

G_DEFINE_TYPE (RtfmGirSearchResult, rtfm_gir_search_result, RTFM_TYPE_SEARCH_RESULT)

static void
rtfm_gir_search_result_finalize (GObject *object)
{
  RtfmGirSearchResult *self = (RtfmGirSearchResult *)object;

  g_clear_pointer (&self->document, g_variant_unref);

  G_OBJECT_CLASS (rtfm_gir_search_result_parent_class)->finalize (object);
}

static void
rtfm_gir_search_result_class_init (RtfmGirSearchResultClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_search_result_finalize;
}

static void
rtfm_gir_search_result_init (RtfmGirSearchResult *self)
{
}

RtfmSearchResult *
rtfm_gir_search_result_new (GVariant *document,
                            gfloat    score)
{
  const gchar *text = NULL;
  RtfmGirSearchResult *ret;
  GVariantDict dict;

  g_return_val_if_fail (document != NULL, NULL);

  g_variant_dict_init (&dict, document);
  g_variant_dict_lookup (&dict, "word", "&s", &text);

  ret = g_object_new (RTFM_GIR_TYPE_SEARCH_RESULT,
                      "score", score,
                      "text", text,
                      NULL);

  ret->document = g_variant_ref_sink (document);

  g_variant_dict_clear (&dict);

  return RTFM_SEARCH_RESULT (ret);
}
