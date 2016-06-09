/* rtfm-gir-doc.c
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

#define G_LOG_DOMAIN "rtfm-gir-doc"

#include "rtfm-gir-doc.h"

struct _RtfmGirDoc
{
  GObject parent_instance;
  GString *text;
  gchar *xml_space;
  gchar *xml_whitespace;
};

G_DEFINE_TYPE (RtfmGirDoc, rtfm_gir_doc, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_XML_SPACE,
  PROP_XML_WHITESPACE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_doc_text (GMarkupParseContext *context,
                   const gchar *text,
                   gsize text_len,
                   gpointer user_data,
                   GError **error)
{
  RtfmGirDoc *self = user_data;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_GIR_IS_DOC (self));

  if (self->text == NULL)
    self->text = g_string_new_len (text, text_len);
  else
    g_string_append_len (self->text, text, text_len);
}

static const GMarkupParser markup_parser = {
  NULL,
  NULL,
  rtfm_gir_doc_text,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_doc_ingest (RtfmGirParserObject *object,
                     GMarkupParseContext *context,
                     const gchar *element_name,
                     const gchar **attribute_names,
                     const gchar **attribute_values,
                     GError **error)
{
  RtfmGirDoc *self = (RtfmGirDoc *)object;

  g_assert (RTFM_GIR_IS_DOC (self));
  g_assert (g_str_equal (element_name, "doc"));

  g_clear_pointer (&self->xml_space, g_free);
  g_clear_pointer (&self->xml_whitespace, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "xml:space", &self->xml_space,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "xml:whitespace", &self->xml_whitespace,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_doc_printf (RtfmGirParserObject *object,
                     GString *str,
                     guint depth)
{
  RtfmGirDoc *self = (RtfmGirDoc *)object;
  guint i;

  g_assert (RTFM_GIR_IS_DOC (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<doc");

  if (self->xml_space != NULL)
    g_string_append_printf (str, " xml:space=\"%s\"", self->xml_space);
  if (self->xml_whitespace != NULL)
    g_string_append_printf (str, " xml:whitespace=\"%s\"", self->xml_whitespace);

  if (self->text != NULL && self->text->len)
    {
      g_autofree gchar *escaped = g_markup_escape_text (self->text->str, self->text->len);
      g_string_append_printf (str, ">%s</doc>\n", escaped);
    }
  else
    g_string_append (str, "></doc>\n");
}

static void
rtfm_gir_doc_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  RtfmGirDoc *self = (RtfmGirDoc *)object;

  switch (prop_id)
    {
    case PROP_XML_SPACE:
      g_value_set_string (value, self->xml_space);
      break;

    case PROP_XML_WHITESPACE:
      g_value_set_string (value, self->xml_whitespace);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_doc_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  RtfmGirDoc *self = (RtfmGirDoc *)object;

  switch (prop_id)
    {
    case PROP_XML_SPACE:
      g_free (self->xml_space);
      self->xml_space = g_value_dup_string (value);
      break;

    case PROP_XML_WHITESPACE:
      g_free (self->xml_whitespace);
      self->xml_whitespace = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_doc_finalize (GObject *object)
{
  RtfmGirDoc *self = (RtfmGirDoc *)object;

  if (self->text != NULL)
    {
      g_string_free (self->text, TRUE);
      self->text = NULL;
    }

  g_clear_pointer (&self->xml_space, g_free);
  g_clear_pointer (&self->xml_whitespace, g_free);

  G_OBJECT_CLASS (rtfm_gir_doc_parent_class)->finalize (object);
}

static void
rtfm_gir_doc_class_init (RtfmGirDocClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_doc_get_property;
  object_class->set_property = rtfm_gir_doc_set_property;
  object_class->finalize = rtfm_gir_doc_finalize;

  parent_class->ingest = rtfm_gir_doc_ingest;
  parent_class->printf = rtfm_gir_doc_printf;

  properties [PROP_XML_SPACE] =
    g_param_spec_string ("xml-space",
                         "xml-space",
                         "xml-space",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_XML_WHITESPACE] =
    g_param_spec_string ("xml-whitespace",
                         "xml-whitespace",
                         "xml-whitespace",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
rtfm_gir_doc_init (RtfmGirDoc *self)
{
}

const gchar *
rtfm_gir_doc_get_xml_space (RtfmGirDoc *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_DOC (self), NULL);

  return self->xml_space;
}

const gchar *
rtfm_gir_doc_get_xml_whitespace (RtfmGirDoc *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_DOC (self), NULL);

  return self->xml_whitespace;
}

RtfmGirDoc *
rtfm_gir_doc_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_DOC, NULL);
}
