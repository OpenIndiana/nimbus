THEME_DIR=$(datadir)/themes

THEME_SUBDIR=metacity-1

THEME_NAME=$(PACKAGE)

DARK_THEME_NAME=dark-$(PACKAGE)
LIGHT_THEME_NAME=light-$(PACKAGE)

THEME_IMAGES =  button-close-icon.png \
		button-close-icon-prelight.png \
		button-close-icon-pressed.png \
		button-close-icon-unfocused.png \
		button-max-icon.png \
		button-max-icon-prelight.png \
		button-max-icon-pressed.png \
		button-max-icon-unfocused.png \
		button-menu-icon.png \
		button-menu-icon-prelight.png \
		button-menu-icon-pressed.png \
		button-menu-icon-unfocused.png \
		button-min-icon.png \
		button-min-icon-prelight.png \
		button-min-icon-pressed.png \
		button-min-icon-unfocused.png \
		button-unmax-icon.png \
		button-unmax-icon-prelight.png \
		button-unmax-icon-pressed.png \
		button-unmax-icon-unfocused.png	

EXTRA_DIST =$(THEME_IMAGES)     \
	    metacity-theme-1.xml  \
	    light-metacity-theme-1.xml \
	    dark-metacity-theme-1.xml  
		
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(THEME_DIR);					     
		$(mkinstalldirs) $(DESTDIR)$(THEME_DIR)/$(DARK_THEME_NAME)/$(THEME_SUBDIR);	     
		$(mkinstalldirs) $(DESTDIR)$(THEME_DIR)/$(LIGHT_THEME_NAME)/$(THEME_SUBDIR);	     
		$(mkinstalldirs) $(DESTDIR)$(THEME_DIR)/$(THEME_NAME)/$(THEME_SUBDIR);	     

		$(INSTALL_DATA) metacity-theme-1.xml $(DESTDIR)$(THEME_DIR)/$(THEME_NAME)/$(THEME_SUBDIR); 
		$(INSTALL_DATA) dark-metacity-theme-1.xml $(DESTDIR)$(THEME_DIR)/$(DARK_THEME_NAME)/$(THEME_SUBDIR)/metacity-theme-1.xml; 
		$(INSTALL_DATA) light-metacity-theme-1.xml $(DESTDIR)$(THEME_DIR)/$(LIGHT_THEME_NAME)/$(THEME_SUBDIR)/metacity-theme-1.xml; 
		 for i in $(THEME_IMAGES); do						     \
		    	$(INSTALL_DATA)  $$i $(DESTDIR)$(THEME_DIR)/$(THEME_NAME)/$(THEME_SUBDIR) ;\
			(cd $(DESTDIR)$(THEME_DIR)/$(DARK_THEME_NAME)/$(THEME_SUBDIR) && ln -fs ../../$(THEME_NAME)/$(THEME_SUBDIR)/$$i .) ;\
			(cd $(DESTDIR)$(THEME_DIR)/$(LIGHT_THEME_NAME)/$(THEME_SUBDIR) && ln -fs ../../$(THEME_NAME)/$(THEME_SUBDIR)/$$i .) \
		 done
uninstall-local:
	rm -Rf $(DESTDIR)$(THEME_DIR)/$(THEME_NAME)/$(THEME_SUBDIR)
	rm -Rf $(DESTDIR)$(THEME_DIR)/$(DARK_THEME_NAME)/$(THEME_SUBDIR)
	rm -Rf $(DESTDIR)$(THEME_DIR)/$(LIGHT_THEME_NAME)/$(THEME_SUBDIR)
