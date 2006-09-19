size = 96x96
context = devices
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		audio-card.png 		\
		computer.png 		\
		media-cdrom-audio.png 		\
		media-cdrom.png 		\
		media-dvd.png 		\
		printer-networked.png 		\
		printer-new.png 		\
		printer.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
