/* fuzzy-index-cursor.c
 *
 * Copyright (C) 2016 Christian Hergert <christian@hergert.me>
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

#define G_LOG_DOMAIN "fuzzy-index-cursor"

#include <string.h>

#include "fuzzy-index-cursor.h"
#include "fuzzy-index-match.h"

struct _FuzzyIndexCursor
{
  GObject       object;
  gchar        *query;
  GVariantDict *documents;
  GVariantDict *keys;
  GVariantDict *tables;
  GArray       *matches;
  guint         max_matches;
  guint         case_sensitive : 1;
};

typedef struct
{
  guint position;
  guint document_id;
} FuzzyIndexItem;

typedef struct
{
  const gchar *key;
  guint        document_id;
  gfloat       score;
} FuzzyMatch;

typedef struct
{
  const FuzzyIndexItem * const *tables;
  const gsize                  *tables_n_elements;
  gint                         *tables_state;
  guint                         n_tables;
  guint                         max_matches;
  const gchar                  *needle;
  GHashTable                   *matches;
} FuzzyLookup;

enum {
  PROP_0,
  PROP_CASE_SENSITIVE,
  PROP_DOCUMENTS,
  PROP_KEYS,
  PROP_TABLES,
  PROP_MAX_MATCHES,
  PROP_QUERY,
  N_PROPS
};

static void async_initable_iface_init (GAsyncInitableIface *iface);
static void list_model_iface_init     (GListModelInterface *iface);

static GParamSpec *properties [N_PROPS];

G_DEFINE_TYPE_EXTENDED (FuzzyIndexCursor, fuzzy_index_cursor, G_TYPE_OBJECT, 0,
                        G_IMPLEMENT_INTERFACE (G_TYPE_ASYNC_INITABLE, async_initable_iface_init)
                        G_IMPLEMENT_INTERFACE (G_TYPE_LIST_MODEL, list_model_iface_init))

static void
fuzzy_index_cursor_finalize (GObject *object)
{
  FuzzyIndexCursor *self = (FuzzyIndexCursor *)object;

  g_clear_pointer (&self->query, g_free);
  g_clear_pointer (&self->matches, g_array_unref);
  g_clear_pointer (&self->documents, g_variant_dict_unref);
  g_clear_pointer (&self->keys, g_variant_dict_unref);
  g_clear_pointer (&self->tables, g_variant_dict_unref);

  G_OBJECT_CLASS (fuzzy_index_cursor_parent_class)->finalize (object);
}

static void
fuzzy_index_cursor_get_property (GObject    *object,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  FuzzyIndexCursor *self = FUZZY_INDEX_CURSOR(object);

  switch (prop_id)
    {
    case PROP_CASE_SENSITIVE:
      g_value_set_boolean (value, self->case_sensitive);
      break;

    case PROP_MAX_MATCHES:
      g_value_set_uint (value, self->max_matches);
      break;

    case PROP_QUERY:
      g_value_set_string (value, self->query);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
fuzzy_index_cursor_set_property (GObject      *object,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  FuzzyIndexCursor *self = FUZZY_INDEX_CURSOR(object);

  switch (prop_id)
    {
    case PROP_CASE_SENSITIVE:
      self->case_sensitive = g_value_get_boolean (value);
      break;

    case PROP_DOCUMENTS:
      self->documents = g_value_dup_boxed (value);
      break;

    case PROP_KEYS:
      self->keys = g_value_dup_boxed (value);
      break;

    case PROP_TABLES:
      self->tables = g_value_dup_boxed (value);
      break;

    case PROP_MAX_MATCHES:
      self->max_matches = g_value_get_uint (value);
      break;

    case PROP_QUERY:
      self->query = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
fuzzy_index_cursor_class_init (FuzzyIndexCursorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = fuzzy_index_cursor_finalize;
  object_class->get_property = fuzzy_index_cursor_get_property;
  object_class->set_property = fuzzy_index_cursor_set_property;

  properties [PROP_CASE_SENSITIVE] =
    g_param_spec_boolean ("case-sensitive",
                          "Case Sensitive",
                          "Case Sensitive",
                          FALSE,
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  properties [PROP_DOCUMENTS] =
    g_param_spec_boxed ("documents",
                        "Documents",
                        "The dictionary of documents",
                        G_TYPE_VARIANT_DICT,
                        (G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  properties [PROP_KEYS] =
    g_param_spec_boxed ("keys",
                        "Keys",
                        "The dictionary of document id to key",
                        G_TYPE_VARIANT_DICT,
                        (G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  properties [PROP_TABLES] =
    g_param_spec_boxed ("tables",
                        "Tables",
                        "The dictionary of character indexes",
                        G_TYPE_VARIANT_DICT,
                        (G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  properties [PROP_QUERY] =
    g_param_spec_string ("query",
                         "Query",
                         "The query for the index",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  properties [PROP_MAX_MATCHES] =
    g_param_spec_uint ("max-matches",
                       "Max Matches",
                       "The max number of matches to display",
                       0,
                       G_MAXUINT,
                       0,
                       (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
fuzzy_index_cursor_init (FuzzyIndexCursor *self)
{
  self->matches = g_array_new (FALSE, FALSE, sizeof (FuzzyMatch));
}

static const gchar *
find_key_for_document (FuzzyIndexCursor *self,
                       guint             document_id)
{
  g_autoptr(GVariant) variant = NULL;
  const gchar *ret = NULL;
  gchar key[16];

  g_assert (FUZZY_IS_INDEX_CURSOR (self));

  g_snprintf (key, sizeof key, "%u", document_id);
  variant = g_variant_dict_lookup_value (self->keys, key, G_VARIANT_TYPE_STRING);

  /*
   * NOTE: This looks unsafe, but since we are using a single mmap()'d file
   *       and sub-variants all from the parent variants data, we can simply
   *       get a pointer into that mmap region.
   */
  if (variant != NULL)
    ret = g_variant_get_string (variant, NULL);

  return ret;
}

static gint
fuzzy_match_compare (gconstpointer a,
                     gconstpointer b)
{
  const FuzzyMatch *ma = a;
  const FuzzyMatch *mb = b;

  if (ma->score < mb->score)
    return 1;
  else if (ma->score > mb->score)
    return -1;

  return strcmp (ma->key, mb->key);
}

static gboolean
fuzzy_do_match (const FuzzyLookup    *lookup,
                const FuzzyIndexItem *item,
                guint                 table_index,
                gint                  score)
{
  const FuzzyIndexItem *table;
  const FuzzyIndexItem *iter;
  gssize n_elements;
  gint *state;
  gint iter_score;

  g_assert (lookup != NULL);
  g_assert (item != NULL);
  g_assert (score >= 0);

  table = lookup->tables [table_index];
  n_elements = (gssize)lookup->tables_n_elements [table_index];
  state = &lookup->tables_state [table_index];

  for (; state [0] < n_elements; state [0]++)
    {
      gpointer lookup_score;

      iter = &table [state [0]];

      if ((iter->document_id < item->document_id) ||
          ((iter->document_id == item->document_id) &&
           (iter->position <= item->position)))
        continue;
      else if (iter->document_id > item->document_id)
        break;

      iter_score = score + (iter->position - item->position);

      if (table_index + 1 < lookup->n_tables)
        {
          if (fuzzy_do_match (lookup, iter, table_index + 1, iter_score))
            return TRUE;
          continue;
        }

      if (!g_hash_table_lookup_extended (lookup->matches,
                                         GUINT_TO_POINTER (iter->document_id),
                                         NULL,
                                         (gpointer *)&lookup_score) ||
          iter_score < GPOINTER_TO_INT (lookup_score))
        g_hash_table_insert (lookup->matches,
                             GUINT_TO_POINTER (iter->document_id),
                             GINT_TO_POINTER (iter_score));

      return TRUE;
    }

  return FALSE;
}

static void
fuzzy_index_cursor_worker (GTask        *task,
                           gpointer      source_object,
                           gpointer      task_data,
                           GCancellable *cancellable)
{
  FuzzyIndexCursor *self = source_object;
  g_autoptr(GHashTable) matches = NULL;
  g_autoptr(GPtrArray) tables = NULL;
  g_autoptr(GArray) tables_n_elements = NULL;
  g_autofree gint *tables_state = NULL;
  g_autofree gchar *freeme = NULL;
  const gchar *query;
  FuzzyLookup lookup = { 0 };
  GHashTableIter iter;
  const gchar *str;
  gpointer key, value;

  g_assert (FUZZY_IS_INDEX_CURSOR (self));
  g_assert (G_IS_TASK (task));

  if (g_task_return_error_if_cancelled (task))
    return;

  /* No matches with empty query */
  if (self->query == NULL || *self->query == '\0')
    goto cleanup;

  /* If we are not case-sensitive, we need to downcase the query string */
  query = self->query;
  if (self->case_sensitive == FALSE)
    query = freeme = g_utf8_strdown (query, -1);

  tables = g_ptr_array_new ();
  tables_n_elements = g_array_new (FALSE, FALSE, sizeof (gsize));
  matches = g_hash_table_new (NULL, NULL);

  for (str = query; *str; str = g_utf8_next_char (str))
    {
      gunichar ch = g_utf8_get_char (str);
      g_autoptr(GVariant) table = NULL;
      gconstpointer fixed;
      gsize n_elements;
      gchar char_key[8];

      char_key [g_unichar_to_utf8 (ch, char_key)] = '\0';
      table = g_variant_dict_lookup_value (self->tables,
                                           char_key,
                                           (const GVariantType *)"a(ii)");

      /* No possible matches, missing table for character */
      if (table == NULL)
        goto cleanup;

      fixed = g_variant_get_fixed_array (table, &n_elements, sizeof (FuzzyIndexItem));
      g_array_append_val (tables_n_elements, n_elements);
      g_ptr_array_add (tables, (gpointer)fixed);
    }

  g_assert (tables->len > 0);
  g_assert (tables->len == tables_n_elements->len);

  tables_state = g_new0 (gint, tables->len);

  lookup.matches = matches;
  lookup.tables = (const FuzzyIndexItem * const *)tables->pdata;
  lookup.tables_n_elements = (const gsize *)tables_n_elements->data;
  lookup.tables_state = tables_state;
  lookup.n_tables = tables->len;
  lookup.needle = query;
  lookup.max_matches = self->max_matches;

  if G_LIKELY (lookup.n_tables > 1)
    {
      guint i;

      for (i = 0; i < lookup.tables_n_elements[0]; i++)
        {
          const FuzzyIndexItem *item;

          item = &lookup.tables[0][i];
          fuzzy_do_match (&lookup, item, 1, 0);
        }
    }
  else
    {
      guint last_id = G_MAXUINT;
      guint i;

      for (i = 0; i < lookup.tables_n_elements[0]; i++)
        {
          const FuzzyIndexItem *item;
          FuzzyMatch match;

          item = &lookup.tables[0][i];

          if (item->document_id != last_id)
            {
              last_id = item->document_id;

              match.document_id = item->document_id;
              match.score = 0;
              match.key = find_key_for_document (self, item->document_id);

              g_array_append_val (self->matches, match);
            }
        }

      goto cleanup;
    }

  if (g_task_return_error_if_cancelled (task))
    return;

  g_hash_table_iter_init (&iter, matches);

  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      FuzzyMatch match;

      match.document_id = GPOINTER_TO_INT (key);
      match.key = find_key_for_document (self, match.document_id);
      match.score = 1.0 / (strlen (match.key) + GPOINTER_TO_INT (value));

      g_array_append_val (self->matches, match);
    }

  if (g_task_return_error_if_cancelled (task))
    return;

  g_array_sort (self->matches, fuzzy_match_compare);
  if (lookup.max_matches > 0 && lookup.max_matches < self->matches->len)
    g_array_set_size (self->matches, lookup.max_matches);

cleanup:
  g_task_return_boolean (task, TRUE);
}

static void
fuzzy_index_cursor_init_async (GAsyncInitable      *initable,
                               gint                 io_priority,
                               GCancellable        *cancellable,
                               GAsyncReadyCallback  callback,
                               gpointer             user_data)
{
  FuzzyIndexCursor *self = (FuzzyIndexCursor *)initable;
  g_autoptr(GTask) task = NULL;

  g_assert (FUZZY_IS_INDEX_CURSOR (self));
  g_assert (!cancellable || G_IS_CANCELLABLE (cancellable));

  task = g_task_new (self, cancellable, callback, user_data);
  g_task_set_source_tag (task, fuzzy_index_cursor_init_async);
  g_task_set_priority (task, io_priority);
  g_task_set_check_cancellable (task, FALSE);
  g_task_run_in_thread (task, fuzzy_index_cursor_worker);
}

static gboolean
fuzzy_index_cursor_init_finish (GAsyncInitable  *initiable,
                                GAsyncResult    *result,
                                GError         **error)
{
  g_assert (FUZZY_IS_INDEX_CURSOR (initiable));
  g_assert (G_IS_TASK (result));

  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
async_initable_iface_init (GAsyncInitableIface *iface)
{
  iface->init_async = fuzzy_index_cursor_init_async;
  iface->init_finish = fuzzy_index_cursor_init_finish;
}

static GType
fuzzy_index_cursor_get_item_type (GListModel *model)
{
  return FUZZY_TYPE_INDEX_MATCH;
}

static guint
fuzzy_index_cursor_get_n_items (GListModel *model)
{
  FuzzyIndexCursor *self = (FuzzyIndexCursor *)model;

  g_assert (FUZZY_IS_INDEX_CURSOR (self));

  return self->matches->len;
}

static gpointer
fuzzy_index_cursor_get_item (GListModel *model,
                             guint       position)
{
  FuzzyIndexCursor *self = (FuzzyIndexCursor *)model;
  g_autoptr(GVariant) document = NULL;
  FuzzyMatch *match;
  gchar key[16];

  g_assert (FUZZY_IS_INDEX_CURSOR (self));
  g_assert (position < self->matches->len);

  match = &g_array_index (self->matches, FuzzyMatch, position);

  g_snprintf (key, sizeof key, "%u", match->document_id);
  document = g_variant_dict_lookup_value (self->documents, key, NULL);

  return g_object_new (FUZZY_TYPE_INDEX_MATCH,
                       "document", document,
                       "key", match->key,
                       "score", match->score,
                       NULL);
}

static void
list_model_iface_init (GListModelInterface *iface)
{
  iface->get_item_type = fuzzy_index_cursor_get_item_type;
  iface->get_n_items = fuzzy_index_cursor_get_n_items;
  iface->get_item = fuzzy_index_cursor_get_item;
}
