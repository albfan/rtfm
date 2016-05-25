/* rtfm.h
 *
 * Copyright (C) 2016 Christian Hergert
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

#ifndef RTFM_H
#define RTFM_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RTFM_INSIDE
# include "rtfm-types.h"
# include "rtfm-collection.h"
# include "rtfm-item.h"
# include "rtfm-library.h"
# include "rtfm-path.h"
# include "rtfm-path-bar.h"
# include "rtfm-path-element.h"
# include "rtfm-provider.h"
# include "rtfm-sidebar.h"
# include "rtfm-sidebar-row.h"
# include "rtfm-search-settings.h"
# include "rtfm-stack-list.h"
# include "rtfm-view.h"
# include "rtfm-view-addin.h"
# include "rtfm-view-group.h"
# include "rtfm-version.h"
# include "rtfm-window.h"
#undef RTFM_INSIDE

G_END_DECLS

#endif /* RTFM_H */
