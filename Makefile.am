@INTLTOOL_THEME_RULE@

AUTOMAKE_OPTIONS = dist-bzip2

SUBDIRS=metacity po

themedir = $(datadir)/themes/$(PACKAGE)/
theme_in_files = index.theme.in
theme_DATA = $(theme_in_files:.theme.in=.theme)

EXTRA_DIST= \
	autogen.sh					\
	index.theme					\
	ChangeLog					\
	$(theme_in_files)				

DISTCLEANFILES=intltool-extract intltool-merge intltool-update
