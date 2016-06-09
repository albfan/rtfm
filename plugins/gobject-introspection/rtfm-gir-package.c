/* rtfm-gir-package.c
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

#define G_LOG_DOMAIN "rtfm-gir-package"

#include "rtfm-gir-package.h"

struct _RtfmGirPackage
{
  GObject parent_instance;
  gchar *name;
};

G_DEFINE_TYPE (RtfmGirPackage, rtfm_gir_package, RTFM_GIR_TYPE_PARSER_OBJECT)

enum {
  PROP_0,
  PROP_NAME,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static gboolean
rtfm_gir_package_ingest (RtfmGirParserObject *object,
                         GMarkupParseContext *context,
                         const gchar *element_name,
                         const gchar **attribute_names,
                         const gchar **attribute_values,
                         GError **error)
{
  RtfmGirPackage *self = (RtfmGirPackage *)object;

  g_assert (RTFM_GIR_IS_PACKAGE (self));
  g_assert (g_str_equal (element_name, "package"));

  g_clear_pointer (&self->name, g_free);

  if (!rtfm_gir_g_markup_collect_attributes (element_name, attribute_names, attribute_values, error,
                                             G_MARKUP_COLLECT_STRDUP | G_MARKUP_COLLECT_OPTIONAL, "name", &self->name,
                                             G_MARKUP_COLLECT_INVALID, NULL, NULL))
    return FALSE;

  return TRUE;
}

static void
rtfm_gir_package_printf (RtfmGirParserObject *object,
                         GString *str,
                         guint depth)
{
  RtfmGirPackage *self = (RtfmGirPackage *)object;
  guint i;

  g_assert (RTFM_GIR_IS_PACKAGE (self));

  for (i = 0; i < depth; i++)
    g_string_append (str, "  ");
  g_string_append (str, "<package");

  if (self->name != NULL)
    g_string_append_printf (str, " name=\"%s\"", self->name);

  g_string_append (str, "/>\n");
}

static void
rtfm_gir_package_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  RtfmGirPackage *self = (RtfmGirPackage *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, self->name);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_package_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  RtfmGirPackage *self = (RtfmGirPackage *)object;

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (self->name);
      self->name = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_package_finalize (GObject *object)
{
  RtfmGirPackage *self = (RtfmGirPackage *)object;

  g_clear_pointer (&self->name, g_free);

  G_OBJECT_CLASS (rtfm_gir_package_parent_class)->finalize (object);
}

static void
rtfm_gir_package_class_init (RtfmGirPackageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  RtfmGirParserObjectClass *parent_class = RTFM_GIR_PARSER_OBJECT_CLASS (klass);

  object_class->get_property = rtfm_gir_package_get_property;
  object_class->set_property = rtfm_gir_package_set_property;
  object_class->finalize = rtfm_gir_package_finalize;

  parent_class->ingest = rtfm_gir_package_ingest;
  parent_class->printf = rtfm_gir_package_printf;

  properties [PROP_NAME] =
    g_param_spec_string ("name",
                         "name",
                         "name",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_package_init (RtfmGirPackage *self)
{
}

const gchar *
rtfm_gir_package_get_name (RtfmGirPackage *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_PACKAGE (self), NULL);

  return self->name;
}

RtfmGirPackage *
rtfm_gir_package_new (void)
{
  return g_object_new (RTFM_GIR_TYPE_PACKAGE, NULL);
}
