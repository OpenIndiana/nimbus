size = 192x192
context = devices
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		audio-card.png 		\
		battery.png 		\
		computer.png 		\
		drive-removable-media.png 		\
		input-keyboard.png 		\
		input-mouse.png 		\
		media-cdrom-audio.png 		\
		media-cdrom.png 		\
		media-dvd.png 		\
		memory.png 		\
		modem.png 		\
		mouse-ball.png 		\
		scanner.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
