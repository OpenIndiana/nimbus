size = 192x192
context = places
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		application-x-executable.png 		\
		blockdev.png 		\
		bookmark-missing.png 		\
		bookmark.png 		\
		file.png 		\
		folder-drag-accept.png 		\
		folder.png 		\
		network-workgroup.png 		\
		user-desktop.png 		\
		user-trash-full.png 		\
		user-trash.png 		\
		web.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
