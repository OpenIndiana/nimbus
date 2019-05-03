size = 16x16
context = apps
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		accessories-archive-manager.png 		\
		accessories-character-map.png 		\
		administration-printers.png 		\
		administration-time-and-date.png 		\
		administration-users-and-groups.png 		\
		favorites.png 		\
		help-browser.png	\
		ibus-setup.png	\
		internet-firefox.png 		\
		logo-icon-transparent.png 		\
		panel-force-quit.png 		\
		preferences-desktop-default-applications.png \
		preferences-desktop-display.png 		\
		preferences-desktop-font.png 		\
		preferences-desktop-keyboard-accessibility.png 		\
		preferences-desktop-keyboard.png 		\
		preferences-desktop-remote-desktop.png 		\
		preferences-desktop-screen-resolution.png 		\
		preferences-settings-sound.png 		\
		preferences-system-network.png 		\
		preferences-system.png 		\
		realplayer.png 		\
		searchtool.png 		\
		system-print-queue-monitor.png 		\
		system-software-install.png 		\
		thunderbird.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
