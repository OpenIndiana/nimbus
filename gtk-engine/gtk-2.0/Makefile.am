themedir = $(datadir)/themes/$(PACKAGE)/gtk-2.0
darkthemedir = $(datadir)/themes/dark-$(PACKAGE)/gtk-2.0
lightthemedir = $(datadir)/themes/light-$(PACKAGE)/gtk-2.0
theme_DATA =	gtkrc		\
		iconrc		\
		dialog_authentication.png  \
		dialog_info.png  \
		dialog_warning.png  \
		dialog_error.png  \
		dialog_question.png  \
		stock_dnd.png  \
		stock_dnd_multiple.png  \
		about.png  \
		add.png  \
		apply.png  \
		format-text-bold.png  \
		cancel.png  \
		cdrom_16.png  \
		cdrom_24.png  \
		clear.png  \
		close.png  \
		colorselector.png  \
		convert.png  \
		connect.png  \
		edit-copy_16.png  \
		edit-copy_24.png  \
		edit-cut_16.png  \
		edit-cut_24.png  \
		delete_16.png  \
		delete_24.png  \
		directory.png  \
		disconnect.png  \
		edit.png  \
		execute.png  \
		file.png  \
		edit-find.png  \
		edit-find-replace.png  \
		floppy.png  \
		fullscreen.png  \
		go-bottom.png  \
		go-first.png  \
		go-last.png  \
		go-top.png  \
		go-previous.png  \
		go-down.png  \
		go-next.png  \
		go-up.png  \
		harddisk.png  \
		help.png  \
		go-home.png  \
		index.png  \
		format-indent-more.png  \
		info.png  \
		format-indent-less.png  \
		format-text-italic.png  \
		go-jump.png  \
		format-justify-center.png  \
		format-justify-fill.png  \
		format-justify-left.png  \
		format-justify-right.png  \
		leave_fullscreen.png  \
		broken_image.png  \
		media-ffwd.png  \
		media-next.png  \
		media-pause.png  \
		media-play.png  \
		media-prev.png  \
		media-record.png  \
		media-rewind.png  \
		media-stop.png  \
		network.png  \
		document-new.png  \
		no.png  \
		ok.png  \
		document-open.png  \
		edit-paste.png  \
		preferences.png  \
		document-print.png  \
		document-print-preview.png  \
		document-properties.png  \
		quit.png  \
		edit-redo.png  \
		view-refresh.png  \
		remove.png  \
		revert.png  \
		document-save.png  \
		document-save-as.png  \
		colorselector.png  \
		font.png  \
		sort_ascending.png  \
		sort_descending.png  \
		spellcheck.png  \
		stop.png  \
		format-text-strikethrough.png  \
		undelete.png  \
		format-text-underline.png  \
		edit-undo.png  \
		yes.png  \
		zoom-original.png  \
		zoom-best-fit.png  \
		zoom-in.png  \
		zoom-out.png  

EXTRA_DIST = $(theme_DATA)    \
	     light-gtkrc    \
	     dark-gtkrc	    \
	     iconrc.in	      



install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(darkthemedir);
	for i in $(theme_DATA); do   \
	  (cd $(DESTDIR)$(darkthemedir) && ln -fs ../../$(PACKAGE)/gtk-2.0/$$i .)  \
	done
	(cd $(DESTDIR)$(darkthemedir) && rm -f $(DESTDIR)$(darkthemedir)/gtkrc);
	$(INSTALL_DATA) dark-gtkrc $(DESTDIR)$(darkthemedir)/gtkrc;
	$(mkinstalldirs) $(DESTDIR)$(lightthemedir);
	for i in $(theme_DATA); do   \
	  (cd $(DESTDIR)$(lightthemedir) && ln -fs ../../$(PACKAGE)/gtk-2.0/$$i .)  \
	done
	(cd $(DESTDIR)$(lightthemedir) && rm -f $(DESTDIR)$(lightthemedir)/gtkrc);
	$(INSTALL_DATA) light-gtkrc $(DESTDIR)$(lightthemedir)/gtkrc;	
