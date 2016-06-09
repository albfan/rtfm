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

#include "rtfm-gir-file.h"
#include "rtfm-gir-parser.h"

struct _RtfmGirFile
{
  GObject parent_instance;
  GFile *file;
  RtfmGirRepository *repository;
};

enum {
  PROP_0,
  PROP_FILE,
  PROP_REPOSITORY,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void async_initable_iface_init (GAsyncInitableIface *iface);

G_DEFINE_TYPE_EXTENDED (RtfmGirFile, rtfm_gir_file, RTFM_TYPE_ITEM, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_ASYNC_INITABLE,
                                               async_initable_iface_init))

static void
rtfm_gir_file_finalize (GObject *object)
{
  RtfmGirFile *self = (RtfmGirFile *)object;

  g_clear_object (&self->file);

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

    case PROP_REPOSITORY:
      g_value_set_object (value, self->repository);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
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
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
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

  properties [PROP_REPOSITORY] =
    g_param_spec_object ("repository",
                         "Repository",
                         "Repository",
                         RTFM_GIR_TYPE_REPOSITORY,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_gir_file_init (RtfmGirFile *self)
{
}

static void
rtfm_gir_file_init_worker (GTask        *task,
                           gpointer      source_object,
                           gpointer      task_data,
                           GCancellable *cancellable)
{
  g_autoptr(RtfmGirParser) parser = NULL;
  g_autoptr(RtfmGirRepository) repository = NULL;
  GFile *file = task_data;
  GError *error = NULL;

  g_assert (G_IS_TASK (task));
  g_assert (G_IS_FILE (file));

  parser = rtfm_gir_parser_new ();

  repository = rtfm_gir_parser_parse_file (parser, file, cancellable, &error);

  if (repository == NULL)
    {
      g_task_return_error (task, error);
      return;
    }

  g_task_return_pointer (task, g_steal_pointer (&repository), g_object_unref);
}

static void
rtfm_gir_file_init_async (GAsyncInitable      *initable,
                          gint                 io_priority,
                          GCancellable        *cancellable,
                          GAsyncReadyCallback  callback,
                          gpointer             user_data)
{
  RtfmGirFile *self = (RtfmGirFile *)initable;
  g_autoptr(GTask) task = NULL;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_task_data (task, g_object_ref (self->file), g_object_unref);
  g_task_set_priority (task, io_priority);
  g_task_run_in_thread (task, rtfm_gir_file_init_worker);
}

static gboolean
rtfm_gir_file_init_finish (GAsyncInitable  *initable,
                           GAsyncResult    *result,
                           GError         **error)
{
  RtfmGirFile *self = (RtfmGirFile *)initable;
  RtfmGirRepository *repository;

  g_assert (RTFM_GIR_IS_FILE (self));
  g_assert (G_IS_TASK (result));

  repository = g_task_propagate_pointer (G_TASK (result), error);

  if (repository != NULL)
    {
      g_clear_object (&self->repository);
      self->repository = repository;
    }

  return repository != NULL;
}

static void
async_initable_iface_init (GAsyncInitableIface *iface)
{
  iface->init_async = rtfm_gir_file_init_async;
  iface->init_finish = rtfm_gir_file_init_finish;
}

/**
 * rtfm_gir_file_get_file:
 *
 * Returns: (transfer none): A #GFile.
 */
GFile *
rtfm_gir_file_get_file (RtfmGirFile *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FILE (self), NULL);

  return self->file;
}

/**
 * rtfm_gir_file_get_repository:
 *
 * Returns: (transfer none): A #GFile.
 */
RtfmGirRepository *
rtfm_gir_file_get_repository (RtfmGirFile *self)
{
  g_return_val_if_fail (RTFM_GIR_IS_FILE (self), NULL);

  return self->repository;
}

RtfmGirFile *
rtfm_gir_file_new (GFile *file)
{
  g_return_val_if_fail (G_IS_FILE (file), NULL);

  return g_object_new (RTFM_GIR_TYPE_FILE,
                       "file", file,
                       NULL);
}
