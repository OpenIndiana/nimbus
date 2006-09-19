size = 20x20
context = status
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		dialog-apply.png 		\
		dialog-cancel.png 		\
		dialog-close.png 		\
		dialog-no.png 		\
		dialog-ok.png 		\
		dialog-yes.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
