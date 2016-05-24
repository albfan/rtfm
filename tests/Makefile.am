EXTRA_DIST =
TESTS =
noinst_PROGRAMS =

AM_CPPFLAGS = \
	$(RTFM_CFLAGS) \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src \
	$(NULL)

AM_LDFLAGS = \
	$(RTFM_LIBS) \
	$(top_builddir)/src/librtfm-@API_VERSION@.la \
	-export-dynamic \
	$(NULL)

TESTS_ENVIRONMENT= \
	GI_TYPELIB_PATH="$(top_builddir)/src:$(GI_TYPELIB_PATH)" \
	RTFM_IN_TREE_PLUGINS=1 \
	G_TEST_SRCDIR="$(abs_srcdir)" \
	G_TEST_BUILDDIR="$(abs_builddir)" \
	G_DEBUG=gc-friendly \
	GSETTINGS_BACKEND=memory \
	PYTHONDONTWRITEBYTECODE=yes \
	MALLOC_CHECK_=2 \
	MALLOC_PERTURB_=$$(($${RANDOM:-256} % 256))

LOG_COMPILER = $(top_srcdir)/build-aux/tap-test

TESTS += test-rtfm-path
noinst_PROGRAMS += test-rtfm-path

-include $(top_srcdir)/git.mk
