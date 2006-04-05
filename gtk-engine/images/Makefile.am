
IMAGES = \
	button-corner-active-bottom-left.png		\
	button-corner-active-bottom-right.png		\
	button-corner-active-top-left.png		\
	button-corner-active-top-right.png		\
	button-corner-disabled-bottom-left.png		\
	button-corner-disabled-bottom-right.png		\
	button-corner-disabled-top-left.png		\
	button-corner-disabled-top-right.png		\
	button-corner-normal-bottom-left.png		\
	button-corner-normal-bottom-right.png		\
	button-corner-normal-top-left.png		\
	button-corner-normal-top-right.png		\
	button-corner-prelight-bottom-left.png		\
	button-corner-prelight-bottom-right.png		\
	button-corner-prelight-top-left.png		\
	button-corner-prelight-top-right.png		\
	button-default-corner-active-bottom-left.png	\
	button-default-corner-active-bottom-right.png	\
	button-default-corner-active-top-left.png	\
	button-default-corner-active-top-right.png	\
	button-default-corner-normal-bottom-left.png	\
	button-default-corner-normal-bottom-right.png	\
	button-default-corner-normal-top-left.png	\
	button-default-corner-normal-top-right.png	\
	button-default-corner-prelight-bottom-left.png	\
	button-default-corner-prelight-bottom-right.png	\
	button-default-corner-prelight-top-left.png	\
	button-default-corner-prelight-top-right.png	\
	combo-caret-active.png				\
	combo-caret-disable.png				\
	combo-caret-normal.png				\
	combo-caret-prelight.png



noinst_DATA = images.h
CLEANFILES = $(noinst_DATA)

BUILT_SOURCES =	images.h
    
images.h: $(IMAGES) Makefile
	rm -f images.h ;\
	for tmp in $(IMAGES) ;\
	do \
	pix_name=`echo  $$tmp | sed -e "s/\.png//g" | sed -e "s/-/_/g"` ;\
	$(GDK_PIXBUF_CSOURCE) --raw --name=$$pix_name $$tmp >> images.h ;\
	done

EXTRA_DIST = $(IMAGES) images.h
