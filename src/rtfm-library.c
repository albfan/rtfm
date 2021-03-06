/* rtfm-library.c
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

#define G_LOG_DOMAIN "rtfm-library"

#include <girepository.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <libpeas/peas.h>

#include "rtfm-collection.h"
#include "rtfm-item.h"
#include "rtfm-item-private.h"
#include "rtfm-library.h"
#include "rtfm-library-private.h"
#include "rtfm-provider.h"

/**
 * SECTION:rtfm-library
 * @title: RtfmLibrary
 * @short_description: The documentation library
 *
 * The #RtfmLibrary class represents a loaded library. It manages the plugins
 * to build and modify the library as well as the library contents itself.
 *
 * The documentation is tracked as a tree and allows for plugins to populate
 * tree nodes as well reorder the tree as necessary. For example, a GObject
 * Introspection plugin might add all of the repositories it knows about,
 * but another plugin might adjust those into categories.
 *
 * The tree is populated with #RtfmItem instances. This encapsulates what
 * will be displayed to the screen. Before the items are displayed, then
 * will allow plugins to annotate the item and add ancillary information
 * such as external links, source code references, and other useful data.
 */

struct _RtfmLibrary
{
  GObject           parent_instance;

  PeasEngine       *engine;
  PeasExtensionSet *providers;
  RtfmItem         *root;
};

typedef struct
{
  RtfmItem       *destination;
  RtfmCollection *source;
  GList          *providers;
  guint           active;
} PopulateState;

typedef struct
{
  RtfmSearchSettings *search_settings;
  RtfmSearchResults  *search_results;
  GList              *providers;
} SearchState;

G_DEFINE_TYPE (RtfmLibrary, rtfm_library, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_ROOT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static void
search_state_free (gpointer data)
{
  SearchState *state = data;

  g_clear_object (&state->search_settings);
  g_clear_object (&state->search_results);
  g_list_free_full (state->providers, g_object_unref);
  state->providers = NULL;
  g_slice_free (SearchState, state);
}

static void
populate_state_free (gpointer data)
{
  PopulateState *state = data;

  g_clear_object (&state->source);
  g_clear_object (&state->destination);
  g_list_free_full (state->providers, g_object_unref);
  state->providers = NULL;
  g_slice_free (PopulateState, state);
}

static void
rtfm_library_reset (RtfmLibrary *self)
{
  g_assert (RTFM_IS_LIBRARY (self));

  g_clear_object (&self->root);

  self->root = g_object_new (RTFM_TYPE_ITEM,
                             "id", "root",
                             "icon-name", "help-contents-symbolic",
                             "title", _("Developer Library"),
                             NULL);

  _rtfm_item_set_is_root (self->root, TRUE);

  g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_ROOT]);
}

static void
rtfm_library_provider_added (PeasExtensionSet *set,
                             PeasPluginInfo   *plugin_info,
                             PeasExtension    *exten,
                             gpointer          user_data)
{
  RtfmLibrary *self = user_data;
  RtfmProvider *provider = (RtfmProvider *)exten;

  g_assert (PEAS_IS_EXTENSION_SET (set));
  g_assert (plugin_info != NULL);
  g_assert (RTFM_IS_PROVIDER (provider));
  g_assert (RTFM_IS_LIBRARY (self));

  rtfm_provider_initialize (provider, self);
  rtfm_library_reset (self);
}

static void
rtfm_library_provider_removed (PeasExtensionSet *set,
                               PeasPluginInfo   *plugin_info,
                               PeasExtension    *exten,
                               gpointer          user_data)
{
  RtfmLibrary *self = user_data;
  RtfmProvider *provider = (RtfmProvider *)exten;

  g_assert (PEAS_IS_EXTENSION_SET (set));
  g_assert (plugin_info != NULL);
  g_assert (RTFM_IS_PROVIDER (provider));
  g_assert (RTFM_IS_LIBRARY (self));

  rtfm_provider_shutdown (provider, self);
  rtfm_library_reset (self);
}

static void
rtfm_library_constructed (GObject *object)
{
  RtfmLibrary *self = (RtfmLibrary *)object;
  const GList *plugin_infos;

  G_OBJECT_CLASS (rtfm_library_parent_class)->constructed (object);

  self->engine = peas_engine_new ();
  peas_engine_enable_loader (self->engine, "python3");

  if (g_getenv ("RTFM_IN_TREE_PLUGINS") != NULL)
    peas_engine_prepend_search_path (self->engine, "plugins", "plugins");
  else
    peas_engine_add_search_path (self->engine,
                                 PACKAGE_LIBDIR"/plugins",
                                 PACKAGE_DATADIR"/plugins");

  plugin_infos = peas_engine_get_plugin_list (self->engine);

  for (; plugin_infos != NULL; plugin_infos = plugin_infos->next)
    {
      PeasPluginInfo *plugin_info = plugin_infos->data;

      peas_engine_load_plugin (self->engine, plugin_info);
    }

  self->providers = peas_extension_set_new (self->engine,
                                            RTFM_TYPE_PROVIDER,
                                            NULL);

  g_signal_connect_object (self->providers,
                           "extension-added",
                           G_CALLBACK (rtfm_library_provider_added),
                           self,
                           0);

  g_signal_connect_object (self->providers,
                           "extension-removed",
                           G_CALLBACK (rtfm_library_provider_removed),
                           self,
                           0);

  peas_extension_set_foreach (self->providers,
                              rtfm_library_provider_added,
                              self);
}

static void
rtfm_library_finalize (GObject *object)
{
  RtfmLibrary *self = (RtfmLibrary *)object;

  g_clear_object (&self->root);
  g_clear_object (&self->providers);
  g_clear_object (&self->engine);

  G_OBJECT_CLASS (rtfm_library_parent_class)->finalize (object);
}

static void
rtfm_library_get_property (GObject    *object,
                           guint       prop_id,
                           GValue     *value,
                           GParamSpec *pspec)
{
  RtfmLibrary *self = RTFM_LIBRARY (object);

  switch (prop_id)
    {
    case PROP_ROOT:
      g_value_set_object (value, rtfm_library_get_root (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_library_class_init (RtfmLibraryClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkIconTheme *icon_theme;
  GdkScreen *screen;

  object_class->constructed = rtfm_library_constructed;
  object_class->finalize = rtfm_library_finalize;
  object_class->get_property = rtfm_library_get_property;

  properties [PROP_ROOT] =
    g_param_spec_object ("root",
                         "Root",
                         "The root of the documentation tree.",
                         RTFM_TYPE_ITEM,
                         (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  if (NULL != (screen = gdk_screen_get_default ()) &&
      NULL != (icon_theme = gtk_icon_theme_get_default ()))
    gtk_icon_theme_add_resource_path (icon_theme, "/org/gnome/rtfm/icons/");

  if (g_getenv ("RTFM_IN_TREE_PLUGINS") == NULL)
    {
      g_irepository_prepend_search_path (PACKAGE_LIBDIR"/girepository-1.0");
      g_irepository_prepend_library_path (PACKAGE_LIBDIR);
    }
}

static void
rtfm_library_init (RtfmLibrary *self)
{
  rtfm_library_reset (self);
}

static void
collect_providers (PeasExtensionSet *set,
                   PeasPluginInfo   *plugin_info,
                   PeasExtension    *exten,
                   gpointer          user_data)
{
  GList **list = user_data;

  *list = g_list_prepend (*list, g_object_ref (exten));
}

static void
rtfm_library_copy_into (RtfmItem *item,
                        gpointer  user_data)
{
  RtfmItem *parent = user_data;

  g_assert (RTFM_IS_ITEM (item));
  g_assert (RTFM_IS_ITEM (parent));

  rtfm_item_append (parent, item);
}

static void
rtfm_library_populate_cb (GObject      *object,
                          GAsyncResult *result,
                          gpointer      user_data)
{
  RtfmProvider *provider = (RtfmProvider *)object;
  g_autoptr(GTask) task = user_data;
  g_autoptr(GError) error = NULL;
  PopulateState *state;

  g_assert (RTFM_IS_PROVIDER (provider));
  g_assert (G_IS_TASK (task));

  state = g_task_get_task_data (task);
  state->active--;

  if (!rtfm_provider_populate_finish (provider, result, &error))
    g_debug ("%s", error->message);

  if (state->active == 0)
    {
      GList *iter;

      /* Allow providers to post-process the list */
      for (iter = state->providers; iter != NULL; iter = iter->next)
        {
          provider = iter->data;

          rtfm_provider_postprocess (provider, state->source);
        }

      rtfm_collection_foreach (state->source,
                               rtfm_library_copy_into,
                               state->destination);

      g_task_return_boolean (task, TRUE);
    }
}

void
rtfm_library_populate_async (RtfmLibrary         *self,
                             RtfmItem            *item,
                             GCancellable        *cancellable,
                             GAsyncReadyCallback  callback,
                             gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;
  PopulateState *state;
  RtfmPath *path;
  GList *providers = NULL;
  GList *iter;

  g_return_if_fail (RTFM_IS_LIBRARY (self));
  g_return_if_fail (RTFM_IS_ITEM (item));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);

  if (_rtfm_item_get_populated (item))
    {
      g_task_return_boolean (task, TRUE);
      return;
    }

  _rtfm_item_set_populated (item, TRUE);

  peas_extension_set_foreach (self->providers, collect_providers, &providers);

  if (providers == NULL)
    {
      g_task_return_boolean (task, TRUE);
      return;
    }

  path = rtfm_item_get_path (item);

  state = g_slice_new0 (PopulateState);
  state->source = rtfm_collection_new (path);
  state->destination = g_object_ref (item);
  state->active = g_list_length (providers);
  state->providers = providers;

  g_task_set_task_data (task, state, populate_state_free);

  for (iter = providers; iter != NULL; iter = iter->next)
    {
      RtfmProvider *provider = iter->data;

      rtfm_provider_populate_async (provider,
                                    item,
                                    state->source,
                                    cancellable,
                                    rtfm_library_populate_cb,
                                    g_object_ref (task));
    }
}

gboolean
rtfm_library_populate_finish (RtfmLibrary   *self,
                              GAsyncResult  *result,
                              GError       **error)
{
  g_return_val_if_fail (RTFM_IS_LIBRARY (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (result), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}

PeasEngine *
rtfm_library_get_engine (RtfmLibrary *self)
{
  g_return_val_if_fail (RTFM_IS_LIBRARY (self), NULL);

  return self->engine;
}

static void
rtfm_library_get_item_by_id_cb (PeasExtensionSet *set,
                                PeasPluginInfo   *plugin_info,
                                PeasExtension    *exten,
                                gpointer          user_data)
{
  RtfmProvider *provider = (RtfmProvider *)exten;
  struct {
    const gchar *id;
    RtfmItem    *result;
  } *get_item_by_id = user_data;

  g_assert (PEAS_IS_EXTENSION_SET (set));
  g_assert (plugin_info != NULL);
  g_assert (RTFM_IS_PROVIDER (provider));
  g_assert (get_item_by_id != NULL);

  if (get_item_by_id->result == NULL)
    get_item_by_id->result = rtfm_provider_load_item (provider, get_item_by_id->id);
}

/**
 * rtfm_library_get_item_by_id:
 * @self: An #RtfmLibrary
 * @id: The identifier of the item to load.
 *
 * Requests that the various #RtfmProvider plugins load a given item.
 *
 * The item will not have extensions loaded, so if you need a fully
 * extended item, you will need to call rtfm_library_extend_item_async()
 * with the result.
 *
 * Returns: (transfer full) (nullable): An #RtfmItem or %NULL.
 */
RtfmItem *
rtfm_library_get_item_by_id (RtfmLibrary *self,
                             const gchar *id)
{
  struct {
    const gchar *id;
    RtfmItem    *result;
  } get_item_by_id = { id, NULL };

  g_return_val_if_fail (RTFM_IS_LIBRARY (self), NULL);
  g_return_val_if_fail (id != NULL, NULL);

  peas_extension_set_foreach (self->providers,
                              rtfm_library_get_item_by_id_cb,
                              &get_item_by_id);

  return get_item_by_id.result;
}

static void
rtfm_library_get_provider_cb (PeasExtensionSet *set,
                              PeasPluginInfo   *plugin_info,
                              PeasExtension    *exten,
                              gpointer          user_data)
{
  struct {
    const gchar  *id;
    RtfmProvider *provider;
  } *lookup = user_data;

  g_assert (PEAS_IS_EXTENSION_SET (set));
  g_assert (plugin_info != NULL);
  g_assert (RTFM_IS_PROVIDER (exten));
  g_assert (lookup != NULL);
  g_assert (lookup->id != NULL);

  if (g_strcmp0 (lookup->id, peas_plugin_info_get_module_name (plugin_info)) == 0)
    lookup->provider = RTFM_PROVIDER (exten);
}

/**
 * rtfm_library_get_provider:
 * @self: An #RtfmLibrary
 * @id: the module id for the provider
 *
 * Returns the provider instance by module id.
 *
 * The module id is the module name used by the libpeas based plugin.
 *
 * Returns: (transfer none) (nullable): An #RtfmProvider.
 */
RtfmProvider *
rtfm_library_get_provider (RtfmLibrary *self,
                           const gchar *id)
{
  struct {
    const gchar  *id;
    RtfmProvider *provider;
  } lookup = { id, NULL };

  g_return_val_if_fail (RTFM_IS_LIBRARY (self), NULL);

  peas_extension_set_foreach (self->providers,
                              rtfm_library_get_provider_cb,
                              &lookup);

  return lookup.provider;
}

RtfmLibrary *
rtfm_library_new (void)
{
  return g_object_new (RTFM_TYPE_LIBRARY, NULL);
}

/**
 * rtfm_library_get_default:
 *
 * Returns: (transfer none): An #RtfmLibrary.
 */
RtfmLibrary *
rtfm_library_get_default (void)
{
  static RtfmLibrary *instance;

  if (g_once_init_enter (&instance))
    g_once_init_leave (&instance, rtfm_library_new ());

  return instance;
}

/**
 * rtfm_library_get_root:
 * @self: An #RtfmLibrary
 *
 * Gets the root node of the library.
 *
 * The library is tracked as a tree of #RtfmItem. The items are periodically built by the
 * various plugins and pruned when possible. These items are used to nativate the
 * documentation by the viewer.
 *
 * Plugins can re-arrange the item tree as they see fit.
 *
 * Returns: (transfer none): An #RtfmItem.
 */
RtfmItem *
rtfm_library_get_root (RtfmLibrary *self)
{
  g_return_val_if_fail (RTFM_IS_LIBRARY (self), NULL);

  return self->root;
}

static void
rtfm_library_search_cb (GObject      *object,
                        GAsyncResult *result,
                        gpointer      user_data)
{
  RtfmProvider *provider = (RtfmProvider *)object;
  g_autoptr(GTask) task = user_data;
  g_autoptr(GError) error = NULL;
  SearchState *state;

  g_assert (RTFM_IS_PROVIDER (provider));
  g_assert (G_IS_TASK (task));

  if (!rtfm_provider_search_finish (provider, result, &error))
    {
      if (!g_error_matches (error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        g_warning ("%s", error->message);
    }

  state = g_task_get_task_data (task);

  state->providers = g_list_remove (state->providers, provider);
  g_object_unref (provider);

  if (state->providers == NULL)
    g_task_return_boolean (task, TRUE);
}

void
rtfm_library_search_async (RtfmLibrary         *self,
                           RtfmSearchSettings  *search_settings,
                           RtfmSearchResults   *search_results,
                           GCancellable        *cancellable,
                           GAsyncReadyCallback  callback,
                           gpointer             user_data)
{
  g_autoptr(GTask) task = NULL;
  SearchState *state;
  GList *providers = NULL;
  GList *iter;

  g_return_if_fail (RTFM_IS_LIBRARY (self));
  g_return_if_fail (RTFM_IS_SEARCH_SETTINGS (search_settings));
  g_return_if_fail (RTFM_IS_SEARCH_RESULTS (search_results));
  g_return_if_fail (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_check_cancellable (task, FALSE);
  g_task_set_source_tag (task, rtfm_library_search_async);

  peas_extension_set_foreach (self->providers, collect_providers, &providers);

  state = g_slice_new0 (SearchState);
  state->search_settings = g_object_ref (search_settings);
  state->search_results = g_object_ref (search_results);
  state->providers = providers;

  g_task_set_task_data (task, state, search_state_free);

  for (iter = providers; iter != NULL; iter = iter->next)
    {
      RtfmProvider *provider = iter->data;

      rtfm_provider_search_async (provider,
                                  search_settings,
                                  search_results,
                                  cancellable,
                                  rtfm_library_search_cb,
                                  g_object_ref (task));
    }

  if (providers == NULL)
    g_task_return_boolean (task, TRUE);
}

gboolean
rtfm_library_search_finish (RtfmLibrary   *self,
                            GAsyncResult  *result,
                            GError       **error)
{
  g_return_val_if_fail (RTFM_IS_LIBRARY (self), FALSE);
  g_return_val_if_fail (G_IS_TASK (result), FALSE);

  return g_task_propagate_boolean (G_TASK (result), error);
}
