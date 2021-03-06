/* rtfm-gir-parser.h
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

#ifndef RTFM_GIR_PARSER_H
#define RTFM_GIR_PARSER_H

#include <gio/gio.h>

#include "rtfm-gir-parser-types.h"
#include "rtfm-gir-repository.h"

G_BEGIN_DECLS

#define RTFM_GIR_TYPE_PARSER (rtfm_gir_parser_get_type())

G_DECLARE_FINAL_TYPE (RtfmGirParser, rtfm_gir_parser, RTFM_GIR, PARSER, GObject)

RtfmGirParser *rtfm_gir_parser_new (void);
RtfmGirRepository *rtfm_gir_parser_parse_file (RtfmGirParser *self,
                                           GFile *file,
                                           GCancellable *cancellable,
                                           GError **error);

G_END_DECLS

#endif /* RTFM_GIR_PARSER_H */
