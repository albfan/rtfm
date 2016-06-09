/* rtfm-gir-view-addin.c
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

#include <glib/gi18n.h>

#include "rtfm-gir-item.h"
#include "rtfm-gir-view-addin.h"

struct _RtfmGirViewAddin
{
  GObject    object;

  GtkLabel  *doc;
  GtkLabel  *title;
};

static void view_addin_iface_init (RtfmViewAddinInterface *iface);

G_DEFINE_TYPE_EXTENDED (RtfmGirViewAddin, rtfm_gir_view_addin, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (RTFM_TYPE_VIEW_ADDIN, view_addin_iface_init))

static void
rtfm_gir_view_addin_class_init (RtfmGirViewAddinClass *klass)
{
}

static void
rtfm_gir_view_addin_init (RtfmGirViewAddin *self)
{
}

static void
rtfm_gir_view_addin_load (RtfmViewAddin *addin,
                          RtfmView      *view)
{
  RtfmGirViewAddin *self = (RtfmGirViewAddin *)addin;

  g_assert (RTFM_IS_VIEW_ADDIN (self));
  g_assert (RTFM_IS_VIEW (view));

  self->title = g_object_new (GTK_TYPE_LABEL,
                              "visible", TRUE,
                              "xalign", 0.0f,
                              NULL);
  gtk_container_add (GTK_CONTAINER (view), GTK_WIDGET (self->title));

  self->doc = g_object_new (GTK_TYPE_LABEL,
                            "visible", TRUE,
                            "xalign", 0.0f,
                            NULL);
  gtk_container_add (GTK_CONTAINER (view), GTK_WIDGET (self->doc));
}

static void
rtfm_gir_view_addin_unload (RtfmViewAddin *addin,
                            RtfmView      *view)
{
  RtfmGirViewAddin *self = (RtfmGirViewAddin *)addin;

  g_assert (RTFM_IS_VIEW_ADDIN (self));
  g_assert (RTFM_IS_VIEW (view));

  gtk_widget_destroy (GTK_WIDGET (self->title));
  gtk_widget_destroy (GTK_WIDGET (self->doc));
}

static void
rtfm_gir_view_addin_set_item (RtfmViewAddin *addin,
                              RtfmItem      *item)
{
  RtfmGirViewAddin *self = (RtfmGirViewAddin *)addin;
  g_autofree gchar *doc = NULL;
  const gchar *title = NULL;

  g_assert (RTFM_IS_VIEW_ADDIN (self));
  g_assert (!item || RTFM_IS_ITEM (item));

  if (RTFM_GIR_IS_ITEM (item))
    {
      GObject *object = rtfm_gir_item_get_object (RTFM_GIR_ITEM (item));

      title = rtfm_item_get_title (item);

      if (g_object_class_find_property (G_OBJECT_GET_CLASS (object), "doc"))
        g_object_get (object, "doc", &doc, NULL);
    }

  gtk_label_set_label (self->doc, doc);
  gtk_label_set_label (self->title, title);
}

static void
view_addin_iface_init (RtfmViewAddinInterface *iface)
{
  iface->load = rtfm_gir_view_addin_load;
  iface->unload = rtfm_gir_view_addin_unload;
  iface->set_item = rtfm_gir_view_addin_set_item;
}
