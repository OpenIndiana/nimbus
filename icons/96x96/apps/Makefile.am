size = 96x96
context = apps
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		administration-login-screen-setup.png 		\
		administration-printers.png 		\
		administration-users-and-groups.png 		\
		administration.png 		\
		advanced-directory.png 		\
		gaim.png 		\
		graphics-image-viewer.png 		\
		internet-firefox.png 		\
		logo-icon-transparent.png 		\
		office-envince.png 		\
		preferences-desktop-display.png 		\
		preferences-desktop-font.png 		\
		preferences-desktop-keyboard-shortcuts.png 		\
		preferences-desktop-menus-and-toolbars.png 		\
		preferences-desktop-screen-resolution.png 		\
		preferences-desktop-screensaver.png 		\
		preferences-desktop-theme.png 		\
		preferences-desktop-wallpaper.png 		\
		preferences-desktop-window-behavior.png 		\
		preferences-settings-sound.png 		\
		preferences-system-network.png 		\
		preferences-system.png 		\
		realplayer.png 		\
		sound-totem-movie-player.png 		\
		system-lock-screen.png 		\
		system-performance-monitor.png 		\
		system-print-queue-monitor.png 		\
		thunderbird.png 		\
		universal-access-dasher.png 		\
		universal-access-gok.png 		\
		utilities-terminal.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
