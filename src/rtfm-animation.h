/* rtfm-animation.h
 *
 * Copyright (C) 2010-2016 Christian Hergert <christian@hergert.me>
 *
 * This file is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RTFM_ANIMATION_H
#define RTFM_ANIMATION_H

#include <gdk/gdk.h>

G_BEGIN_DECLS

#define RTFM_TYPE_ANIMATION      (rtfm_animation_get_type())
#define RTFM_TYPE_ANIMATION_MODE (rtfm_animation_mode_get_type())

G_DECLARE_FINAL_TYPE (RtfmAnimation, rtfm_animation,
                      RTFM, ANIMATION, GInitiallyUnowned)

typedef enum   _RtfmAnimationMode    RtfmAnimationMode;

enum _RtfmAnimationMode
{
  RTFM_ANIMATION_LINEAR,
  RTFM_ANIMATION_EASE_IN_QUAD,
  RTFM_ANIMATION_EASE_OUT_QUAD,
  RTFM_ANIMATION_EASE_IN_OUT_QUAD,
  RTFM_ANIMATION_EASE_IN_CUBIC,
  RTFM_ANIMATION_EASE_OUT_CUBIC,
  RTFM_ANIMATION_EASE_IN_OUT_CUBIC,

  RTFM_ANIMATION_LAST
};

GType         rtfm_animation_mode_get_type (void);
void          rtfm_animation_start         (RtfmAnimation     *animation);
void          rtfm_animation_stop          (RtfmAnimation     *animation);
void          rtfm_animation_add_property  (RtfmAnimation     *animation,
                                           GParamSpec       *pspec,
                                           const GValue     *value);

RtfmAnimation *rtfm_object_animatev         (gpointer          object,
                                           RtfmAnimationMode  mode,
                                           guint             duration_msec,
                                           GdkFrameClock    *frame_clock,
                                           const gchar      *first_property,
                                           va_list           args);
RtfmAnimation* rtfm_object_animate          (gpointer          object,
                                           RtfmAnimationMode  mode,
                                           guint             duration_msec,
                                           GdkFrameClock    *frame_clock,
                                           const gchar      *first_property,
                                           ...) G_GNUC_NULL_TERMINATED;
RtfmAnimation* rtfm_object_animate_full     (gpointer          object,
                                           RtfmAnimationMode  mode,
                                           guint             duration_msec,
                                           GdkFrameClock    *frame_clock,
                                           GDestroyNotify    notify,
                                           gpointer          notify_data,
                                           const gchar      *first_property,
                                           ...) G_GNUC_NULL_TERMINATED;

G_END_DECLS

#endif /* RTFM_ANIMATION_H */
