/* rtfm-search-view-row.c
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

#define G_LOG_DOMAIN "rtfm-search-view-row"

#include "rtfm-search-view-row.h"

struct _RtfmSearchViewRow
{
  GtkListBoxRow     parent_instance;

  RtfmSearchResult *result;

  GtkLabel         *label;
};

G_DEFINE_TYPE (RtfmSearchViewRow, rtfm_search_view_row, GTK_TYPE_LIST_BOX_ROW)

enum {
  PROP_0,
  PROP_RESULT,
  N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * rtfm_search_view_row_get_result:
 *
 * Returns: (transfer none) (nullable): A #RtfmSearchResult.
 */
RtfmSearchResult *
rtfm_search_view_row_get_result (RtfmSearchViewRow *self)
{
  g_return_val_if_fail (RTFM_IS_SEARCH_VIEW_ROW (self), NULL);

  return self->result;
}

static void
rtfm_search_view_row_set_result (RtfmSearchViewRow *self,
                                 RtfmSearchResult  *result)
{
  g_return_if_fail (RTFM_IS_SEARCH_VIEW_ROW (self));
  g_return_if_fail (RTFM_IS_SEARCH_RESULT (result));

  if (g_set_object (&self->result, result))
    {
      const gchar *text = rtfm_search_result_get_text (result);

      gtk_label_set_label (self->label, text);
    }
}

static void
rtfm_search_view_row_finalize (GObject *object)
{
  RtfmSearchViewRow *self = (RtfmSearchViewRow *)object;

  g_clear_object (&self->result);

  G_OBJECT_CLASS (rtfm_search_view_row_parent_class)->finalize (object);
}

static void
rtfm_search_view_row_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
  RtfmSearchViewRow *self = RTFM_SEARCH_VIEW_ROW (object);

  switch (prop_id)
    {
    case PROP_RESULT:
      g_value_set_object (value, self->result);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_view_row_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  RtfmSearchViewRow *self = RTFM_SEARCH_VIEW_ROW (object);

  switch (prop_id)
    {
    case PROP_RESULT:
      rtfm_search_view_row_set_result (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
rtfm_search_view_row_class_init (RtfmSearchViewRowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_search_view_row_finalize;
  object_class->get_property = rtfm_search_view_row_get_property;
  object_class->set_property = rtfm_search_view_row_set_property;

  properties [PROP_RESULT] =
    g_param_spec_object ("result",
                         "Result",
                         "Result",
                         RTFM_TYPE_SEARCH_RESULT,
                         (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
rtfm_search_view_row_init (RtfmSearchViewRow *self)
{
  GtkBox *box;

  box = g_object_new (GTK_TYPE_BOX,
                      "orientation", GTK_ORIENTATION_HORIZONTAL,
                      "visible", TRUE,
                      NULL);

  self->label = g_object_new (GTK_TYPE_LABEL,
                              "hexpand", TRUE,
                              "visible", TRUE,
                              "xalign", 0.0f,
                              NULL);

  gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (self->label));
  gtk_container_add (GTK_CONTAINER (self), GTK_WIDGET (box));
}
