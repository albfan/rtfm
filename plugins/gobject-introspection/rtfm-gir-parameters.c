/* rtfm-gir-parameters.c
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

#define G_LOG_DOMAIN "rtfm-gir-parameters"

#include "rtfm-gir-parameters.h"
#include "rtfm-gir-markup.h"
#include "rtfm-gir-parameter.h"

#if 0
# define ENTRY     do { g_printerr ("ENTRY: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); } while (0)
# define EXIT      do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return; } while (0)
# define RETURN(r) do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return r; } while (0)
#else
# define ENTRY
# define EXIT return
# define RETURN(r) do { return r; } while (0)
#endif

struct _RtfmGirParameters
{
  RtfmGirBase base;

  gchar *ingest_element_name;

  GPtrArray *parameter;
};

G_DEFINE_TYPE (RtfmGirParameters, rtfm_gir_parameters, RTFM_TYPE_GIR_BASE)

static gboolean
rtfm_gir_parameters_ingest (RtfmGirBase          *base,
                            GMarkupParseContext  *context,
                            const gchar          *element_name,
                            const gchar         **attribute_names,
                            const gchar         **attribute_values,
                            GError              **error);

static void
rtfm_gir_parameters_finalize (GObject *object)
{
  RtfmGirParameters *self = (RtfmGirParameters *)object;

  g_clear_pointer (&self->parameter, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_parameters_parent_class)->finalize (object);
}


static void
rtfm_gir_parameters_class_init (RtfmGirParametersClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_parameters_finalize;

  base_class->ingest = rtfm_gir_parameters_ingest;
}

static void
rtfm_gir_parameters_init (RtfmGirParameters *self)
{
}

static void
rtfm_gir_parameters_start_element (GMarkupParseContext  *context,
                                   const gchar          *element_name,
                                   const gchar         **attribute_names,
                                   const gchar         **attribute_values,
                                   gpointer              user_data,
                                   GError              **error)
{
  RtfmGirParameters *self = user_data;

  ENTRY;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_PARAMETERS (self));
  g_assert (error != NULL);

  if (FALSE) {}
  else if ((g_strcmp0 (element_name, "parameter") == 0) || (g_strcmp0 (element_name, "instance-parameter") == 0))
    {
      g_autoptr(RtfmGirParameter) parameter = NULL;

      parameter = g_object_new (RTFM_TYPE_GIR_PARAMETER, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (parameter),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->parameter == NULL)
        self->parameter = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->parameter, g_steal_pointer (&parameter));
    }


  EXIT;
}

static void
rtfm_gir_parameters_end_element (GMarkupParseContext  *context,
                                 const gchar          *element_name,
                                 gpointer              user_data,
                                 GError              **error)
{
  RtfmGirParameters *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_PARAMETERS (self));
  g_assert (error != NULL);

  if (g_strcmp0 (element_name, self->ingest_element_name) == 0)
    {
      g_markup_parse_context_pop (context);
      g_clear_pointer (&self->ingest_element_name, g_free);
    }
}

static void
rtfm_gir_parameters_text (GMarkupParseContext  *context,
                          const gchar          *text,
                          gsize                 text_len,
                          gpointer              user_data,
                          GError              **error)
{
  RtfmGirParameters *self = user_data;
  const gchar *element_name;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_IS_GIR_PARAMETERS (self));
  g_assert (error != NULL);

}

static void
rtfm_gir_parameters_error (GMarkupParseContext *context,
                           GError              *error,
                           gpointer             user_data)
{
  RtfmGirParameters *self = user_data;

  g_assert (context != NULL);
  g_assert (RTFM_IS_GIR_PARAMETERS (self));
  g_assert (error != NULL);

  g_clear_pointer (&self->ingest_element_name, g_free);
}

static const GMarkupParser markup_parser = {
  rtfm_gir_parameters_start_element,
  rtfm_gir_parameters_end_element,
  rtfm_gir_parameters_text,
  NULL,
  rtfm_gir_parameters_error,
};

static gboolean
rtfm_gir_parameters_ingest (RtfmGirBase          *base,
                            GMarkupParseContext  *context,
                            const gchar          *element_name,
                            const gchar         **attribute_names,
                            const gchar         **attribute_values,
                            GError              **error)
{
  RtfmGirParameters *self = (RtfmGirParameters *)base;

  ENTRY;

  g_assert (RTFM_IS_GIR_PARAMETERS (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  self->ingest_element_name = g_strdup (element_name);

  g_markup_parse_context_push (context, &markup_parser, self);

  RETURN (TRUE);
}

gboolean
rtfm_gir_parameters_has_parameters (RtfmGirParameters *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_PARAMETERS (self), FALSE);

  return self->parameter != NULL && self->parameter->len > 0;
}

/**
 * rtfm_gir_parameters_get_parameters:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirParameter):
 *  An array of #RtfmGirParameter or %NULL.
 */
GPtrArray *
rtfm_gir_parameters_get_parameters (RtfmGirParameters *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_PARAMETERS (self), NULL);

  return self->parameter;
}
