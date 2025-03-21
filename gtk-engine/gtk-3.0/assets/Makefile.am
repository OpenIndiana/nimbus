assetsdir = $(datadir)/themes/$(PACKAGE)/gtk-3.0/assets
darkassetsdir = $(datadir)/themes/dark-$(PACKAGE)/gtk-3.0/assets
lightassetsdir = $(datadir)/themes/light-$(PACKAGE)/gtk-3.0/assets
assets_DATA =	checkbox-unchecked-hover.png \
	checkbox-unchecked-insensitive.png \
	radio-unselected.png \
	checkbox-mixed-hover.png \
	checkbox-checked-insensitive.png \
	radio-mixed.png \
	checkbox-unchecked-active.png \
	radio-unselected-insensitive.png \
	checkbox-mixed-insensitive.png \
	radio-selected-hover.png \
	scrollbar-slider.png \
	checkbox-unchecked.png \
	radio-mixed-insensitive.png \
	radio-selected-active.png \
	radio-selected.png \
	radio-unselected-hover.png \
	checkbox-checked.png \
	radio-selected-insensitive.png \
	radio-mixed-hover.png \
	radio-mixed-symbolic.png \
	radio-unselected-active.png \
	checkbox-checked-hover.png \
	radio-mixed-active.png \
	checkbox-mixed.png \
	scrollbar-slider-horiz.png \
	checkbox-checked-active.png \
	checkbox-mixed-active.png \
	checkbox-mixed-symbolic.png

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(darkassetsdir);
	for i in $(assets_DATA); do   \
	  (cd $(DESTDIR)$(darkassetsdir) && ln -fs ../../../$(PACKAGE)/gtk-3.0/assets/$$i .)  \
	done
	$(mkinstalldirs) $(DESTDIR)$(lightassetsdir);
	for i in $(assets_DATA); do   \
	  (cd $(DESTDIR)$(lightassetsdir) && ln -fs ../../../$(PACKAGE)/gtk-3.0/assets/$$i .)  \
	done

EXTRA_DIST = $(assets_DATA)
