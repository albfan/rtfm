/* rtfm-gir-namespace.c
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

#define G_LOG_DOMAIN "rtfm-gir-namespace"

#include "rtfm-gir-namespace.h"
#include "rtfm-gir-markup.h"
#include "rtfm-gir-alias.h"
#include "rtfm-gir-bitfield.h"
#include "rtfm-gir-callback.h"
#include "rtfm-gir-class.h"
#include "rtfm-gir-constant.h"
#include "rtfm-gir-enumeration.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-record.h"

#if 0
# define ENTRY     do { g_printerr ("ENTRY: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); } while (0)
# define EXIT      do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return; } while (0)
# define RETURN(r) do { g_printerr (" EXIT: %s(): %d: (%s)\n", G_STRFUNC, __LINE__, element_name); return r; } while (0)
#else
# define ENTRY
# define EXIT return
# define RETURN(r) do { return r; } while (0)
#endif

struct _RtfmGirNamespace
{
  RtfmGirBase base;

  gchar *ingest_element_name;

  const gchar *name;
  const gchar *version;
  const gchar *shared_library;
  const gchar *c_identifier_prefixes;
  const gchar *c_symbol_prefixes;
  GPtrArray *alias;
  GPtrArray *bitfield;
  GPtrArray *callback;
  GPtrArray *class;
  GPtrArray *constant;
  GPtrArray *enumeration;
  GPtrArray *function;
  GPtrArray *record;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_VERSION,
  PROP_SHARED_LIBRARY,
  PROP_C_IDENTIFIER_PREFIXES,
  PROP_C_SYMBOL_PREFIXES,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

G_DEFINE_TYPE (RtfmGirNamespace, rtfm_gir_namespace, RTFM_TYPE_GIR_BASE)

static gboolean
rtfm_gir_namespace_ingest (RtfmGirBase          *base,
                           GMarkupParseContext  *context,
                           const gchar          *element_name,
                           const gchar         **attribute_names,
                           const gchar         **attribute_values,
                           GError              **error);

static void
rtfm_gir_namespace_finalize (GObject *object)
{
  RtfmGirNamespace *self = (RtfmGirNamespace *)object;

  self->name = NULL;
  self->version = NULL;
  self->shared_library = NULL;
  self->c_identifier_prefixes = NULL;
  self->c_symbol_prefixes = NULL;
  g_clear_pointer (&self->alias, g_ptr_array_unref);
  g_clear_pointer (&self->bitfield, g_ptr_array_unref);
  g_clear_pointer (&self->callback, g_ptr_array_unref);
  g_clear_pointer (&self->class, g_ptr_array_unref);
  g_clear_pointer (&self->constant, g_ptr_array_unref);
  g_clear_pointer (&self->enumeration, g_ptr_array_unref);
  g_clear_pointer (&self->function, g_ptr_array_unref);
  g_clear_pointer (&self->record, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_namespace_parent_class)->finalize (object);
}

static void
rtfm_gir_namespace_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  RtfmGirNamespace *self = (RtfmGirNamespace *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_SHARED_LIBRARY:
      g_value_set_string (value, self->shared_library);
      break;

    case PROP_C_IDENTIFIER_PREFIXES:
      g_value_set_string (value, self->c_identifier_prefixes);
      break;

    case PROP_C_SYMBOL_PREFIXES:
      g_value_set_string (value, self->c_symbol_prefixes);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_namespace_class_init (RtfmGirNamespaceClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_namespace_finalize;
  object_class->get_property = rtfm_gir_namespace_get_property;

  base_class->ingest = rtfm_gir_namespace_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_SHARED_LIBRARY] =
    g_param_spec_string ("shared-library",
                         "shared-library",
                         "shared-library",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_IDENTIFIER_PREFIXES] =
    g_param_spec_string ("c-identifier-prefixes",
                         "c-identifier-prefixes",
                         "c-identifier-prefixes",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_SYMBOL_PREFIXES] =
    g_param_spec_string ("c-symbol-prefixes",
                         "c-symbol-prefixes",
                         "c-symbol-prefixes",
                         NULL,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_namespace_init (RtfmGirNamespace *self)
{
}

static void
rtfm_gir_namespace_start_element (GMarkupParseContext  *context,
                                  const gchar          *element_name,
                                  const gchar         **attribute_names,
                                  const gchar         **attribute_values,
                                  gpointer              user_data,
                                  GError              **error)
{
  RtfmGirNamespace *self = user_data;

  ENTRY;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_NAMESPACE (self));
  g_assert (error != NULL);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "alias") == 0)
    {
      g_autoptr(RtfmGirAlias) alias = NULL;

      alias = g_object_new (RTFM_TYPE_GIR_ALIAS, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (alias), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (alias),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->alias == NULL)
        self->alias = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->alias, g_steal_pointer (&alias));
    }
  else if (g_strcmp0 (element_name, "bitfield") == 0)
    {
      g_autoptr(RtfmGirBitfield) bitfield = NULL;

      bitfield = g_object_new (RTFM_TYPE_GIR_BITFIELD, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (bitfield), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (bitfield),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->bitfield == NULL)
        self->bitfield = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->bitfield, g_steal_pointer (&bitfield));
    }
  else if (g_strcmp0 (element_name, "callback") == 0)
    {
      g_autoptr(RtfmGirCallback) callback = NULL;

      callback = g_object_new (RTFM_TYPE_GIR_CALLBACK, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (callback), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (callback),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->callback == NULL)
        self->callback = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->callback, g_steal_pointer (&callback));
    }
  else if (g_strcmp0 (element_name, "class") == 0)
    {
      g_autoptr(RtfmGirClass) class = NULL;

      class = g_object_new (RTFM_TYPE_GIR_CLASS, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (class), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (class),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->class == NULL)
        self->class = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->class, g_steal_pointer (&class));
    }
  else if (g_strcmp0 (element_name, "constant") == 0)
    {
      g_autoptr(RtfmGirConstant) constant = NULL;

      constant = g_object_new (RTFM_TYPE_GIR_CONSTANT, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (constant), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (constant),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->constant == NULL)
        self->constant = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->constant, g_steal_pointer (&constant));
    }
  else if (g_strcmp0 (element_name, "enumeration") == 0)
    {
      g_autoptr(RtfmGirEnumeration) enumeration = NULL;

      enumeration = g_object_new (RTFM_TYPE_GIR_ENUMERATION, NULL);
      rtfm_gir_base_set_parent (RTFM_GIR_BASE (enumeration), RTFM_GIR_BASE (self));

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (enumeration),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->enumeration == NULL)
        self->enumeration = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->enumeration, g_steal_pointer (&enumeration));
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


  EXIT;
}

static void
rtfm_gir_namespace_end_element (GMarkupParseContext  *context,
                                const gchar          *element_name,
                                gpointer              user_data,
                                GError              **error)
{
  RtfmGirNamespace *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_NAMESPACE (self));
  g_assert (error != NULL);

  if (g_strcmp0 (element_name, self->ingest_element_name) == 0)
    {
      g_markup_parse_context_pop (context);
      g_clear_pointer (&self->ingest_element_name, g_free);
    }
}

static void
rtfm_gir_namespace_text (GMarkupParseContext  *context,
                         const gchar          *text,
                         gsize                 text_len,
                         gpointer              user_data,
                         GError              **error)
{
  RtfmGirNamespace *self = user_data;
  const gchar *element_name;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_IS_GIR_NAMESPACE (self));
  g_assert (error != NULL);

}

static void
rtfm_gir_namespace_error (GMarkupParseContext *context,
                          GError              *error,
                          gpointer             user_data)
{
  RtfmGirNamespace *self = user_data;

  g_assert (context != NULL);
  g_assert (RTFM_IS_GIR_NAMESPACE (self));
  g_assert (error != NULL);

  g_clear_pointer (&self->ingest_element_name, g_free);
}

static const GMarkupParser markup_parser = {
  rtfm_gir_namespace_start_element,
  rtfm_gir_namespace_end_element,
  rtfm_gir_namespace_text,
  NULL,
  rtfm_gir_namespace_error,
};

static gboolean
rtfm_gir_namespace_ingest (RtfmGirBase          *base,
                           GMarkupParseContext  *context,
                           const gchar          *element_name,
                           const gchar         **attribute_names,
                           const gchar         **attribute_values,
                           GError              **error)
{
  RtfmGirNamespace *self = (RtfmGirNamespace *)base;
  const gchar *name = NULL;
  const gchar *version = NULL;
  const gchar *shared_library = NULL;
  const gchar *c_identifier_prefixes = NULL;
  const gchar *c_symbol_prefixes = NULL;

  ENTRY;

  g_assert (RTFM_IS_GIR_NAMESPACE (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  self->ingest_element_name = g_strdup (element_name);

  self->name = NULL;
  self->version = NULL;
  self->shared_library = NULL;
  self->c_identifier_prefixes = NULL;
  self->c_symbol_prefixes = NULL;

  if (!rtfm_g_markup_collect_some_attributes (element_name,
                                              attribute_names,
                                              attribute_values,
                                              error,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "name", &name,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "version", &version,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "shared-library", &shared_library,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "c:identifier-prefixes", &c_identifier_prefixes,
                                              G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "c:symbol-prefixes", &c_symbol_prefixes,
                                              G_MARKUP_COLLECT_INVALID))
    RETURN (FALSE);

  self->name = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), name);
  self->version = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), version);
  self->shared_library = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), shared_library);
  self->c_identifier_prefixes = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), c_identifier_prefixes);
  self->c_symbol_prefixes = rtfm_gir_base_intern_string (RTFM_GIR_BASE (self), c_symbol_prefixes);

  g_markup_parse_context_push (context, &markup_parser, self);

  RETURN (TRUE);
}

gboolean
rtfm_gir_namespace_has_aliases (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), FALSE);

  return self->alias != NULL && self->alias->len > 0;
}

/**
 * rtfm_gir_namespace_get_aliases:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirAlias):
 *  An array of #RtfmGirAlias or %NULL.
 */
GPtrArray *
rtfm_gir_namespace_get_aliases (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), NULL);

  return self->alias;
}

gboolean
rtfm_gir_namespace_has_bitfields (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), FALSE);

  return self->bitfield != NULL && self->bitfield->len > 0;
}

/**
 * rtfm_gir_namespace_get_bitfields:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirBitfield):
 *  An array of #RtfmGirBitfield or %NULL.
 */
GPtrArray *
rtfm_gir_namespace_get_bitfields (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), NULL);

  return self->bitfield;
}

gboolean
rtfm_gir_namespace_has_callbacks (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), FALSE);

  return self->callback != NULL && self->callback->len > 0;
}

/**
 * rtfm_gir_namespace_get_callbacks:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirCallback):
 *  An array of #RtfmGirCallback or %NULL.
 */
GPtrArray *
rtfm_gir_namespace_get_callbacks (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), NULL);

  return self->callback;
}

gboolean
rtfm_gir_namespace_has_classes (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), FALSE);

  return self->class != NULL && self->class->len > 0;
}

/**
 * rtfm_gir_namespace_get_classes:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirClass):
 *  An array of #RtfmGirClass or %NULL.
 */
GPtrArray *
rtfm_gir_namespace_get_classes (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), NULL);

  return self->class;
}

gboolean
rtfm_gir_namespace_has_constants (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), FALSE);

  return self->constant != NULL && self->constant->len > 0;
}

/**
 * rtfm_gir_namespace_get_constants:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirConstant):
 *  An array of #RtfmGirConstant or %NULL.
 */
GPtrArray *
rtfm_gir_namespace_get_constants (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), NULL);

  return self->constant;
}

gboolean
rtfm_gir_namespace_has_enumerations (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), FALSE);

  return self->enumeration != NULL && self->enumeration->len > 0;
}

/**
 * rtfm_gir_namespace_get_enumerations:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirEnumeration):
 *  An array of #RtfmGirEnumeration or %NULL.
 */
GPtrArray *
rtfm_gir_namespace_get_enumerations (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), NULL);

  return self->enumeration;
}

gboolean
rtfm_gir_namespace_has_functions (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), FALSE);

  return self->function != NULL && self->function->len > 0;
}

/**
 * rtfm_gir_namespace_get_functions:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirFunction):
 *  An array of #RtfmGirFunction or %NULL.
 */
GPtrArray *
rtfm_gir_namespace_get_functions (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), NULL);

  return self->function;
}

gboolean
rtfm_gir_namespace_has_records (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), FALSE);

  return self->record != NULL && self->record->len > 0;
}

/**
 * rtfm_gir_namespace_get_records:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirRecord):
 *  An array of #RtfmGirRecord or %NULL.
 */
GPtrArray *
rtfm_gir_namespace_get_records (RtfmGirNamespace *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_NAMESPACE (self), NULL);

  return self->record;
}
