/* rtfm-gir-repository.c
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

#define G_LOG_DOMAIN "rtfm-gir-repository"

#include "rtfm-gir-repository.h"

#include "rtfm-gir-include.h"
#include "rtfm-gir-c-include.h"
#include "rtfm-gir-package.h"
#include "rtfm-gir-namespace.h"

struct _RtfmGirRepository
{
  GObject parent_instance;
  gchar *version;
  gchar *c_identifier_prefixes;
  gchar *c_symbol_prefixes;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirRepository, rtfm_gir_repository, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_VERSION,
  PROP_C_IDENTIFIER_PREFIXES,
  PROP_C_SYMBOL_PREFIXES,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_repository_get_children (RtfmGirParserObject *object)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  g_assert (RTFM_GIR_IS_REPOSITORY (self));

  return self->children;
}

static void
rtfm_gir_repository_start_element (GMarkupParseContext *context,
                                   const gchar *element_name,
                                   const gchar **attribute_names,
                                   const gchar **attribute_values,
                                   gpointer user_data,
                                   GError **error)
{
  RtfmGirRepository *self = user_data;

  g_assert (RTFM_GIR_IS_REPOSITORY (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  if (FALSE) {}
  else if (g_str_equal (element_name, "include"))
    {
      g_autoptr(RtfmGirInclude) child = NULL;

      child = rtfm_gir_include_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "c:include"))
    {
      g_autoptr(RtfmGirCInclude) child = NULL;

      child = rtfm_gir_c_include_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "package"))
    {
      g_autoptr(RtfmGirPackage) child = NULL;

      child = rtfm_gir_package_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "namespace"))
    {
      g_autoptr(RtfmGirNamespace) child = NULL;

      child = rtfm_gir_namespace_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_repository_end_element (GMarkupParseContext *context,
                                 const gchar *element_name,
                                 gpointer user_data,
                                 GError **error)
{
  g_assert (RTFM_GIR_IS_REPOSITORY (user_data));
  g_assert (context != NULL);
  g_assert (element_name != NULL);

  if (FALSE) {}
  else if (g_str_equal (element_name, "namespace"))
    {
      g_markup_parse_context_pop (context);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_repository_start_element,
  rtfm_gir_repository_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_repository_ingest (RtfmGirParserObject *object,
                            GMarkupParseContext *context,
                            const gchar *element_name,
                            const gchar **attribute_names,
                            const gchar **attribute_values,
                            GError **error)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  g_assert (RTFM_GIR_IS_REPOSITORY (self));
  g_assert (g_str_equal (element_name, "repository"));

  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->c_identifier_prefixes, g_free);
  g_clear_pointer (&self->c_symbol_prefixes, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "version", &self->version,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:identifier-prefixes", &self->c_identifier_prefixes,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:symbol-prefixes", &self->c_symbol_prefixes,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_repository_printf (RtfmGirParserObject *object,
                            GString *str,
                            guint depth)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;
  guint i;

  g_assert (RTFM_GIR_IS_REPOSITORY (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<repository");

  if (self->version != NULL)
    g_string_append_printf (str, " version=\"%s\"", self->version);
  if (self->c_identifier_prefixes != NULL)
    g_string_append_printf (str, " c:identifier-prefixes=\"%s\"", self->c_identifier_prefixes);
  if (self->c_symbol_prefixes != NULL)
    g_string_append_printf (str, " c:symbol-prefixes=\"%s\"", self->c_symbol_prefixes);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</repository>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_repository_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  switch (prop_id)
    {
    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_C_IDENTIFIER_PREFIXES:
      g_value_set_string (value, self->c_identifier_prefixes);
      break;

    case PROP_C_SYMBOL_PREFIXES:
      g_value_set_string (value, self->c_symbol_prefixes);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_repository_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  switch (prop_id)
    {
    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
      break;

    case PROP_C_IDENTIFIER_PREFIXES:
      g_free (self->c_identifier_prefixes);
      self->c_identifier_prefixes = g_value_dup_string (value);
      break;

    case PROP_C_SYMBOL_PREFIXES:
      g_free (self->c_symbol_prefixes);
      self->c_symbol_prefixes = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_repository_finalize (GObject *object)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->c_identifier_prefixes, g_free);
  g_clear_pointer (&self->c_symbol_prefixes, g_free);
  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_repository_parent_class)->finalize (object);
}

static void
rtfm_gir_repository_class_init (RtfmGirRepositoryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_repository_get_property;
  object_class->set_property = rtfm_gir_repository_set_property;
  object_class->finalize = rtfm_gir_repository_finalize;

  parent_class->ingest = rtfm_gir_repository_ingest;
  parent_class->printf = rtfm_gir_repository_printf;
  parent_class->get_children = rtfm_gir_repository_get_children;

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_IDENTIFIER_PREFIXES] =
    g_param_spec_string ("c-identifier-prefixes",
                         "c-identifier-prefixes",
                         "c-identifier-prefixes",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_SYMBOL_PREFIXES] =
    g_param_spec_string ("c-symbol-prefixes",
                         "c-symbol-prefixes",
                         "c-symbol-prefixes",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
rtfm_gir_repository_init (RtfmGirRepository *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_repository_get_version (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_REPOSITORY (self), NULL);

  return self->version;
}

const gchar *
rtfm_gir_repository_get_c_identifier_prefixes (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_REPOSITORY (self), NULL);

  return self->c_identifier_prefixes;
}

const gchar *
rtfm_gir_repository_get_c_symbol_prefixes (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_REPOSITORY (self), NULL);

  return self->c_symbol_prefixes;
}

RtfmGirRepository *
rtfm_gir_repository_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_REPOSITORY, NULL);
}
