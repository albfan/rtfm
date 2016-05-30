bin_PROGRAMS = rtfm-gir-compiler

plugindir = $(libdir)/rtfm/plugins
plugin_LTLIBRARIES = librtfm-plugin-gir.la

data_objects_sources = \
	rtfm-gir-alias.c \
	rtfm-gir-alias.h \
	rtfm-gir-bitfield.c \
	rtfm-gir-bitfield.h \
	rtfm-gir-callback.c \
	rtfm-gir-callback.h \
	rtfm-gir-c-include.c \
	rtfm-gir-c-include.h \
	rtfm-gir-class.c \
	rtfm-gir-class.h \
	rtfm-gir-field.c \
	rtfm-gir-field.h \
	rtfm-gir-function.c \
	rtfm-gir-function.h \
	rtfm-gir-include.c \
	rtfm-gir-include.h \
	rtfm-gir-member.c \
	rtfm-gir-member.h \
	rtfm-gir-method.c \
	rtfm-gir-method.h \
	rtfm-gir-namespace.c \
	rtfm-gir-namespace.h \
	rtfm-gir-package.c \
	rtfm-gir-package.h \
	rtfm-gir-parameter.c \
	rtfm-gir-parameter.h \
	rtfm-gir-property.c \
	rtfm-gir-property.h \
	rtfm-gir-record.c \
	rtfm-gir-record.h \
	rtfm-gir-repository.c \
	rtfm-gir-repository.h \
	rtfm-gir-return-value.c \
	rtfm-gir-return-value.h \
	rtfm-gir-type.c \
	rtfm-gir-type.h \
	$(NULL)

librtfm_plugin_gir_la_SOURCES = \
	$(data_objects_sources) \
	rtfm-gir-provider.c \
	rtfm-gir-provider.h \
	rtfm-gir-plugin.c \
	rtfm-gir-plugin.h \
	$(NULL)

librtfm_plugin_gir_la_CFLAGS = $(PLUGIN_CFLAGS)
librtfm_plugin_gir_la_LDFLAGS = $(PLUGIN_LDFLAGS)

rtfm_gir_compiler_SOURCES = \
	$(data_objects_sources) \
	rtfm-gir-compiler.c \
	$(NULL)

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
