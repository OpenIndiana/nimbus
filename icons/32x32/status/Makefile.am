size = 32x32
context = status
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		dialog-error.png 		\
		dialog-information.png 		\
		dialog-warning.png 		\
		network-error.png 		\
		network-idle.png 		\
		network-offline.png 		\
		network-receive.png 		\
		network-transmit-receive.png 		\
		network-transmit.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
