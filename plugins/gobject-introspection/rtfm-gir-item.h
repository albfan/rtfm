/* rtfm-gir-item.h
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

#ifndef RTFM_GIR_ITEM_H
#define RTFM_GIR_ITEM_H

#include <rtfm.h>

G_BEGIN_DECLS

#define RTFM_TYPE_GIR_ITEM (rtfm_gir_item_get_type())

G_DECLARE_DERIVABLE_TYPE (RtfmGirItem, rtfm_gir_item, RTFM, GIR_ITEM, RtfmItem)

struct _RtfmGirItemClass
{
  RtfmItem item_class;
};

G_END_DECLS

#endif /* RTFM_GIR_ITEM_H */
