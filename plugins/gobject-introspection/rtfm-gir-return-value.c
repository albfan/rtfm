/* rtfm-gir-return-value.c
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

#define G_LOG_DOMAIN "rtfm-gir-return-value"

#include "rtfm-gir-return-value.h"

#include "rtfm-gir-doc-version.h"
#include "rtfm-gir-doc-stability.h"
#include "rtfm-gir-doc.h"
#include "rtfm-gir-doc-deprecated.h"
#include "rtfm-gir-type.h"
#include "rtfm-gir-array.h"

struct _RtfmGirReturnValue
{
  GObject parent_instance;
  gchar *introspectable;
  gchar *nullable;
  gchar *closure;
  gchar *scope;
  gchar *destroy;
  gchar *skip;
  gchar *allow_none;
  gchar *transfer_ownership;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirReturnValue, rtfm_gir_return_value, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_INTROSPECTABLE,
  PROP_NULLABLE,
  PROP_CLOSURE,
  PROP_SCOPE,
  PROP_DESTROY,
  PROP_SKIP,
  PROP_ALLOW_NONE,
  PROP_TRANSFER_OWNERSHIP,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_return_value_get_children (RtfmGirParserObject *object)
{
  RtfmGirReturnValue *self = (RtfmGirReturnValue *)object;

  g_assert (RTFM_GIR_IS_RETURN_VALUE (self));

  return self->children;
}

static void
rtfm_gir_return_value_start_element (GMarkupParseContext *context,
                                     const gchar *element_name,
                                     const gchar **attribute_names,
                                     const gchar **attribute_values,
                                     gpointer user_data,
                                     GError **error)
{
  RtfmGirReturnValue *self = user_data;

  g_assert (RTFM_GIR_IS_RETURN_VALUE (self));
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
  else if (g_str_equal (element_name, "type"))
    {
      g_autoptr(RtfmGirType) child = NULL;

      child = rtfm_gir_type_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "array"))
    {
      g_autoptr(RtfmGirArray) child = NULL;

      child = rtfm_gir_array_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_return_value_end_element (GMarkupParseContext *context,
                                   const gchar *element_name,
                                   gpointer user_data,
                                   GError **error)
{
  g_assert (RTFM_GIR_IS_RETURN_VALUE (user_data));
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
  rtfm_gir_return_value_start_element,
  rtfm_gir_return_value_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_return_value_ingest (RtfmGirParserObject *object,
                              GMarkupParseContext *context,
                              const gchar *element_name,
                              const gchar **attribute_names,
                              const gchar **attribute_values,
                              GError **error)
{
  RtfmGirReturnValue *self = (RtfmGirReturnValue *)object;

  g_assert (RTFM_GIR_IS_RETURN_VALUE (self));
  g_assert (g_str_equal (element_name, "return-value"));

  g_clear_pointer (&self->introspectable, g_free);
  g_clear_pointer (&self->nullable, g_free);
  g_clear_pointer (&self->closure, g_free);
  g_clear_pointer (&self->scope, g_free);
  g_clear_pointer (&self->destroy, g_free);
  g_clear_pointer (&self->skip, g_free);
  g_clear_pointer (&self->allow_none, g_free);
  g_clear_pointer (&self->transfer_ownership, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "introspectable", &self->introspectable,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "nullable", &self->nullable,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "closure", &self->closure,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "scope", &self->scope,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "destroy", &self->destroy,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "skip", &self->skip,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "allow-none", &self->allow_none,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "transfer-ownership", &self->transfer_ownership,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_return_value_printf (RtfmGirParserObject *object,
                              GString *str,
                              guint depth)
{
  RtfmGirReturnValue *self = (RtfmGirReturnValue *)object;
  guint i;

  g_assert (RTFM_GIR_IS_RETURN_VALUE (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<return-value");

  if (self->introspectable != NULL)
    g_string_append_printf (str, " introspectable=\"%s\"", self->introspectable);
  if (self->nullable != NULL)
    g_string_append_printf (str, " nullable=\"%s\"", self->nullable);
  if (self->closure != NULL)
    g_string_append_printf (str, " closure=\"%s\"", self->closure);
  if (self->scope != NULL)
    g_string_append_printf (str, " scope=\"%s\"", self->scope);
  if (self->destroy != NULL)
    g_string_append_printf (str, " destroy=\"%s\"", self->destroy);
  if (self->skip != NULL)
    g_string_append_printf (str, " skip=\"%s\"", self->skip);
  if (self->allow_none != NULL)
    g_string_append_printf (str, " allow-none=\"%s\"", self->allow_none);
  if (self->transfer_ownership != NULL)
    g_string_append_printf (str, " transfer-ownership=\"%s\"", self->transfer_ownership);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</return-value>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_return_value_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  RtfmGirReturnValue *self = (RtfmGirReturnValue *)object;

  switch (prop_id)
    {
    case PROP_INTROSPECTABLE:
      g_value_set_string (value, self->introspectable);
      break;

    case PROP_NULLABLE:
      g_value_set_string (value, self->nullable);
      break;

    case PROP_CLOSURE:
      g_value_set_string (value, self->closure);
      break;

    case PROP_SCOPE:
      g_value_set_string (value, self->scope);
      break;

    case PROP_DESTROY:
      g_value_set_string (value, self->destroy);
      break;

    case PROP_SKIP:
      g_value_set_string (value, self->skip);
      break;

    case PROP_ALLOW_NONE:
      g_value_set_string (value, self->allow_none);
      break;

    case PROP_TRANSFER_OWNERSHIP:
      g_value_set_string (value, self->transfer_ownership);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_return_value_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  RtfmGirReturnValue *self = (RtfmGirReturnValue *)object;

  switch (prop_id)
    {
    case PROP_INTROSPECTABLE:
      g_free (self->introspectable);
      self->introspectable = g_value_dup_string (value);
      break;

    case PROP_NULLABLE:
      g_free (self->nullable);
      self->nullable = g_value_dup_string (value);
      break;

    case PROP_CLOSURE:
      g_free (self->closure);
      self->closure = g_value_dup_string (value);
      break;

    case PROP_SCOPE:
      g_free (self->scope);
      self->scope = g_value_dup_string (value);
      break;

    case PROP_DESTROY:
      g_free (self->destroy);
      self->destroy = g_value_dup_string (value);
      break;

    case PROP_SKIP:
      g_free (self->skip);
      self->skip = g_value_dup_string (value);
      break;

    case PROP_ALLOW_NONE:
      g_free (self->allow_none);
      self->allow_none = g_value_dup_string (value);
      break;

    case PROP_TRANSFER_OWNERSHIP:
      g_free (self->transfer_ownership);
      self->transfer_ownership = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_return_value_finalize (GObject *object)
{
  RtfmGirReturnValue *self = (RtfmGirReturnValue *)object;

  g_clear_pointer (&self->introspectable, g_free);
  g_clear_pointer (&self->nullable, g_free);
  g_clear_pointer (&self->closure, g_free);
  g_clear_pointer (&self->scope, g_free);
  g_clear_pointer (&self->destroy, g_free);
  g_clear_pointer (&self->skip, g_free);
  g_clear_pointer (&self->allow_none, g_free);
  g_clear_pointer (&self->transfer_ownership, g_free);
  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_return_value_parent_class)->finalize (object);
}

static void
rtfm_gir_return_value_class_init (RtfmGirReturnValueClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_return_value_get_property;
  object_class->set_property = rtfm_gir_return_value_set_property;
  object_class->finalize = rtfm_gir_return_value_finalize;

  parent_class->ingest = rtfm_gir_return_value_ingest;
  parent_class->printf = rtfm_gir_return_value_printf;
  parent_class->get_children = rtfm_gir_return_value_get_children;

  properties [PROP_INTROSPECTABLE] =
    g_param_spec_string ("introspectable",
                         "introspectable",
                         "introspectable",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_NULLABLE] =
    g_param_spec_string ("nullable",
                         "nullable",
                         "nullable",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_CLOSURE] =
    g_param_spec_string ("closure",
                         "closure",
                         "closure",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_SCOPE] =
    g_param_spec_string ("scope",
                         "scope",
                         "scope",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DESTROY] =
    g_param_spec_string ("destroy",
                         "destroy",
                         "destroy",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_SKIP] =
    g_param_spec_string ("skip",
                         "skip",
                         "skip",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_ALLOW_NONE] =
    g_param_spec_string ("allow-none",
                         "allow-none",
                         "allow-none",
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
rtfm_gir_return_value_init (RtfmGirReturnValue *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_return_value_get_introspectable (RtfmGirReturnValue *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RETURN_VALUE (self), NULL);

  return self->introspectable;
}

const gchar *
rtfm_gir_return_value_get_nullable (RtfmGirReturnValue *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RETURN_VALUE (self), NULL);

  return self->nullable;
}

const gchar *
rtfm_gir_return_value_get_closure (RtfmGirReturnValue *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RETURN_VALUE (self), NULL);

  return self->closure;
}

const gchar *
rtfm_gir_return_value_get_scope (RtfmGirReturnValue *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RETURN_VALUE (self), NULL);

  return self->scope;
}

const gchar *
rtfm_gir_return_value_get_destroy (RtfmGirReturnValue *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RETURN_VALUE (self), NULL);

  return self->destroy;
}

const gchar *
rtfm_gir_return_value_get_skip (RtfmGirReturnValue *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RETURN_VALUE (self), NULL);

  return self->skip;
}

const gchar *
rtfm_gir_return_value_get_allow_none (RtfmGirReturnValue *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RETURN_VALUE (self), NULL);

  return self->allow_none;
}

const gchar *
rtfm_gir_return_value_get_transfer_ownership (RtfmGirReturnValue *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_RETURN_VALUE (self), NULL);

  return self->transfer_ownership;
}

RtfmGirReturnValue *
rtfm_gir_return_value_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_RETURN_VALUE, NULL);
}
