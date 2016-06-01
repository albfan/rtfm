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
#include "rtfm-gir-alias.h"
#include "rtfm-gir-class.h"
#include "rtfm-gir-callback.h"
#include "rtfm-gir-bitfield.h"
#include "rtfm-gir-record.h"
#include "rtfm-gir-function.h"

struct _RtfmGirNamespace
{
  RtfmGirBase base;
  gchar *name;
  gchar *version;
  gchar *shared_library;
  gchar *c_identifier_prefixes;
  gchar *c_symbol_prefixes;
  GPtrArray *alias;
  GPtrArray *class;
  GPtrArray *callback;
  GPtrArray *bitfield;
  GPtrArray *record;
  GPtrArray *function;
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

G_DEFINE_TYPE (RtfmGirNamespace, rtfm_gir_namespace, RTFM_TYPE_GIR_BASE)

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_namespace_ingest (RtfmGirBase       *base,
                           xmlTextReaderPtr   reader,
                           GError           **error);

static void
rtfm_gir_namespace_finalize (GObject *object)
{
  RtfmGirNamespace *self = (RtfmGirNamespace *)object;

  g_clear_pointer (&self->name, g_free);
  g_clear_pointer (&self->version, g_free);
  g_clear_pointer (&self->shared_library, g_free);
  g_clear_pointer (&self->c_identifier_prefixes, g_free);
  g_clear_pointer (&self->c_symbol_prefixes, g_free);
  g_clear_pointer (&self->c_symbol_prefixes, g_ptr_array_unref);
  g_clear_pointer (&self->c_symbol_prefixes, g_ptr_array_unref);
  g_clear_pointer (&self->c_symbol_prefixes, g_ptr_array_unref);
  g_clear_pointer (&self->c_symbol_prefixes, g_ptr_array_unref);
  g_clear_pointer (&self->c_symbol_prefixes, g_ptr_array_unref);
  g_clear_pointer (&self->c_symbol_prefixes, g_ptr_array_unref);

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
rtfm_gir_namespace_set_property (GObject       *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  RtfmGirNamespace *self = (RtfmGirNamespace *)object;

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

    case PROP_SHARED_LIBRARY:
      g_free (self->shared_library);
      self->shared_library = g_value_dup_string (value);
      break;

    case PROP_C_IDENTIFIER_PREFIXES:
      g_free (self->c_identifier_prefixes);
      self->c_identifier_prefixes = g_value_dup_string (value);
      break;

    case PROP_C_SYMBOL_PREFIXES:
      g_free (self->c_symbol_prefixes);
      self->c_symbol_prefixes = g_value_dup_string (value);
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
  object_class->set_property = rtfm_gir_namespace_set_property;

  base_class->ingest = rtfm_gir_namespace_ingest;

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

  properties [PROP_SHARED_LIBRARY] =
    g_param_spec_string ("shared-library",
                         "shared-library",
                         "shared-library",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_IDENTIFIER_PREFIXES] =
    g_param_spec_string ("c-identifier-prefixes",
                         "c-identifier-prefixes",
                         "c-identifier-prefixes",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  properties [PROP_C_SYMBOL_PREFIXES] =
    g_param_spec_string ("c-symbol-prefixes",
                         "c-symbol-prefixes",
                         "c-symbol-prefixes",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_namespace_init (RtfmGirNamespace *self)
{
}

static gboolean
rtfm_gir_namespace_ingest (RtfmGirBase       *base,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  RtfmGirNamespace *self = (RtfmGirNamespace *)base;
  xmlChar *name;
  xmlChar *version;
  xmlChar *shared_library;
  xmlChar *c_identifier_prefixes;
  xmlChar *c_symbol_prefixes;

  g_assert (RTFM_IS_GIR_NAMESPACE (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  name = xmlTextReaderGetAttribute (reader, (const xmlChar *)"name");
  version = xmlTextReaderGetAttribute (reader, (const xmlChar *)"version");
  shared_library = xmlTextReaderGetAttribute (reader, (const xmlChar *)"shared-library");
  c_identifier_prefixes = xmlTextReaderGetAttribute (reader, (const xmlChar *)"c:identifier-prefixes");
  c_symbol_prefixes = xmlTextReaderGetAttribute (reader, (const xmlChar *)"c:symbol-prefixes");

  /* Copy properties to object */
  self->name = g_strdup ((gchar *)name);
  self->version = g_strdup ((gchar *)version);
  self->shared_library = g_strdup ((gchar *)shared_library);
  self->c_identifier_prefixes = g_strdup ((gchar *)c_identifier_prefixes);
  self->c_symbol_prefixes = g_strdup ((gchar *)c_symbol_prefixes);

  /* Free libxml allocated strings */
  xmlFree (name);
  xmlFree (version);
  xmlFree (shared_library);
  xmlFree (c_identifier_prefixes);
  xmlFree (c_symbol_prefixes);

  if (xmlTextReaderRead (reader) != 1)
    return FALSE;

  while (xmlTextReaderNodeType (reader) != XML_ELEMENT_NODE)
    {
      if (xmlTextReaderNext (reader) != 1)
        return FALSE;
    }

  do
    {
      const gchar *element_name;

      if (xmlTextReaderNodeType (reader) != XML_ELEMENT_NODE)
        continue;

      element_name = (const gchar *)xmlTextReaderConstName (reader);

      if (FALSE) { }
      else if (g_strcmp0 (element_name, "alias") == 0)
        {
          g_autoptr(RtfmGirAlias) alias = NULL;

          alias = g_object_new (RTFM_TYPE_GIR_ALIAS, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (alias), reader, error))
            return FALSE;

          if (self->alias == NULL)
            self->alias = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->alias, g_steal_pointer (&alias));
        }
      else if (g_strcmp0 (element_name, "class") == 0)
        {
          g_autoptr(RtfmGirClass) class = NULL;

          class = g_object_new (RTFM_TYPE_GIR_CLASS, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (class), reader, error))
            return FALSE;

          if (self->class == NULL)
            self->class = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->class, g_steal_pointer (&class));
        }
      else if (g_strcmp0 (element_name, "callback") == 0)
        {
          g_autoptr(RtfmGirCallback) callback = NULL;

          callback = g_object_new (RTFM_TYPE_GIR_CALLBACK, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (callback), reader, error))
            return FALSE;

          if (self->callback == NULL)
            self->callback = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->callback, g_steal_pointer (&callback));
        }
      else if (g_strcmp0 (element_name, "bitfield") == 0)
        {
          g_autoptr(RtfmGirBitfield) bitfield = NULL;

          bitfield = g_object_new (RTFM_TYPE_GIR_BITFIELD, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (bitfield), reader, error))
            return FALSE;

          if (self->bitfield == NULL)
            self->bitfield = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->bitfield, g_steal_pointer (&bitfield));
        }
      else if (g_strcmp0 (element_name, "record") == 0)
        {
          g_autoptr(RtfmGirRecord) record = NULL;

          record = g_object_new (RTFM_TYPE_GIR_RECORD, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (record), reader, error))
            return FALSE;

          if (self->record == NULL)
            self->record = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->record, g_steal_pointer (&record));
        }
      else if (g_strcmp0 (element_name, "function") == 0)
        {
          g_autoptr(RtfmGirFunction) function = NULL;

          function = g_object_new (RTFM_TYPE_GIR_FUNCTION, NULL);

          if (!rtfm_gir_base_ingest (RTFM_GIR_BASE (function), reader, error))
            return FALSE;

          if (self->function == NULL)
            self->function = g_ptr_array_new_with_free_func (g_object_unref);

          g_ptr_array_add (self->function, g_steal_pointer (&function));
        }
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
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