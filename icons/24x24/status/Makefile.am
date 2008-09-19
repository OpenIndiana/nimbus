size = 24x24
context = status
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		audio-volume-high.png 		\
		audio-volume-low.png 		\
		audio-volume-medium.png 		\
		audio-volume-muted.png 		\
		audio-volume-zero.png 		\
		dialog-error.png 		\
		dialog-information.png 		\
		dialog-password.png 		\
		dialog-question.png 		\
		dialog-warning.png 		\
		gnome-netstatus-0-24.png 		\
		gnome-netstatus-25-49.png 		\
		gnome-netstatus-50-74.png 		\
		gnome-netstatus-75-100.png 		\
		gnome-netstatus-wireless-error.png 		\
		gnome-netstatus-wireless-offline.png 		\
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
