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

#define G_LOG_DOMAIN "rtfm-gir-search-result"

#include <glib/gi18n.h>
#include <string.h>

#include "rtfm-gir-search-result.h"
#include "rtfm-gir-util.h"

#include "rtfm-gir-constructor.h"
#include "rtfm-gir-class.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-method.h"
#include "rtfm-gir-record.h"

struct _RtfmGirSearchResult
{
  GObject   parent_instance;
  GType     item_type;
  GVariant *document;
};

enum {
  PROP_0,
  PROP_DOCUMENT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

G_DEFINE_TYPE (RtfmGirSearchResult, rtfm_gir_search_result, RTFM_TYPE_SEARCH_RESULT)

static void
rtfm_gir_search_result_set_document (RtfmGirSearchResult *self,
                                     GVariant            *document)
{
  const gchar *text = NULL;
  const gchar *id = NULL;
  const gchar *icon_name = NULL;
  const gchar *category = NULL;
  GVariantDict dict;

  g_return_if_fail (RTFM_GIR_IS_SEARCH_RESULT (self));

  if (document == NULL)
    return;

  self->document = g_variant_ref_sink (document);

  g_variant_dict_init (&dict, document);
  g_variant_dict_lookup (&dict, "id", "&s", &id);
  g_variant_dict_lookup (&dict, "word", "&s", &text);

  if (id != NULL)
    {
      if (FALSE) {}
      else if (strstr (id, "method[") != NULL)
        {
          icon_name = "lang-method-symbolic";
          self->item_type = RTFM_GIR_TYPE_METHOD;
          category = _("Methods");
        }
      else if (strstr (id, "function[") != NULL)
        {
          icon_name = "lang-function-symbolic";
          self->item_type = RTFM_GIR_TYPE_FUNCTION;
          category = _("Functions");
        }
      else if (strstr (id, "ctor[") != NULL)
        {
          icon_name = "lang-method-symbolic";
          self->item_type = RTFM_GIR_TYPE_CONSTRUCTOR;
          category = _("Constructors");
        }
      else if (strstr (id, "record[") != NULL)
        {
          icon_name = "lang-struct-symbolic";
          self->item_type = RTFM_GIR_TYPE_RECORD;
          category = _("Structs");
        }
      else if (strstr (id, "class[") != NULL)
        {
          icon_name = "lang-class-symbolic";
          self->item_type = RTFM_GIR_TYPE_CLASS;
          category = _("Classes");
        }
      else if (strstr (id, "namespace[") != NULL)
        {
          icon_name = "lang-namespace-symbolic";
          self->item_type = RTFM_GIR_TYPE_NAMESPACE;
          category = _("Namespaces");
        }
    }

  if (category != NULL)
    rtfm_search_result_set_category (RTFM_SEARCH_RESULT (self), category);

  if (icon_name != NULL)
    rtfm_search_result_set_icon_name (RTFM_SEARCH_RESULT (self), icon_name);

  if (text != NULL)
    rtfm_search_result_set_text (RTFM_SEARCH_RESULT (self), text);

  g_variant_dict_clear (&dict);
}

static void
rtfm_gir_search_result_finalize (GObject *object)
{
  RtfmGirSearchResult *self = (RtfmGirSearchResult *)object;

  g_clear_pointer (&self->document, g_variant_unref);

  G_OBJECT_CLASS (rtfm_gir_search_result_parent_class)->finalize (object);
}

static void
rtfm_gir_search_result_get_property (GObject    *object,
                                     guint       prop_id,
                                     GValue     *value,
                                     GParamSpec *pspec)
{
  RtfmGirSearchResult *self = RTFM_GIR_SEARCH_RESULT (object);

  switch (prop_id)
    {
    case PROP_DOCUMENT:
      g_value_set_variant (value, self->document);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_search_result_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  RtfmGirSearchResult *self = RTFM_GIR_SEARCH_RESULT (object);

  switch (prop_id)
    {
    case PROP_DOCUMENT:
      rtfm_gir_search_result_set_document (self, g_value_get_variant (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_search_result_class_init (RtfmGirSearchResultClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_search_result_finalize;
  object_class->get_property = rtfm_gir_search_result_get_property;
  object_class->set_property = rtfm_gir_search_result_set_property;

  properties [PROP_DOCUMENT] =
    g_param_spec_variant ("document",
                          "Document",
                          "The document for the search result.",
                          G_VARIANT_TYPE_ANY,
                          NULL,
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_search_result_init (RtfmGirSearchResult *self)
{
}

RtfmSearchResult *
rtfm_gir_search_result_new (const gchar *nsname,
                            GVariant    *document,
                            gfloat       score)
{
  RtfmGirSearchResult *ret;

  g_return_val_if_fail (document != NULL, NULL);

  ret = g_object_new (RTFM_GIR_TYPE_SEARCH_RESULT,
                      "subtitle", nsname,
                      "document", document,
                      "score", score,
                      NULL);

  /* Rescore for categories, etc */
  rtfm_search_result_set_score (RTFM_SEARCH_RESULT (ret), rtfm_gir_rescore (ret));

  return RTFM_SEARCH_RESULT (ret);
}

GType
rtfm_gir_search_result_get_item_type (RtfmGirSearchResult *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_SEARCH_RESULT (self), G_TYPE_INVALID);

  return self->item_type;
}
