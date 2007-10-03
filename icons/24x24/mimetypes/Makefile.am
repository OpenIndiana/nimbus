size = 24x24
context = mimetypes
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		gnome-fs-regular.icon 		\
		gnome-mime-application-x-backup.png 		\
		gnome-mime-application-x-core.png 		\
		gnome-mime-x-directory-nfs-server.png 		\
		gnome-mime-x-directory-nfs-share.png 		\
		gnome-mime-x-directory-smb-server.png 		\
		gnome-mime-x-directory-smb-share.png 		\
		stock_generic-mimetype.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
