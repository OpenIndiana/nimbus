/* Nimbus Theme Engine
 * Copyright (C) 2001 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Written by Owen Taylor <otaylor@redhat.com>
 * modified by Alexander Larsson <alexl@redhat.com>
 */

#include <gtk/gtkrc.h>
#include "nimbus_utils.h"

typedef struct _NimbusData	    NimbusData;
typedef struct _NimbusRcStyle	    NimbusRcStyle;
typedef struct _NimbusRcStyleClass  NimbusRcStyleClass;
typedef struct _NimbusButton	    NimbusButton;
typedef struct _NimbusTextfield	    NimbusTextfield;
typedef struct _NimbusSpinSeparator NimbusSpinSeparator;
typedef struct _NimbusScrollbar	    NimbusScrollbar;
typedef struct _NimbusPane	    NimbusPane;
typedef struct _NimbusScale	    NimbusScale;
typedef struct _NimbusProgress	    NimbusProgress;
typedef struct _NimbusTab	    NimbusTab;
typedef struct _NimbusMenu	    NimbusMenu;
typedef struct _NimbusHandlebar	    NimbusHandlebar;

extern GType nimbus_type_rc_style;

#define NIMBUS_TYPE_RC_STYLE              nimbus_type_rc_style
#define NIMBUS_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), NIMBUS_TYPE_RC_STYLE, NimbusRcStyle))
#define NIMBUS_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), NIMBUS_TYPE_RC_STYLE, NimbusRcStyleClass))
#define NIMBUS_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), NIMBUS_TYPE_RC_STYLE))
#define NIMBUS_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), NIMBUS_TYPE_RC_STYLE))
#define NIMBUS_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), NIMBUS_TYPE_RC_STYLE, NimbusRcStyleClass))

struct _NimbusButton
{
  GdkPixbuf *corner_top_left;
  GdkPixbuf *corner_top_right;
  GdkPixbuf *corner_bottom_left;
  GdkPixbuf *corner_bottom_right;
  GSList    *gradients;
};

struct _NimbusTextfield 
{
  GdkColor *gradient_line1;
  GdkColor *gradient_line2;
  GdkColor *gradient_line3;
  GdkColor *vertical_line_gradient1;
  GdkColor *vertical_line_gradient2;
  GdkColor *bottom_line;
  GdkColor *vertical_line;
};

struct _NimbusSpinSeparator
{
  GdkColor *top;
  GdkColor *bottom;
};

struct _NimbusScrollbar
{
  GdkPixbuf *button_start;
  GdkPixbuf *button_end;
  GdkPixbuf *bkg;
  GdkPixbuf *slider_start;
  GdkPixbuf *slider_mid;
  GdkPixbuf *slider_end;
};

struct _NimbusPane
{
  GdkPixbuf*		pane_h;
  GdkPixbuf*		pane_v;
  GdkColor*		innerline;
  GdkColor*		outline;

};
  
struct _NimbusScale
{
  GdkPixbuf *button;
  GdkPixbuf *bkg_start;
  GdkPixbuf *bkg_mid;
  GdkPixbuf *bkg_end;
};

struct _NimbusProgress
{
  NimbusButton*	bar;
  NimbusButton*	bkg;
  GdkPixbuf*	corner_top_left;
  GdkPixbuf*	corner_top_right;
  GdkPixbuf*	corner_bottom_left;
  GdkPixbuf*	corner_bottom_right;  
  GdkPixbuf*	border_top;
  GdkPixbuf*	border_right;
  GdkPixbuf*	border_left;
  GdkPixbuf*	border_bottom;
};

struct _NimbusTab
{
  GdkColor*	start;
  GdkColor*	mid;
  GdkColor*	end;
  GdkColor*	junction;
  NimbusButton  *tab;
};

struct _NimbusMenu
{
  GdkColor*	border;
  GdkColor*	shadow;
  /* mini gradient */
  GdkColor*	start;
  GdkColor*	mid_start;
  GdkColor*	mid_end;
  GdkColor*	end;
};

struct _NimbusHandlebar
{
  GdkPixbuf*		top;
  GdkPixbuf*		mid;
  GdkPixbuf*		bottom;
};

struct _NimbusData
{
  NimbusButton*		button[5]; 
  NimbusButton*		dark_button[5]; 
  NimbusButton*		button_default[5]; 
  NimbusButton*		header_button[5]; 
  NimbusButton*		arrow_button[5]; 
  NimbusButton*		combo_entry_button[5]; 
  GdkPixbuf*		drop_shadow[5];
  GdkPixbuf*		dark_drop_shadow[5];
  GdkPixbuf*		combo_arrow[5];
  NimbusTextfield*	textfield_color[5];
  NimbusSpinSeparator*	spin_color[5];
  GdkPixbuf*		arrow_up[5];
  GdkPixbuf*		arrow_down[5];
  GdkPixbuf*		check_not_set[5];
  GdkPixbuf*		check_set[5];
  GdkPixbuf*		check_inconsistent[5];
  GdkPixbuf*		check_menu_set[5];
  GdkPixbuf*		radio_not_set[5];
  GdkPixbuf*		radio_set[5];
  GdkPixbuf*		radio_inconsistent[5];
  GdkPixbuf*		radio_menu_set[5];
  NimbusScrollbar*	scroll_h[5];
  NimbusScrollbar*	scroll_v[5];
  NimbusPane*		pane;
  NimbusPane*		dark_pane;
  NimbusScale*		scale_h[5];
  NimbusScale*		scale_v[5];
  NimbusProgress*	progress;
  NimbusTab*		tab[5];
  GdkColor*		menubar_border;
  GdkColor*		dark_menubar_border;
  NimbusGradient*	menubar;
  NimbusGradient*	dark_menubar;
  NimbusMenu*		menu;
  NimbusMenu*		dark_menu;
  GdkColor*		hline;
  GdkColor*		dark_hline;
  GdkColor*		vline;
  GdkColor*		dark_vline;
  NimbusHandlebar*	handlebar[2];
};

void nimbus_init_button_drop_shadow (NimbusRcStyle* nimbus_rc,
				     NimbusData* rc, 
				     GtkStateType state, 
				     int size);

void nimbus_init_scrollbar (NimbusData* rc,
			    GtkStateType state, 
			    int size,
			    gboolean horizontal);

void nimbus_init_scale (NimbusData* rc,
			GtkStateType state, 
			int size,
			gboolean horizontal);

void nimbus_init_progress (NimbusData* rc,
			   int height,
			   int width);

void nimbus_init_handle_bar (NimbusData* rc,
			     int size,
			     GtkOrientation orientation);
	
struct _NimbusRcStyle
{
  GtkRcStyle parent_instance; /* must be first */

  NimbusData  *data;
  gboolean     dark;

};


struct _NimbusRcStyleClass
{
  GtkRcStyleClass parent_class;
};

void nimbus_rc_style_register_type (GTypeModule *module);
