/* rtfm-gir-compiler.c
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <glib/gi18n.h>
#include <libxml/xmlreader.h>
#include <rtfm.h>
#include <stdlib.h>

#include "rtfm-gir-repository.h"

static void
error_handler (void                    *arg,
               const char              *msg,
               xmlParserSeverities      severity,
               xmlTextReaderLocatorPtr  locator)
{
  gchar **target = arg;
  guint line;

  g_free (*target);

  line = xmlTextReaderLocatorLineNumber (locator);
  *target = g_strdup_printf ("Line %d: %s", line, msg);
}

static inline gboolean
is_element_named (xmlTextReaderPtr  reader,
                  const gchar      *name)
{
  g_assert (reader != NULL);
  g_assert (name != NULL);

  if (xmlTextReaderNodeType (reader) == XML_ELEMENT_NODE)
    return (0 == g_strcmp0 (name, (gchar *)xmlTextReaderConstName (reader)));

  return FALSE;
}

static void
set_error_string (gchar       **dest,
                  const gchar  *message)
{
  g_free (*dest);
  *dest = g_strdup (message);
}

static RtfmGirRepository *
rtfm_gir_compiler_parse (xmlTextReaderPtr   reader,
                         GError           **error)
{
  g_autoptr(RtfmGirRepository) repository = NULL;
  gchar *error_string = NULL;
  int r;

  g_assert (reader != NULL);
  g_assert (error != NULL);

  xmlTextReaderSetErrorHandler (reader, error_handler, &error_string);

skip_node:
  r = xmlTextReaderNext (reader);
  if (r <= 0)
    goto return_new_error;

  if (xmlTextReaderNodeType (reader) == XML_COMMENT_NODE)
    goto skip_node;

  if (!is_element_named (reader, "repository"))
    {
      set_error_string (&error_string, "Failed to locate repository node");
      goto return_new_error;
    }

  repository = g_object_new (RTFM_TYPE_GIR_REPOSITORY, NULL);

  if (!rtfm_gir_repository_ingest (repository, reader, error))
    return FALSE;

  return g_steal_pointer (&repository);

return_new_error:
  if (*error == NULL)
    {
      if (error_string)
        g_set_error (error,
                     G_FILE_ERROR,
                     G_FILE_ERROR_FAILED,
                     "%s", error_string);
      else
        g_set_error (error,
                     G_FILE_ERROR,
                     G_FILE_ERROR_FAILED,
                     "An unknown error occurred");
    }

  return NULL;
}

static gboolean
rtfm_gir_compiler_compile (GFile   *in_file,
                           GFile   *out_file,
                           GError **error)
{
  g_autoptr(RtfmGirRepository) repository = NULL;
  g_autofree gchar *contents = NULL;
  xmlTextReaderPtr reader;
  gsize len;

  g_assert (G_IS_FILE (in_file));
  g_assert (G_IS_FILE (out_file));
  g_assert (error != NULL);

  if (!g_file_load_contents (in_file, NULL, &contents, &len, NULL, error))
    return FALSE;

  reader = xmlReaderForMemory (contents,
                               len,
                               NULL,
                               NULL,
                               (XML_PARSE_NOENT |
                                XML_PARSE_NONET |
                                XML_PARSE_NSCLEAN |
                                XML_PARSE_COMPACT));

  if (reader == NULL)
    {
      g_set_error (error,
                   G_FILE_ERROR,
                   G_FILE_ERROR_FAILED,
                   "Failed to create XmlTextReader");
      return FALSE;
    }

  repository = rtfm_gir_compiler_parse (reader, error);

  if (repository == NULL)
    return FALSE;

  return TRUE;

#if 0
  return g_file_replace_contents (out_file,
                                  g_variant_get_data (variant),
                                  g_variant_get_size (variant),
                                  NULL,
                                  FALSE,
                                  G_FILE_CREATE_REPLACE_DESTINATION,
                                  NULL,
                                  NULL,
                                  error);
#endif
}

gint
main (gint   argc,
      gchar *argv[])
{
  g_autofree gchar *library_version = NULL;
  GOptionEntry entries[] = {
    { "library-version", 0, 0, G_OPTION_ARG_STRING, &library_version,
      N_("The version of the library or program .gir"),
      N_("VERSION") },
    { NULL }
  };

  g_autoptr(GOptionContext) context = NULL;
  g_autoptr(GError) error = NULL;
  g_autoptr(GFile) file = NULL;
  g_autoptr(GFile) parent = NULL;
  g_autoptr(GFile) outfile = NULL;
  g_autofree gchar *basename = NULL;
  g_autofree gchar *outbasename = NULL;

  context = g_option_context_new (_("GIR_FILE - generate documentation for a .gir file"));
  g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);

  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
      g_printerr ("%s\n", error->message);
      return EXIT_FAILURE;
    }

  if (argc != 2)
    {
      g_autofree gchar *help = g_option_context_get_help (context, TRUE, NULL);
      g_printerr ("%s\n", help);
      return EXIT_FAILURE;
    }

  if (!g_str_has_suffix (argv[1], ".gir"))
    {
      g_printerr ("\"%s\" does not look like a .gir file.\n", argv[1]);
      return EXIT_FAILURE;
    }

  file = g_file_new_for_commandline_arg (argv[1]);

  if (!g_file_query_exists (file, NULL) ||
      g_file_query_file_type (file, 0, NULL) != G_FILE_TYPE_REGULAR)
    {
      g_autofree gchar *filename = g_file_get_path (file);
      g_printerr ("Invalid file: %s\n", filename);
      return EXIT_FAILURE;
    }

  parent = g_file_get_parent (file);
  basename = g_file_get_basename (file);
  outbasename = g_strdup_printf ("%s.rtfm", basename);
  outfile = g_file_get_child (parent, outbasename);

  if (!rtfm_gir_compiler_compile (file, outfile, &error))
    {
      g_printerr ("%s\n", error->message);
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
