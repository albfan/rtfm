/* rtfm-gir-method.c
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

#define G_LOG_DOMAIN "rtfm-gir-method"

#include "rtfm-gir-method.h"
#include "rtfm-gir-markup.h"
#include "rtfm-gir-return-value.h"
#include "rtfm-gir-parameters.h"

#if 0
# define ENTRY     do { g_printerr ("ENTRY: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); } while (0)
# define EXIT      do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return; } while (0)
# define RETURN(r) do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return r; } while (0)
#else
# define ENTRY
# define EXIT return
# define RETURN(r) do { return r; } while (0)
#endif

struct _RtfmGirMethod
{
  RtfmGirBase base;

  gchar *ingest_element_name;

  gchar *name;
  gchar *c_identifier;
  gchar *version;
  GString *doc;
  RtfmGirReturnValue *return_value;
  RtfmGirParameters *parameters;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_C_IDENTIFIER,
  PROP_VERSION,
  PROP_DOC,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

G_DEFINE_TYPE (RtfmGirMethod, rtfm_gir_method, RTFM_TYPE_GIR_BASE)

static gboolean
rtfm_gir_method_ingest (RtfmGirBase          *base,
                        GMarkupParseContext  *context,
                        const gchar          *element_name,
                        const gchar         **attribute_names,
                        const gchar         **attribute_values,
                        GError              **error);

static void
rtfm_gir_method_finalize (GObject *object)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_identifier, g_free);
  g_clear_pointer (&self->version, g_free);
  g_string_free (self->doc, TRUE);
  self->doc = NULL;

  G_OBJECT_CLASS (rtfm_gir_method_parent_class)->finalize (object);
}

static void
rtfm_gir_method_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_C_IDENTIFIER:
      g_value_set_string (value, self->c_identifier);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_DOC:
      g_value_set_string (value, self->doc->str);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_method_set_property (GObject       *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RtfmGirMethod *self = (RtfmGirMethod *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_C_IDENTIFIER:
      g_free (self->c_identifier);
      self->c_identifier = g_value_dup_string (value);
      break;

    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
      break;

    case PROP_DOC:
      if (self->doc != NULL)
        g_string_set_size (self->doc, 0);
      else
        self->doc = g_string_new (NULL);
      g_string_append (self->doc, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_method_class_init (RtfmGirMethodClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_method_finalize;
  object_class->get_property = rtfm_gir_method_get_property;
  object_class->set_property = rtfm_gir_method_set_property;

  base_class->ingest = rtfm_gir_method_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_IDENTIFIER] =
    g_param_spec_string ("c-identifier",
                         "c-identifier",
                         "c-identifier",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DOC] =
    g_param_spec_string ("doc",
                         "doc",
                         "doc",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_method_init (RtfmGirMethod *self)
{
}

static void
rtfm_gir_method_start_element (GMarkupParseContext  *context,
                               const gchar          *element_name,
                               const gchar         **attribute_names,
                               const gchar         **attribute_values,
                               gpointer              user_data,
                               GError              **error)
{
  RtfmGirMethod *self = user_data;

  ENTRY;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_METHOD (self));
  g_assert (error != NULL);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "doc") == 0)
    {
      /* Do nothing */
    }
  else if (g_strcmp0 (element_name, "return-value") == 0)
    {
      g_autoptr(RtfmGirReturnValue) return_value = NULL;

      return_value = g_object_new (RTFM_TYPE_GIR_RETURN_VALUE, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (return_value),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      g_set_object (&self->return_value, return_value);
    }
  else if (g_strcmp0 (element_name, "parameters") == 0)
    {
      g_autoptr(RtfmGirParameters) parameters = NULL;

      parameters = g_object_new (RTFM_TYPE_GIR_PARAMETERS, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (parameters),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      g_set_object (&self->parameters, parameters);
    }


  EXIT;
}

static void
rtfm_gir_method_end_element (GMarkupParseContext  *context,
                             const gchar          *element_name,
                             gpointer              user_data,
                             GError              **error)
{
  RtfmGirMethod *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_METHOD (self));
  g_assert (error != NULL);

  if (g_strcmp0 (element_name, self->ingest_element_name) == 0)
    {
      g_markup_parse_context_pop (context);
      g_clear_pointer (&self->ingest_element_name, g_free);
    }
}

static void
rtfm_gir_method_text (GMarkupParseContext  *context,
                      const gchar          *text,
                      gsize                 text_len,
                      gpointer              user_data,
                      GError              **error)
{
  RtfmGirMethod *self = user_data;
  const gchar *element_name;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_IS_GIR_METHOD (self));
  g_assert (error != NULL);

  element_name = g_markup_parse_context_get_element (context);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "doc") == 0)
    {
      g_string_append_len (self->doc, text, text_len);
    }
}

static void
rtfm_gir_method_error (GMarkupParseContext *context,
                       GError              *error,
                       gpointer             user_data)
{
  RtfmGirMethod *self = user_data;

  g_assert (context != NULL);
  g_assert (RTFM_IS_GIR_METHOD (self));
  g_assert (error != NULL);

  g_clear_pointer (&self->ingest_element_name, g_free);
}

static const GMarkupParser markup_parser = {
  rtfm_gir_method_start_element,
  rtfm_gir_method_end_element,
  rtfm_gir_method_text,
  NULL,
  rtfm_gir_method_error,
};

static gboolean
rtfm_gir_method_ingest (RtfmGirBase          *base,
                        GMarkupParseContext  *context,
                        const gchar          *element_name,
                        const gchar         **attribute_names,
                        const gchar         **attribute_values,
                        GError              **error)
{
  RtfmGirMethod *self = (RtfmGirMethod *)base;

  ENTRY;

  g_assert (RTFM_IS_GIR_METHOD (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  self->ingest_element_name = g_strdup (element_name);

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_identifier, g_free);
  g_clear_pointer (&self->version, g_free);

  if (!rtfm_g_markup_collect_some_attributes (element_name,
                                              attribute_names,
                                              attribute_values,
                                              error,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:identifier", &self->c_identifier,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "version", &self->version,
                                              G_MARKUP_COLLECT_INVALID))
    RETURN (FALSE);

  g_markup_parse_context_push (context, &markup_parser, self);

  RETURN (TRUE);
}

/**
 * rtfm_gir_method_get_return_value:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirReturnValue or %NULL.
 */
RtfmGirReturnValue *
rtfm_gir_method_get_return_value (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_METHOD (self), NULL);

  return self->return_value;
}

/**
 * rtfm_gir_method_get_parameters:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirParameters or %NULL.
 */
RtfmGirParameters *
rtfm_gir_method_get_parameters (RtfmGirMethod *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_METHOD (self), NULL);

  return self->parameters;
}
