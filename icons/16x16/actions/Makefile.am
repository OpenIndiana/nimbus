size = 16x16
context = actions
iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = 				 		\
		application-exit.png 		\
		broken-image.png 		\
		connect.png 		\
		convert.png 		\
		disconnect.png 		\
		document-new.png 		\
		document-open.png 		\
		document-print-preview.png 		\
		document-print.png 		\
		document-properties.png 		\
		document-save-as.png 		\
		document-save.png 		\
		edit-copy.png 		\
		edit-cut.png 		\
		edit-delete.png 		\
		edit-find-replace.png 		\
		edit-find.png 		\
		edit-font.png 		\
		edit-paste.png 		\
		edit-redo.png 		\
		edit-revert.png 		\
		edit-undelete.png 		\
		edit-undo.png 		\
		edit.png 		\
		execute.png 		\
		format-indent-less.png 		\
		format-indent-more.png 		\
		format-justify-center.png 		\
		format-justify-fill.png 		\
		format-justify-left.png 		\
		format-justify-right.png 		\
		format-text-bold.png 		\
		format-text-italic.png 		\
		format-text-strikethrough.png 		\
		format-text-underline.png 		\
		go-bottom.png 		\
		go-down.png 		\
		go-first.png 		\
		go-home.png 		\
		go-jump.png 		\
		go-last.png 		\
		go-next.png 		\
		go-previous.png 		\
		go-top.png 		\
		go-up.png 		\
		help-about.png 		\
		help-contents.png 		\
		index.png 		\
		info.png 		\
		list-add.png 		\
		list-remove.png 		\
		network.png 		\
		pan-down-symbolic.png	\
		pan-end-symbolic-rtl.png	\
		pan-end-symbolic.png	\
		pan-start-symbolic.png	\
		pan-up-symbolic.png	\
		preferences.png 		\
		process-stop.png 		\
		quit.png 		\
		tools-check-spelling.png 		\
		view-fullscreen.png 		\
		view-leave-fullscreen.png 		\
		view-refresh.png 		\
		view-sort-ascending.png 		\
		view-sort-descending.png 		\
		zoom-in.png 		\
		zoom-original.png 		\
		zoom-out.png
EXTRA_DIST = $(icons_DATA)

install-data-local: install-iconsDATA
	(cd $(DESTDIR)$(themedir)/$(size) && $(ICONMAP) -c $(context) -e $(iconsrcdir)/icons/my-mapping.xml)

uninstall-local:
	rm -Rf $(DESTDIR)$(themedir)/$(size)/$(context)
