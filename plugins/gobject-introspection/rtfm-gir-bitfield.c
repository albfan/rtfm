/* rtfm-gir-bitfield.c
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

#define G_LOG_DOMAIN "rtfm-gir-bitfield"

#include "rtfm-gir-bitfield.h"
#include "rtfm-gir-markup.h"
#include "rtfm-gir-member.h"

#if 0
# define ENTRY     do { g_printerr ("ENTRY: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); } while (0)
# define EXIT      do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return; } while (0)
# define RETURN(r) do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return r; } while (0)
#else
# define ENTRY
# define EXIT return
# define RETURN(r) do { return r; } while (0)
#endif

struct _RtfmGirBitfield
{
  RtfmGirBase base;

  gchar *ingest_element_name;

  gchar *name;
  gchar *version;
  gchar *glib_type_name;
  gchar *glib_get_type;
  gchar *c_type;
  GString *doc;
  GPtrArray *member;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_VERSION,
  PROP_GLIB_TYPE_NAME,
  PROP_GLIB_GET_TYPE,
  PROP_C_TYPE,
  PROP_DOC,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

G_DEFINE_TYPE (RtfmGirBitfield, rtfm_gir_bitfield, RTFM_TYPE_GIR_BASE)

static gboolean
rtfm_gir_bitfield_ingest (RtfmGirBase          *base,
                          GMarkupParseContext  *context,
                          const gchar          *element_name,
                          const gchar         **attribute_names,
                          const gchar         **attribute_values,
                          GError              **error);

static void
rtfm_gir_bitfield_finalize (GObject *object)
{
  RtfmGirBitfield *self = (RtfmGirBitfield *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->glib_type_name, g_free);
  g_clear_pointer (&self->glib_get_type, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_string_free (self->doc, TRUE);
  self->doc = NULL;
  g_clear_pointer (&self->member, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_bitfield_parent_class)->finalize (object);
}

static void
rtfm_gir_bitfield_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  RtfmGirBitfield *self = (RtfmGirBitfield *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_GLIB_TYPE_NAME:
      g_value_set_string (value, self->glib_type_name);
      break;

    case PROP_GLIB_GET_TYPE:
      g_value_set_string (value, self->glib_get_type);
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
rtfm_gir_bitfield_set_property (GObject       *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  RtfmGirBitfield *self = (RtfmGirBitfield *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
      break;

    case PROP_GLIB_TYPE_NAME:
      g_free (self->glib_type_name);
      self->glib_type_name = g_value_dup_string (value);
      break;

    case PROP_GLIB_GET_TYPE:
      g_free (self->glib_get_type);
      self->glib_get_type = g_value_dup_string (value);
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
rtfm_gir_bitfield_class_init (RtfmGirBitfieldClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_bitfield_finalize;
  object_class->get_property = rtfm_gir_bitfield_get_property;
  object_class->set_property = rtfm_gir_bitfield_set_property;

  base_class->ingest = rtfm_gir_bitfield_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_TYPE_NAME] =
    g_param_spec_string ("glib-type-name",
                         "glib-type-name",
                         "glib-type-name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_GLIB_GET_TYPE] =
    g_param_spec_string ("glib-get-type",
                         "glib-get-type",
                         "glib-get-type",
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
rtfm_gir_bitfield_init (RtfmGirBitfield *self)
{
}

static void
rtfm_gir_bitfield_start_element (GMarkupParseContext  *context,
                                 const gchar          *element_name,
                                 const gchar         **attribute_names,
                                 const gchar         **attribute_values,
                                 gpointer              user_data,
                                 GError              **error)
{
  RtfmGirBitfield *self = user_data;

  ENTRY;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_BITFIELD (self));
  g_assert (error != NULL);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "doc") == 0)
    {
      /* Do nothing */
    }
  else if (g_strcmp0 (element_name, "member") == 0)
    {
      g_autoptr(RtfmGirMember) member = NULL;

      member = g_object_new (RTFM_TYPE_GIR_MEMBER, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (member),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->member == NULL)
        self->member = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->member, g_steal_pointer (&member));
    }


  EXIT;
}

static void
rtfm_gir_bitfield_end_element (GMarkupParseContext  *context,
                               const gchar          *element_name,
                               gpointer              user_data,
                               GError              **error)
{
  RtfmGirBitfield *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_BITFIELD (self));
  g_assert (error != NULL);

  if (g_strcmp0 (element_name, self->ingest_element_name) == 0)
    {
      g_markup_parse_context_pop (context);
      g_clear_pointer (&self->ingest_element_name, g_free);
    }
}

static void
rtfm_gir_bitfield_text (GMarkupParseContext  *context,
                        const gchar          *text,
                        gsize                 text_len,
                        gpointer              user_data,
                        GError              **error)
{
  RtfmGirBitfield *self = user_data;
  const gchar *element_name;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_IS_GIR_BITFIELD (self));
  g_assert (error != NULL);

  element_name = g_markup_parse_context_get_element (context);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "doc") == 0)
    {
      g_string_append_len (self->doc, text, text_len);
    }
}

static void
rtfm_gir_bitfield_error (GMarkupParseContext *context,
                         GError              *error,
                         gpointer             user_data)
{
  RtfmGirBitfield *self = user_data;

  g_assert (context != NULL);
  g_assert (RTFM_IS_GIR_BITFIELD (self));
  g_assert (error != NULL);

  g_clear_pointer (&self->ingest_element_name, g_free);
}

static const GMarkupParser markup_parser = {
  rtfm_gir_bitfield_start_element,
  rtfm_gir_bitfield_end_element,
  rtfm_gir_bitfield_text,
  NULL,
  rtfm_gir_bitfield_error,
};

static gboolean
rtfm_gir_bitfield_ingest (RtfmGirBase          *base,
                          GMarkupParseContext  *context,
                          const gchar          *element_name,
                          const gchar         **attribute_names,
                          const gchar         **attribute_values,
                          GError              **error)
{
  RtfmGirBitfield *self = (RtfmGirBitfield *)base;

  ENTRY;

  g_assert (RTFM_IS_GIR_BITFIELD (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  self->ingest_element_name = g_strdup (element_name);

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->glib_type_name, g_free);
  g_clear_pointer (&self->glib_get_type, g_free);
  g_clear_pointer (&self->c_type, g_free);

  if (!rtfm_g_markup_collect_some_attributes (element_name,
                                              attribute_names,
                                              attribute_values,
                                              error,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "version", &self->version,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:type-name", &self->glib_type_name,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:get-type", &self->glib_get_type,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:type", &self->c_type,
                                              G_MARKUP_COLLECT_INVALID))
    RETURN (FALSE);

  g_markup_parse_context_push (context, &markup_parser, self);

  RETURN (TRUE);
}

gboolean
rtfm_gir_bitfield_has_members (RtfmGirBitfield *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_BITFIELD (self), FALSE);

  return self->member != NULL && self->member->len > 0;
}

/**
 * rtfm_gir_bitfield_get_members:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirMember):
 *  An array of #RtfmGirMember or %NULL.
 */
GPtrArray *
rtfm_gir_bitfield_get_members (RtfmGirBitfield *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_BITFIELD (self), NULL);

  return self->member;
}
