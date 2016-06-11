/* rtfm-gir-parameter.c
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

#define G_LOG_DOMAIN "rtfm-gir-parameter"

#include "rtfm-gir-parameter.h"

#include "rtfm-gir-doc-version.h"
#include "rtfm-gir-doc-stability.h"
#include "rtfm-gir-doc.h"
#include "rtfm-gir-doc-deprecated.h"
#include "rtfm-gir-type.h"
#include "rtfm-gir-array.h"
#include "rtfm-gir-varargs.h"

struct _RtfmGirParameter
{
  GObject parent_instance;
  const gchar *name;
  const gchar *nullable;
  const gchar *allow_none;
  const gchar *introspectable;
  const gchar *closure;
  const gchar *destroy;
  const gchar *scope;
  const gchar *direction;
  const gchar *caller_allocates;
  const gchar *optional;
  const gchar *skip;
  const gchar *transfer_ownership;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirParameter, rtfm_gir_parameter, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_NAME,
  PROP_NULLABLE,
  PROP_ALLOW_NONE,
  PROP_INTROSPECTABLE,
  PROP_CLOSURE,
  PROP_DESTROY,
  PROP_SCOPE,
  PROP_DIRECTION,
  PROP_CALLER_ALLOCATES,
  PROP_OPTIONAL,
  PROP_SKIP,
  PROP_TRANSFER_OWNERSHIP,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_parameter_get_children (RtfmGirParserObject *object)
{
  RtfmGirParameter *self = (RtfmGirParameter *)object;

  g_assert (RTFM_GIR_IS_PARAMETER (self));

  return self->children;
}

static void
rtfm_gir_parameter_start_element (GMarkupParseContext *context,
                                  const gchar *element_name,
                                  const gchar **attribute_names,
                                  const gchar **attribute_values,
                                  gpointer user_data,
                                  GError **error)
{
  RtfmGirParameter *self = user_data;
  RtfmGirParserContext *parser_context;

  g_assert (RTFM_GIR_IS_PARAMETER (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  parser_context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));

  if (FALSE) {}
  else if (g_str_equal (element_name, "doc-version"))
    {
      g_autoptr(RtfmGirDocVersion) child = NULL;

      child = rtfm_gir_doc_version_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc-stability"))
    {
      g_autoptr(RtfmGirDocStability) child = NULL;

      child = rtfm_gir_doc_stability_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc"))
    {
      g_autoptr(RtfmGirDoc) child = NULL;

      child = rtfm_gir_doc_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "doc-deprecated"))
    {
      g_autoptr(RtfmGirDocDeprecated) child = NULL;

      child = rtfm_gir_doc_deprecated_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "type"))
    {
      g_autoptr(RtfmGirType) child = NULL;

      child = rtfm_gir_type_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "array"))
    {
      g_autoptr(RtfmGirArray) child = NULL;

      child = rtfm_gir_array_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "varargs"))
    {
      g_autoptr(RtfmGirVarargs) child = NULL;

      child = rtfm_gir_varargs_new (parser_context);

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      _rtfm_gir_parser_object_set_parent (RTFM_GIR_PARSER_OBJECT (child), RTFM_GIR_PARSER_OBJECT (self));

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_parameter_end_element (GMarkupParseContext *context,
                                const gchar *element_name,
                                gpointer user_data,
                                GError **error)
{
  g_assert (RTFM_GIR_IS_PARAMETER (user_data));
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
  else if (g_str_equal (element_name, "type"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "array"))
    {
      g_markup_parse_context_pop (context);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_parameter_start_element,
  rtfm_gir_parameter_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_parameter_ingest (RtfmGirParserObject *object,
                           GMarkupParseContext *context,
                           const gchar *element_name,
                           const gchar **attribute_names,
                           const gchar **attribute_values,
                           GError **error)
{
  RtfmGirParameter *self = (RtfmGirParameter *)object;
  RtfmGirParserContext *parser_context;
  const gchar *name = NULL;
  const gchar *nullable = NULL;
  const gchar *allow_none = NULL;
  const gchar *introspectable = NULL;
  const gchar *closure = NULL;
  const gchar *destroy = NULL;
  const gchar *scope = NULL;
  const gchar *direction = NULL;
  const gchar *caller_allocates = NULL;
  const gchar *optional = NULL;
  const gchar *skip = NULL;
  const gchar *transfer_ownership = NULL;

  g_assert (RTFM_GIR_IS_PARAMETER (self));
  g_assert (g_str_equal (element_name, "parameter"));

  parser_context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));


  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "nullable", &nullable,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "allow-none", &allow_none,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "introspectable", &introspectable,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "closure", &closure,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "destroy", &destroy,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "scope", &scope,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "direction", &direction,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "caller-allocates", &caller_allocates,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "optional", &optional,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "skip", &skip,
                                             G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "transfer-ownership", &transfer_ownership,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  self->name = rtfm_gir_parser_context_intern_string (parser_context, name);
  self->nullable = rtfm_gir_parser_context_intern_string (parser_context, nullable);
  self->allow_none = rtfm_gir_parser_context_intern_string (parser_context, allow_none);
  self->introspectable = rtfm_gir_parser_context_intern_string (parser_context, introspectable);
  self->closure = rtfm_gir_parser_context_intern_string (parser_context, closure);
  self->destroy = rtfm_gir_parser_context_intern_string (parser_context, destroy);
  self->scope = rtfm_gir_parser_context_intern_string (parser_context, scope);
  self->direction = rtfm_gir_parser_context_intern_string (parser_context, direction);
  self->caller_allocates = rtfm_gir_parser_context_intern_string (parser_context, caller_allocates);
  self->optional = rtfm_gir_parser_context_intern_string (parser_context, optional);
  self->skip = rtfm_gir_parser_context_intern_string (parser_context, skip);
  self->transfer_ownership = rtfm_gir_parser_context_intern_string (parser_context, transfer_ownership);

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_parameter_printf (RtfmGirParserObject *object,
                           GString *str,
                           guint depth)
{
  RtfmGirParameter *self = (RtfmGirParameter *)object;
  guint i;

  g_assert (RTFM_GIR_IS_PARAMETER (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<parameter");

  if (self->name != NULL)
    g_string_append_printf (str, " name=\"%s\"", self->name);
  if (self->nullable != NULL)
    g_string_append_printf (str, " nullable=\"%s\"", self->nullable);
  if (self->allow_none != NULL)
    g_string_append_printf (str, " allow-none=\"%s\"", self->allow_none);
  if (self->introspectable != NULL)
    g_string_append_printf (str, " introspectable=\"%s\"", self->introspectable);
  if (self->closure != NULL)
    g_string_append_printf (str, " closure=\"%s\"", self->closure);
  if (self->destroy != NULL)
    g_string_append_printf (str, " destroy=\"%s\"", self->destroy);
  if (self->scope != NULL)
    g_string_append_printf (str, " scope=\"%s\"", self->scope);
  if (self->direction != NULL)
    g_string_append_printf (str, " direction=\"%s\"", self->direction);
  if (self->caller_allocates != NULL)
    g_string_append_printf (str, " caller-allocates=\"%s\"", self->caller_allocates);
  if (self->optional != NULL)
    g_string_append_printf (str, " optional=\"%s\"", self->optional);
  if (self->skip != NULL)
    g_string_append_printf (str, " skip=\"%s\"", self->skip);
  if (self->transfer_ownership != NULL)
    g_string_append_printf (str, " transfer-ownership=\"%s\"", self->transfer_ownership);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</parameter>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_parameter_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  RtfmGirParameter *self = (RtfmGirParameter *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_NULLABLE:
      g_value_set_string (value, self->nullable);
      break;

    case PROP_ALLOW_NONE:
      g_value_set_string (value, self->allow_none);
      break;

    case PROP_INTROSPECTABLE:
      g_value_set_string (value, self->introspectable);
      break;

    case PROP_CLOSURE:
      g_value_set_string (value, self->closure);
      break;

    case PROP_DESTROY:
      g_value_set_string (value, self->destroy);
      break;

    case PROP_SCOPE:
      g_value_set_string (value, self->scope);
      break;

    case PROP_DIRECTION:
      g_value_set_string (value, self->direction);
      break;

    case PROP_CALLER_ALLOCATES:
      g_value_set_string (value, self->caller_allocates);
      break;

    case PROP_OPTIONAL:
      g_value_set_string (value, self->optional);
      break;

    case PROP_SKIP:
      g_value_set_string (value, self->skip);
      break;

    case PROP_TRANSFER_OWNERSHIP:
      g_value_set_string (value, self->transfer_ownership);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_parameter_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  RtfmGirParameter *self = (RtfmGirParameter *)object;
  RtfmGirParserContext *context = rtfm_gir_parser_object_get_parser_context (RTFM_GIR_PARSER_OBJECT (self));

  switch (prop_id)
    {
    case PROP_NAME:
      self->name = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_NULLABLE:
      self->nullable = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_ALLOW_NONE:
      self->allow_none = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_INTROSPECTABLE:
      self->introspectable = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_CLOSURE:
      self->closure = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_DESTROY:
      self->destroy = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_SCOPE:
      self->scope = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_DIRECTION:
      self->direction = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_CALLER_ALLOCATES:
      self->caller_allocates = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_OPTIONAL:
      self->optional = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_SKIP:
      self->skip = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    case PROP_TRANSFER_OWNERSHIP:
      self->transfer_ownership = rtfm_gir_parser_context_intern_string (context, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_parameter_finalize (GObject *object)
{
  RtfmGirParameter *self = (RtfmGirParameter *)object;

  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_parameter_parent_class)->finalize (object);
}

static void
rtfm_gir_parameter_class_init (RtfmGirParameterClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_parameter_get_property;
  object_class->set_property = rtfm_gir_parameter_set_property;
  object_class->finalize = rtfm_gir_parameter_finalize;

  parent_class->ingest = rtfm_gir_parameter_ingest;
  parent_class->printf = rtfm_gir_parameter_printf;
  parent_class->get_children = rtfm_gir_parameter_get_children;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_NULLABLE] =
    g_param_spec_string ("nullable",
                         "nullable",
                         "nullable",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_ALLOW_NONE] =
    g_param_spec_string ("allow-none",
                         "allow-none",
                         "allow-none",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_INTROSPECTABLE] =
    g_param_spec_string ("introspectable",
                         "introspectable",
                         "introspectable",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_CLOSURE] =
    g_param_spec_string ("closure",
                         "closure",
                         "closure",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DESTROY] =
    g_param_spec_string ("destroy",
                         "destroy",
                         "destroy",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_SCOPE] =
    g_param_spec_string ("scope",
                         "scope",
                         "scope",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DIRECTION] =
    g_param_spec_string ("direction",
                         "direction",
                         "direction",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_CALLER_ALLOCATES] =
    g_param_spec_string ("caller-allocates",
                         "caller-allocates",
                         "caller-allocates",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_OPTIONAL] =
    g_param_spec_string ("optional",
                         "optional",
                         "optional",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_SKIP] =
    g_param_spec_string ("skip",
                         "skip",
                         "skip",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_TRANSFER_OWNERSHIP] =
    g_param_spec_string ("transfer-ownership",
                         "transfer-ownership",
                         "transfer-ownership",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_parameter_init (RtfmGirParameter *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_parameter_get_name (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_parameter_get_nullable (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->nullable;
}

const gchar *
rtfm_gir_parameter_get_allow_none (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->allow_none;
}

const gchar *
rtfm_gir_parameter_get_introspectable (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->introspectable;
}

const gchar *
rtfm_gir_parameter_get_closure (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->closure;
}

const gchar *
rtfm_gir_parameter_get_destroy (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->destroy;
}

const gchar *
rtfm_gir_parameter_get_scope (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->scope;
}

const gchar *
rtfm_gir_parameter_get_direction (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->direction;
}

const gchar *
rtfm_gir_parameter_get_caller_allocates (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->caller_allocates;
}

const gchar *
rtfm_gir_parameter_get_optional (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->optional;
}

const gchar *
rtfm_gir_parameter_get_skip (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->skip;
}

const gchar *
rtfm_gir_parameter_get_transfer_ownership (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PARAMETER (self), NULL);

  return self->transfer_ownership;
}

RtfmGirParameter *
rtfm_gir_parameter_new (RtfmGirParserContext *parser_context)
{
  return g_object_new (RTFM_GIR_TYPE_PARAMETER,
                       "parser-context", parser_context,
                       NULL);
}
