size = 48x48
context = actions
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		document-print.png 		\
		edit.png 		\
		go-home.png 		\
		network.png 		\
		preferences.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
