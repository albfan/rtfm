/* rtfm-search-view.c
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

#define G_LOG_DOMAIN "rtfm-search-view"

#include "rtfm-search-result.h"
#include "rtfm-search-results.h"
#include "rtfm-search-view.h"
#include "rtfm-search-view-row.h"
#include "rtfm-widget.h"

struct _RtfmSearchView
{
  GtkBin             parent_instance;

  guint              invalidate_handler;

  RtfmSearchResults *search_results;

  GtkStack          *stack;
  GtkListBox        *list_box;
  GtkScrolledWindow *scrolled_window;

  /* Pool of rows to avoid creating new ones */
  GQueue             trashed_rows;
};

G_DEFINE_TYPE (RtfmSearchView, rtfm_search_view, GTK_TYPE_BIN)

enum {
  PROP_0,
  PROP_SEARCH_RESULTS,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

static GtkWidget *
rtfm_search_view_create_row_func (gpointer item,
                                  gpointer user_data)
{
  RtfmSearchView *self = user_data;
  RtfmSearchResult *result = item;
  RtfmSearchViewRow *ret;

  g_assert (RTFM_IS_SEARCH_RESULT (result));
  g_assert (RTFM_IS_SEARCH_VIEW (self));

  if (self->trashed_rows.length)
    {
      ret = g_queue_pop_tail (&self->trashed_rows);
      rtfm_search_view_row_set_result (ret, result);
      rtfm_g_object_unref_in_main (ret);
    }
  else
    {
      ret = g_object_new (RTFM_TYPE_SEARCH_VIEW_ROW,
                          "result", result,
                          "visible", TRUE,
                          NULL);
    }

  return GTK_WIDGET (ret);
}

static gboolean
rtfm_search_view_do_invalidate (gpointer data)
{
  RtfmSearchView *self = data;

  g_assert (RTFM_IS_SEARCH_VIEW (self));

  self->invalidate_handler = 0;

  gtk_list_box_invalidate_headers (self->list_box);

  return G_SOURCE_REMOVE;
}

static void
rtfm_search_view_queue_invalidate (RtfmSearchView *self)
{
  g_assert (RTFM_IS_SEARCH_VIEW (self));

  if (self->invalidate_handler == 0)
    self->invalidate_handler = g_timeout_add (0, rtfm_search_view_do_invalidate, self);
}

static void
rtfm_search_view_items_changed (RtfmSearchResults *results,
                                guint              position,
                                guint              added,
                                guint              removed,
                                RtfmSearchView    *self)
{
  guint n_items;

  g_assert (RTFM_IS_SEARCH_RESULTS (results));

  n_items = g_list_model_get_n_items (G_LIST_MODEL (results));

  if (n_items == 0)
    gtk_stack_set_visible_child_name (self->stack, "no-results");
  else
    gtk_stack_set_visible_child_name (self->stack, "results");

  rtfm_search_view_queue_invalidate (self);
}


static void
rtfm_search_view_connect (RtfmSearchView *self)
{
  guint n_items;

  g_return_if_fail (RTFM_IS_SEARCH_VIEW (self));

  n_items = g_list_model_get_n_items (G_LIST_MODEL (self->search_results));

  gtk_list_box_bind_model (self->list_box,
                           G_LIST_MODEL (self->search_results),
                           rtfm_search_view_create_row_func,
                           self,
                           NULL);

  g_signal_connect (self->search_results,
                    "items-changed",
                    G_CALLBACK (rtfm_search_view_items_changed),
                    self);

  if (n_items == 0)
    gtk_stack_set_visible_child_name (self->stack, "no-results");
  else
    gtk_stack_set_visible_child_name (self->stack, "results");
}

static void
rtfm_search_view_disconnect (RtfmSearchView *self)
{
  g_return_if_fail (RTFM_IS_SEARCH_VIEW (self));

  g_signal_handlers_disconnect_by_func (self->search_results,
                                        G_CALLBACK (rtfm_search_view_items_changed),
                                        self);

  gtk_list_box_bind_model (self->list_box, NULL, NULL, NULL, NULL);
}

static void
rtfm_search_view_header_func (GtkListBoxRow *row,
                              GtkListBoxRow *before,
                              gpointer       user_data)
{
  RtfmSearchResult *before_result;
  RtfmSearchResult *result;
  const gchar *cat;
  const gchar *before_cat = NULL;

  g_assert (RTFM_IS_SEARCH_VIEW_ROW (row));
  g_assert (!before || RTFM_IS_SEARCH_VIEW_ROW (before));

  result = rtfm_search_view_row_get_result (RTFM_SEARCH_VIEW_ROW (row));
  cat = rtfm_search_result_get_category (result);

  if (before != NULL)
    {
      before_result = rtfm_search_view_row_get_result (RTFM_SEARCH_VIEW_ROW (before));
      before_cat = rtfm_search_result_get_category (before_result);
    }

  /* Strings are interned, pointer comparison is fine */
  if (cat != NULL && cat != before_cat)
    {
      GtkLabel *label;

      label = g_object_new (GTK_TYPE_LABEL,
                            "xalign", 0.0f,
                            "label", cat,
                            "visible", TRUE,
                            NULL);
      gtk_list_box_row_set_header (row, GTK_WIDGET (label));
    }
  else
    {
      gtk_list_box_row_set_header (row, NULL);
    }
}

static void
rtfm_search_view_row_remove (RtfmSearchView    *self,
                             RtfmSearchViewRow *row,
                             GtkListBox        *list_box)
{
  g_assert (RTFM_IS_SEARCH_VIEW (self));

  /* If we are in destroy, row might not be valid. */
  if (gtk_widget_in_destruction (GTK_WIDGET (self)))
    return;

  g_assert (GTK_IS_LIST_BOX (list_box));
  g_assert (RTFM_IS_SEARCH_VIEW_ROW (row));

  if (self->trashed_rows.length < 25)
    {
      rtfm_search_view_row_set_result (row, NULL);
      g_queue_push_head (&self->trashed_rows, g_object_ref (row));
    }
}

static void
rtfm_search_view_destroy (GtkWidget *widget)
{
  RtfmSearchView *self = (RtfmSearchView *)widget;

  g_assert (GTK_IS_WIDGET (widget));

  g_queue_foreach (&self->trashed_rows, (GFunc)g_object_unref, NULL);
  g_queue_clear (&self->trashed_rows);

  GTK_WIDGET_CLASS (rtfm_search_view_parent_class)->destroy (widget);
}

static void
rtfm_search_view_finalize (GObject *object)
{
  RtfmSearchView *self = (RtfmSearchView *)object;

  if (self->invalidate_handler != 0)
    {
      g_source_remove (self->invalidate_handler);
      self->invalidate_handler = 0;
    }

  g_clear_object (&self->search_results);

  G_OBJECT_CLASS (rtfm_search_view_parent_class)->finalize (object);
}

static void
rtfm_search_view_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  RtfmSearchView *self = RTFM_SEARCH_VIEW (object);

  switch (prop_id)
    {
    case PROP_SEARCH_RESULTS:
      g_value_set_object (value, rtfm_search_view_get_search_results (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_view_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  RtfmSearchView *self = RTFM_SEARCH_VIEW (object);

  switch (prop_id)
    {
    case PROP_SEARCH_RESULTS:
      rtfm_search_view_set_search_results (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_view_class_init (RtfmSearchViewClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = rtfm_search_view_finalize;
  object_class->get_property = rtfm_search_view_get_property;
  object_class->set_property = rtfm_search_view_set_property;

  widget_class->destroy = rtfm_search_view_destroy;

  properties [PROP_SEARCH_RESULTS] =
    g_param_spec_object ("search-results",
                         "Search Results",
                         "Search Results",
                         RTFM_TYPE_SEARCH_RESULTS,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/rtfm/ui/rtfm-search-view.ui");

  gtk_widget_class_set_css_name (widget_class, "searchview");

  gtk_widget_class_bind_template_child (widget_class, RtfmSearchView, list_box);
  gtk_widget_class_bind_template_child (widget_class, RtfmSearchView, scrolled_window);
  gtk_widget_class_bind_template_child (widget_class, RtfmSearchView, stack);

  rtfm_gtk_widget_class_set_css_from_resource (widget_class,
                                               NULL,
                                               "/org/gnome/rtfm/css/rtfm-search-view.css");
}

static void
rtfm_search_view_init (RtfmSearchView *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  g_queue_init (&self->trashed_rows);

  g_signal_connect_object (self,
                           "remove",
                           G_CALLBACK (rtfm_search_view_row_remove),
                           self,
                           G_CONNECT_SWAPPED | G_CONNECT_AFTER);

  gtk_list_box_set_header_func (self->list_box,
                                rtfm_search_view_header_func,
                                self,
                                NULL);
}

GtkWidget *
rtfm_search_view_new (void)
{
  return g_object_new (RTFM_TYPE_SEARCH_VIEW, NULL);
}

/**
 * rtfm_search_view_get_search_results:
 * @self: A #RtfmSearchResults
 *
 * Gets the search results being viewed.
 *
 * Returns: (nullable) (transfer none): An #RtfmSearchResults or %NULL.
 */
RtfmSearchResults *
rtfm_search_view_get_search_results (RtfmSearchView *self)
{
  g_return_val_if_fail (RTFM_IS_SEARCH_VIEW (self), NULL);

  return self->search_results;
}

void
rtfm_search_view_set_search_results (RtfmSearchView    *self,
                                     RtfmSearchResults *search_results)
{
  g_return_if_fail (RTFM_IS_SEARCH_VIEW (self));
  g_return_if_fail (RTFM_IS_SEARCH_RESULTS (search_results));

  if (search_results != self->search_results)
    {
      if (self->search_results != NULL)
        {
          rtfm_search_view_disconnect (self);
          g_clear_object (&self->search_results);
        }

      if (search_results != NULL)
        {
          self->search_results = g_object_ref (search_results);
          rtfm_search_view_connect (self);
        }

      g_object_notify_by_pspec (G_OBJECT (self), properties [PROP_SEARCH_RESULTS]);
    }
}
