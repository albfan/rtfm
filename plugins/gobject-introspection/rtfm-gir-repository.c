/* rtfm-gir-repository.c
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

#define G_LOG_DOMAIN "rtfm-gir-repository"

#include "rtfm-gir-repository.h"
#include "rtfm-gir-include.h"
#include "rtfm-gir-package.h"
#include "rtfm-gir-c-include.h"
#include "rtfm-gir-namespace.h"

struct _RtfmGirRepository
{
  RtfmItem parent_instance;
  gchar *version;
  RtfmGirInclude *include;
  RtfmGirPackage *package;
  RtfmGirCInclude *c_include;
  RtfmGirNamespace *namespace;
};

enum {
  PROP_0,
  PROP_VERSION,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirRepository, rtfm_gir_repository, RTFM_TYPE_ITEM)

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_repository_finalize (GObject *object)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  g_clear_pointer (&self->version, g_free);

  G_OBJECT_CLASS (rtfm_gir_repository_parent_class)->finalize (object);
}

static void
rtfm_gir_repository_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  switch (prop_id)
    {
    case PROP_VERSION:
      g_value_set_string (value, self->version);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_repository_set_property (GObject       *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  RtfmGirRepository *self = (RtfmGirRepository *)object;

  switch (prop_id)
    {
    case PROP_VERSION:
      g_free (self->version);
      self->version = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_repository_class_init (RtfmGirRepositoryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_repository_finalize;
  object_class->get_property = rtfm_gir_repository_get_property;
  object_class->set_property = rtfm_gir_repository_set_property;

  properties [PROP_VERSION] =
    g_param_spec_string ("version",
                         "version",
                         "version",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_repository_init (RtfmGirRepository *self)
{
}

gboolean
rtfm_gir_repository_ingest (RtfmGirRepository   *self,
                          xmlTextReaderPtr   reader,
                          GError           **error)
{
  xmlChar *version;

  g_assert (RTFM_IS_GIR_REPOSITORY (self));
  g_assert (reader != NULL);

  /* Read properties from element */
  version = xmlTextReaderGetAttribute (reader, (const xmlChar *)"version");

  /* Copy properties to object */
  self->version = g_strdup ((gchar *)version);

  /* Free libxml allocated strings */
  xmlFree (version);

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
      else if (g_strcmp0 (element_name, "include") == 0)
        {
          g_autoptr(RtfmGirInclude) include = NULL;

          include = g_object_new (RTFM_TYPE_GIR_INCLUDE, NULL);

          if (!rtfm_gir_include_ingest (include, reader, error))
            return FALSE;

          g_set_object (&self->include, include);
        }
      else if (g_strcmp0 (element_name, "package") == 0)
        {
          g_autoptr(RtfmGirPackage) package = NULL;

          package = g_object_new (RTFM_TYPE_GIR_PACKAGE, NULL);

          if (!rtfm_gir_package_ingest (package, reader, error))
            return FALSE;

          g_set_object (&self->package, package);
        }
      else if (g_strcmp0 (element_name, "c:include") == 0)
        {
          g_autoptr(RtfmGirCInclude) c_include = NULL;

          c_include = g_object_new (RTFM_TYPE_GIR_C_INCLUDE, NULL);

          if (!rtfm_gir_c_include_ingest (c_include, reader, error))
            return FALSE;

          g_set_object (&self->c_include, c_include);
        }
      else if (g_strcmp0 (element_name, "namespace") == 0)
        {
          g_autoptr(RtfmGirNamespace) namespace = NULL;

          namespace = g_object_new (RTFM_TYPE_GIR_NAMESPACE, NULL);

          if (!rtfm_gir_namespace_ingest (namespace, reader, error))
            return FALSE;

          g_set_object (&self->namespace, namespace);
        }
    }
  while (xmlTextReaderNext (reader) == 1);


  return TRUE;
}

/**
 * rtfm_gir_repository_get_include:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirInclude or %NULL.
 */
RtfmGirInclude *
rtfm_gir_repository_get_include (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_REPOSITORY (self), NULL);

  return self->include;
}

/**
 * rtfm_gir_repository_get_package:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirPackage or %NULL.
 */
RtfmGirPackage *
rtfm_gir_repository_get_package (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_REPOSITORY (self), NULL);

  return self->package;
}

/**
 * rtfm_gir_repository_get_c_include:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirCInclude or %NULL.
 */
RtfmGirCInclude *
rtfm_gir_repository_get_c_include (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_REPOSITORY (self), NULL);

  return self->c_include;
}

/**
 * rtfm_gir_repository_get_namespace:
 *
 * Returns: (nullable) (transfer none): An #RtfmGirNamespace or %NULL.
 */
RtfmGirNamespace *
rtfm_gir_repository_get_namespace (RtfmGirRepository *self)
{
  g_return_val_if_fail (RTFM_IS_GIR_REPOSITORY (self), NULL);

  return self->namespace;
}
