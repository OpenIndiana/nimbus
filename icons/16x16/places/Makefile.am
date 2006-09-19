size = 16x16
context = places
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		application-x-executable.png 		\
		bookmark-missing.png 		\
		bookmark.png 		\
		file.png 		\
		folder-drag-accept.png 		\
		folder.png 		\
		network-server.png 		\
		network-workgroup.png 		\
		share.png 		\
		user-desktop.png 		\
		user-home.png 		\
		user-trash_empty.png 		\
		user-trash_full.png 		\
		web.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
