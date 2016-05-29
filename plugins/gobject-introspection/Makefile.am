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

include $(top_srcdir)/plugins/Makefile.plugins

-include $(top_srcdir)/git.mk
