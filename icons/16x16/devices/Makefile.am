size = 16x16
context = devices
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		audio-card.png 		\
		drive-harddisk.png 		\
		drive-removable-media.png 		\
		input-keyboard.png 		\
		media-cdrom-audio.png 		\
		media-cdrom.png 		\
		media-dvd.png 		\
		media-floppy.png 		\
		media-zip.png 		\
		memory.png 		\
		printer-new.png 		\
		printer.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
