SUBDIRS = data contrib src plugins tests po

EXTRA_DIST = AUTHORS

AUTHORS:
	$(AM_V_GEN)if test -d "$(srcdir)/.git"; \
	then \
		echo Creating $@ && \
		( cd "$(top_srcdir)" && \
		  echo '# Generated by Makefile. Do not edit.'; echo; \
		  git log --no-merges --pretty=format:"%an" $(SUBDIRS) \
		  | sort | uniq ) > $@.tmp \
		&& mv -f $@.tmp $@ \
		|| ( rm -f $@.tmp ; \
		     echo Failed to generate $@ >&2 ); \
	else touch $@; fi

# Generate the ChangeLog.
@GENERATE_CHANGELOG_RULES@
dist-hook: dist-ChangeLog

GITIGNOREFILES = \
	**/*.swp \
	*.o \
	aclocal.m4 \
	build-aux \
	ChangeLog \
	config \
	config.h.in \
	gtk-doc.m4 \
	gtk-doc.make \
	INSTALL \
	$(NULL)

run:
	PEAS_DEBUG=1 \
	RTFM_IN_TREE_PLUGINS=1 \
	GI_TYPELIB_PATH=src:$(GI_TYPELIB_PATH) \
	GOBJECT_DEBUG=instance-count \
	PYTHONDONTWRITEBYTECODE=yes \
	PATH=$(top_builddir)/src:${PATH} \
	$(LIBTOOL) --mode=execute gdb -ex run --args src/rtfm

search:
	PEAS_DEBUG=1 \
	RTFM_IN_TREE_PLUGINS=1 \
	GI_TYPELIB_PATH=src:$(GI_TYPELIB_PATH) \
	GOBJECT_DEBUG=instance-count \
	PYTHONDONTWRITEBYTECODE=yes \
	PATH=$(top_builddir)/src:${PATH} \
	$(LIBTOOL) --mode=execute gdb -ex run --args src/rtfm -k gtkwidget

-include $(top_srcdir)/git.mk
