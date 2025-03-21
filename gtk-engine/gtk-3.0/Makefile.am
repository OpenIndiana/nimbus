SUBDIRS = apps assets

themedir = $(datadir)/themes/$(PACKAGE)/gtk-3.0
darkthemedir = $(datadir)/themes/dark-$(PACKAGE)/gtk-3.0
lightthemedir = $(datadir)/themes/light-$(PACKAGE)/gtk-3.0
theme_DATA =	gtk-widgets-assets.css  \
		gtk-widgets.css  \
		gtk.css \
		settings.ini \
		thumbnail.png

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(darkthemedir);
	for i in $(theme_DATA); do   \
	  (cd $(DESTDIR)$(darkthemedir) && ln -fs ../../$(PACKAGE)/gtk-3.0/$$i .)  \
	done
	$(mkinstalldirs) $(DESTDIR)$(lightthemedir);
	for i in $(theme_DATA); do   \
	  (cd $(DESTDIR)$(lightthemedir) && ln -fs ../../$(PACKAGE)/gtk-3.0/$$i .)  \
	done

EXTRA_DIST = $(theme_DATA)
