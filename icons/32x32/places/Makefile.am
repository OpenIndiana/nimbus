size = 32x32
context = places
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		bookmark-missing.png 		\
		bookmark.png 		\
		file.png 		\
		folder-drag-accept.png 		\
		folder.png 		\
		ftp.png 		\
		network-workgroup.png 		\
		nfs.png 		\
		share.png 		\
		smb.png 		\
		ssh.png 		\
		start-here.png 		\
		user-trash-full.png 		\
		user-trash.png 		\
		web.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
