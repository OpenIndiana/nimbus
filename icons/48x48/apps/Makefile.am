size = 48x48
context = apps
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		accessories-archive-manager.png 		\
		accessories-calculator.png 		\
		accessories-character-map.png 		\
		accessories-dictionary.png 		\
		accessories-pda-sync.png 		\
		accessories-text-editor.png 		\
		administration-login-screen-setup.png 		\
		administration-power-management.png 		\
		administration-printers.png 		\
		administration-software-update.png 		\
		administration-time-and-date.png 		\
		administration-users-and-groups.png 		\
		administration.png 		\
		advanced-directory.png 		\
		evolution.png 		\
		favorites.png 		\
		gaim.png 		\
		gnome-main-menu.png 		\
		graphics-image-viewer.png 		\
		help-browser.png 		\
		ibus-setup.png	\
		internet-firefox.png 		\
		logo-icon-transparent.png 		\
		office-envince.png 		\
		panel-force-quit.png 		\
		preferences-desktop-display.png 		\
		preferences-desktop-font.png 		\
		preferences-desktop-keyboard-accessibility.png 		\
		preferences-desktop-keyboard-shortcuts.png 		\
		preferences-desktop-keyboard.png 		\
		preferences-desktop-menu-editor.png 		\
		preferences-desktop-menus-and-toolbars.png 		\
		preferences-desktop-mouse.png 		\
		preferences-desktop-preferred-applications.png 		\
		preferences-desktop-remote-desktop.png 		\
		preferences-desktop-screen-resolution.png 		\
		preferences-desktop-screensaver.png 		\
		preferences-desktop-theme.png 		\
		preferences-desktop-wallpaper.png 		\
		preferences-desktop-window-behavior.png 		\
		preferences-palmosdevices.png 		\
		preferences-settings-sound.png 		\
		preferences-system-network.png 		\
		preferences-system.png 		\
		programs.png 		\
		realplayer.png 		\
		run.png 		\
		searchtool.png 		\
		sound-java-multimedia-player.png 		\
		sound-recorder.png 		\
		sound-totem-movie-player.png 		\
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
		system-software-install.png 		\
		thunderbird.png 		\
		universal-access-dasher.png 		\
		universal-access-gok.png 		\
		utilities-terminal.png 		\
		xsane.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
