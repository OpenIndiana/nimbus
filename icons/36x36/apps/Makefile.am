size = 36x36
context = apps
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		accessories-calculator.png 		\
		advanced-directory.png 		\
		gnome-spinner-rest.png 		\
		gnome-spinner.png 		\
		preferences-desktop-display.png 		\
		preferences-desktop-screen-resolution.png 		\
		searchtool.png 		\
		system-print-queue-monitor.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
