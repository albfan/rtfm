/* rtfm-view-addin.c
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

#define G_LOG_DOMAIN "rtfm-view-addin"

#include "rtfm-view-addin.h"

G_DEFINE_INTERFACE (RtfmViewAddin, rtfm_view_addin, G_TYPE_OBJECT)

static void
rtfm_view_addin_default_init (RtfmViewAddinInterface *iface)
{
}

void
rtfm_view_addin_load (RtfmViewAddin *self,
                      RtfmView      *view)
{
  g_return_if_fail (RTFM_IS_VIEW_ADDIN (self));
  g_return_if_fail (RTFM_IS_VIEW (view));

  if (RTFM_VIEW_ADDIN_GET_IFACE (self)->load)
    RTFM_VIEW_ADDIN_GET_IFACE (self)->load (self, view);
}

void
rtfm_view_addin_unload (RtfmViewAddin *self,
                        RtfmView      *view)
{
  g_return_if_fail (RTFM_IS_VIEW_ADDIN (self));
  g_return_if_fail (RTFM_IS_VIEW (view));

  if (RTFM_VIEW_ADDIN_GET_IFACE (self)->unload)
    RTFM_VIEW_ADDIN_GET_IFACE (self)->unload (self, view);
}

void
rtfm_view_addin_set_item (RtfmViewAddin *self,
                          RtfmItem      *item)
{
  g_return_if_fail (RTFM_IS_VIEW_ADDIN (self));
  g_return_if_fail (!item || RTFM_IS_ITEM (item));

  if (RTFM_VIEW_ADDIN_GET_IFACE (self)->set_item)
    RTFM_VIEW_ADDIN_GET_IFACE (self)->set_item (self, item);
}
