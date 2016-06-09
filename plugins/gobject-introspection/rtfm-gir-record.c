/* rtfm-gir-record.c
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

#define G_LOG_DOMAIN "rtfm-gir-record"

#include "rtfm-gir-record.h"

#include "rtfm-gir-doc-version.h"
#include "rtfm-gir-doc-stability.h"
#include "rtfm-gir-doc.h"
#include "rtfm-gir-doc-deprecated.h"
#include "rtfm-gir-annotation.h"
#include "rtfm-gir-field.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-union.h"
#include "rtfm-gir-method.h"
#include "rtfm-gir-constructor.h"
#include "rtfm-gir-property.h"

struct _RtfmGirRecord
{
  GObject parent_instance;
  gchar *introspectable;
  gchar *deprecated;
  gchar *deprecated_version;
  gchar *version;
  gchar *stability;
  gchar *name;
  gchar *c_type;
  gchar *disguised;
  gchar *glib_type_name;
  gchar *glib_get_type;
  gchar *c_symbol_prefix;
  gchar *foreign;
  gchar *glib_is_gtype_struct_for;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirRecord, rtfm_gir_record, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_INTROSPECTABLE,
  PROP_DEPRECATED,
  PROP_DEPRECATED_VERSION,
  PROP_VERSION,
  PROP_STABILITY,
  PROP_NAME,
  PROP_C_TYPE,
  PROP_DISGUISED,
  PROP_GLIB_TYPE_NAME,
  PROP_GLIB_GET_TYPE,
  PROP_C_SYMBOL_PREFIX,
  PROP_FOREIGN,
  PROP_GLIB_IS_GTYPE_STRUCT_FOR,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_record_get_children (RtfmGirParserObject *object)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;

  g_assert (RTFM_GIR_IS_RECORD (self));

  return self->children;
}

static void
rtfm_gir_record_start_element (GMarkupParseContext *context,
                               const gchar *element_name,
                               const gchar **attribute_names,
                               const gchar **attribute_values,
                               gpointer user_data,
                               GError **error)
{
  RtfmGirRecord *self = user_data;

  g_assert (RTFM_GIR_IS_RECORD (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  if (FALSE) {}
  else if (g_str_equal (element_name, "doc-version"))
    {
      g_autoptr(RtfmGirDocVersion) child = NULL;

      child = rtfm_gir_doc_version_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc-stability"))
    {
      g_autoptr(RtfmGirDocStability) child = NULL;

      child = rtfm_gir_doc_stability_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc"))
    {
      g_autoptr(RtfmGirDoc) child = NULL;

      child = rtfm_gir_doc_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc-deprecated"))
    {
      g_autoptr(RtfmGirDocDeprecated) child = NULL;

      child = rtfm_gir_doc_deprecated_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "annotation"))
    {
      g_autoptr(RtfmGirAnnotation) child = NULL;

      child = rtfm_gir_annotation_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "field"))
    {
      g_autoptr(RtfmGirField) child = NULL;

      child = rtfm_gir_field_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "function"))
    {
      g_autoptr(RtfmGirFunction) child = NULL;

      child = rtfm_gir_function_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "union"))
    {
      g_autoptr(RtfmGirUnion) child = NULL;

      child = rtfm_gir_union_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "method"))
    {
      g_autoptr(RtfmGirMethod) child = NULL;

      child = rtfm_gir_method_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "constructor"))
    {
      g_autoptr(RtfmGirConstructor) child = NULL;

      child = rtfm_gir_constructor_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "property"))
    {
      g_autoptr(RtfmGirProperty) child = NULL;

      child = rtfm_gir_property_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_record_end_element (GMarkupParseContext *context,
                             const gchar *element_name,
                             gpointer user_data,
                             GError **error)
{
  g_assert (RTFM_GIR_IS_RECORD (user_data));
  g_assert (context != NULL);
  g_assert (element_name != NULL);

  if (FALSE) {}
  else if (g_str_equal (element_name, "doc-version"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "doc-stability"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "doc"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "doc-deprecated"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "field"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "function"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "union"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "method"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "constructor"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "property"))
    {
      g_markup_parse_context_pop (context);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_record_start_element,
  rtfm_gir_record_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_record_ingest (RtfmGirParserObject *object,
                        GMarkupParseContext *context,
                        const gchar *element_name,
                        const gchar **attribute_names,
                        const gchar **attribute_values,
                        GError **error)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;

  g_assert (RTFM_GIR_IS_RECORD (self));
  g_assert (g_str_equal (element_name, "record"));

  g_clear_pointer (&self->introspectable, g_free);
  g_clear_pointer (&self->deprecated, g_free);
  g_clear_pointer (&self->deprecated_version, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->stability, g_free);
  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_clear_pointer (&self->disguised, g_free);
  g_clear_pointer (&self->glib_type_name, g_free);
  g_clear_pointer (&self->glib_get_type, g_free);
  g_clear_pointer (&self->c_symbol_prefix, g_free);
  g_clear_pointer (&self->foreign, g_free);
  g_clear_pointer (&self->glib_is_gtype_struct_for, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "introspectable", &self->introspectable,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "deprecated", &self->deprecated,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "deprecated-version", &self->deprecated_version,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "version", &self->version,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "stability", &self->stability,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:type", &self->c_type,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "disguised", &self->disguised,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:type-name", &self->glib_type_name,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:get-type", &self->glib_get_type,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:symbol-prefix", &self->c_symbol_prefix,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "foreign", &self->foreign,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:is-gtype-struct-for", &self->glib_is_gtype_struct_for,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_record_printf (RtfmGirParserObject *object,
                        GString *str,
                        guint depth)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;
  guint i;

  g_assert (RTFM_GIR_IS_RECORD (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<record");

  if (self->introspectable != NULL)
    g_string_append_printf (str, " introspectable=\"%s\"", self->introspectable);
  if (self->deprecated != NULL)
    g_string_append_printf (str, " deprecated=\"%s\"", self->deprecated);
  if (self->deprecated_version != NULL)
    g_string_append_printf (str, " deprecated-version=\"%s\"", self->deprecated_version);
  if (self->version != NULL)
    g_string_append_printf (str, " version=\"%s\"", self->version);
  if (self->stability != NULL)
    g_string_append_printf (str, " stability=\"%s\"", self->stability);
  if (self->name != NULL)
    g_string_append_printf (str, " name=\"%s\"", self->name);
  if (self->c_type != NULL)
    g_string_append_printf (str, " c:type=\"%s\"", self->c_type);
  if (self->disguised != NULL)
    g_string_append_printf (str, " disguised=\"%s\"", self->disguised);
  if (self->glib_type_name != NULL)
    g_string_append_printf (str, " glib:type-name=\"%s\"", self->glib_type_name);
  if (self->glib_get_type != NULL)
    g_string_append_printf (str, " glib:get-type=\"%s\"", self->glib_get_type);
  if (self->c_symbol_prefix != NULL)
    g_string_append_printf (str, " c:symbol-prefix=\"%s\"", self->c_symbol_prefix);
  if (self->foreign != NULL)
    g_string_append_printf (str, " foreign=\"%s\"", self->foreign);
  if (self->glib_is_gtype_struct_for != NULL)
    g_string_append_printf (str, " glib:is-gtype-struct-for=\"%s\"", self->glib_is_gtype_struct_for);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</record>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_record_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;

  switch (prop_id)
    {
    case PROP_INTROSPECTABLE:
      g_value_set_string (value, self->introspectable);
      break;

    case PROP_DEPRECATED:
      g_value_set_string (value, self->deprecated);
      break;

    case PROP_DEPRECATED_VERSION:
      g_value_set_string (value, self->deprecated_version);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_STABILITY:
      g_value_set_string (value, self->stability);
      break;

    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_C_TYPE:
      g_value_set_string (value, self->c_type);
      break;

    case PROP_DISGUISED:
      g_value_set_string (value, self->disguised);
      break;

    case PROP_GLIB_TYPE_NAME:
      g_value_set_string (value, self->glib_type_name);
      break;

    case PROP_GLIB_GET_TYPE:
      g_value_set_string (value, self->glib_get_type);
      break;

    case PROP_C_SYMBOL_PREFIX:
      g_value_set_string (value, self->c_symbol_prefix);
      break;

    case PROP_FOREIGN:
      g_value_set_string (value, self->foreign);
      break;

    case PROP_GLIB_IS_GTYPE_STRUCT_FOR:
      g_value_set_string (value, self->glib_is_gtype_struct_for);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_record_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;

  switch (prop_id)
    {
    case PROP_INTROSPECTABLE:
      g_free (self->introspectable);
      self->introspectable = g_value_dup_string (value);
      break;

    case PROP_DEPRECATED:
      g_free (self->deprecated);
      self->deprecated = g_value_dup_string (value);
      break;

    case PROP_DEPRECATED_VERSION:
      g_free (self->deprecated_version);
      self->deprecated_version = g_value_dup_string (value);
      break;

    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
      break;

    case PROP_STABILITY:
      g_free (self->stability);
      self->stability = g_value_dup_string (value);
      break;

    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_C_TYPE:
      g_free (self->c_type);
      self->c_type = g_value_dup_string (value);
      break;

    case PROP_DISGUISED:
      g_free (self->disguised);
      self->disguised = g_value_dup_string (value);
      break;

    case PROP_GLIB_TYPE_NAME:
      g_free (self->glib_type_name);
      self->glib_type_name = g_value_dup_string (value);
      break;

    case PROP_GLIB_GET_TYPE:
      g_free (self->glib_get_type);
      self->glib_get_type = g_value_dup_string (value);
      break;

    case PROP_C_SYMBOL_PREFIX:
      g_free (self->c_symbol_prefix);
      self->c_symbol_prefix = g_value_dup_string (value);
      break;

    case PROP_FOREIGN:
      g_free (self->foreign);
      self->foreign = g_value_dup_string (value);
      break;

    case PROP_GLIB_IS_GTYPE_STRUCT_FOR:
      g_free (self->glib_is_gtype_struct_for);
      self->glib_is_gtype_struct_for = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_record_finalize (GObject *object)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;

  g_clear_pointer (&self->introspectable, g_free);
  g_clear_pointer (&self->deprecated, g_free);
  g_clear_pointer (&self->deprecated_version, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->stability, g_free);
  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_clear_pointer (&self->disguised, g_free);
  g_clear_pointer (&self->glib_type_name, g_free);
  g_clear_pointer (&self->glib_get_type, g_free);
  g_clear_pointer (&self->c_symbol_prefix, g_free);
  g_clear_pointer (&self->foreign, g_free);
  g_clear_pointer (&self->glib_is_gtype_struct_for, g_free);
  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_record_parent_class)->finalize (object);
}

static void
rtfm_gir_record_class_init (RtfmGirRecordClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_record_get_property;
  object_class->set_property = rtfm_gir_record_set_property;
  object_class->finalize = rtfm_gir_record_finalize;

  parent_class->ingest = rtfm_gir_record_ingest;
  parent_class->printf = rtfm_gir_record_printf;
  parent_class->get_children = rtfm_gir_record_get_children;

  properties [PROP_INTROSPECTABLE] =
    g_param_spec_string ("introspectable",
                         "introspectable",
                         "introspectable",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DEPRECATED] =
    g_param_spec_string ("deprecated",
                         "deprecated",
                         "deprecated",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DEPRECATED_VERSION] =
    g_param_spec_string ("deprecated-version",
                         "deprecated-version",
                         "deprecated-version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_STABILITY] =
    g_param_spec_string ("stability",
                         "stability",
                         "stability",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_TYPE] =
    g_param_spec_string ("c-type",
                         "c-type",
                         "c-type",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DISGUISED] =
    g_param_spec_string ("disguised",
                         "disguised",
                         "disguised",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_TYPE_NAME] =
    g_param_spec_string ("glib-type-name",
                         "glib-type-name",
                         "glib-type-name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_GET_TYPE] =
    g_param_spec_string ("glib-get-type",
                         "glib-get-type",
                         "glib-get-type",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_SYMBOL_PREFIX] =
    g_param_spec_string ("c-symbol-prefix",
                         "c-symbol-prefix",
                         "c-symbol-prefix",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_FOREIGN] =
    g_param_spec_string ("foreign",
                         "foreign",
                         "foreign",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_IS_GTYPE_STRUCT_FOR] =
    g_param_spec_string ("glib-is-gtype-struct-for",
                         "glib-is-gtype-struct-for",
                         "glib-is-gtype-struct-for",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
rtfm_gir_record_init (RtfmGirRecord *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_record_get_introspectable (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->introspectable;
}

const gchar *
rtfm_gir_record_get_deprecated (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->deprecated;
}

const gchar *
rtfm_gir_record_get_deprecated_version (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->deprecated_version;
}

const gchar *
rtfm_gir_record_get_version (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->version;
}

const gchar *
rtfm_gir_record_get_stability (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->stability;
}

const gchar *
rtfm_gir_record_get_name (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_record_get_c_type (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->c_type;
}

const gchar *
rtfm_gir_record_get_disguised (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->disguised;
}

const gchar *
rtfm_gir_record_get_glib_type_name (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->glib_type_name;
}

const gchar *
rtfm_gir_record_get_glib_get_type (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->glib_get_type;
}

const gchar *
rtfm_gir_record_get_c_symbol_prefix (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->c_symbol_prefix;
}

const gchar *
rtfm_gir_record_get_foreign (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->foreign;
}

const gchar *
rtfm_gir_record_get_glib_is_gtype_struct_for (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RECORD (self), NULL);

  return self->glib_is_gtype_struct_for;
}

RtfmGirRecord *
rtfm_gir_record_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_RECORD, NULL);
}
