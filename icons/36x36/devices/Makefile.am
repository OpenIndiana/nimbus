size = 36x36
context = devices
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		printer-new.png 		\
		printer.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
