size = 16x16
context = status
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		audio-volume-high.png 		\
		audio-volume-low.png 		\
		audio-volume-medium.png 		\
		audio-volume-muted.png 		\
		audio-volume-zero.png 		\
		dialog-close.png 		\
		dialog-error.png 		\
		dialog-information.png 		\
		dialog-warning.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
