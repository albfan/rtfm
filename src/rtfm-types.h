/* rtfm-types.h
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

#ifndef RTFM_TYPES_H
#define RTFM_TYPES_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RTFM_TYPE_COLLECTION (rtfm_collection_get_type())
#define RTFM_TYPE_ITEM (rtfm_item_get_type())
#define RTFM_TYPE_LIBRARY (rtfm_library_get_type())
#define RTFM_TYPE_PATH (rtfm_path_get_type())
#define RTFM_TYPE_PATH_BAR (rtfm_path_bar_get_type())
#define RTFM_TYPE_PATH_ELEMENT (rtfm_path_element_get_type())
#define RTFM_TYPE_PROVIDER (rtfm_provider_get_type ())
#define RTFM_TYPE_SEARCH_RESULT (rtfm_search_result_get_type())
#define RTFM_TYPE_SEARCH_RESULTS (rtfm_search_results_get_type())
#define RTFM_TYPE_SEARCH_SETTINGS (rtfm_search_settings_get_type())
#define RTFM_TYPE_SIDEBAR (rtfm_sidebar_get_type())
#define RTFM_TYPE_SIDEBAR_ROW (rtfm_sidebar_row_get_type())
#define RTFM_TYPE_STACK_LIST (rtfm_stack_list_get_type())
#define RTFM_TYPE_VIEW (rtfm_view_get_type())
#define RTFM_TYPE_VIEW_ADDIN (rtfm_view_addin_get_type ())
#define RTFM_TYPE_VIEW_GROUP (rtfm_view_group_get_type())
#define RTFM_TYPE_WINDOW (rtfm_window_get_type())

G_DECLARE_DERIVABLE_TYPE (RtfmItem, rtfm_item, RTFM, ITEM, GObject)
G_DECLARE_DERIVABLE_TYPE (RtfmSidebar, rtfm_sidebar, RTFM, SIDEBAR, GtkBox)
G_DECLARE_DERIVABLE_TYPE (RtfmStackList, rtfm_stack_list, RTFM, STACK_LIST, GtkBin)
G_DECLARE_DERIVABLE_TYPE (RtfmView, rtfm_view, RTFM, VIEW, GtkBin)
G_DECLARE_DERIVABLE_TYPE (RtfmViewGroup, rtfm_view_group, RTFM, VIEW_GROUP, GtkBox)
G_DECLARE_DERIVABLE_TYPE (RtfmSearchResult, rtfm_search_result, RTFM, SEARCH_RESULT, GObject)

G_DECLARE_FINAL_TYPE (RtfmCollection, rtfm_collection, RTFM, COLLECTION, GObject)
G_DECLARE_FINAL_TYPE (RtfmLibrary, rtfm_library, RTFM, LIBRARY, GObject)
G_DECLARE_FINAL_TYPE (RtfmPath, rtfm_path, RTFM, PATH, GObject)
G_DECLARE_FINAL_TYPE (RtfmPathBar, rtfm_path_bar, RTFM, PATH_BAR, GtkBox)
G_DECLARE_FINAL_TYPE (RtfmPathElement, rtfm_path_element, RTFM, PATH_ELEMENT, GObject)
G_DECLARE_FINAL_TYPE (RtfmSearchResults, rtfm_search_results, RTFM, SEARCH_RESULTS, GObject)
G_DECLARE_FINAL_TYPE (RtfmSearchSettings, rtfm_search_settings, RTFM, SEARCH_SETTINGS, GObject)
G_DECLARE_FINAL_TYPE (RtfmSidebarRow, rtfm_sidebar_row, RTFM, SIDEBAR_ROW, GtkListBoxRow)
G_DECLARE_FINAL_TYPE (RtfmWindow, rtfm_window, RTFM, WINDOW, GtkApplicationWindow)

G_DECLARE_INTERFACE (RtfmProvider, rtfm_provider, RTFM, PROVIDER, GObject)
G_DECLARE_INTERFACE (RtfmViewAddin, rtfm_view_addin, RTFM, VIEW_ADDIN, GObject)

G_END_DECLS

#endif /* RTFM_TYPES_H */
