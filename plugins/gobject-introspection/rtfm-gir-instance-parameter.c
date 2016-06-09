/* rtfm-gir-instance-parameter.c
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

#define G_LOG_DOMAIN "rtfm-gir-instance-parameter"

#include "rtfm-gir-instance-parameter.h"

#include "rtfm-gir-doc-version.h"
#include "rtfm-gir-doc-stability.h"
#include "rtfm-gir-doc.h"
#include "rtfm-gir-doc-deprecated.h"
#include "rtfm-gir-type.h"

struct _RtfmGirInstanceParameter
{
  GObject parent_instance;
  gchar *name;
  gchar *nullable;
  gchar *allow_none;
  gchar *direction;
  gchar *caller_allocates;
  gchar *transfer_ownership;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirInstanceParameter, rtfm_gir_instance_parameter, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_NAME,
  PROP_NULLABLE,
  PROP_ALLOW_NONE,
  PROP_DIRECTION,
  PROP_CALLER_ALLOCATES,
  PROP_TRANSFER_OWNERSHIP,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_instance_parameter_get_children (RtfmGirParserObject *object)
{
  RtfmGirInstanceParameter *self = (RtfmGirInstanceParameter *)object;

  g_assert (RTFM_GIR_IS_INSTANCE_PARAMETER (self));

  return self->children;
}

static void
rtfm_gir_instance_parameter_start_element (GMarkupParseContext *context,
                                           const gchar *element_name,
                                           const gchar **attribute_names,
                                           const gchar **attribute_values,
                                           gpointer user_data,
                                           GError **error)
{
  RtfmGirInstanceParameter *self = user_data;

  g_assert (RTFM_GIR_IS_INSTANCE_PARAMETER (self));
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
}

static void
rtfm_gir_instance_parameter_end_element (GMarkupParseContext *context,
                                         const gchar *element_name,
                                         gpointer user_data,
                                         GError **error)
{
  g_assert (RTFM_GIR_IS_INSTANCE_PARAMETER (user_data));
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
}

static const GMarkupParser markup_parser = {
  rtfm_gir_instance_parameter_start_element,
  rtfm_gir_instance_parameter_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_instance_parameter_ingest (RtfmGirParserObject *object,
                                    GMarkupParseContext *context,
                                    const gchar *element_name,
                                    const gchar **attribute_names,
                                    const gchar **attribute_values,
                                    GError **error)
{
  RtfmGirInstanceParameter *self = (RtfmGirInstanceParameter *)object;

  g_assert (RTFM_GIR_IS_INSTANCE_PARAMETER (self));
  g_assert (g_str_equal (element_name, "instance-parameter"));

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->nullable, g_free);
  g_clear_pointer (&self->allow_none, g_free);
  g_clear_pointer (&self->direction, g_free);
  g_clear_pointer (&self->caller_allocates, g_free);
  g_clear_pointer (&self->transfer_ownership, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "nullable", &self->nullable,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "allow-none", &self->allow_none,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "direction", &self->direction,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "caller-allocates", &self->caller_allocates,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "transfer-ownership", &self->transfer_ownership,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_instance_parameter_printf (RtfmGirParserObject *object,
                                    GString *str,
                                    guint depth)
{
  RtfmGirInstanceParameter *self = (RtfmGirInstanceParameter *)object;
  guint i;

  g_assert (RTFM_GIR_IS_INSTANCE_PARAMETER (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<instance-parameter");

  if (self->name != NULL)
    g_string_append_printf (str, " name=\"%s\"", self->name);
  if (self->nullable != NULL)
    g_string_append_printf (str, " nullable=\"%s\"", self->nullable);
  if (self->allow_none != NULL)
    g_string_append_printf (str, " allow-none=\"%s\"", self->allow_none);
  if (self->direction != NULL)
    g_string_append_printf (str, " direction=\"%s\"", self->direction);
  if (self->caller_allocates != NULL)
    g_string_append_printf (str, " caller-allocates=\"%s\"", self->caller_allocates);
  if (self->transfer_ownership != NULL)
    g_string_append_printf (str, " transfer-ownership=\"%s\"", self->transfer_ownership);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</instance-parameter>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_instance_parameter_get_property (GObject    *object,
                                          guint       prop_id,
                                          GValue     *value,
                                          GParamSpec *pspec)
{
  RtfmGirInstanceParameter *self = (RtfmGirInstanceParameter *)object;

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

    case PROP_DIRECTION:
      g_value_set_string (value, self->direction);
      break;

    case PROP_CALLER_ALLOCATES:
      g_value_set_string (value, self->caller_allocates);
      break;

    case PROP_TRANSFER_OWNERSHIP:
      g_value_set_string (value, self->transfer_ownership);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_instance_parameter_set_property (GObject      *object,
                                          guint         prop_id,
                                          const GValue *value,
                                          GParamSpec   *pspec)
{
  RtfmGirInstanceParameter *self = (RtfmGirInstanceParameter *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_NULLABLE:
      g_free (self->nullable);
      self->nullable = g_value_dup_string (value);
      break;

    case PROP_ALLOW_NONE:
      g_free (self->allow_none);
      self->allow_none = g_value_dup_string (value);
      break;

    case PROP_DIRECTION:
      g_free (self->direction);
      self->direction = g_value_dup_string (value);
      break;

    case PROP_CALLER_ALLOCATES:
      g_free (self->caller_allocates);
      self->caller_allocates = g_value_dup_string (value);
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
rtfm_gir_instance_parameter_finalize (GObject *object)
{
  RtfmGirInstanceParameter *self = (RtfmGirInstanceParameter *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->nullable, g_free);
  g_clear_pointer (&self->allow_none, g_free);
  g_clear_pointer (&self->direction, g_free);
  g_clear_pointer (&self->caller_allocates, g_free);
  g_clear_pointer (&self->transfer_ownership, g_free);
  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_instance_parameter_parent_class)->finalize (object);
}

static void
rtfm_gir_instance_parameter_class_init (RtfmGirInstanceParameterClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_instance_parameter_get_property;
  object_class->set_property = rtfm_gir_instance_parameter_set_property;
  object_class->finalize = rtfm_gir_instance_parameter_finalize;

  parent_class->ingest = rtfm_gir_instance_parameter_ingest;
  parent_class->printf = rtfm_gir_instance_parameter_printf;
  parent_class->get_children = rtfm_gir_instance_parameter_get_children;

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

  properties [PROP_TRANSFER_OWNERSHIP] =
    g_param_spec_string ("transfer-ownership",
                         "transfer-ownership",
                         "transfer-ownership",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void
rtfm_gir_instance_parameter_init (RtfmGirInstanceParameter *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_instance_parameter_get_name (RtfmGirInstanceParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_INSTANCE_PARAMETER (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_instance_parameter_get_nullable (RtfmGirInstanceParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_INSTANCE_PARAMETER (self), NULL);

  return self->nullable;
}

const gchar *
rtfm_gir_instance_parameter_get_allow_none (RtfmGirInstanceParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_INSTANCE_PARAMETER (self), NULL);

  return self->allow_none;
}

const gchar *
rtfm_gir_instance_parameter_get_direction (RtfmGirInstanceParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_INSTANCE_PARAMETER (self), NULL);

  return self->direction;
}

const gchar *
rtfm_gir_instance_parameter_get_caller_allocates (RtfmGirInstanceParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_INSTANCE_PARAMETER (self), NULL);

  return self->caller_allocates;
}

const gchar *
rtfm_gir_instance_parameter_get_transfer_ownership (RtfmGirInstanceParameter *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_INSTANCE_PARAMETER (self), NULL);

  return self->transfer_ownership;
}

RtfmGirInstanceParameter *
rtfm_gir_instance_parameter_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_INSTANCE_PARAMETER, NULL);
}
