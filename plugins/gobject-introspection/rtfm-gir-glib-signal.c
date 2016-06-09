/* rtfm-gir-glib-signal.c
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

#define G_LOG_DOMAIN "rtfm-gir-glib-signal"

#include "rtfm-gir-glib-signal.h"

#include "rtfm-gir-doc-version.h"
#include "rtfm-gir-doc-stability.h"
#include "rtfm-gir-doc.h"
#include "rtfm-gir-doc-deprecated.h"
#include "rtfm-gir-annotation.h"
#include "rtfm-gir-parameters.h"
#include "rtfm-gir-return-value.h"

struct _RtfmGirGlibSignal
{
  GObject parent_instance;
  gchar *introspectable;
  gchar *deprecated;
  gchar *deprecated_version;
  gchar *version;
  gchar *stability;
  gchar *name;
  gchar *detailed;
  gchar *when;
  gchar *action;
  gchar *no_hooks;
  gchar *no_recurse;
  GPtrArray *children;
};

G_DEFINE_TYPE (RtfmGirGlibSignal, rtfm_gir_glib_signal, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_INTROSPECTABLE,
  PROP_DEPRECATED,
  PROP_DEPRECATED_VERSION,
  PROP_VERSION,
  PROP_STABILITY,
  PROP_NAME,
  PROP_DETAILED,
  PROP_WHEN,
  PROP_ACTION,
  PROP_NO_HOOKS,
  PROP_NO_RECURSE,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GPtrArray *
rtfm_gir_glib_signal_get_children (RtfmGirParserObject *object)
{
  RtfmGirGlibSignal *self = (RtfmGirGlibSignal *)object;

  g_assert (RTFM_GIR_IS_GLIB_SIGNAL (self));

  return self->children;
}

static void
rtfm_gir_glib_signal_start_element (GMarkupParseContext *context,
                                    const gchar *element_name,
                                    const gchar **attribute_names,
                                    const gchar **attribute_values,
                                    gpointer user_data,
                                    GError **error)
{
  RtfmGirGlibSignal *self = user_data;

  g_assert (RTFM_GIR_IS_GLIB_SIGNAL (self));
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
  else if (g_str_equal (element_name, "parameters"))
    {
      g_autoptr(RtfmGirParameters) child = NULL;

      child = rtfm_gir_parameters_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
  else if (g_str_equal (element_name, "return-value"))
    {
      g_autoptr(RtfmGirReturnValue) child = NULL;

      child = rtfm_gir_return_value_new ();

      if (!rtfm_gir_parser_object_ingest (RTFM_GIR_PARSER_OBJECT (child), context, element_name, attribute_names, attribute_values, error))
        return;

      g_ptr_array_add (self->children, g_steal_pointer (&child));
    }
}

static void
rtfm_gir_glib_signal_end_element (GMarkupParseContext *context,
                                  const gchar *element_name,
                                  gpointer user_data,
                                  GError **error)
{
  g_assert (RTFM_GIR_IS_GLIB_SIGNAL (user_data));
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
  else if (g_str_equal (element_name, "parameters"))
    {
      g_markup_parse_context_pop (context);
    }
  else if (g_str_equal (element_name, "return-value"))
    {
      g_markup_parse_context_pop (context);
    }
}

static const GMarkupParser markup_parser = {
  rtfm_gir_glib_signal_start_element,
  rtfm_gir_glib_signal_end_element,
  NULL,
  NULL,
  NULL,
};

static gboolean
rtfm_gir_glib_signal_ingest (RtfmGirParserObject *object,
                             GMarkupParseContext *context,
                             const gchar *element_name,
                             const gchar **attribute_names,
                             const gchar **attribute_values,
                             GError **error)
{
  RtfmGirGlibSignal *self = (RtfmGirGlibSignal *)object;

  g_assert (RTFM_GIR_IS_GLIB_SIGNAL (self));
  g_assert (g_str_equal (element_name, "glib:signal"));

  g_clear_pointer (&self->introspectable, g_free);
  g_clear_pointer (&self->deprecated, g_free);
  g_clear_pointer (&self->deprecated_version, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->stability, g_free);
  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->detailed, g_free);
  g_clear_pointer (&self->when, g_free);
  g_clear_pointer (&self->action, g_free);
  g_clear_pointer (&self->no_hooks, g_free);
  g_clear_pointer (&self->no_recurse, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "introspectable", &self->introspectable,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "deprecated", &self->deprecated,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "deprecated-version", &self->deprecated_version,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "version", &self->version,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "stability", &self->stability,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "detailed", &self->detailed,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "when", &self->when,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "action", &self->action,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "no-hooks", &self->no_hooks,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "no-recurse", &self->no_recurse,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  g_markup_parse_context_push (context, &markup_parser, self);

  return TRUE;
}

static void
rtfm_gir_glib_signal_printf (RtfmGirParserObject *object,
                             GString *str,
                             guint depth)
{
  RtfmGirGlibSignal *self = (RtfmGirGlibSignal *)object;
  guint i;

  g_assert (RTFM_GIR_IS_GLIB_SIGNAL (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<glib:signal");

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
  if (self->detailed != NULL)
    g_string_append_printf (str, " detailed=\"%s\"", self->detailed);
  if (self->when != NULL)
    g_string_append_printf (str, " when=\"%s\"", self->when);
  if (self->action != NULL)
    g_string_append_printf (str, " action=\"%s\"", self->action);
  if (self->no_hooks != NULL)
    g_string_append_printf (str, " no-hooks=\"%s\"", self->no_hooks);
  if (self->no_recurse != NULL)
    g_string_append_printf (str, " no-recurse=\"%s\"", self->no_recurse);

  if (self->children != NULL && self->children->len > 0)
    {
      g_string_append (str, ">\n");

      for (i = 0; i < self->children->len; i++)
        rtfm_gir_parser_object_printf (g_ptr_array_index (self->children, i), str, depth + 1);

      for (i = 0; i < depth; i++)
        g_string_append (str, "  ");
      g_string_append (str, "</glib:signal>\n");
    }
  else
    {
      g_string_append (str, "/>\n");
    }
}

static void
rtfm_gir_glib_signal_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  RtfmGirGlibSignal *self = (RtfmGirGlibSignal *)object;

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

    case PROP_DETAILED:
      g_value_set_string (value, self->detailed);
      break;

    case PROP_WHEN:
      g_value_set_string (value, self->when);
      break;

    case PROP_ACTION:
      g_value_set_string (value, self->action);
      break;

    case PROP_NO_HOOKS:
      g_value_set_string (value, self->no_hooks);
      break;

    case PROP_NO_RECURSE:
      g_value_set_string (value, self->no_recurse);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_glib_signal_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  RtfmGirGlibSignal *self = (RtfmGirGlibSignal *)object;

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

    case PROP_DETAILED:
      g_free (self->detailed);
      self->detailed = g_value_dup_string (value);
      break;

    case PROP_WHEN:
      g_free (self->when);
      self->when = g_value_dup_string (value);
      break;

    case PROP_ACTION:
      g_free (self->action);
      self->action = g_value_dup_string (value);
      break;

    case PROP_NO_HOOKS:
      g_free (self->no_hooks);
      self->no_hooks = g_value_dup_string (value);
      break;

    case PROP_NO_RECURSE:
      g_free (self->no_recurse);
      self->no_recurse = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_glib_signal_finalize (GObject *object)
{
  RtfmGirGlibSignal *self = (RtfmGirGlibSignal *)object;

  g_clear_pointer (&self->introspectable, g_free);
  g_clear_pointer (&self->deprecated, g_free);
  g_clear_pointer (&self->deprecated_version, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->stability, g_free);
  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->detailed, g_free);
  g_clear_pointer (&self->when, g_free);
  g_clear_pointer (&self->action, g_free);
  g_clear_pointer (&self->no_hooks, g_free);
  g_clear_pointer (&self->no_recurse, g_free);
  g_clear_pointer (&self->children, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_glib_signal_parent_class)->finalize (object);
}

static void
rtfm_gir_glib_signal_class_init (RtfmGirGlibSignalClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_glib_signal_get_property;
  object_class->set_property = rtfm_gir_glib_signal_set_property;
  object_class->finalize = rtfm_gir_glib_signal_finalize;

  parent_class->ingest = rtfm_gir_glib_signal_ingest;
  parent_class->printf = rtfm_gir_glib_signal_printf;
  parent_class->get_children = rtfm_gir_glib_signal_get_children;

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

  properties [PROP_DETAILED] =
    g_param_spec_string ("detailed",
                         "detailed",
                         "detailed",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_WHEN] =
    g_param_spec_string ("when",
                         "when",
                         "when",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_ACTION] =
    g_param_spec_string ("action",
                         "action",
                         "action",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_NO_HOOKS] =
    g_param_spec_string ("no-hooks",
                         "no-hooks",
                         "no-hooks",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_NO_RECURSE] =
    g_param_spec_string ("no-recurse",
                         "no-recurse",
                         "no-recurse",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_glib_signal_init (RtfmGirGlibSignal *self)
{
  self->children = g_ptr_array_new_with_free_func (g_object_unref);
}

const gchar *
rtfm_gir_glib_signal_get_introspectable (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->introspectable;
}

const gchar *
rtfm_gir_glib_signal_get_deprecated (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->deprecated;
}

const gchar *
rtfm_gir_glib_signal_get_deprecated_version (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->deprecated_version;
}

const gchar *
rtfm_gir_glib_signal_get_version (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->version;
}

const gchar *
rtfm_gir_glib_signal_get_stability (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->stability;
}

const gchar *
rtfm_gir_glib_signal_get_name (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->name;
}

const gchar *
rtfm_gir_glib_signal_get_detailed (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->detailed;
}

const gchar *
rtfm_gir_glib_signal_get_when (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->when;
}

const gchar *
rtfm_gir_glib_signal_get_action (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->action;
}

const gchar *
rtfm_gir_glib_signal_get_no_hooks (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->no_hooks;
}

const gchar *
rtfm_gir_glib_signal_get_no_recurse (RtfmGirGlibSignal *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_GLIB_SIGNAL (self), NULL);

  return self->no_recurse;
}

RtfmGirGlibSignal *
rtfm_gir_glib_signal_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_GLIB_SIGNAL, NULL);
}
