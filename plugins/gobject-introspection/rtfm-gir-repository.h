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

#include <libxml/xmlreader.h>
#include <rtfm.h>

#include "rtfm-gir-include.h"
#include "rtfm-gir-package.h"
#include "rtfm-gir-c-include.h"
#include "rtfm-gir-namespace.h"

G_BEGIN_DECLS

#define RTFM_TYPE_GIR_REPOSITORY (rtfm_gir_repository_get_type())

G_DECLARE_FINAL_TYPE (RtfmGirRepository, rtfm_gir_repository, RTFM, GIR_REPOSITORY, RtfmItem)

gboolean rtfm_gir_repository_ingest (RtfmGirRepository  *self,
                                     xmlTextReaderPtr    reader,
                                     GError            **error);
RtfmGirInclude *rtfm_gir_repository_get_include (RtfmGirRepository *self);
RtfmGirPackage *rtfm_gir_repository_get_package (RtfmGirRepository *self);
RtfmGirCInclude *rtfm_gir_repository_get_c_include (RtfmGirRepository *self);
RtfmGirNamespace *rtfm_gir_repository_get_namespace (RtfmGirRepository *self);

G_END_DECLS

#endif /* RTFM_GIR_REPOSITORY_H */
