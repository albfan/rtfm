/* test-util.c
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

#include "fuzzy-util.h"

static void
test_variant_hash (void)
{
  g_autoptr(GVariant) base = NULL;
  g_autoptr(GVariant) v1 = NULL;
  g_autoptr(GVariant) v2 = NULL;

  base = g_variant_new_string ("foo");
  g_variant_ref_sink (base);

  v1 = g_variant_new ("(v)", base);
  g_variant_ref_sink (v1);

  v2 = g_variant_new ("(v)", base);
  g_variant_ref_sink (v2);

  g_assert_cmpint (fuzzy_g_variant_hash (v1), ==, fuzzy_g_variant_hash (v2));
  g_assert_cmpint (g_variant_equal (v1, v2), ==, TRUE);
}

gint
main (gint argc,
      gchar *argv[])
{
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/Util/GVariant/hash", test_variant_hash);
  return g_test_run ();
}
