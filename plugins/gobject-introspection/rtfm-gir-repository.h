/* rtfm-gir-repository.h
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

#ifndef RTFM_GIR_REPOSITORY_H
#define RTFM_GIR_REPOSITORY_H

#include "rtfm-gir-base.h"
#include "rtfm-gir-include.h"
#include "rtfm-gir-package.h"
#include "rtfm-gir-c-include.h"
#include "rtfm-gir-namespace.h"

G_BEGIN_DECLS

#define RTFM_TYPE_GIR_REPOSITORY (rtfm_gir_repository_get_type())

G_DECLARE_FINAL_TYPE (RtfmGirRepository, rtfm_gir_repository, RTFM, GIR_REPOSITORY, RtfmGirBase)

RtfmGirRepository *rtfm_gir_repository_new            (GFile              *file);
GPtrArray         *rtfm_gir_repository_get_includes   (RtfmGirRepository  *self);
GPtrArray         *rtfm_gir_repository_get_packages   (RtfmGirRepository  *self);
GPtrArray         *rtfm_gir_repository_get_c_includes (RtfmGirRepository  *self);
RtfmGirNamespace  *rtfm_gir_repository_get_namespace  (RtfmGirRepository  *self);
GFile *rtfm_gir_repository_get_file (RtfmGirRepository *self);
void rtfm_gir_repository_build_index_async (RtfmGirRepository   *self,
                                            GFile               *file,
                                            GCancellable        *cancellable,
                                            GAsyncReadyCallback  callback,
                                            gpointer             user_data);
gboolean rtfm_gir_repository_build_index_finish (RtfmGirRepository  *self,
                                                 GAsyncResult       *result,
                                                 GError            **error);

G_END_DECLS

#endif /* RTFM_GIR_REPOSITORY_H */
