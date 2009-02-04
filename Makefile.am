@INTLTOOL_THEME_RULE@

AUTOMAKE_OPTIONS = dist-bzip2

SUBDIRS=gtk-engine metacity icons po

themedir = $(datadir)/themes/$(PACKAGE)/
darkthemedir = $(datadir)/themes/dark-$(PACKAGE)/
lightthemedir = $(datadir)/themes/light-$(PACKAGE)/
theme_in_files = index.theme.in \
		 dark-index.theme.in \
		 light-index.theme.in

theme_DATA = $(theme_in_files:.theme.in=.theme)

EXTRA_DIST= \
	autogen.sh					\
	index.theme					\
	dark-index.theme				\
	dark-index.theme.in				\
	light-index.theme				\
	light-index.theme.in				\
	ChangeLog					\
	$(theme_in_files)				

install-data-local:
	$(mkinstalldirs)  $(DESTDIR)$(darkthemedir);
	rm -f $(DESTDIR)$(themedir)/dark-index.theme
	$(INSTALL_DATA) dark-index.theme $(DESTDIR)$(darkthemedir)/index.theme
	$(mkinstalldirs)  $(DESTDIR)$(lightthemedir);
	rm -f $(DESTDIR)$(themedir)/light-index.theme
	$(INSTALL_DATA) light-index.theme $(DESTDIR)$(lightthemedir)/index.theme

DISTCLEANFILES=intltool-extract intltool-merge intltool-update
