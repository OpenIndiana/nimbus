size = 72x72
context = apps
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		advanced-directory.png 		\
		logo-icon-transparent.png 		\
		preferences-desktop-display.png 		\
		preferences-desktop-screen-resolution.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
