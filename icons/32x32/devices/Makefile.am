size = 32x32
context = devices
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		audio-card.png 		\
		input-keyboard.png 		\
		network-wired.png 		\
		network-wireless.png 		\
		printer-networked.png 		\
		printer-new.png 		\
		printer.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
