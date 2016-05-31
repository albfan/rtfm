/* rtfm-gir-file.c
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

#define G_LOG_DOMAIN "rtfm-gir-file"

#include <glib/gi18n.h>

#include "rtfm-gir-file.h"
#include "rtfm-gir-repository.h"

G_DEFINE_AUTOPTR_CLEANUP_FUNC (xmlTextReader, xmlFreeTextReader)

struct _RtfmGirFile
{
  RtfmItem           parent;

  GMutex             mutex;

  GFile             *file;
  RtfmGirRepository *repository;
};

enum {
  PROP_0,
  PROP_FILE,
  N_PROPS
};

G_DEFINE_TYPE (RtfmGirFile, rtfm_gir_file, RTFM_TYPE_ITEM)

static GParamSpec *properties [N_PROPS];

static void
rtfm_gir_file_finalize (GObject *object)
{
  RtfmGirFile *self = (RtfmGirFile *)object;

  g_clear_object (&self->file);
  g_clear_object (&self->repository);
  g_mutex_clear (&self->mutex);

  G_OBJECT_CLASS (rtfm_gir_file_parent_class)->finalize (object);
}

static void
rtfm_gir_file_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  RtfmGirFile *self = RTFM_GIR_FILE (object);

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, self->file);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_file_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  RtfmGirFile *self = RTFM_GIR_FILE (object);

  switch (prop_id)
    {
    case PROP_FILE:
      self->file = g_value_dup_object (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_gir_file_class_init (RtfmGirFileClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_file_finalize;
  object_class->get_property = rtfm_gir_file_get_property;
  object_class->set_property = rtfm_gir_file_set_property;

  properties [PROP_FILE] =
    g_param_spec_object ("file",
                         "File",
                         "File",
                         G_TYPE_FILE,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_file_init (RtfmGirFile *self)
{
  g_mutex_init (&self->mutex);
}

#define IS_ELEMENT_NAMED(r,name) \
  ((xmlTextReaderNodeType(r) == XML_ELEMENT_NODE) && \
   (0 == g_strcmp0 (name, (gchar *)xmlTextReaderConstName(r))))

static void
rtfm_gir_file_load_worker (GTask        *task,
                           gpointer      source_object,
                           gpointer      task_data,
                           GCancellable *cancellable)
{
  RtfmGirFile *self = source_object;
  RtfmCollection *collection = task_data;
  g_autoptr(RtfmGirRepository) repository = NULL;
  g_autoptr(xmlTextReader) reader = NULL;
  g_autofree gchar *path = NULL;
  GError *error = NULL;

  g_assert (G_IS_TASK (task));
  g_assert (RTFM_IS_GIR_FILE (self));
  g_assert (task_data == NULL);
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  if (!g_file_is_native (self->file) ||
      NULL == (path = g_file_get_path (self->file)))
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_INVALID_FILENAME,
                               "The file must be a local file.");
      return;
    }

  reader = xmlNewTextReaderFilename (path);

  if (reader == NULL)
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_INVALID_FILENAME,
                               "Failed to create xmlTextReader");
      return;
    }

skip_node:
  if (xmlTextReaderNext (reader) != 1)
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_INVALID_FILENAME,
                               "Failed to read from xmlTextReader");
      return;
    }

  if (xmlTextReaderNodeType (reader) == XML_COMMENT_NODE)
    goto skip_node;

  if (!IS_ELEMENT_NAMED (reader, "repository"))
    {
      g_task_return_new_error (task,
                               G_IO_ERROR,
                               G_IO_ERROR_FAILED,
                               "Failed to locate repository element");
      return;
    }

  repository = g_object_new (RTFM_TYPE_GIR_REPOSITORY, NULL);

  if (!rtfm_gir_repository_ingest (repository, reader, &error))
    {
      g_task_return_error (task, error);
      return;
    }

  /*
   * Stash the repository for later lookups so we can avoid
   * reparsing the XML document. Since we lazily parse these
   * documents, we can avoid loading a bunch of items into
   * memory since search uses an alternate index.
   */
  g_mutex_lock (&self->mutex);
  if (self->repository == NULL)
    self->repository = g_object_ref (repository);
  g_mutex_unlock (&self->mutex);

  g_task_return_pointer (task,
                         g_steal_pointer (&repository),
                         g_object_unref);
}

void
rtfm_gir_file_load_async (RtfmGirFile         *self,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;

  g_assert (RTFM_IS_GIR_FILE (self));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);

  if (self->repository != NULL)
    g_task_return_pointer (task,
                           g_object_ref (self->repository),
                           g_object_unref);
  else
    g_task_run_in_thread (task, rtfm_gir_file_load_worker);
}

/**
 * rtfm_gir_file_load_finish:
 * @self: An #RtfmGirFile
 * @result: A #GAsyncResult
 * @error: A location for a #GError or %NULL.
 *
 * Completes an asynchronous request to rtfm_gir_file_load_async().
 * The result is the loaded repository or %NULL up on failure.
 *
 * Returns: (transfer full): An #RtfmRepository.
 */
RtfmGirRepository *
rtfm_gir_file_load_finish (RtfmGirFile   *self,
                           GAsyncResult  *result,
                           GError       **error)
{
  g_assert (RTFM_IS_GIR_FILE (self));
  g_assert (G_IS_TASK (result));

  return g_task_propagate_pointer (G_TASK (result), error);
}
