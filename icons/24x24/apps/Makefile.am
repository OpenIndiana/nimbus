size = 24x24
context = apps
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		accessories-dictionary.png 		\
		accessories-text-editor.png 		\
		administration-login-screen-setup.png 		\
		administration-network-configuration.png 		\
		administration-power-management.png 		\
		administration-printers.png 		\
		administration-time-and-date.png 		\
		administration-users-and-groups.png 		\
		advanced-directory.png 		\
		evolution.png 		\
		favorites.png 		\
		gaim.png 		\
		graphics-image-viewer.png 		\
		internet-firefox.png 		\
		logo-icon-transparent.png 		\
		office-envince.png 		\
		panel-force-quit.png 		\
		preferences-desktop-display.png 		\
		preferences-desktop-font.png 		\
		preferences-desktop-menu-editor.png 		\
		preferences-desktop-menus-and-toolbars.png 		\
		preferences-desktop-mouse.png 		\
		preferences-desktop-preferred-applications.png 		\
		preferences-desktop-remote-desktop.png 		\
		preferences-desktop-screen-resolution.png 		\
		preferences-desktop-screensaver.png 		\
		preferences-desktop-theme.png 		\
		preferences-desktop-wallpaper.png 		\
		preferences-palmosdevices.png 		\
		preferences-settings-sound.png 		\
		preferences-system.png 		\
		process-working.png 		\
		programs.png 		\
		searchtool.png 		\
		sound-java-multimedia-player.png 		\
		sound-totem-movie-player.png 		\
		system-about.png 		\
		system-lock-screen.png 		\
		system-log-out.png 		\
		system-performance-monitor.png 		\
		system-print-queue-monitor.png 		\
		system-reboot.png 		\
		system-shutdown.png 		\
		thunderbird.png 		\
		utilities-terminal.png 		\
		xsane.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
