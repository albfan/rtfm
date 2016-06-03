/* rtfm-gir-constructor.c
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

#define G_LOG_DOMAIN "rtfm-gir-constructor"

#include "rtfm-gir-constructor.h"
#include "rtfm-gir-markup.h"
#include "rtfm-gir-return-value.h"

#if 0
# define ENTRY     do { g_printerr ("ENTRY: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); } while (0)
# define EXIT      do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return; } while (0)
# define RETURN(r) do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return r; } while (0)
#else
# define ENTRY
# define EXIT return
# define RETURN(r) do { return r; } while (0)
#endif

struct _RtfmGirConstructor
{
  RtfmGirBase base;

  gchar *ingest_element_name;

  const gchar *name;
  const gchar *c_identifier;
  GString *doc;
  RtfmGirReturnValue *return_value;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_C_IDENTIFIER,
  PROP_DOC,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

G_DEFINE_TYPE (RtfmGirConstructor, rtfm_gir_constructor, RTFM_TYPE_GIR_BASE)

static gboolean
rtfm_gir_constructor_ingest (RtfmGirBase          *base,
                             GMarkupParseContext  *context,
                             const gchar          *element_name,
                             const gchar         **attribute_names,
                             const gchar         **attribute_values,
                             GError              **error);

static void
rtfm_gir_constructor_finalize (GObject *object)
{
  RtfmGirConstructor *self = (RtfmGirConstructor *)object;

  self->name = NULL;
  self->c_identifier = NULL;
  g_string_free (self->doc, TRUE);
  self->doc = NULL;

  G_OBJECT_CLASS (rtfm_gir_constructor_parent_class)->finalize (object);
}

static void
rtfm_gir_constructor_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  RtfmGirConstructor *self = (RtfmGirConstructor *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_C_IDENTIFIER:
      g_value_set_string (value, self->c_identifier);
      break;

    case PROP_DOC:
      g_value_set_string (value, self->doc->str);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_constructor_class_init (RtfmGirConstructorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_constructor_finalize;
  object_class->get_property = rtfm_gir_constructor_get_property;

  base_class->ingest = rtfm_gir_constructor_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_IDENTIFIER] =
    g_param_spec_string ("c-identifier",
                         "c-identifier",
                         "c-identifier",
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
rtfm_gir_constructor_init (RtfmGirConstructor *self)
{
}

static void
rtfm_gir_constructor_start_element (GMarkupParseContext  *context,
                                    const gchar          *element_name,
                                    const gchar         **attribute_names,
                                    const gchar         **attribute_values,
                                    gpointer              user_data,
                                    GError              **error)
{
  RtfmGirConstructor *self = user_data;

  ENTRY;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_CONSTRUCTOR (self));
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
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (return_value), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (return_value),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      g_set_object (&self->return_value, return_value);
    }


  EXIT;
}

static void
rtfm_gir_constructor_end_element (GMarkupParseContext  *context,
                                  const gchar          *element_name,
                                  gpointer              user_data,
                                  GError              **error)
{
  RtfmGirConstructor *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_CONSTRUCTOR (self));
  g_assert (error != NULL);

  if (g_strcmp0 (element_name, self->ingest_element_name) == 0)
    {
      g_markup_parse_context_pop (context);
      g_clear_pointer (&self->ingest_element_name, g_free);
    }
}

static void
rtfm_gir_constructor_text (GMarkupParseContext  *context,
                           const gchar          *text,
                           gsize                 text_len,
                           gpointer              user_data,
                           GError              **error)
{
  RtfmGirConstructor *self = user_data;
  const gchar *element_name;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_IS_GIR_CONSTRUCTOR (self));
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
rtfm_gir_constructor_error (GMarkupParseContext *context,
                            GError              *error,
                            gpointer             user_data)
{
  RtfmGirConstructor *self = user_data;

  g_assert (context != NULL);
  g_assert (RTFM_IS_GIR_CONSTRUCTOR (self));
  g_assert (error != NULL);

  g_clear_pointer (&self->ingest_element_name, g_free);
}

static const GMarkupParser markup_parser = {
  rtfm_gir_constructor_start_element,
  rtfm_gir_constructor_end_element,
  rtfm_gir_constructor_text,
  NULL,
  rtfm_gir_constructor_error,
};

static gboolean
rtfm_gir_constructor_ingest (RtfmGirBase          *base,
                             GMarkupParseContext  *context,
                             const gchar          *element_name,
                             const gchar         **attribute_names,
                             const gchar         **attribute_values,
                             GError              **error)
{
  RtfmGirConstructor *self = (RtfmGirConstructor *)base;
  const gchar *name = NULL;
  const gchar *c_identifier = NULL;

  ENTRY;

  g_assert (RTFM_IS_GIR_CONSTRUCTOR (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  self->ingest_element_name = g_strdup (element_name);

  self->name = NULL;
  self->c_identifier = NULL;

  if (!rtfm_g_markup_collect_some_attributes (element_name,
                                              attribute_names,
                                              attribute_values,
                                              error,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "c:identifier", &c_identifier,
                                              G_MARKUP_COLLECT_INVALID))
    RETURN (FALSE);

  self->name = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), name);
  self->c_identifier = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), c_identifier);

  g_markup_parse_context_push (context, &markup_parser, self);

  RETURN (TRUE);
}

/**
 * rtfm_gir_constructor_get_return_value:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirReturnValue or %NULL.
 */
RtfmGirReturnValue *
rtfm_gir_constructor_get_return_value (RtfmGirConstructor *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CONSTRUCTOR (self), NULL);

  return self->return_value;
}
