size = 32x32
context = categories
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		applications-accessories.png 		\
		applications-developer-tools.png 		\
		applications-internet.png 		\
		applications-system.png 		\
		applications-universal-access.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
