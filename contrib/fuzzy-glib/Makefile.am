noinst_PROGRAMS = test-builder

pkglib_LTLIBRARIES = libfuzzy-glib-@API_VERSION@.la

libfuzzy_glib_@API_VERSION@_la_SOURCES = \
	fuzzy-index.c \
	fuzzy-index.h \
	fuzzy-index-builder.c \
	fuzzy-index-builder.h \
	fuzzy-index-cursor.c \
	fuzzy-index-cursor.h \
	fuzzy-index-match.c \
	fuzzy-index-match.h \
	fuzzy-version.h \
	fuzzy-glib.h \
	$(NULL)

libfuzzy_glib_@API_VERSION@_la_CFLAGS = \
	$(FUZZY_GLIB_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

libfuzzy_glib_@API_VERSION@_la_LIBADD = \
	$(FUZZY_GLIB_LIBS) \
	$(NULL)

libfuzzy_glib_@API_VERSION@_la_LDFLAGS = \
	$(WARN_LDFLAGS) \
	$(NULL)

test_builder_SOURCES = test-builder.c
test_builder_CFLAGS = $(FUZZY_GLIB_CFLAGS)
test_builder_LDADD = $(FUZZY_GLIB_LIBS) libfuzzy-glib-@API_VERSION@.la

-include $(top_srcdir)/git.mk
