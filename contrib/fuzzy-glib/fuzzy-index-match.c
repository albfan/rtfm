/* fuzzy-index-match.c
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

#define G_LOG_DOMAIN "fuzzy-index-match"

#include "fuzzy-index-match.h"

struct _FuzzyIndexMatch
{
  GObject   object;
  GVariant *document;
  gchar    *key;
  gfloat    score;
};

enum {
  PROP_0,
  PROP_DOCUMENT,
  PROP_KEY,
  PROP_SCORE,
  N_PROPS
};

G_DEFINE_TYPE (FuzzyIndexMatch, fuzzy_index_match, G_TYPE_OBJECT)

static GParamSpec *properties [N_PROPS];

static void
fuzzy_index_match_finalize (GObject *object)
{
  FuzzyIndexMatch *self = (FuzzyIndexMatch *)object;

  g_clear_pointer (&self->document, g_variant_unref);
  g_clear_pointer (&self->key, g_free);

  G_OBJECT_CLASS (fuzzy_index_match_parent_class)->finalize (object);
}

static void
fuzzy_index_match_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  FuzzyIndexMatch *self = FUZZY_INDEX_MATCH (object);

  switch (prop_id)
    {
    case PROP_DOCUMENT:
      g_value_set_variant (value, self->document);
      break;

    case PROP_SCORE:
      g_value_set_float (value, self->score);
      break;

    case PROP_KEY:
      g_value_set_string (value, self->key);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
fuzzy_index_match_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  FuzzyIndexMatch *self = FUZZY_INDEX_MATCH (object);

  switch (prop_id)
    {
    case PROP_DOCUMENT:
      self->document = g_value_dup_variant (value);
      break;

    case PROP_SCORE:
      self->score = g_value_get_float (value);
      break;

    case PROP_KEY:
      self->key = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
fuzzy_index_match_class_init (FuzzyIndexMatchClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = fuzzy_index_match_finalize;
  object_class->get_property = fuzzy_index_match_get_property;
  object_class->set_property = fuzzy_index_match_set_property;

  properties [PROP_DOCUMENT] =
    g_param_spec_variant ("document",
                          "Document",
                          "Document",
                          G_VARIANT_TYPE_ANY,
                          NULL,
                          (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  properties [PROP_KEY] =
    g_param_spec_string ("key",
                         "Key",
                         "The string key that was inserted for the document",
                         NULL,
                         (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  properties [PROP_SCORE] =
    g_param_spec_float ("score",
                        "Score",
                        "Score",
                        -G_MINFLOAT,
                        G_MAXFLOAT,
                        0.0f,
                        (G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS));

  g_object_class_install_properties (object_class, N_PROPS, properties);
}

static void
fuzzy_index_match_init (FuzzyIndexMatch *match)
{
}

/**
 * fuzzy_index_match_get_document:
 *
 * Returns: (transfer none): A #GVariant.
 */
GVariant *
fuzzy_index_match_get_document (FuzzyIndexMatch *self)
{
  g_return_val_if_fail (FUZZY_IS_INDEX_MATCH (self), NULL);

  return self->document;
}

gfloat
fuzzy_index_match_get_score (FuzzyIndexMatch *self)
{
  g_return_val_if_fail (FUZZY_IS_INDEX_MATCH (self), 0.0f);

  return self->score;
}

const gchar *
fuzzy_index_match_get_key (FuzzyIndexMatch *self)
{
  g_return_val_if_fail (FUZZY_IS_INDEX_MATCH (self), NULL);

  return self->key;
}
