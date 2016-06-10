plugindir = $(libdir)/rtfm/plugins
plugin_LTLIBRARIES = librtfm-plugin-gir.la
plugin_DATA = gobject-introspection.plugin

librtfm_plugin_gir_la_SOURCES = \
	rtfm-gir-alias.c \
	rtfm-gir-alias.h \
	rtfm-gir-annotation.c \
	rtfm-gir-annotation.h \
	rtfm-gir-array.c \
	rtfm-gir-array.h \
	rtfm-gir-bitfield.c \
	rtfm-gir-bitfield.h \
	rtfm-gir-c-include.c \
	rtfm-gir-c-include.h \
	rtfm-gir-callback.c \
	rtfm-gir-callback.h \
	rtfm-gir-class.c \
	rtfm-gir-class.h \
	rtfm-gir-constant.c \
	rtfm-gir-constant.h \
	rtfm-gir-constructor.c \
	rtfm-gir-constructor.h \
	rtfm-gir-doc-deprecated.c \
	rtfm-gir-doc-deprecated.h \
	rtfm-gir-doc-stability.c \
	rtfm-gir-doc-stability.h \
	rtfm-gir-doc-version.c \
	rtfm-gir-doc-version.h \
	rtfm-gir-doc.c \
	rtfm-gir-doc.h \
	rtfm-gir-enumeration.c \
	rtfm-gir-enumeration.h \
	rtfm-gir-field.c \
	rtfm-gir-field.h \
	rtfm-gir-file.c \
	rtfm-gir-file.h \
	rtfm-gir-function.c \
	rtfm-gir-function.h \
	rtfm-gir-glib-boxed.c \
	rtfm-gir-glib-boxed.h \
	rtfm-gir-glib-signal.c \
	rtfm-gir-glib-signal.h \
	rtfm-gir-implements.c \
	rtfm-gir-implements.h \
	rtfm-gir-include.c \
	rtfm-gir-include.h \
	rtfm-gir-instance-parameter.c \
	rtfm-gir-instance-parameter.h \
	rtfm-gir-interface.c \
	rtfm-gir-interface.h \
	rtfm-gir-item.c \
	rtfm-gir-item.h \
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
	rtfm-gir-parameters.c \
	rtfm-gir-parameters.h \
	rtfm-gir-parser-object.c \
	rtfm-gir-parser-types.h \
	rtfm-gir-parser.c \
	rtfm-gir-parser.h \
	rtfm-gir-plugin.c \
	rtfm-gir-plugin.h \
	rtfm-gir-prerequisite.c \
	rtfm-gir-prerequisite.h \
	rtfm-gir-property.c \
	rtfm-gir-property.h \
	rtfm-gir-provider.c \
	rtfm-gir-provider.h \
	rtfm-gir-record.c \
	rtfm-gir-record.h \
	rtfm-gir-repository.c \
	rtfm-gir-repository.h \
	rtfm-gir-return-value.c \
	rtfm-gir-return-value.h \
	rtfm-gir-search-result.c \
	rtfm-gir-search-result.h \
	rtfm-gir-type.c \
	rtfm-gir-type.h \
	rtfm-gir-union.c \
	rtfm-gir-union.h \
	rtfm-gir-util.c \
	rtfm-gir-util.h \
	rtfm-gir-varargs.c \
	rtfm-gir-varargs.h \
	rtfm-gir-view-addin.c \
	rtfm-gir-view-addin.h \
	rtfm-gir-virtual-method.c \
	rtfm-gir-virtual-method.h \
	$(NULL)

librtfm_plugin_gir_la_CFLAGS = \
	$(PLUGIN_CFLAGS) \
	-I$(top_srcdir)/contrib/fuzzy-glib \
	-I$(top_builddir)/contrib/fuzzy-glib \
	$(NULL)

librtfm_plugin_gir_la_LIBADD = \
	$(top_builddir)/contrib/fuzzy-glib/libfuzzy-glib-@API_VERSION@.la \
	$(NULL)

librtfm_plugin_gir_la_LDFLAGS = $(PLUGIN_LDFLAGS)

include $(top_srcdir)/plugins/Makefile.plugins

-include $(top_srcdir)/git.mk
