/* rtfm-gir-class.c
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

#define G_LOG_DOMAIN "rtfm-gir-class"

#include "rtfm-gir-class.h"
#include "rtfm-gir-markup.h"
#include "rtfm-gir-field.h"
#include "rtfm-gir-implements.h"
#include "rtfm-gir-method.h"
#include "rtfm-gir-function.h"
#include "rtfm-gir-virtual-method.h"
#include "rtfm-gir-property.h"
#include "rtfm-gir-constructor.h"
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

struct _RtfmGirClass
{
  RtfmGirBase base;

  gchar *ingest_element_name;

  gchar *name;
  gchar *c_symbol_prefix;
  gchar *c_type;
  gchar *version;
  gchar *parent;
  gchar *glib_type_name;
  gchar *glib_get_type;
  GString *doc;
  GPtrArray *field;
  GPtrArray *implements;
  GPtrArray *method;
  GPtrArray *function;
  GPtrArray *virtual_method;
  GPtrArray *property;
  GPtrArray *constructor;
  GPtrArray *unions;
};

enum {
  PROP_0,
  PROP_NAME,
  PROP_C_SYMBOL_PREFIX,
  PROP_C_TYPE,
  PROP_VERSION,
  PROP_PARENT,
  PROP_GLIB_TYPE_NAME,
  PROP_GLIB_GET_TYPE,
  PROP_DOC,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirClass, rtfm_gir_class, RTFM_TYPE_GIR_BASE)

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_class_ingest (RtfmGirBase          *base,
                       GMarkupParseContext  *context,
                       const gchar          *element_name,
                       const gchar         **attribute_names,
                       const gchar         **attribute_values,
                       GError              **error);

static void
rtfm_gir_class_finalize (GObject *object)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_symbol_prefix, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->parent, g_free);
  g_clear_pointer (&self->glib_type_name, g_free);
  g_clear_pointer (&self->glib_get_type, g_free);
  g_string_free (self->doc, TRUE);
  self->doc = NULL;
  g_clear_pointer (&self->field, g_ptr_array_unref);
  g_clear_pointer (&self->implements, g_ptr_array_unref);
  g_clear_pointer (&self->method, g_ptr_array_unref);
  g_clear_pointer (&self->function, g_ptr_array_unref);
  g_clear_pointer (&self->virtual_method, g_ptr_array_unref);
  g_clear_pointer (&self->property, g_ptr_array_unref);
  g_clear_pointer (&self->constructor, g_ptr_array_unref);
  g_clear_pointer (&self->unions, g_ptr_array_unref);

  G_OBJECT_CLASS (rtfm_gir_class_parent_class)->finalize (object);
}

static void
rtfm_gir_class_get_property (GObject    *object,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    case PROP_C_SYMBOL_PREFIX:
      g_value_set_string (value, self->c_symbol_prefix);
      break;

    case PROP_C_TYPE:
      g_value_set_string (value, self->c_type);
      break;

    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    case PROP_PARENT:
      g_value_set_string (value, self->parent);
      break;

    case PROP_GLIB_TYPE_NAME:
      g_value_set_string (value, self->glib_type_name);
      break;

    case PROP_GLIB_GET_TYPE:
      g_value_set_string (value, self->glib_get_type);
      break;

    case PROP_DOC:
      g_value_set_string (value, self->doc->str);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_class_set_property (GObject       *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  RtfmGirClass *self = (RtfmGirClass *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    case PROP_C_SYMBOL_PREFIX:
      g_free (self->c_symbol_prefix);
      self->c_symbol_prefix = g_value_dup_string (value);
      break;

    case PROP_C_TYPE:
      g_free (self->c_type);
      self->c_type = g_value_dup_string (value);
      break;

    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
      break;

    case PROP_PARENT:
      g_free (self->parent);
      self->parent = g_value_dup_string (value);
      break;

    case PROP_GLIB_TYPE_NAME:
      g_free (self->glib_type_name);
      self->glib_type_name = g_value_dup_string (value);
      break;

    case PROP_GLIB_GET_TYPE:
      g_free (self->glib_get_type);
      self->glib_get_type = g_value_dup_string (value);
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
rtfm_gir_class_class_init (RtfmGirClassClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirBaseClass *base_class = RTFM_GIR_BASE_CLASS (klass);

  object_class->finalize = rtfm_gir_class_finalize;
  object_class->get_property = rtfm_gir_class_get_property;
  object_class->set_property = rtfm_gir_class_set_property;

  base_class->ingest = rtfm_gir_class_ingest;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_SYMBOL_PREFIX] =
    g_param_spec_string ("c-symbol-prefix",
                         "c-symbol-prefix",
                         "c-symbol-prefix",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_TYPE] =
    g_param_spec_string ("c-type",
                         "c-type",
                         "c-type",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_PARENT] =
    g_param_spec_string ("parent",
                         "parent",
                         "parent",
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

  properties [PROP_DOC] =
    g_param_spec_string ("doc",
                         "doc",
                         "doc",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_class_init (RtfmGirClass *self)
{
}

static void
rtfm_gir_class_start_element (GMarkupParseContext  *context,
                              const gchar          *element_name,
                              const gchar         **attribute_names,
                              const gchar         **attribute_values,
                              gpointer              user_data,
                              GError              **error)
{
  RtfmGirClass *self = user_data;

  ENTRY;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);
  g_assert (RTFM_IS_GIR_CLASS (self));
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
  else if (g_strcmp0 (element_name, "implements") == 0)
    {
      g_autoptr(RtfmGirImplements) implements = NULL;

      implements = g_object_new (RTFM_TYPE_GIR_IMPLEMENTS, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (implements),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->implements == NULL)
        self->implements = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->implements, g_steal_pointer (&implements));
    }
  else if (g_strcmp0 (element_name, "method") == 0)
    {
      g_autoptr(RtfmGirMethod) method = NULL;

      method = g_object_new (RTFM_TYPE_GIR_METHOD, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (method),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->method == NULL)
        self->method = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->method, g_steal_pointer (&method));
    }
  else if (g_strcmp0 (element_name, "function") == 0)
    {
      g_autoptr(RtfmGirFunction) function = NULL;

      function = g_object_new (RTFM_TYPE_GIR_FUNCTION, NULL);

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
  else if (g_strcmp0 (element_name, "virtual-method") == 0)
    {
      g_autoptr(RtfmGirVirtualMethod) virtual_method = NULL;

      virtual_method = g_object_new (RTFM_TYPE_GIR_VIRTUAL_METHOD, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (virtual_method),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->virtual_method == NULL)
        self->virtual_method = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->virtual_method, g_steal_pointer (&virtual_method));
    }
  else if (g_strcmp0 (element_name, "property") == 0)
    {
      g_autoptr(RtfmGirProperty) property = NULL;

      property = g_object_new (RTFM_TYPE_GIR_PROPERTY, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (property),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->property == NULL)
        self->property = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->property, g_steal_pointer (&property));
    }
  else if (g_strcmp0 (element_name, "constructor") == 0)
    {
      g_autoptr(RtfmGirConstructor) constructor = NULL;

      constructor = g_object_new (RTFM_TYPE_GIR_CONSTRUCTOR, NULL);

      if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (constructor),
                                 context,
                                 element_name,
                                 attribute_names,
                                 attribute_values,
                                 error))
        return;

      if (self->constructor == NULL)
        self->constructor = g_ptr_array_new_with_free_func (g_object_unref);

      g_ptr_array_add (self->constructor, g_steal_pointer (&constructor));
    }
  else if (g_strcmp0 (element_name, "union") == 0)
    {
      g_autoptr(RtfmGirUnion) unions = NULL;

      unions = g_object_new (RTFM_TYPE_GIR_UNION, NULL);

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
rtfm_gir_class_end_element (GMarkupParseContext  *context,
                            const gchar          *element_name,
                            gpointer              user_data,
                            GError              **error)
{
  RtfmGirClass *self = user_data;

  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (RTFM_IS_GIR_CLASS (self));
  g_assert (error != NULL);

  if (g_strcmp0 (element_name, self->ingest_element_name) == 0)
    {
      g_markup_parse_context_pop (context);
      g_clear_pointer (&self->ingest_element_name, g_free);
    }
}

static void
rtfm_gir_class_text (GMarkupParseContext  *context,
                     const gchar          *text,
                     gsize                 text_len,
                     gpointer              user_data,
                     GError              **error)
{
  RtfmGirClass *self = user_data;
  const gchar *element_name;

  g_assert (context != NULL);
  g_assert (text != NULL);
  g_assert (RTFM_IS_GIR_CLASS (self));
  g_assert (error != NULL);

  element_name = g_markup_parse_context_get_element (context);

  if (FALSE) {}
  else if (g_strcmp0 (element_name, "doc") == 0)
    {
      g_string_append_len (self->doc, text, text_len);
    }
}

static void
rtfm_gir_class_error (GMarkupParseContext *context,
                      GError              *error,
                      gpointer             user_data)
{
  RtfmGirClass *self = user_data;

  g_assert (context != NULL);
  g_assert (RTFM_IS_GIR_CLASS (self));
  g_assert (error != NULL);

  g_clear_pointer (&self->ingest_element_name, g_free);
}

static const GMarkupParser markup_parser = {
  rtfm_gir_class_start_element,
  rtfm_gir_class_end_element,
  rtfm_gir_class_text,
  NULL,
  rtfm_gir_class_error,
};

static gboolean
rtfm_gir_class_ingest (RtfmGirBase          *base,
                       GMarkupParseContext  *context,
                       const gchar          *element_name,
                       const gchar         **attribute_names,
                       const gchar         **attribute_values,
                       GError              **error)
{
  RtfmGirClass *self = (RtfmGirClass *)base;

  ENTRY;

  g_assert (RTFM_IS_GIR_CLASS (self));
  g_assert (context != NULL);
  g_assert (element_name != NULL);
  g_assert (attribute_names != NULL);
  g_assert (attribute_values != NULL);

  self->ingest_element_name = g_strdup (element_name);

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->c_symbol_prefix, g_free);
  g_clear_pointer (&self->c_type, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->parent, g_free);
  g_clear_pointer (&self->glib_type_name, g_free);
  g_clear_pointer (&self->glib_get_type, g_free);

  if (!rtfm_g_markup_collect_some_attributes (element_name,
                                              attribute_names,
                                              attribute_values,
                                              error,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:symbol-prefix", &self->c_symbol_prefix,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "c:type", &self->c_type,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "version", &self->version,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "parent", &self->parent,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:type-name", &self->glib_type_name,
                                              G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "glib:get-type", &self->glib_get_type,
                                              G_MARKUP_COLLECT_INVALID))
    RETURN (FALSE);

  g_markup_parse_context_push (context, &markup_parser, self);

  RETURN (TRUE);
}

gboolean
rtfm_gir_class_has_fields (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), FALSE);

  return self->field != NULL && self->field->len > 0;
}

/**
 * rtfm_gir_class_get_fields:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirField):
 *  An array of #RtfmGirField or %NULL.
 */
GPtrArray *
rtfm_gir_class_get_fields (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), NULL);

  return self->field;
}

gboolean
rtfm_gir_class_has_implements (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), FALSE);

  return self->implements != NULL && self->implements->len > 0;
}

/**
 * rtfm_gir_class_get_implements:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirImplements):
 *  An array of #RtfmGirImplements or %NULL.
 */
GPtrArray *
rtfm_gir_class_get_implements (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), NULL);

  return self->implements;
}

gboolean
rtfm_gir_class_has_methods (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), FALSE);

  return self->method != NULL && self->method->len > 0;
}

/**
 * rtfm_gir_class_get_methods:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirMethod):
 *  An array of #RtfmGirMethod or %NULL.
 */
GPtrArray *
rtfm_gir_class_get_methods (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), NULL);

  return self->method;
}

gboolean
rtfm_gir_class_has_functions (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), FALSE);

  return self->function != NULL && self->function->len > 0;
}

/**
 * rtfm_gir_class_get_functions:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirFunction):
 *  An array of #RtfmGirFunction or %NULL.
 */
GPtrArray *
rtfm_gir_class_get_functions (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), NULL);

  return self->function;
}

gboolean
rtfm_gir_class_has_virtual_methods (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), FALSE);

  return self->virtual_method != NULL && self->virtual_method->len > 0;
}

/**
 * rtfm_gir_class_get_virtual_methods:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirVirtualMethod):
 *  An array of #RtfmGirVirtualMethod or %NULL.
 */
GPtrArray *
rtfm_gir_class_get_virtual_methods (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), NULL);

  return self->virtual_method;
}

gboolean
rtfm_gir_class_has_properties (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), FALSE);

  return self->property != NULL && self->property->len > 0;
}

/**
 * rtfm_gir_class_get_properties:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirProperty):
 *  An array of #RtfmGirProperty or %NULL.
 */
GPtrArray *
rtfm_gir_class_get_properties (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), NULL);

  return self->property;
}

gboolean
rtfm_gir_class_has_constructors (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), FALSE);

  return self->constructor != NULL && self->constructor->len > 0;
}

/**
 * rtfm_gir_class_get_constructors:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirConstructor):
 *  An array of #RtfmGirConstructor or %NULL.
 */
GPtrArray *
rtfm_gir_class_get_constructors (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), NULL);

  return self->constructor;
}

gboolean
rtfm_gir_class_has_unions (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), FALSE);

  return self->unions != NULL && self->unions->len > 0;
}

/**
 * rtfm_gir_class_get_unions:
 *
 * Returns: (nullable) (transfer none) (element-type Rtfm.GirUnion):
 *  An array of #RtfmGirUnion or %NULL.
 */
GPtrArray *
rtfm_gir_class_get_unions (RtfmGirClass *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_CLASS (self), NULL);

  return self->unions;
}
