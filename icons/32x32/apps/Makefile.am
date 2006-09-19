size = 32x32
context = apps
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		accessories-character-map.png 		\
		accessories-dictionary.png 		\
		accessories-text-editor.png 		\
		administration-login-screen-setup.png 		\
		administration-network-configuration.png 		\
		administration-printers.png 		\
		administration-users-and-groups.png 		\
		administration.png 		\
		advanced-directory.png 		\
		evolution.png 		\
		favorites.png 		\
		graphics-image-viewer.png 		\
		help-browser.png 		\
		internet-firefox.png 		\
		logo-icon-transparent.png 		\
		office-envince.png 		\
		preferences-desktop-font.png 		\
		preferences-desktop-keyboard-accessibility.png 		\
		preferences-desktop-keyboard.png 		\
		preferences-desktop-menu-editor.png 		\
		preferences-desktop-menus-and-toolbars.png 		\
		preferences-desktop-preferred-applications.png 		\
		preferences-desktop-screensaver.png 		\
		preferences-desktop-wallpaper.png 		\
		preferences-desktop-window-behavior.png 		\
		preferences-settings-sound.png 		\
		programs.png 		\
		realplayer.png 		\
		sound-java-multimedia-player.png 		\
		sound-recorder.png 		\
		sun_java.png 		\
		system-about.png 		\
		system-file-manager.png 		\
		system-lock-screen.png 		\
		system-log-file-viewer.png 		\
		system-log-out.png 		\
		system-new-login-in-a-window.png 		\
		system-performance-monitor.png 		\
		system-print-queue-monitor.png 		\
		system-reboot.png 		\
		system-shutdown.png 		\
		thunderbird.png 		\
		utilities-terminal.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
