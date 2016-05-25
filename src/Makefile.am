lib_LTLIBRARIES = librtfm-@API_VERSION@.la
bin_PROGRAMS = rtfm

librtfm_public_api = \
	rtfm.h \
	rtfm-collection.c \
	rtfm-collection.h \
	rtfm-item.c \
	rtfm-item.h \
	rtfm-library.c \
	rtfm-library.h \
	rtfm-path.c \
	rtfm-path.h \
	rtfm-path-bar.c \
	rtfm-path-bar.h \
	rtfm-path-element.c \
	rtfm-path-element.h \
	rtfm-provider.c \
	rtfm-provider.h \
	rtfm-search-settings.c \
	rtfm-search-settings.h \
	rtfm-sidebar.c \
	rtfm-sidebar.h \
	rtfm-sidebar-row.c \
	rtfm-sidebar-row.h \
	rtfm-stack-list.c \
	rtfm-stack-list.h \
	rtfm-types.h \
	rtfm-version.h \
	rtfm-view.c \
	rtfm-view.h \
	rtfm-view-addin.c \
	rtfm-view-addin.h \
	rtfm-view-group.c \
	rtfm-view-group.h \
	rtfm-window.c \
	rtfm-window.h \
	$(NULL)

librtfm_@API_VERSION@_la_SOURCES = \
	$(librtfm_public_api) \
	rtfm-animation.c \
	rtfm-animation.h \
	rtfm-frame-source.c \
	rtfm-frame-source.h \
	rtfm-item-private.h \
	rtfm-library-private.h \
	rtfm-path-bar-label.c \
	rtfm-path-bar-label.h \
	rtfm-rect.c \
	rtfm-rect.h \
	rtfm-widget.c \
	rtfm-widget.h \
	$(NULL)

nodist_librtfm_@API_VERSION@_la_SOURCES = \
	rtfm-resources.c \
	rtfm-resources.h \
	$(NULL)

librtfm_@API_VERSION@_la_CFLAGS = \
	$(RTFM_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)

librtfm_@API_VERSION@_la_LIBADD = \
	$(RTFM_LIBS) \
	$(NULL)

librtfm_@API_VERSION@_la_LDFLAGS = \
	$(WARN_LDFLAGS) \
	-no-undefined \
	-export-symbols-regex '^rtfm_.*' \
	$(NULL)

rtfm_SOURCES = \
	main.c \
	$(NULL)
rtfm_LDADD = \
	$(RTFM_LIBS) \
	librtfm-@API_VERSION@.la \
	$(NULL)
rtfm_LDFLAGS = $(WARN_LDFLAGS)
rtfm_CFLAGS = \
	$(RTFM_CFLAGS) \
	$(WARN_LDFLAGS) \
	$(NULL)

EXTRA_DIST =
CLEANFILES =
DISTCLEANFILES =
BUILT_SOURCES =

glib_resources_xml = resources/rtfm.gresource.xml
glib_resources_c = rtfm-resources.c
glib_resources_h = rtfm-resources.h
glib_resources_namespace = rtfm
include $(top_srcdir)/gresources.mk

if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all

Rtfm-@API_VERSION@.0.gir: librtfm-@API_VERSION@.la
Rtfm_@API_VERSION@_0_gir_INCLUDES = Gio-2.0 Gtk-3.0 Peas-1.0
Rtfm_@API_VERSION@_0_gir_CFLAGS = $(RTFM_CFLAGS)
Rtfm_@API_VERSION@_0_gir_LIBS = librtfm-@API_VERSION@.la
Rtfm_@API_VERSION@_0_gir_FILES = $(librtfm_public_api)
Rtfm_@API_VERSION@_0_gir_SCANNERFLAGS = --c-include="rtfm.h" -n Rtfm
INTROSPECTION_GIRS += Rtfm-@API_VERSION@.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(pkglibdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

-include $(top_srcdir)/git.mk
