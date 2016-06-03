/* rtfm-gir-union.c
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

#define G_LOG_DOMAIN "rtfm-gir-union"

#include "rtfm-gir-union.h"
#include "rtfm-gir-markup.h"
#include "rtfm-gir-record.h"
#include "rtfm-gir-field.h"

#if 0
# define ENTRY     do { g_printerr ("ENTRY: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); } while (0)
# define EXIT      do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return; } while (0)
# define RETURN(r) do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return r; } while (0)
#else
# define ENTRY
# define EXIT return
# define RETURN(r) do { return r; } while (0)
#endif

struct _RtfmGirUnion
{
  RtfmGirBase base;

  gchar *ingest_element_name;

  const gchar *name;
  const gchar *c_type;
  GString *doc;
  GPtrArray *record;
  GPtrArray *field;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_C_TYPE,
  PROP_DOC,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

G_DEFINE_TYPE (RtfmGirUnion, rtfm_gir_union, RTFM_TYPE_GIR_BASE)

static gboolean
rtfm_gir_union_ingest (RtfmGirBase          *base,
                       GMarkupParseContext  *context,
                       const gchar          *element_name,
                       const gchar         **attribute_names,
                       const gchar         **attribute_values,
                       GError              **error);

static void
rtfm_gir_union_finalize (GObject *object)
{
  RtfmGirUnion *self = (RtfmGirUnion *)object;

  self->name = NULL;
  self->c_type = NULL;
  g_string_free (self->doc, TRUE);
  self->doc = NULL;
  g_clear_pointer (&self->record, g_ptr_array_unref);
  g_clear_pointer (&self->field, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_union_parent_class)->finalize (object);
}

static void
rtfm_gir_union_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  RtfmGirUnion *self = (RtfmGirUnion *)object;

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
rtfm_gir_union_class_init (RtfmGirUnionClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_union_finalize;
  object_class->get_property = rtfm_gir_union_get_property;

  base_class->ingest = rtfm_gir_union_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_TYPE] =
    g_param_spec_string ("c-type",
                         "c-type",
                         "c-type",
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
rtfm_gir_union_init (RtfmGirUnion *self)
{
}

static void
rtfm_gir_union_start_element (GMarkupParseContext  *context,
                              const gchar          *element_name,
                              const gchar         **attribute_names,
                              const gchar         **attribute_values,
                              gpointer              user_data,
                              GError              **error)
{
  RtfmGirUnion *self = user_data;

  ENTRY;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_UNION (self));
  g_assert (error != NULL);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "doc") == 0)
    {
      /* Do nothing */
    }
  else if (g_strcmp0 (element_name, "record") == 0)
    {
      g_autoptr(RtfmGirRecord) record = NULL;

      record = g_object_new (RTFM_TYPE_GIR_RECORD, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (record), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (record),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->record == NULL)
        self->record = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->record, g_steal_pointer (&record));
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


  EXIT;
}

static void
rtfm_gir_union_end_element (GMarkupParseContext  *context,
                            const gchar          *element_name,
                            gpointer              user_data,
                            GError              **error)
{
  RtfmGirUnion *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_UNION (self));
  g_assert (error != NULL);

  if (g_strcmp0 (element_name, self->ingest_element_name) == 0)
    {
      g_markup_parse_context_pop (context);
      g_clear_pointer (&self->ingest_element_name, g_free);
    }
}

static void
rtfm_gir_union_text (GMarkupParseContext  *context,
                     const gchar          *text,
                     gsize                 text_len,
                     gpointer              user_data,
                     GError              **error)
{
  RtfmGirUnion *self = user_data;
  const gchar *element_name;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_IS_GIR_UNION (self));
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
rtfm_gir_union_error (GMarkupParseContext *context,
                      GError              *error,
                      gpointer             user_data)
{
  RtfmGirUnion *self = user_data;

  g_assert (context != NULL);
  g_assert (RTFM_IS_GIR_UNION (self));
  g_assert (error != NULL);

  g_clear_pointer (&self->ingest_element_name, g_free);
}

static const GMarkupParser markup_parser = {
  rtfm_gir_union_start_element,
  rtfm_gir_union_end_element,
  rtfm_gir_union_text,
  NULL,
  rtfm_gir_union_error,
};

static gboolean
rtfm_gir_union_ingest (RtfmGirBase          *base,
                       GMarkupParseContext  *context,
                       const gchar          *element_name,
                       const gchar         **attribute_names,
                       const gchar         **attribute_values,
                       GError              **error)
{
  RtfmGirUnion *self = (RtfmGirUnion *)base;
  const gchar *name = NULL;
  const gchar *c_type = NULL;

  ENTRY;

  g_assert (RTFM_IS_GIR_UNION (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  self->ingest_element_name = g_strdup (element_name);

  self->name = NULL;
  self->c_type = NULL;

  if (!rtfm_g_markup_collect_some_attributes (element_name,
                                              attribute_names,
                                              attribute_values,
                                              error,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "c:type", &c_type,
                                              G_MARKUP_COLLECT_INVALID))
    RETURN (FALSE);

  self->name = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), name);
  self->c_type = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), c_type);

  g_markup_parse_context_push (context, &markup_parser, self);

  RETURN (TRUE);
}

gboolean
rtfm_gir_union_has_records (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_UNION (self), FALSE);

  return self->record != NULL && self->record->len > 0;
}

/**
 * rtfm_gir_union_get_records:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirRecord):
 *  An array of #RtfmGirRecord or %NULL.
 */
GPtrArray *
rtfm_gir_union_get_records (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_UNION (self), NULL);

  return self->record;
}

gboolean
rtfm_gir_union_has_fields (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_UNION (self), FALSE);

  return self->field != NULL && self->field->len > 0;
}

/**
 * rtfm_gir_union_get_fields:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirField):
 *  An array of #RtfmGirField or %NULL.
 */
GPtrArray *
rtfm_gir_union_get_fields (RtfmGirUnion *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_UNION (self), NULL);

  return self->field;
}
