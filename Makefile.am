@INTLTOOL_THEME_RULE@

AUTOMAKE_OPTIONS = dist-bzip2

SUBDIRS=gtk-engine metacity icons po

themedir = $(datadir)/themes/$(PACKAGE)/
darkthemedir = $(datadir)/themes/dark-$(PACKAGE)/
theme_in_files = index.theme.in \
		 dark-index.theme.in

theme_DATA = $(theme_in_files:.theme.in=.theme)

EXTRA_DIST= \
	autogen.sh					\
	index.theme					\
	dark-index.theme				\
	ChangeLog					\
	$(theme_in_files)				

install-data-local:
	$(mkinstalldirs)  $(DESTDIR)$(darkthemedir);
	rm -f $(DESTDIR)$(themedir)/dark-index.theme
	$(INSTALL_DATA) dark-index.theme $(DESTDIR)$(darkthemedir)/index.theme

DISTCLEANFILES=intltool-extract intltool-merge intltool-update
