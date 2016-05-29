bin_PROGRAMS = rtfm-gir-compiler

plugindir = $(libdir)/rtfm/plugins
plugin_LTLIBRARIES = librtfm-plugin-gir.la

librtfm_plugin_gir_la_SOURCES = \
	rtfm-gir-provider.c \
	rtfm-gir-provider.h \
	rtfm-gir-item.c \
	rtfm-gir-item.h \
	rtfm-gir-plugin.c \
	$(NULL)

librtfm_plugin_gir_la_CFLAGS = $(PLUGIN_CFLAGS)
librtfm_plugin_gir_la_LDFLAGS = $(PLUGIN_LDFLAGS)

rtfm_gir_compiler_SOURCES = rtfm-gir-compiler.c
rtfm_gir_compiler_CFLAGS = \
	$(RTFM_CFLAGS) \
	-I$(top_builddir)/src \
	-I$(top_srcdir)/src \
	$(NULL)
rtfm_gir_compiler_LDADD = \
	$(RTFM_LIBS) \
	$(top_builddir)/src/librtfm-@API_VERSION@.la \
	$(NULL)

include $(top_srcdir)/plugins/Makefile.plugins

-include $(top_srcdir)/git.mk
