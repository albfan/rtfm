/* rtfm-gir-provider.c
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

#define G_LOG_DOMAIN "rtfm-gir-provider"

#include <glib/gi18n.h>
#include <string.h>

#include "rtfm-gir-alias.h"
#include "rtfm-gir-class.h"
#include "rtfm-gir-file.h"
#include "rtfm-gir-item.h"
#include "rtfm-gir-namespace.h"
#include "rtfm-gir-provider.h"
#include "rtfm-gir-repository.h"
#include "rtfm-gir-search-result.h"

#define RTFM_GIR_PROVIDER_SEARCH_MAX 25

struct _RtfmGirProvider
{
  GObject    object;

  GPtrArray *files;
  GPtrArray *search_indexes;

  GSList    *search_index_tasks;
  guint      search_indexes_loaded : 1;
};

typedef struct
{
  RtfmSearchResults *results;
  gchar *query;
  guint active;
} SearchState;

typedef struct
{
  guint active;
} LoadIndexState;

static void provider_iface_init (RtfmProviderInterface *iface);

G_DEFINE_TYPE_EXTENDED (RtfmGirProvider, rtfm_gir_provider, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (RTFM_TYPE_PROVIDER, provider_iface_init))

static void
search_state_free (gpointer data)
{
  SearchState *state = data;

  g_clear_pointer (&state->query, g_free);
  g_clear_object (&state->results);
  g_slice_free (SearchState, state);
}

static void
rtfm_gir_provider_finalize (GObject *object)
{
  RtfmGirProvider *self = (RtfmGirProvider *)object;

  g_clear_pointer (&self->files, g_ptr_array_unref);
  g_clear_pointer (&self->search_indexes, g_ptr_array_unref);

  g_slist_free_full (self->search_index_tasks, g_object_unref);
  self->search_index_tasks = NULL;

  G_OBJECT_CLASS (rtfm_gir_provider_parent_class)->finalize (object);
}

static void
rtfm_gir_provider_class_init (RtfmGirProviderClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_gir_provider_finalize;
}

static void
rtfm_gir_provider_init (RtfmGirProvider *self)
{
  self->files = g_ptr_array_new_with_free_func (g_object_unref);
  self->search_indexes = g_ptr_array_new_with_free_func (g_object_unref);
}

static void
rtfm_gir_provider_load_index_cb (GObject      *object,
                                 GAsyncResult *result,
                                 gpointer      user_data)
{
  RtfmGirFile *file = (RtfmGirFile *)object;
  g_autoptr(FuzzyIndex) index = NULL;
  g_autoptr(GTask) task = user_data;
  RtfmGirProvider *self;
  LoadIndexState *state;
  GError *error = NULL;

  g_assert (RTFM_GIR_IS_FILE (file));
  g_assert (G_IS_TASK (task));
  g_assert (G_IS_ASYNC_RESULT (result));

  self = g_task_get_source_object (task);
  state = g_task_get_task_data (task);

  index = rtfm_gir_file_load_index_finish (file, result, &error);

  if (index == NULL)
    g_warning ("%s", error->message);
  else
    g_ptr_array_add (self->search_indexes, g_steal_pointer (&index));

  state->active--;

  if (state->active == 0)
    {
      GSList *tasks;
      GSList *iter;

      self->search_indexes_loaded = TRUE;

      tasks = self->search_index_tasks;
      self->search_index_tasks = NULL;

      for (iter = tasks; iter != NULL; iter = iter->next)
        {
          g_autoptr(GTask) iter_task = iter->data;

          g_task_return_boolean (iter_task, TRUE);
        }

      g_slist_free (tasks);
    }
}

static void
rtfm_gir_provider_load_indexes_async (RtfmGirProvider     *self,
                                      GCancellable        *cancellable,
                                      GAsyncReadyCallback  callback,
                                      gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;
  LoadIndexState *state;
  guint i;

  g_assert (RTFM_IS_GIR_PROVIDER (self));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_source_tag (task, rtfm_gir_provider_load_indexes_async);
  g_task_set_check_cancellable (task, FALSE);

  if (self->search_indexes_loaded || self->files->len == 0)
    {
      g_task_return_boolean (task, TRUE);
      return;
    }

  state = g_new (LoadIndexState, 1);
  state->active = self->files->len;
  g_task_set_task_data (task, state, g_free);

  self->search_index_tasks = g_slist_prepend (self->search_index_tasks, g_object_ref (task));

  for (i = 0; i < self->files->len; i++)
    {
      RtfmGirFile *file = g_ptr_array_index (self->files, i);

      rtfm_gir_file_load_index_async (file,
                                      cancellable,
                                      rtfm_gir_provider_load_index_cb,
                                      g_object_ref (task));
    }
}

static gboolean
rtfm_gir_provider_load_indexes_finish (RtfmGirProvider  *self,
                                       GAsyncResult     *result,
                                       GError          **error)
{
  g_assert (RTFM_IS_GIR_PROVIDER (self));
  g_assert (G_IS_TASK (result));

  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
rtfm_gir_provider_load_directory (RtfmGirProvider *self,
                                  const gchar     *path,
                                  GCancellable    *cancellable)
{
  g_autoptr(GFile) parent = NULL;
  g_autoptr(GFileEnumerator) enumerator = NULL;
  g_autoptr(GError) error = NULL;
  g_autofree gchar *index_dir = NULL;
  gpointer ptr;

  g_assert (RTFM_IS_GIR_PROVIDER (self));

  if (!g_file_test (path, G_FILE_TEST_IS_DIR))
    return;

  parent = g_file_new_for_path (path);
  enumerator = g_file_enumerate_children (parent,
                                          G_FILE_ATTRIBUTE_STANDARD_NAME,
                                          G_FILE_QUERY_INFO_NONE,
                                          NULL,
                                          &error);

  if (enumerator == NULL)
    {
      g_warning ("%s", error->message);
      return;
    }

  while (NULL != (ptr = g_file_enumerator_next_file (enumerator, cancellable, &error)))
    {
      g_autoptr(GFileInfo) file_info = ptr;
      g_autoptr(GFile) file = NULL;
      const gchar *name;

      name = g_file_info_get_name (file_info);
      file = g_file_get_child (parent, name);

      g_ptr_array_add (self->files, rtfm_gir_file_new (file));
    }

  if (error != NULL)
    {
      g_warning ("%s", error->message);
      return;
    }

  index_dir = g_build_filename (g_get_user_cache_dir (),
                                "rtfm",
                                "gobject-introspection",
                                NULL);

  if (!g_file_test (index_dir, G_FILE_TEST_IS_DIR))
    g_mkdir_with_parents (index_dir, 0750);

  rtfm_gir_provider_load_indexes_async (self, NULL, NULL, NULL);
}

static void
rtfm_gir_provider_reload (RtfmGirProvider *self)
{
  g_assert (RTFM_IS_GIR_PROVIDER (self));

  rtfm_gir_provider_load_directory (self, "/usr/share/gir-1.0", NULL);
}

static void
rtfm_gir_provider_initialize (RtfmProvider *provider,
                              RtfmLibrary  *library)
{
  RtfmGirProvider *self = (RtfmGirProvider *)provider;

  g_assert (RTFM_IS_GIR_PROVIDER (self));
  g_assert (RTFM_IS_LIBRARY (library));

  rtfm_gir_provider_reload (self);
}

static void
rtfm_gir_provider_shutdown (RtfmProvider *provider,
                            RtfmLibrary  *library)
{
  RtfmGirProvider *self = (RtfmGirProvider *)provider;

  g_assert (RTFM_IS_GIR_PROVIDER (self));
  g_assert (RTFM_IS_LIBRARY (library));

}

static void
rtfm_gir_provider_populate_cb (GObject      *object,
                               GAsyncResult *result,
                               gpointer      user_data)
{
  g_autoptr(GTask) task = user_data;
  RtfmGirItem *item = (RtfmGirItem *)object;
  GError *error = NULL;

  g_assert (RTFM_GIR_IS_ITEM (item));
  g_assert (G_IS_ASYNC_RESULT (result));
  g_assert (G_IS_TASK (task));

  if (!rtfm_gir_item_populate_finish (item, result, &error))
    g_task_return_error (task, error);
  else
    g_task_return_boolean (task, TRUE);
}

static void
rtfm_gir_provider_populate_async (RtfmProvider        *provider,
                                  RtfmItem            *parent,
                                  RtfmCollection      *collection,
                                  GCancellable        *cancellable,
                                  GAsyncReadyCallback  callback,
                                  gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;
  RtfmGirProvider *self = (RtfmGirProvider *)provider;
  g_autoptr(RtfmPath) path = NULL;

  g_assert (RTFM_IS_GIR_PROVIDER (self));
  g_assert (RTFM_IS_COLLECTION (collection));
  g_assert (!parent || RTFM_IS_ITEM (parent));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_task_data (task, g_object_ref (collection), g_object_unref);

  path = rtfm_collection_get_path (collection);

  if (rtfm_path_is_empty (path))
    {
      guint i;

      for (i = 0; i < self->files->len; i++)
        {
          RtfmGirFile *file = g_ptr_array_index (self->files, i);
          g_autoptr(RtfmGirItem) gir_item = rtfm_gir_item_new (G_OBJECT (file));

          rtfm_collection_append (collection, RTFM_ITEM (gir_item));
        }

      g_task_return_boolean (task, TRUE);

      return;
    }
  else if (RTFM_GIR_IS_ITEM (parent))
    {
      rtfm_gir_item_populate_async (RTFM_GIR_ITEM (parent),
                                    collection,
                                    cancellable,
                                    rtfm_gir_provider_populate_cb,
                                    g_object_ref (task));
    }
}

static gboolean
rtfm_gir_provider_populate_finish (RtfmProvider  *provider,
                                   GAsyncResult  *result,
                                   GError       **error)
{
  g_assert (RTFM_IS_GIR_PROVIDER (provider));
  g_assert (G_IS_TASK (result));

  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
rtfm_gir_provider_query_cb (GObject      *object,
                            GAsyncResult *result,
                            gpointer      user_data)
{
  FuzzyIndex *index = (FuzzyIndex *)object;
  g_autoptr(GListModel) ret = NULL;
  g_autoptr(GTask) task = user_data;
  SearchState *state;
  GError *error = NULL;

  g_assert (FUZZY_IS_INDEX (index));
  g_assert (G_IS_TASK (task));

  state = g_task_get_task_data (task);

  if (NULL != (ret = fuzzy_index_query_finish (index, result, &error)))
    {
      const gchar *nsname;
      guint n_items;
      guint i;

      nsname = fuzzy_index_get_metadata_string (index, "namespace");

      n_items = g_list_model_get_n_items (ret);

      for (i = 0; i < n_items; i++)
        {
          g_autoptr(RtfmSearchResult) res = NULL;
          g_autoptr(FuzzyIndexMatch) match = g_list_model_get_item (ret, i);
          GVariant *variant = fuzzy_index_match_get_document (match);
          gfloat score = fuzzy_index_match_get_score (match);

          /*
           * If this score is too low to get added to the results, then we can
           * stop doing any more processing on these search results (as they
           * are sorted by score).
           */
          if (!rtfm_search_results_accepts_with_score (state->results, score))
            break;

          res = rtfm_gir_search_result_new (nsname, variant, score);

          rtfm_search_results_add (state->results, res);
        }
    }

  state->active--;

  if (state->active == 0)
    g_task_return_boolean (task, TRUE);
}

static void
rtfm_gir_provider_search_ready_cb (GObject      *object,
                                   GAsyncResult *result,
                                   gpointer      user_data)
{
  RtfmGirProvider *self = (RtfmGirProvider *)object;
  g_autoptr(GTask) task = user_data;
  SearchState *state;
  GError *error = NULL;
  guint i;

  g_assert (RTFM_IS_GIR_PROVIDER (self));
  g_assert (G_IS_TASK (task));

  if (!rtfm_gir_provider_load_indexes_finish (self, result, &error))
    {
      g_task_return_error (task, error);
      return;
    }

  state = g_task_get_task_data (task);
  state->active = self->search_indexes->len;

  if (state->active == 0)
    {
      g_task_return_boolean (task, TRUE);
      return;
    }

  for (i = 0; i < self->search_indexes->len; i++)
    {
      FuzzyIndex *index = g_ptr_array_index (self->search_indexes, i);

      fuzzy_index_query_async (index,
                               state->query,
                               RTFM_GIR_PROVIDER_SEARCH_MAX,
                               g_task_get_cancellable (task),
                               rtfm_gir_provider_query_cb,
                               g_object_ref (task));
    }
}

static void
rtfm_gir_provider_search_async (RtfmProvider        *provider,
                                RtfmSearchSettings  *search_settings,
                                RtfmSearchResults   *search_results,
                                GCancellable        *cancellable,
                                GAsyncReadyCallback  callback,
                                gpointer             user_data)
{
  RtfmGirProvider *self = (RtfmGirProvider *)provider;
  g_autoptr(GTask) task = NULL;
  SearchState *state;
  const gchar *search_text;

  g_assert (RTFM_IS_GIR_PROVIDER (self));
  g_assert (RTFM_IS_SEARCH_SETTINGS (search_settings));
  g_assert (RTFM_IS_SEARCH_RESULTS (search_results));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);

  search_text = rtfm_search_settings_get_search_text (search_settings);

  state = g_slice_new0 (SearchState);
  state->query = g_strdup (search_text);
  state->results = g_object_ref (search_results);
  state->active = self->search_indexes->len;

  g_task_set_task_data (task, state, search_state_free);

  rtfm_gir_provider_load_indexes_async (self,
                                        cancellable,
                                        rtfm_gir_provider_search_ready_cb,
                                        g_object_ref (task));
}

static gboolean
rtfm_gir_provider_search_finish (RtfmProvider  *provider,
                                 GAsyncResult  *result,
                                 GError       **error)
{
  g_return_val_if_fail (RTFM_IS_GIR_PROVIDER (provider), FALSE);
  g_return_val_if_fail (G_IS_TASK (result), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
provider_iface_init (RtfmProviderInterface *iface)
{
  iface->initialize = rtfm_gir_provider_initialize;
  iface->shutdown = rtfm_gir_provider_shutdown;
  iface->populate_async = rtfm_gir_provider_populate_async;
  iface->populate_finish = rtfm_gir_provider_populate_finish;
  iface->search_async = rtfm_gir_provider_search_async;
  iface->search_finish = rtfm_gir_provider_search_finish;
}
