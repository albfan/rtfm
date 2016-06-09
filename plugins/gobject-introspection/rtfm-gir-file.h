/* rtfm-gir-file.h
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

#ifndef RTFM_GIR_FILE_H
#define RTFM_GIR_FILE_H

#include <gio/gio.h>

#include "rtfm-gir-item.h"
#include "rtfm-gir-repository.h"

G_BEGIN_DECLS

#define RTFM_GIR_TYPE_FILE (rtfm_gir_file_get_type())

G_DECLARE_FINAL_TYPE (RtfmGirFile, rtfm_gir_file, RTFM_GIR, FILE, RtfmGirItem)

RtfmGirFile       *rtfm_gir_file_new            (GFile       *file);
GFile             *rtfm_gir_file_get_file       (RtfmGirFile *self);
RtfmGirRepository *rtfm_gir_file_get_repository (RtfmGirFile *self);

G_END_DECLS

#endif /* RTFM_GIR_FILE_H */
