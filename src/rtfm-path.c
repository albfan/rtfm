/* rtfm-path.c
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

#define G_LOG_DOMAIN "rtfm-path"

#include "rtfm-path.h"

struct _RtfmPath
{
  GObject  parent_instance;
  GQueue  *elements;
};

G_DEFINE_TYPE (RtfmPath, rtfm_path, G_TYPE_OBJECT)

static void
rtfm_path_finalize (GObject *object)
{
  RtfmPath *self = (RtfmPath *)object;

  g_queue_free_full (self->elements, g_object_unref);
  self->elements = NULL;

  G_OBJECT_CLASS (rtfm_path_parent_class)->finalize (object);
}

static void
rtfm_path_class_init (RtfmPathClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = rtfm_path_finalize;
}

static void
rtfm_path_init (RtfmPath *self)
{
  self->elements = g_queue_new ();
}

/**
 * rtfm_path_get_elements:
 *
 * Returns: (transfer none) (element-type Rtfm.PathElement): The elements of the path.
 */
GList *
rtfm_path_get_elements (RtfmPath *self)
{
  g_return_val_if_fail (RTFM_IS_PATH (self), NULL);

  return self->elements->head;
}

RtfmPath *
rtfm_path_new (void)
{
  return g_object_new (RTFM_TYPE_PATH, NULL);
}

void
rtfm_path_push_element (RtfmPath        *self,
                        RtfmPathElement *element)
{
  g_return_if_fail (RTFM_IS_PATH (self));
  g_return_if_fail (RTFM_IS_PATH_ELEMENT (element));

  g_queue_push_tail (self->elements, g_object_ref (element));
}

gboolean
rtfm_path_has_prefix (RtfmPath *self,
                      RtfmPath *prefix)
{
  const GList *iter;
  const GList *spec;

  g_return_val_if_fail (RTFM_IS_PATH (self), FALSE);
  g_return_val_if_fail (RTFM_IS_PATH (prefix), FALSE);

  if (self->elements->length < prefix->elements->length)
    return FALSE;

  for (iter = self->elements->head, spec = prefix->elements->head;
       iter != NULL && spec != NULL;
       iter = iter->next, spec = spec->next)
    {
      RtfmPathElement *spec_element = spec->data;
      RtfmPathElement *iter_element = iter->data;
      const gchar *spec_id = rtfm_path_element_get_id (spec_element);
      const gchar *iter_id = rtfm_path_element_get_id (iter_element);

      if (g_strcmp0 (spec_id, iter_id) == 0)
        continue;

      return FALSE;
    }

  return TRUE;
}

guint
rtfm_path_get_length (RtfmPath *self)
{
  g_return_val_if_fail (RTFM_IS_PATH (self), 0);

  return self->elements->length;
}

gchar *
rtfm_path_printf (RtfmPath *self)
{
  const GList *iter;
  GString *str;

  g_return_val_if_fail (RTFM_IS_PATH (self), NULL);

  str = g_string_new (NULL);

  for (iter = self->elements->head; iter != NULL; iter = iter->next)
    {
      RtfmPathElement *element = iter->data;
      const gchar *id = rtfm_path_element_get_id (element);

      g_string_append (str, id);
      if (iter->next != NULL)
        g_string_append_c (str, ',');
    }

  return g_string_free (str, FALSE);
}

gboolean
rtfm_path_is_empty (RtfmPath *self)
{
  g_return_val_if_fail (RTFM_IS_PATH (self), FALSE);

  return self->elements->length == 0;
}

/**
 * rtfm_path_get_element:
 *
 * Gets the path element found at @index.
 *
 * Indexes start from zero.
 *
 * Returns: (nullable) (transfer none): An #RtfmPathElement.
 */
RtfmPathElement *
rtfm_path_get_element (RtfmPath *self,
                       guint     index)
{
  g_return_val_if_fail (RTFM_IS_PATH (self), NULL);
  g_return_val_if_fail (index < self->elements->length, NULL);

  return g_queue_peek_nth (self->elements, index);
}
