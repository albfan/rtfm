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
#include "rtfm-gir-markup.h"
#include "rtfm-gir-field.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-union.h"

#if 0
# define ENTRY     do { g_printerr ("ENTRY: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); } while (0)
# define EXIT      do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return; } while (0)
# define RETURN(r) do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return r; } while (0)
#else
# define ENTRY
# define EXIT return
# define RETURN(r) do { return r; } while (0)
#endif

struct _RtfmGirRecord
{
  RtfmGirBase base;

  gchar *ingest_element_name;

  gchar *name;
  gchar *c_type;
  GString *doc;
  GPtrArray *field;
  GPtrArray *function;
  GPtrArray *unions;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_C_TYPE,
  PROP_DOC,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

G_DEFINE_TYPE (RtfmGirRecord, rtfm_gir_record, RTFM_TYPE_GIR_BASE)

static gboolean
rtfm_gir_record_ingest (RtfmGirBase          *base,
                        GMarkupParseContext  *context,
                        const gchar          *element_name,
                        const gchar         **attribute_names,
                        const gchar         **attribute_values,
                        GError              **error);

static void
rtfm_gir_record_finalize (GObject *object)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_string_free (self->doc, TRUE);
  self->doc = NULL;
  g_clear_pointer (&self->field, g_ptr_array_unref);
  g_clear_pointer (&self->function, g_ptr_array_unref);
  g_clear_pointer (&self->unions, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_record_parent_class)->finalize (object);
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
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_C_TYPE:
      g_value_set_string (value, self->c_type);
      break;

    case PROP_DOC:
      g_value_set_string (value, self->doc->str);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_record_set_property (GObject       *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  RtfmGirRecord *self = (RtfmGirRecord *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_C_TYPE:
      g_free (self->c_type);
      self->c_type = g_value_dup_string (value);
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
rtfm_gir_record_class_init (RtfmGirRecordClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_record_finalize;
  object_class->get_property = rtfm_gir_record_get_property;
  object_class->set_property = rtfm_gir_record_set_property;

  base_class->ingest = rtfm_gir_record_ingest;

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

  properties [PROP_DOC] =
    g_param_spec_string ("doc",
                         "doc",
                         "doc",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_record_init (RtfmGirRecord *self)
{
}

static void
rtfm_gir_record_start_element (GMarkupParseContext  *context,
                               const gchar          *element_name,
                               const gchar         **attribute_names,
                               const gchar         **attribute_values,
                               gpointer              user_data,
                               GError              **error)
{
  RtfmGirRecord *self = user_data;

  ENTRY;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_RECORD (self));
  g_assert (error != NULL);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "doc") == 0)
    {
      /* Do nothing */
    }
  else if (g_strcmp0 (element_name, "field") == 0)
    {
      g_autoptr(RtfmGirField) field = NULL;

      field = g_object_new (RTFM_TYPE_GIR_FIELD, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (field), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (field),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->field == NULL)
        self->field = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->field, g_steal_pointer (&field));
    }
  else if (g_strcmp0 (element_name, "function") == 0)
    {
      g_autoptr(RtfmGirFunction) function = NULL;

      function = g_object_new (RTFM_TYPE_GIR_FUNCTION, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (function), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (function),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->function == NULL)
        self->function = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->function, g_steal_pointer (&function));
    }
  else if (g_strcmp0 (element_name, "union") == 0)
    {
      g_autoptr(RtfmGirUnion) unions = NULL;

      unions = g_object_new (RTFM_TYPE_GIR_UNION, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (unions), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (unions),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->unions == NULL)
        self->unions = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->unions, g_steal_pointer (&unions));
    }


  EXIT;
}

static void
rtfm_gir_record_end_element (GMarkupParseContext  *context,
                             const gchar          *element_name,
                             gpointer              user_data,
                             GError              **error)
{
  RtfmGirRecord *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_RECORD (self));
  g_assert (error != NULL);

  if (g_strcmp0 (element_name, self->ingest_element_name) == 0)
    {
      g_markup_parse_context_pop (context);
      g_clear_pointer (&self->ingest_element_name, g_free);
    }
}

static void
rtfm_gir_record_text (GMarkupParseContext  *context,
                      const gchar          *text,
                      gsize                 text_len,
                      gpointer              user_data,
                      GError              **error)
{
  RtfmGirRecord *self = user_data;
  const gchar *element_name;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_IS_GIR_RECORD (self));
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
rtfm_gir_record_error (GMarkupParseContext *context,
                       GError              *error,
                       gpointer             user_data)
{
  RtfmGirRecord *self = user_data;

  g_assert (context != NULL);
  g_assert (RTFM_IS_GIR_RECORD (self));
  g_assert (error != NULL);

  g_clear_pointer (&self->ingest_element_name, g_free);
}

static const GMarkupParser markup_parser = {
  rtfm_gir_record_start_element,
  rtfm_gir_record_end_element,
  rtfm_gir_record_text,
  NULL,
  rtfm_gir_record_error,
};

static gboolean
rtfm_gir_record_ingest (RtfmGirBase          *base,
                        GMarkupParseContext  *context,
                        const gchar          *element_name,
                        const gchar         **attribute_names,
                        const gchar         **attribute_values,
                        GError              **error)
{
  RtfmGirRecord *self = (RtfmGirRecord *)base;

  ENTRY;

  g_assert (RTFM_IS_GIR_RECORD (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  self->ingest_element_name = g_strdup (element_name);

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_type, g_free);

  if (!rtfm_g_markup_collect_some_attributes (element_name,
                                              attribute_names,
                                              attribute_values,
                                              error,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:type", &self->c_type,
                                              G_MARKUP_COLLECT_INVALID))
    RETURN (FALSE);

  g_markup_parse_context_push (context, &markup_parser, self);

  RETURN (TRUE);
}

gboolean
rtfm_gir_record_has_fields (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_RECORD (self), FALSE);

  return self->field != NULL && self->field->len > 0;
}

/**
 * rtfm_gir_record_get_fields:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirField):
 *  An array of #RtfmGirField or %NULL.
 */
GPtrArray *
rtfm_gir_record_get_fields (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_RECORD (self), NULL);

  return self->field;
}

gboolean
rtfm_gir_record_has_functions (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_RECORD (self), FALSE);

  return self->function != NULL && self->function->len > 0;
}

/**
 * rtfm_gir_record_get_functions:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirFunction):
 *  An array of #RtfmGirFunction or %NULL.
 */
GPtrArray *
rtfm_gir_record_get_functions (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_RECORD (self), NULL);

  return self->function;
}

gboolean
rtfm_gir_record_has_unions (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_RECORD (self), FALSE);

  return self->unions != NULL && self->unions->len > 0;
}

/**
 * rtfm_gir_record_get_unions:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirUnion):
 *  An array of #RtfmGirUnion or %NULL.
 */
GPtrArray *
rtfm_gir_record_get_unions (RtfmGirRecord *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_RECORD (self), NULL);

  return self->unions;
}
