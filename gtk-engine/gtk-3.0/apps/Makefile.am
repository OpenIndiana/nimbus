appsdir = $(datadir)/themes/$(PACKAGE)/gtk-3.0/apps
darkappsdir = $(datadir)/themes/dark-$(PACKAGE)/gtk-3.0/apps
lightappsdir = $(datadir)/themes/light-$(PACKAGE)/gtk-3.0/apps
apps_DATA =	budgie.css \
		unity.css \
		desktop.css \
		synaptic.css \
		libreoffice.css \
		mate.css \
		gnome-flashback.css \
		gnome-terminal.css

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(darkappsdir);
	for i in $(apps_DATA); do   \
	  (cd $(DESTDIR)$(darkappsdir) && ln -fs ../../../$(PACKAGE)/gtk-3.0/apps/$$i .)  \
	done
	$(mkinstalldirs) $(DESTDIR)$(lightappsdir);
	for i in $(apps_DATA); do   \
	  (cd $(DESTDIR)$(lightappsdir) && ln -fs ../../../$(PACKAGE)/gtk-3.0/apps/$$i .)  \
	done

EXTRA_DIST = $(apps_DATA)
