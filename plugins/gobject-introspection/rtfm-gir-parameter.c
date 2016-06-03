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
#include "rtfm-gir-markup.h"
#include "rtfm-gir-type.h"

#if 0
# define ENTRY     do { g_printerr ("ENTRY: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); } while (0)
# define EXIT      do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return; } while (0)
# define RETURN(r) do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return r; } while (0)
#else
# define ENTRY
# define EXIT return
# define RETURN(r) do { return r; } while (0)
#endif

struct _RtfmGirParameter
{
  RtfmGirBase base;

  gchar *ingest_element_name;

  const gchar *name;
  const gchar *transfer_ownership;
  GString *doc;
  RtfmGirType *type;

  guint is_instance : 1;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_TRANSFER_OWNERSHIP,
  PROP_DOC,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

G_DEFINE_TYPE (RtfmGirParameter, rtfm_gir_parameter, RTFM_TYPE_GIR_BASE)

static gboolean
rtfm_gir_parameter_ingest (RtfmGirBase          *base,
                           GMarkupParseContext  *context,
                           const gchar          *element_name,
                           const gchar         **attribute_names,
                           const gchar         **attribute_values,
                           GError              **error);

static void
rtfm_gir_parameter_finalize (GObject *object)
{
  RtfmGirParameter *self = (RtfmGirParameter *)object;

  self->name = NULL;
  self->transfer_ownership = NULL;
  g_string_free (self->doc, TRUE);
  self->doc = NULL;

  G_OBJECT_CLASS (rtfm_gir_parameter_parent_class)->finalize (object);
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

    case PROP_TRANSFER_OWNERSHIP:
      g_value_set_string (value, self->transfer_ownership);
      break;

    case PROP_DOC:
      if (self->doc != NULL)
        g_value_set_string (value, self->doc->str);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_parameter_class_init (RtfmGirParameterClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_parameter_finalize;
  object_class->get_property = rtfm_gir_parameter_get_property;

  base_class->ingest = rtfm_gir_parameter_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_TRANSFER_OWNERSHIP] =
    g_param_spec_string ("transfer-ownership",
                         "transfer-ownership",
                         "transfer-ownership",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_DOC] =
    g_param_spec_string ("doc",
                         "doc",
                         "doc",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_parameter_init (RtfmGirParameter *self)
{
}

static void
rtfm_gir_parameter_start_element (GMarkupParseContext  *context,
                                  const gchar          *element_name,
                                  const gchar         **attribute_names,
                                  const gchar         **attribute_values,
                                  gpointer              user_data,
                                  GError              **error)
{
  RtfmGirParameter *self = user_data;

  ENTRY;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_PARAMETER (self));
  g_assert (error != NULL);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "doc") == 0)
    {
      /* Do nothing */
    }
  else if (g_strcmp0 (element_name, "type") == 0)
    {
      g_autoptr(RtfmGirType) type = NULL;

      type = g_object_new (RTFM_TYPE_GIR_TYPE, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (type), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (type),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      g_set_object (&self->type, type);
    }


  EXIT;
}

static void
rtfm_gir_parameter_end_element (GMarkupParseContext  *context,
                                const gchar          *element_name,
                                gpointer              user_data,
                                GError              **error)
{
  RtfmGirParameter *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_PARAMETER (self));
  g_assert (error != NULL);

  if (g_strcmp0 (element_name, self->ingest_element_name) == 0)
    {
      g_markup_parse_context_pop (context);
      g_clear_pointer (&self->ingest_element_name, g_free);
    }
}

static void
rtfm_gir_parameter_text (GMarkupParseContext  *context,
                         const gchar          *text,
                         gsize                 text_len,
                         gpointer              user_data,
                         GError              **error)
{
  RtfmGirParameter *self = user_data;
  const gchar *element_name;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_IS_GIR_PARAMETER (self));
  g_assert (error != NULL);

  element_name = g_markup_parse_context_get_element (context);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "doc") == 0)
    {
      if (self->doc == NULL)
        self->doc = g_string_new_len (text, text_len);
      else
        g_string_append_len (self->doc, text, text_len);
    }
}

static void
rtfm_gir_parameter_error (GMarkupParseContext *context,
                          GError              *error,
                          gpointer             user_data)
{
  RtfmGirParameter *self = user_data;

  g_assert (context != NULL);
  g_assert (RTFM_IS_GIR_PARAMETER (self));
  g_assert (error != NULL);

  g_clear_pointer (&self->ingest_element_name, g_free);
}

static const GMarkupParser markup_parser = {
  rtfm_gir_parameter_start_element,
  rtfm_gir_parameter_end_element,
  rtfm_gir_parameter_text,
  NULL,
  rtfm_gir_parameter_error,
};

static gboolean
rtfm_gir_parameter_ingest (RtfmGirBase          *base,
                           GMarkupParseContext  *context,
                           const gchar          *element_name,
                           const gchar         **attribute_names,
                           const gchar         **attribute_values,
                           GError              **error)
{
  RtfmGirParameter *self = (RtfmGirParameter *)base;
  const gchar *name = NULL;
  const gchar *transfer_ownership = NULL;

  ENTRY;

  g_assert (RTFM_IS_GIR_PARAMETER (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  self->ingest_element_name = g_strdup (element_name);

  self->name = NULL;
  self->transfer_ownership = NULL;

  self->is_instance = g_str_equal (element_name, "instance-parameter");

  if (!rtfm_g_markup_collect_some_attributes (element_name,
                                              attribute_names,
                                              attribute_values,
                                              error,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "transfer-ownership", &transfer_ownership,
                                              G_MARKUP_COLLECT_INVALID))
    RETURN (FALSE);

  self->name = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), name);
  self->transfer_ownership = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), transfer_ownership);

  g_markup_parse_context_push (context, &markup_parser, self);

  RETURN (TRUE);
}

/**
 * rtfm_gir_parameter_get_referenced:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirType or %NULL.
 */
RtfmGirType *
rtfm_gir_parameter_get_referenced (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_PARAMETER (self), NULL);

  return self->type;
}

gboolean
rtfm_gir_parameter_is_instance (RtfmGirParameter *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_PARAMETER (self), FALSE);

  return self->is_instance;
}
