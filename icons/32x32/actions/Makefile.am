size = 32x32
context = actions
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		document-new.png 		\
		document-open.png 		\
		document-print.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
