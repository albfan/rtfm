/* test-rtfm-path.c
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

#include <rtfm.h>

static void
test_path_basic (void)
{
  g_autoptr(RtfmPath) path1 = NULL;
  g_autoptr(RtfmPath) path2 = NULL;
  g_autoptr(RtfmPath) path3 = NULL;

#define ADD_ELEMENT(path, id, title) \
  { \
    g_autoptr(RtfmPathElement) element = NULL; \
    element = rtfm_path_element_new (id, NULL, title); \
    rtfm_path_append (path, element); \
  }

  path1 = rtfm_path_new ();
  ADD_ELEMENT (path1, "foo", "Foo");
  ADD_ELEMENT (path1, "foo1", "Foo 1");
  ADD_ELEMENT (path1, "foo2", "Foo 2");
  ADD_ELEMENT (path1, "foo3", "Foo 3");
  ADD_ELEMENT (path1, "foo4", "Foo 4");
  ADD_ELEMENT (path1, "foo5", "Foo 5");

  path2 = rtfm_path_new ();
  ADD_ELEMENT (path2, "foo", "Foo");
  ADD_ELEMENT (path2, "foo1", "Foo 1");
  ADD_ELEMENT (path2, "foo2", "Foo 2");
  ADD_ELEMENT (path2, "foo3", "Foo 3");

  path3 = rtfm_path_new ();
  ADD_ELEMENT (path3, "foo", "Foo");
  ADD_ELEMENT (path3, "foo1", "Foo 1");
  ADD_ELEMENT (path3, "foo2", "Foo 2");
  ADD_ELEMENT (path3, "foo4", "Foo 4");

  g_assert (rtfm_path_has_prefix (path1, path2));
  g_assert (!rtfm_path_has_prefix (path2, path1));
  g_assert (rtfm_path_has_prefix (path1, path1));
  g_assert (rtfm_path_has_prefix (path2, path2));
  g_assert (rtfm_path_has_prefix (path3, path3));
  g_assert (!rtfm_path_has_prefix (path1, path3));
  g_assert (!rtfm_path_has_prefix (path2, path3));
  g_assert (!rtfm_path_has_prefix (path3, path1));
  g_assert (!rtfm_path_has_prefix (path3, path2));
}

gint
main (gint argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/Rtfm/Path/basic", test_path_basic);
  return g_test_run ();
}
