/* rtfm-view-addin.h
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

#ifndef RTFM_VIEW_ADDIN_H
#define RTFM_VIEW_ADDIN_H

#include "rtfm-types.h"

G_BEGIN_DECLS

struct _RtfmViewAddinInterface
{
  GTypeInterface parent;

  void (*load)     (RtfmViewAddin *self,
                    RtfmView      *view);
  void (*unload)   (RtfmViewAddin *self,
                    RtfmView      *view);
  void (*set_item) (RtfmViewAddin *self,
                    RtfmItem      *item);
};

void rtfm_view_addin_load     (RtfmViewAddin *self,
                               RtfmView      *view);
void rtfm_view_addin_unload   (RtfmViewAddin *self,
                               RtfmView      *view);
void rtfm_view_addin_set_item (RtfmViewAddin *self,
                               RtfmItem      *item);

G_END_DECLS

#endif /* RTFM_VIEW_ADDIN_H */
