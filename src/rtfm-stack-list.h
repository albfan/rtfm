/* rtfm-stack-list.h
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
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

#ifndef RTFM_STACK_LIST_H
#define RTFM_STACK_LIST_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RTFM_TYPE_STACK_LIST (rtfm_stack_list_get_type())

G_DECLARE_DERIVABLE_TYPE (RtfmStackList, rtfm_stack_list, RTFM, STACK_LIST, GtkBin)

struct _RtfmStackListClass
{
  GtkBinClass parent_instance;

  void (*row_activated)    (RtfmStackList  *self,
                            GtkListBoxRow *row);
  void (*header_activated) (RtfmStackList  *self,
                            GtkListBoxRow *row);
};

typedef GtkWidget *(*RtfmStackListCreateWidgetFunc) (gpointer item,
                                                     gpointer user_data);

GtkWidget *rtfm_stack_list_new        (void);
void       rtfm_stack_list_push       (RtfmStackList                 *self,
                                       GtkWidget                     *header,
                                       GListModel                    *model,
                                       RtfmStackListCreateWidgetFunc  create_widget_func,
                                       gpointer                       user_data,
                                       GDestroyNotify                 user_data_free_func);
void        rtfm_stack_list_pop       (RtfmStackList                 *self);
GListModel *rtfm_stack_list_get_model (RtfmStackList                 *self);
guint       rtfm_stack_list_get_depth (RtfmStackList                 *self);
void        rtfm_stack_list_clear     (RtfmStackList                 *self);

G_END_DECLS

#endif /* RTFM_STACK_LIST_H */
