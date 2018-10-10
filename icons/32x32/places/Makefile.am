size = 32x32
context = places
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		bookmark-missing.png 		\
		bookmark.png 		\
		file.png 		\
		folder-documents.png 		\
		folder-download.png 		\
		folder-drag-accept.png 		\
		folder-music.png 		\
		folder-pictures.png 		\
		folder-publicshare.png 		\
		folder-remote.png 		\
		folder-saved-search.png 	\
		folder-templates.png	 	\
		folder-videos.png	 	\
		folder.png 		\
		gnome-fs-ftp.png 		\
		gnome-fs-nfs.png 		\
		gnome-fs-share.png 		\
		gnome-fs-smb.png 		\
		gnome-fs-ssh.png 		\
		network-workgroup.png 		\
		start-here.png 		\
		user-trash-full.png 		\
		user-trash.png 		\
		web.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
