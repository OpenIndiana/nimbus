#include <gtk/gtk.h>
#include <gtk/gtkenums.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "nimbus_style.h"
#include "nimbus_rc_style.h"
#include "nimbus_utils.h"

static char *state_names[5] = {
    "Normal",
    "Active",
    "Prelight",
    "Selected",
    "Insensitive"};

static char *shadow_names[5] = {
  "GTK_SHADOW_NONE",
  "GTK_SHADOW_IN",
  "GTK_SHADOW_OUT",
  "GTK_SHADOW_ETCHED_IN",
  "GTK_SHADOW_ETCHED_OUT"};

typedef enum
{
  NIMBUS_SPIN_NONE,
  NIMBUS_SPIN_UP,
  NIMBUS_SPIN_DOWN
} NimbusSpinButton;

static void
verbose (const char *format, ...)
{
  va_list args;
  gchar *str;
  static int debug = -1;

  if (format == NULL)
    return;
  
  if (debug < 0)
    {
      if (getenv ("NIMBUS_DEBUG"))
	debug = 1;
      else
	debug = 0;
    }

  if (!debug)
    return;
  
  va_start (args, format);
  str = g_strdup_vprintf (format, args);
  va_end (args);
  
  fputs (str, stderr);

  fflush (stderr);
  
  g_free (str);
}

static int scroll_button_x;
static int scroll_button_y;
static int scroll_button_width;
static int scroll_button_height;
static int scroll_trough_x;
static int scroll_trough_y;
static int scroll_trough_width;
static int scroll_trough_height;
static GtkStateType scroll_slider_state;


#define DETAIL(xx)   ((detail) && (!strcmp(xx, detail)))

static void draw_box               (GtkStyle       *style,
				    GdkWindow      *window,
				    GtkStateType    state_type,
				    GtkShadowType   shadow_type,
				    GdkRectangle   *area,
				    GtkWidget      *widget,
				    const gchar    *detail,
				    gint            x,
				    gint            y,
				    gint            width,
				    gint            height);

static void draw_nimbus_box	   (GtkStyle      *style,
				    GdkWindow     *window,
				    GtkStateType   state_type,
				    GtkShadowType  shadow_type,
				    GdkRectangle  *area,
				    GtkWidget     *widget,
				    const gchar   *detail,
				    NimbusButton  *button,
				    gboolean	   drop_shadow,
				    gint           x,
				    gint           y,
				    gint           width,
				    gint           height,
				    NimbusSpinButton spin_type,
				    GtkOrientation orientation);

static GtkStyleClass *parent_class;

static gboolean check_sane_pixbuf_value (int src_x, int src_y, int width, int height, GdkPixbuf *pixbuf)
{
  /*printf ("checking src_x = %d, int src_y = %d, int width = %d, int height = %d , pixbuf->height = %d, pixbuf->width = %d\n",
	  src_x, src_y, width, height, gdk_pixbuf_get_width (pixbuf), gdk_pixbuf_get_height (pixbuf));*/

  if (!(width >= 0 && height >= 0))
    {
      /* printf (" (%d >= 0 && %d >= 0) is false\n", width, height); */
      return FALSE;
    }
  if (!(src_x >= 0 && src_x + width <= gdk_pixbuf_get_width (pixbuf)))
    {
      /* printf (" (%d >= 0 && %d + %d <= %d) is false\n", src_x , src_x,  width , gdk_pixbuf_get_width (pixbuf)); */
      return FALSE;
    }
  if (!(src_y >= 0 && src_y + height <= gdk_pixbuf_get_height (pixbuf)))
    {
      /* printf (" (%d >= 0 && %d + %d <= %d) is false\n", src_y, src_y,  height, gdk_pixbuf_get_height (pixbuf)); */
      return FALSE;
    }
  return TRUE;
}
static GdkGC *
get_clipping_gc (GdkWindow* window, GdkRectangle *clip)
{
  GdkGC *gc;
  static GSList *clipping_gc_list = NULL;
  GSList* tmp = clipping_gc_list;
  
  while (tmp)
    {
      gc = (GdkGC *) tmp->data;
      if (gdk_gc_get_screen (gc) == 
	  gdk_drawable_get_screen (GDK_DRAWABLE (window)))
	{
	  gdk_gc_set_clip_rectangle (gc, clip);
	  return gc;
	}
      tmp = tmp->next;
    }
  gc =  gdk_gc_new (window);
  gdk_gc_set_clip_rectangle (gc, clip);
  clipping_gc_list = g_slist_append (clipping_gc_list, gc);
  return gc;
}

static GtkWidget *print_ancestors (GtkWidget *widget)
{
  GtkWidget *tmp;

  tmp = widget;
  printf ("widget type %s\n", g_type_name (G_OBJECT_TYPE (widget)));

  tmp = tmp->parent;
  
  while (tmp)
    {
      printf ("parent type %s\n", g_type_name (G_OBJECT_TYPE (tmp)));
      tmp = tmp->parent;
    }
  printf ("\n");
}

static GtkWidget *get_ancestor_of_type (GtkWidget *widget,
				 gchar	 *ancestor_type_s)
{
  GtkWidget *tmp;
  GType ancestor_type = G_TYPE_INVALID;

  tmp = widget;
  ancestor_type = g_type_from_name (ancestor_type_s);
  
  while (tmp)
    {
      if (G_OBJECT_TYPE (tmp) == ancestor_type)
	  return tmp;
      tmp = tmp->parent;
    }
  return NULL;
}

static gboolean 
sanitize_size (GdkWindow      *window,
	       gint           *width,
	       gint           *height)
{
  gboolean set_bg = FALSE;

  if ((*width == -1) && (*height == -1))
    {
      set_bg = GDK_IS_WINDOW (window);
      gdk_window_get_size (window, width, height);
    }
  else if (*width == -1)
    gdk_window_get_size (window, width, NULL);
  else if (*height == -1)
    gdk_window_get_size (window, NULL, height);

  return set_bg;
}
static char *arrow_names[4] = {
  "GTK_ARROW_UP",
  "GTK_ARROW_DOWN",
  "GTK_ARROW_LEFT",
  "GTK_ARROW_RIGHT"
};

static void
draw_arrow (GtkStyle      *style,
            GdkWindow     *window,
            GtkStateType   state_type,
            GtkShadowType  shadow_type,
            GdkRectangle  *area,
            GtkWidget     *widget,
            const gchar   *detail,
            GtkArrowType   arrow_type,
            gboolean       fill,
            gint           x,
            gint           y,
            gint           width,
            gint           height)
{
  gint i, j;
  NimbusData *rc; 
  
  g_return_if_fail (GTK_IS_STYLE (style));

  rc = NIMBUS_RC_STYLE (style->rc_style)->data;
      
  if (DETAIL ("hscrollbar") || DETAIL ("vscrollbar"))
    {
      int offset_x = 0; int offset_y = 0;
      int offset_width = 0;  int offset_height = 0;
      gboolean firefox_hack = FALSE; /* use to avoid drawing start part of the slider for UP and LEFT 
				      * as it would result in a darker slider */
      GdkPixbuf *tmp_pb = NULL;
      GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (widget));

      if (state_type == GTK_STATE_INSENSITIVE)
	return;
      
      if (arrow_type == GTK_ARROW_DOWN)
	{
	  tmp_pb = rc->scroll_v[state_type]->button_end;
	  offset_height += gdk_pixbuf_get_height (tmp_pb) - scroll_button_height;
	  scroll_button_y = (scroll_trough_y + scroll_trough_height) - gdk_pixbuf_get_height (tmp_pb); 
	}
      if (arrow_type == GTK_ARROW_UP)
	{
	  int tmp_y = scroll_button_y;
	  tmp_pb = rc->scroll_v[state_type]->button_start;
	  offset_height += gdk_pixbuf_get_height (tmp_pb) - scroll_button_height; 
	  scroll_button_y = scroll_trough_y;
	  if (tmp_y != scroll_button_y)
	      firefox_hack = TRUE; 
	}
      if (arrow_type == GTK_ARROW_LEFT)
	{
	  int tmp_x = scroll_button_x;
	  tmp_pb = rc->scroll_h[state_type]->button_start;
	  offset_width += gdk_pixbuf_get_width (tmp_pb) - scroll_button_width;
	  scroll_button_x = scroll_trough_x;
	  if (tmp_x != scroll_button_x)
	      firefox_hack = TRUE;
	}
      if (arrow_type == GTK_ARROW_RIGHT)
	{
	  tmp_pb = rc->scroll_h[state_type]->button_end;
	  offset_width += gdk_pixbuf_get_width (tmp_pb) - scroll_button_width;
	  scroll_button_x = (scroll_trough_x + scroll_trough_width) - gdk_pixbuf_get_width (tmp_pb);
	}

      if (tmp_pb && check_sane_pixbuf_value (0,0, scroll_button_width + offset_width, scroll_button_height + offset_height, tmp_pb))
 	gdk_draw_pixbuf (window,
			 NULL, /* don't clip as we're drawing outside the boundary one purpose */
			 tmp_pb,
			 0,0,
			 scroll_button_x +  offset_x,
			 scroll_button_y + offset_y,
			 scroll_button_width + offset_width,
			 scroll_button_height + offset_height,
			 GDK_RGB_DITHER_NONE,0,0);


      /* redraw the slider after the button to achieve the proper effect
       * this is needed as the slider is drawn before the button. */

      tmp_pb = NULL;
      /* height  width offset needed for the translucent button border as end up 
       * darker as 2 translucent colors are added */
      offset_height = 0; 
      offset_width = 0;   
      
      if (adj->lower == adj->value)
	{ /* slider and start button merged  */
	  if (arrow_type == GTK_ARROW_UP)
	    {
	      tmp_pb = rc->scroll_v[scroll_slider_state]->slider_start;
	      offset_x += 1;
	      offset_y += 17;
	      offset_width = -2;
	    }
	  if (arrow_type == GTK_ARROW_LEFT)
	    {
	      tmp_pb = rc->scroll_h[scroll_slider_state]->slider_start;
	      offset_y += 1;
	      offset_x += 17;
	      offset_height = -2;
	    }
	}
      if (adj->value + adj->page_size == adj->upper)
	{ /* slider and end button merged */
	  if (arrow_type == GTK_ARROW_DOWN)
	    {
	      tmp_pb = rc->scroll_v[scroll_slider_state]->slider_end;
	      offset_x += 1;
	      offset_width = -2;
	    }
	  if (arrow_type == GTK_ARROW_RIGHT)
	    {
	      tmp_pb = rc->scroll_h[scroll_slider_state]->slider_end;
	      offset_y +=1;
	      offset_height -=2;
	    }
	}
      if (tmp_pb && !firefox_hack && 
	  check_sane_pixbuf_value ((arrow_type == GTK_ARROW_DOWN || arrow_type == GTK_ARROW_UP) ? 1 : 0,
				   (arrow_type == GTK_ARROW_LEFT || arrow_type == GTK_ARROW_RIGHT) ? 1 : 0,
				   gdk_pixbuf_get_width (tmp_pb) + offset_width,
				   gdk_pixbuf_get_height (tmp_pb) + offset_height, tmp_pb))   
	  gdk_draw_pixbuf (window,
			   NULL, /* don't clip as we're drawing outside the boundary one purpose */
			   tmp_pb,
			   (arrow_type == GTK_ARROW_DOWN || arrow_type == GTK_ARROW_UP) ? 1 : 0,
			   (arrow_type == GTK_ARROW_LEFT || arrow_type == GTK_ARROW_RIGHT) ? 1 : 0,
			   scroll_button_x +  offset_x,
			   scroll_button_y + offset_y,
			   gdk_pixbuf_get_width (tmp_pb) + offset_width,
			   gdk_pixbuf_get_height (tmp_pb) + offset_height,
			   GDK_RGB_DITHER_NONE,0,0);
    }
  else if (get_ancestor_of_type (widget, "GtkComboBox") || 
	   get_ancestor_of_type (widget, "GimpEnumComboBox"))
    {
      GList *tmp_list;
      int vsep_offset = 0;
      GType researched_type = g_type_from_name ("GtkVSeparator");

      /* get the vseparator offset if it exists */

      tmp_list = gtk_container_get_children (GTK_CONTAINER (widget->parent));
      while (tmp_list)
	{
	  GType w_type = G_TYPE_INVALID;
	  GtkWidget *w = (GtkWidget *)tmp_list->data;
	  if (G_OBJECT_TYPE (w) == researched_type)
	    {
	      vsep_offset = w->allocation.width + w->style->xthickness;
	    }
	  tmp_list = tmp_list->next;
	}
      if (tmp_list)
	 g_list_free(tmp_list);

      draw_nimbus_box (style, window, state_type, shadow_type, NULL, widget, "combobox_arrow",
		       rc->arrow_button[state_type],
		       FALSE,
		       x - vsep_offset,
		       widget->parent->parent->allocation.y,
		       widget->parent->parent->allocation.x + widget->parent->parent->allocation.width - x + vsep_offset,
		       widget->parent->parent->allocation.height,
		       NIMBUS_SPIN_NONE, GTK_ORIENTATION_HORIZONTAL);

      if (rc->combo_arrow[state_type])
	gdk_draw_pixbuf (window,
			 get_clipping_gc (window, area),
			 rc->combo_arrow[state_type],
			 0,0,
			 (x - vsep_offset) + (widget->parent->parent->allocation.x + widget->parent->parent->allocation.width - x + vsep_offset - gdk_pixbuf_get_width (rc->combo_arrow[state_type])) / 2,
			 widget->parent->parent->allocation.y + (widget->parent->parent->allocation.height - gdk_pixbuf_get_height (rc->combo_arrow[state_type])) / 2 ,
			 gdk_pixbuf_get_width (rc->combo_arrow[state_type]),
			 gdk_pixbuf_get_height (rc->combo_arrow[state_type]),
			 GDK_RGB_DITHER_NONE,0,0);
    }
  else if (get_ancestor_of_type (widget, "GtkCombo") || 
	   get_ancestor_of_type (widget, "GtkComboBoxEntry") ||
	   get_ancestor_of_type (widget, "GnomeEntry"))
    {
      if (rc->combo_arrow[state_type])
	gdk_draw_pixbuf (window,
			 get_clipping_gc (window, area),
			 rc->combo_arrow[state_type],
			 0,0,
			 x + 1 + (width - gdk_pixbuf_get_width (rc->combo_arrow[state_type])) / 2 ,
			 y,
			 gdk_pixbuf_get_width (rc->combo_arrow[state_type]),
			 gdk_pixbuf_get_height (rc->combo_arrow[state_type]),
			 GDK_RGB_DITHER_NONE,0,0);
    }
  else 
    {
      GdkPixbuf *arrow = NULL;
      int x_center_offset, y_center_offset;

      if (arrow_type == GTK_ARROW_UP)
	{
	  arrow = rc->arrow_up[state_type];
	  if (DETAIL ("spinbutton"))
	    y--;
	}
      if (arrow_type == GTK_ARROW_DOWN)
	arrow = rc->arrow_down[state_type];
      
      if (arrow)
	{
	  x_center_offset = (width - gdk_pixbuf_get_width (arrow)) / 2;
	  y_center_offset = (height - gdk_pixbuf_get_height (arrow)) / 2;

	  gdk_draw_pixbuf (window,
			   get_clipping_gc (window, area),
			   arrow,
			   0,0,
			   x + x_center_offset, y + y_center_offset,
			   gdk_pixbuf_get_width (arrow),
			   gdk_pixbuf_get_height (arrow),
			   GDK_RGB_DITHER_NONE,0,0);
	}
      else
	parent_class->draw_arrow (style, window, state_type, shadow_type, area, widget, detail, arrow_type, fill, x, y, width, height);

    }

  verbose ("draw\t arrow in \t-%s-\n", detail ? detail : "no detail");

}

/**************************************************************************/
static const GtkRequisition default_option_indicator_size = { 7, 13 };
static const GtkBorder default_option_indicator_spacing = { 7, 5, 2, 2 };

static void
option_menu_get_props (GtkWidget      *widget,
		       GtkRequisition *indicator_size,
		       GtkBorder      *indicator_spacing)
{
  GtkRequisition *tmp_size = NULL;
  GtkBorder *tmp_spacing = NULL;
  
  if (widget)
    gtk_widget_style_get (widget, 
			  "indicator-size", &tmp_size,
			  "indicator-spacing", &tmp_spacing,
			  NULL);

  if (tmp_size)
    {
      *indicator_size = *tmp_size;
      g_free (tmp_size);
    }
  else
    *indicator_size = default_option_indicator_size;

  if (tmp_spacing)
    {
      *indicator_spacing = *tmp_spacing;
      g_free (tmp_spacing);
    }
  else
    *indicator_spacing = default_option_indicator_spacing;
}

static void
draw_tab (GtkStyle      *style,
	  GdkWindow     *window,
	  GtkStateType   state_type,
	  GtkShadowType  shadow_type,
	  GdkRectangle  *area,
	  GtkWidget     *widget,
	  const gchar   *detail,
	  gint           x,
	  gint           y,
	  gint           width,
	  gint           height)
{
 if (DETAIL ("optionmenutab"))
  {
    GdkRectangle button_area; 
    GtkRequisition indicator_size;
    GtkBorder indicator_spacing;
    gint border_width;
    gint tab_x;
    option_menu_get_props (widget, &indicator_size, &indicator_spacing);
    
    NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;
    
    border_width = GTK_CONTAINER (widget)->border_width;
    
    button_area.x = widget->allocation.x + border_width;
    button_area.y = widget->allocation.y + border_width;
    button_area.width = widget->allocation.width - 2 * border_width;
    button_area.height = widget->allocation.height - 2 * border_width;

    tab_x = button_area.x + button_area.width - 
	    indicator_size.width - indicator_spacing.right - indicator_spacing.left - widget->style->xthickness;

    draw_nimbus_box (style, window, state_type, shadow_type, area, widget, "option_arrow",
		     rc->combo_entry_button[state_type],
		     FALSE,
		     tab_x, button_area.y,
		     indicator_size.width + indicator_spacing.right + indicator_spacing.left + widget->style->xthickness,
		     button_area.height, 
		     NIMBUS_SPIN_NONE,
		     GTK_ORIENTATION_HORIZONTAL);

    if (rc->combo_arrow[state_type])
      gdk_draw_pixbuf (window,
		       get_clipping_gc (window, area),
		       rc->combo_arrow[state_type],
		       0,0,
		       x, y,
		       gdk_pixbuf_get_width (rc->combo_arrow[state_type]),
		       gdk_pixbuf_get_height (rc->combo_arrow[state_type]),
		       GDK_RGB_DITHER_NONE,0,0);
  }
  else
    parent_class->draw_tab (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);

  verbose ("draw\t tab in \t-%s-\n", detail ? detail : "no detail");
}



/**************************************************************************/

static void
draw_shadow (GtkStyle        *style,
	     GdkWindow       *window,
	     GtkStateType     state_type,
	     GtkShadowType    shadow_type,
	     GdkRectangle    *area,
	     GtkWidget       *widget,
	     const gchar     *detail,
	     gint             x,
	     gint             y,
	     gint             width,
	     gint             height)
{
  NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;

  if (DETAIL ("entry"))
    {
      gboolean general_case = TRUE;
      if (get_ancestor_of_type (widget, "GtkCombo") || 
	  get_ancestor_of_type (widget, "GtkComboBoxEntry") || 
	  get_ancestor_of_type (widget, "GtkSpinButton") ||
	  get_ancestor_of_type (widget, "GnomeEntry"))
	general_case = FALSE; /*combo case */

      /* Special case for Star/OpenOffice Spinbutton redraw problem */
      if (get_ancestor_of_type (widget, "GtkSpinButton") && get_ancestor_of_type (widget, "GtkFixed"))
	gdk_draw_rectangle (window, style->white_gc, TRUE,
			    x+2, y+2, width-4, height-4);

      /* work around for a bug in gtkentry were the state isn't set */
      if (widget)
	state_type = GTK_WIDGET_STATE(widget);

      /* border gradients points */
      gdk_draw_line (window, 
		     nimbus_realize_color (style,rc->textfield_color[state_type]->vertical_line_gradient1, area), 
		     x, y+1, x, y+1);
      
      if (general_case)
	{
	  gdk_draw_line (window, 
			 nimbus_realize_color (style,rc->textfield_color[state_type]->vertical_line_gradient1, area), 
			 x+width-1, y+1, x+width-1, y+1);
	    
	  gdk_draw_line (window, 
			 nimbus_realize_color (style,rc->textfield_color[state_type]->vertical_line_gradient2, area), 
			 x+width-1, y+2, x+width-1, y+2);
	  

	}

      gdk_draw_line (window, 
		     nimbus_realize_color (style,rc->textfield_color[state_type]->vertical_line_gradient2, area), 
		     x, y+2, x, y+2);
      

      /* third gradient line end points as they can't be drawn in draw_flat_box */
      
      /* horizontal gradient */
      gdk_draw_line (window, 
		     nimbus_realize_color (style,rc->textfield_color[state_type]->gradient_line1, area), 
		     x, y, x + width - 1, y);

      gdk_draw_line (window, 
		     nimbus_realize_color (style,rc->textfield_color[state_type]->gradient_line2, area), 
		     x+1, y+1, x+width- (general_case ? 2 :1), y+1);
     
      /* vertical borders */
      gdk_draw_line (window, 
		     nimbus_realize_color (style,rc->textfield_color[state_type]->vertical_line, area), 
		     x, y+3, x, y+height-1);
      
      if (general_case)
	gdk_draw_line (window, 
		       nimbus_realize_color (style,rc->textfield_color[state_type]->vertical_line, area), 
		       x+width-1, y+3, x+width-1, y+height-1);

      /* bottom line */
      gdk_draw_line (window, 
		     nimbus_realize_color (style,rc->textfield_color[state_type]->vertical_line, area), 
		     x+1, 
		     y+height - (general_case ? 1 : 2),
		     x+width - (general_case ? 2 : 1), 
		     y+height - (general_case ? 1 : 2));

      if (!general_case)
	{
	  if (area)
	    gdk_gc_set_clip_rectangle (style->bg_gc[GTK_STATE_ACTIVE], area);
	gdk_draw_line (window, 
		       style->bg_gc[GTK_STATE_ACTIVE],  /* hack to get #d6d9df color */
		       x, 
		       y+height - 1,
		       x+width, 
		       y+height -  1);
	  if (area)
	    gdk_gc_set_clip_rectangle (style->bg_gc[GTK_STATE_ACTIVE], NULL);
	}
      verbose ("draw\t shadow \t-%s-\n", detail ? detail : "no detail");
    }
  else   
    {
      if (shadow_type != GTK_SHADOW_NONE && !DETAIL ("pager"))
	gdk_draw_rectangle (window, nimbus_realize_color (style, rc->pane->outline, area),
			    FALSE, x, y, width-1, height-1);
							
       /* parent_class->draw_shadow (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);  */
      verbose ("draw\t shadow \t-%s-\t shadow_type %s \n", detail ? detail : "no detail", shadow_names[shadow_type]);
    }
}

/**************************************************************************/
static void
draw_nimbus_tab (GtkStyle*	    style,
		 GdkWindow*	    window,
		 GtkStateType	    state_type,
		 GtkShadowType	    shadow_type,
		 GdkRectangle*	    area,
		 GtkWidget*	    widget,
		 const gchar*	    detail,
		 NimbusButton*	    button,
		 gint		    x,
		 gint		    y,
		 gint		    width,
		 gint		    height,
		 NimbusTabPosition  position)
{
  GSList *tmp;


  tmp = button->gradients;
  while (tmp)
    {
      nimbus_draw_gradient (window, style, area,(NimbusGradient*)tmp->data,
			    x, y, width, height, 
			    -1, TRUE, GTK_ORIENTATION_HORIZONTAL, position);
      tmp = tmp->next;
    }
     
  if (button->corner_top_left && position != TAB_POS_LEFT && position != TAB_POS_TOP)
    gdk_draw_pixbuf (window,
    		 get_clipping_gc (window, area),
    		 button->corner_top_left,
    		 0,0,
    		 x,y,
    		 gdk_pixbuf_get_width (button->corner_top_left),
    		 gdk_pixbuf_get_height (button->corner_top_left),
    		 GDK_RGB_DITHER_NONE,0,0);
  
  if (button->corner_top_right && position != TAB_POS_RIGHT && position != TAB_POS_TOP)
    gdk_draw_pixbuf (window,
    		 get_clipping_gc (window, area),
    		 button->corner_top_right,
    		 0,0,
    		 x+ width - gdk_pixbuf_get_width (button->corner_top_right),
    		 y,
    		 gdk_pixbuf_get_width (button->corner_top_right),
    		 gdk_pixbuf_get_height (button->corner_top_right),
    		 GDK_RGB_DITHER_NONE,0,0);

  if (button->corner_bottom_left && position != TAB_POS_BOTTOM && position != TAB_POS_LEFT)
      gdk_draw_pixbuf (window,
    		   get_clipping_gc (window, area),
    		   button->corner_bottom_left,
    		   0,0,
    		   x, 
    		   y + height - gdk_pixbuf_get_height (button->corner_bottom_left),
    		   gdk_pixbuf_get_width (button->corner_bottom_left),
    		   gdk_pixbuf_get_height (button->corner_bottom_left),
    		   GDK_RGB_DITHER_NONE,0,0);
  if (button->corner_bottom_right && position != TAB_POS_BOTTOM && position != TAB_POS_RIGHT)
      gdk_draw_pixbuf (window,
    		   get_clipping_gc (window, area),
    		   button->corner_bottom_right,
    		   0,0,
    		   x+ width -gdk_pixbuf_get_width (button->corner_bottom_right), 
    		   y + height - gdk_pixbuf_get_height (button->corner_bottom_right),			 
    		   gdk_pixbuf_get_width (button->corner_bottom_right),
    		   gdk_pixbuf_get_height (button->corner_bottom_right),
    		   GDK_RGB_DITHER_NONE,0,0);
}

static void 
draw_box_gap (GtkStyle       *style,
	      GdkWindow      *window,
	      GtkStateType    state_type,
	      GtkShadowType   shadow_type,
	      GdkRectangle   *area,
	      GtkWidget      *widget,
	      const gchar    *detail,
	      gint            x,
	      gint            y,
	      gint            width,
	      gint            height,
	      GtkPositionType gap_side,
	      gint            gap_x,
	      gint            gap_width)
{
  NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;

  if (area)
    gdk_gc_set_clip_rectangle (style->black_gc, area);
  
  switch (gap_side)
    {
    case GTK_POS_TOP:
      /* box */
      gdk_draw_line (window, style->black_gc, x, y, x, y + height);
      gdk_draw_line (window, style->black_gc, x + width, y, 
		     x+width, y + height);
      gdk_draw_line (window, style->black_gc, x, y + height, 
		     x+width, y + height);
      /* gap */
      if (gap_x > 0)
	gdk_draw_line (window, style->black_gc, x, y, x + gap_x, y);

      gdk_draw_line (window, style->black_gc, x + gap_x + gap_width, y, x + width, y);
      /* gradient */
      
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->start, area), 
		     x+1, y + 1, x + width-1, y + 1);
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->mid, area),
		     x+1, y + 2, x + width-1, y + 2);
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->end, area),
		     x+1, y + 3, x + width-1, y + 3);
      gdk_draw_line (window, style->black_gc, x, y + 4, x + width, y + 4);
      break;
    case  GTK_POS_BOTTOM:
      /* box */
      height--;
      gdk_draw_line (window, style->black_gc, x, y, x, y + height);
      gdk_draw_line (window, style->black_gc, x + width, y, x+width, y + height);
      gdk_draw_line (window, style->black_gc, x, y, x+width, y);
      /* gap */
      
      if (gap_x > 0)
	gdk_draw_line (window, style->black_gc, x, y + height, x + gap_x, y + height);

      gdk_draw_line (window, style->black_gc, x + gap_x + gap_width, y+ height, x + width, y+ height);
      /* gradient */
      
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->start, area), 
		     x+1, y + height - 1, x + width-1, y + height - 1);
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->mid, area),
		     x+1, y + height - 2, x + width-1, y + height - 2);
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->end, area),
		     x+1, y + height - 3, x + width-1, y + height - 3);
      gdk_draw_line (window, style->black_gc, x, y + height - 4, x + width, y + height - 4);
      break;
    case GTK_POS_LEFT:
      /* box */
      x++;
      gdk_draw_line (window, style->black_gc, x, y, x+width, y);
      gdk_draw_line (window, style->black_gc, x, y+height, x+width, y+height);
      gdk_draw_line (window, style->black_gc, x + width, y, x+width, y + height);
      /* gap */
      if (gap_x > 0)
	gdk_draw_line (window, style->black_gc, x, y, x, y+ gap_x);

      gdk_draw_line (window, style->black_gc, x, y + gap_x + gap_width, 
		     x, y + height);
      /* gradient */
      
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->start, area), 
		     x+1, y + 1, x+1, y+height-1);
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->mid, area),
		     x+2, y + 1, x+2, y+height-1);
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->end, area),
		     x+3, y + 1, x+3, y+height-1);
      gdk_draw_line (window, style->black_gc, x+4, y + 1, x+4, y+height-1);
          break;
    case GTK_POS_RIGHT:
     /* box */
      width--;
      gdk_draw_line (window, style->black_gc, x, y, x+width, y);
      gdk_draw_line (window, style->black_gc, x, y+height, x+width, y+height);
      gdk_draw_line (window, style->black_gc, x, y, x, y + height);
      /* gap */
      if (gap_x > 0)
	gdk_draw_line (window, style->black_gc, x + width, y, x + width, y+ gap_x);

      gdk_draw_line (window, style->black_gc, x+ width, y + gap_x + gap_width, 
		     x+ width, y + height);
      /* gradient */
      
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->start, area), 
		     x+width-1, y + 1, x+width-1, y+height-1);
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->mid, area),
		     x+width-2, y + 1, x+width-2, y+height-1);
      gdk_draw_line (window, nimbus_realize_color (style, rc->tab[state_type]->end, area),
		     x+width-3, y + 1, x+width-3, y+height-1);
      gdk_draw_line (window, style->black_gc, x+width-4, y + 1, x+width-4, y+height-1);
          break;
    }
  if (area)
    gdk_gc_set_clip_rectangle (style->black_gc, NULL);

  /* parent_class->draw_box_gap (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, gap_side, gap_x, gap_width); */
  verbose ("draw\t box gap \t-%s-\n", detail ? detail : "no detail"); 

}

/**************************************************************************/

static void 
draw_extension (GtkStyle       *style,
		GdkWindow      *window,
		GtkStateType    state_type,
		GtkShadowType   shadow_type,
		GdkRectangle   *area,
		GtkWidget      *widget,
		const gchar    *detail,
		gint            x,
		gint            y,
		gint            width,
		gint            height,
		GtkPositionType gap_side)
{
  GdkGC *gc;
  NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;
  NimbusButton *button = rc->button_default[GTK_STATE_NORMAL];
  NimbusTabPosition position = (NimbusTabPosition) gap_side;
  int selected_offset = 1;

  if (state_type == GTK_STATE_ACTIVE)
    {
      button = rc->button[GTK_STATE_NORMAL];
      gc = style->black_gc;
      selected_offset--;
    }
  else
    gc = nimbus_realize_color (style, rc->tab[state_type]->junction, NULL);

  if (position == TAB_POS_BOTTOM)
    y++;
  if (position == TAB_POS_TOP)
    y--;
  if (position == TAB_POS_RIGHT)
    x+=2;
  if (position == TAB_POS_LEFT)
    x--;
  
  draw_nimbus_tab (style, window, state_type, shadow_type, area,
		   widget, detail, button, x, y, width, height, position);

  if (area) gdk_gc_set_clip_rectangle (gc, area);

   switch (gap_side)
    {
    case GTK_POS_TOP:
      gdk_draw_line (window, gc, x+1, y, x+width-1-selected_offset,y);
      break;
    case  GTK_POS_BOTTOM:
      gdk_draw_line (window, gc, x+1, y+height-1, x+width-1-selected_offset,y+height-1);
      break;
    case GTK_POS_LEFT:
       gdk_draw_line (window, gc, x, y+1, x,y+height-1-selected_offset); 
      break;
    case GTK_POS_RIGHT:
       gdk_draw_line (window, gc, x+width-1, y+1, x+width-1,y+height-1-selected_offset); 
      break;
    }
  if (area) gdk_gc_set_clip_rectangle (gc, NULL);


  /* parent_class->draw_extension (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, gap_side); */

  verbose ("draw\t extension \t-%s-\n", detail ? detail : "no detail");
}
    

/**************************************************************************/

static void 
draw_handle (GtkStyle      *style,
	     GdkWindow     *window,
	     GtkStateType   state_type,
	     GtkShadowType  shadow_type,
	     GdkRectangle  *area,
	     GtkWidget     *widget,
	     const gchar   *detail,
	     gint           x,
	     gint           y,
	     gint           width,
	     gint           height,
	     GtkOrientation orientation)
{
  NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;
  GdkPixbuf *pane;
  int c_x = 0, c_y = 0;
  gboolean draw_outline = TRUE;

  /* printf ("in draw_handle detail %s\n", detail ? detail : "no detail"); */

  if ((DETAIL ("handlebox") || DETAIL ("dockitem")) && (get_ancestor_of_type (widget, "PanelToplevel") == NULL))
    {
      height--;
      if (get_ancestor_of_type (widget, "GtkFixed") && get_ancestor_of_type (widget,"GtkHandleBox")) /* heuristic for soffice */
	  height--;

      if (orientation == GTK_ORIENTATION_VERTICAL)
	{
	  nimbus_init_handle_bar (rc, height, orientation);

	  gdk_draw_pixbuf (window,
			   NULL,
			   rc->handlebar[orientation]->top,
			   0, 0,
			   x, y,
			   gdk_pixbuf_get_width (rc->handlebar[orientation]->top),
			   gdk_pixbuf_get_height (rc->handlebar[orientation]->top),
			   GDK_RGB_DITHER_NONE,0,0);
	  gdk_draw_pixbuf (window,
			   NULL,
			   rc->handlebar[orientation]->mid,
			   0, 0,
			   x, y + gdk_pixbuf_get_height (rc->handlebar[orientation]->top),
			   gdk_pixbuf_get_width (rc->handlebar[orientation]->mid),
			   gdk_pixbuf_get_height (rc->handlebar[orientation]->mid),
			   GDK_RGB_DITHER_NONE,0,0);	  
	  gdk_draw_pixbuf (window,
			   NULL,
			   rc->handlebar[orientation]->bottom,
			   0, 0,
			   x, 
			   y + gdk_pixbuf_get_height (rc->handlebar[orientation]->top) + gdk_pixbuf_get_height (rc->handlebar[orientation]->mid),
			   gdk_pixbuf_get_width (rc->handlebar[orientation]->bottom),
			   gdk_pixbuf_get_height (rc->handlebar[orientation]->bottom),
			   GDK_RGB_DITHER_NONE,0,0);
	  draw_box (style, window, state_type, shadow_type, area, widget, "toolbar", x, 
		    y + height, width, 1);

	}
      else
	{
	  nimbus_init_handle_bar (rc, width, orientation);

	  gdk_draw_pixbuf (window,
			   NULL,
			   rc->handlebar[orientation]->top,
			   0, 0,
			   x, y,
			   gdk_pixbuf_get_width (rc->handlebar[orientation]->top),
			   gdk_pixbuf_get_height (rc->handlebar[orientation]->top),
			   GDK_RGB_DITHER_NONE,0,0);
	  gdk_draw_pixbuf (window,
			   NULL,
			   rc->handlebar[orientation]->mid,
			   0, 0,
			   x + gdk_pixbuf_get_width (rc->handlebar[orientation]->top), y,
			   gdk_pixbuf_get_width (rc->handlebar[orientation]->mid),
			   gdk_pixbuf_get_height (rc->handlebar[orientation]->mid),
			   GDK_RGB_DITHER_NONE,0,0);	  
	  gdk_draw_pixbuf (window,
			   NULL,
			   rc->handlebar[orientation]->bottom,
			   0, 0,
			   x + gdk_pixbuf_get_width (rc->handlebar[orientation]->top) + gdk_pixbuf_get_width (rc->handlebar[orientation]->mid), 
			   y,
			   gdk_pixbuf_get_width (rc->handlebar[orientation]->bottom),
			   gdk_pixbuf_get_height (rc->handlebar[orientation]->bottom),
			   GDK_RGB_DITHER_NONE,0,0);
	  draw_box (style, window, state_type, shadow_type, area, widget, "toolbar", x, 
		    y + height, width, 1);
	}
    }
  else
    {
      /* pane and gnome panel case */
      
      if (get_ancestor_of_type (widget, "PanelToplevel"))
	draw_outline = FALSE;
      
      if (orientation == GTK_ORIENTATION_HORIZONTAL)
	{
	  pane = rc->pane->pane_h;
	  if (draw_outline)
	    {
	      gdk_draw_line (window, nimbus_realize_color (style, rc->pane->outline, area),
			     x+1, y, x+width-1, y);      
	      gdk_draw_line (window, nimbus_realize_color (style, rc->pane->outline, area),
			     x+1, y+height, x+width-1, y+height);      
	      gdk_draw_line (window, nimbus_realize_color (style, rc->pane->innerline, area),
			     x+1, y+1, x+width-1, y+1);      
	      gdk_draw_line (window, nimbus_realize_color (style, rc->pane->innerline, area),
			     x+1, y+height-1, x+width-1, y+height-1);      
	    }
	  
	  c_y = 1;
	}
      else
	{
	  pane = rc->pane->pane_v;
	  if (draw_outline)
	    {
	      gdk_draw_line (window, nimbus_realize_color (style, rc->pane->outline, area),
			     x, y, x, y+height-1);      
	      gdk_draw_line (window, nimbus_realize_color (style, rc->pane->outline, area),
			     x+width-1, y, x+width-1, y+height-1);      
	      gdk_draw_line (window, nimbus_realize_color (style, rc->pane->innerline, area),
			     x+1, y, x+1, y+height-1);      
	      gdk_draw_line (window, nimbus_realize_color (style, rc->pane->innerline, area),
			     x+width-2, y, x+width-2, y+height-1);      
	    }
	}
      
      c_x = (width - gdk_pixbuf_get_width (pane)) / 2;
      c_y += (height - gdk_pixbuf_get_height (pane)) /2;
      
      gdk_draw_pixbuf (window,
		       get_clipping_gc (window, area),
		       pane,
		       0,0,
		       x + c_x, y + c_y,
		       gdk_pixbuf_get_width (pane),
		       gdk_pixbuf_get_height (pane),
		       GDK_RGB_DITHER_NONE,0,0);
      
    }
   /* parent_class->draw_handle (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, orientation);  */
  verbose ("draw\t handle \t-%s-\n", detail ? detail : "no detail");

}

/**************************************************************************/

static void 
draw_flat_box (GtkStyle      *style,
	       GdkWindow     *window,
	       GtkStateType   state_type,
	       GtkShadowType  shadow_type,
	       GdkRectangle  *area,
	       GtkWidget     *widget,
	       const gchar   *detail,
	       gint           x,
	       gint           y,
	       gint           width,
	       gint           height)
{
 parent_class->draw_flat_box (style, window, state_type, shadow_type, area,
			       widget, detail, x, y, width, height);

  verbose ("draw\t flat box \t-%s-\n", detail ? detail : "no detail");
}
/**************************************************************************/
static void
draw_nimbus_box (GtkStyle      *style,
		 GdkWindow     *window,
		 GtkStateType   state_type,
		 GtkShadowType  shadow_type,
		 GdkRectangle  *area,
		 GtkWidget     *widget,
		 const gchar   *detail,
		 NimbusButton  *button,
		 gboolean	    drop_shadow,
		 gint           x,
		 gint           y,
		 gint           width,
		 gint           height,
		 NimbusSpinButton spin_type,
		 GtkOrientation orientation)
{
  gboolean draw_top = TRUE, draw_bottom = TRUE, draw_partial_from_start = TRUE;
  NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;

  GSList *tmp;
  int drop_shadow_offset = 1;
  int bottom_left_c_w = 0, bottom_right_c_w = 0;
  int partial_height = -1;

  if (spin_type == NIMBUS_SPIN_DOWN)
    {
      draw_top = FALSE;
      draw_partial_from_start = FALSE;
      partial_height = (height - drop_shadow_offset) / 2;
    }
  if (spin_type == NIMBUS_SPIN_UP)
    {
      draw_bottom = FALSE;
      partial_height = (height - drop_shadow_offset) / 2 + 1;
    }
  
  tmp = button->gradients;
  while (tmp)
    {
      nimbus_draw_gradient (window, style, area, (NimbusGradient*)tmp->data,
			    x, y, width, height - drop_shadow_offset, 
			    partial_height, draw_partial_from_start,
			    orientation, NO_TAB);
      tmp = tmp->next;
    }
     
  if (button->corner_top_left && draw_top)
    gdk_draw_pixbuf (window,
    		 get_clipping_gc (window, area),
    		 button->corner_top_left,
    		 0,0,
    		 x,y,
    		 gdk_pixbuf_get_width (button->corner_top_left),
    		 gdk_pixbuf_get_height (button->corner_top_left),
    		 GDK_RGB_DITHER_NONE,0,0);
  
  if (button->corner_top_right && draw_top)
    gdk_draw_pixbuf (window,
    		 get_clipping_gc (window, area),
    		 button->corner_top_right,
    		 0,0,
    		 x+ width - gdk_pixbuf_get_width (button->corner_top_right),
    		 y,
    		 gdk_pixbuf_get_width (button->corner_top_right),
    		 gdk_pixbuf_get_height (button->corner_top_right),
    		 GDK_RGB_DITHER_NONE,0,0);

  if (button->corner_bottom_left && draw_bottom)
    {
      bottom_left_c_w = gdk_pixbuf_get_width (button->corner_bottom_left);
      gdk_draw_pixbuf (window,
    		   get_clipping_gc (window, area),
    		   button->corner_bottom_left,
    		   0,0,
    		   x, 
    		   y + height - gdk_pixbuf_get_height (button->corner_bottom_left),
    		   bottom_left_c_w,
    		   gdk_pixbuf_get_height (button->corner_bottom_left),
    		   GDK_RGB_DITHER_NONE,0,0);
    }
  if (button->corner_bottom_right && draw_bottom)
    {
      bottom_right_c_w = gdk_pixbuf_get_width (button->corner_bottom_right);
      gdk_draw_pixbuf (window,
    		   get_clipping_gc (window, area),
    		   button->corner_bottom_right,
    		   0,0,
    		   x+ width - bottom_right_c_w, 
    		   y + height - gdk_pixbuf_get_height (button->corner_bottom_right),			 
    		   bottom_right_c_w,
    		   gdk_pixbuf_get_height (button->corner_bottom_right),
    		   GDK_RGB_DITHER_NONE,0,0);
    }
  
  if ((state_type != GTK_STATE_INSENSITIVE) && drop_shadow && draw_bottom)
    {
      nimbus_init_button_drop_shadow (rc, state_type, width);

      if (check_sane_pixbuf_value (0, 0, width - (bottom_left_c_w + bottom_right_c_w),  gdk_pixbuf_get_height (rc->drop_shadow[state_type]), rc->drop_shadow[state_type]))
	gdk_draw_pixbuf (window,
			 get_clipping_gc (window, area),
			 rc->drop_shadow[state_type],
			 0,0,
			 x + bottom_left_c_w, 
			 y + height-1,
			 width - (bottom_left_c_w + bottom_right_c_w),
			 gdk_pixbuf_get_height (rc->drop_shadow[state_type]),
			 GDK_RGB_DITHER_NONE,0,0);
    }
}


/**************************************************************************/

static void
draw_progress (GtkStyle      *style,
	       GdkWindow     *window,
	       GtkStateType   state_type,
	       GtkShadowType  shadow_type,
	       GdkRectangle  *area,
	       GtkWidget     *widget,
	       const gchar   *detail,
	       gint           x,
	       gint           y,
	       gint           width,
	       gint           height)
{	  
  GtkOrientation orientation = GTK_ORIENTATION_HORIZONTAL;
  GtkProgressBarOrientation p_orientation = GTK_PROGRESS_LEFT_TO_RIGHT; /*assuming horizontal by default */

  if (GTK_IS_PROGRESS_BAR (widget)) 
    p_orientation = gtk_progress_bar_get_orientation (GTK_PROGRESS_BAR (widget));

  nimbus_init_progress (NIMBUS_RC_STYLE (style->rc_style)->data, height, width + 1);
  
  if (p_orientation == GTK_PROGRESS_BOTTOM_TO_TOP ||
      p_orientation == GTK_PROGRESS_TOP_TO_BOTTOM)
    orientation = GTK_ORIENTATION_VERTICAL;

  NimbusProgress *progress = NIMBUS_RC_STYLE (style->rc_style)->data->progress;

  draw_nimbus_box (style, window, state_type, shadow_type, area,
		       widget, detail, progress->bar, FALSE,
		       x, y, width, height, NIMBUS_SPIN_NONE,
		       orientation);

  if (orientation == GTK_ORIENTATION_VERTICAL)
    width--;

  /* corners */
  gdk_draw_pixbuf (window,			 
		   get_clipping_gc (window, area),
		   progress->corner_top_left,
		   0,0,
		   x - gdk_pixbuf_get_width (progress->corner_top_left),
		   y - gdk_pixbuf_get_height (progress->corner_top_left),
		   gdk_pixbuf_get_width (progress->corner_top_left),
		   gdk_pixbuf_get_height (progress->corner_top_left),
		   GDK_RGB_DITHER_NONE,0,0);
  gdk_draw_pixbuf (window,			 
		   get_clipping_gc (window, area),
		   progress->corner_top_right,
		   0,0,
		   x + width + 1,
		   y - gdk_pixbuf_get_height (progress->corner_top_right),
		   gdk_pixbuf_get_width (progress->corner_top_right),
		   gdk_pixbuf_get_height (progress->corner_top_right),
		   GDK_RGB_DITHER_NONE,0,0);
  gdk_draw_pixbuf (window,			 
		   get_clipping_gc (window, area),
		   progress->corner_bottom_left,
		   0,0,
		   x - gdk_pixbuf_get_width (progress->corner_bottom_left),
		   y + height + 1 - gdk_pixbuf_get_height (progress->corner_bottom_left),
		   gdk_pixbuf_get_width (progress->corner_bottom_left),
		   gdk_pixbuf_get_height (progress->corner_bottom_left),
		   GDK_RGB_DITHER_NONE,0,0);
  gdk_draw_pixbuf (window,			 
		   get_clipping_gc (window, area),
		   progress->corner_bottom_right,
		   0,0,
		   x + width + 1,
		   y + height + 1 - gdk_pixbuf_get_height (progress->corner_bottom_right),
		   gdk_pixbuf_get_width (progress->corner_bottom_right),
		   gdk_pixbuf_get_height (progress->corner_bottom_right),
		   GDK_RGB_DITHER_NONE,0,0);
  /* lines */

  if (check_sane_pixbuf_value (0, 0, gdk_pixbuf_get_width (progress->border_left), height - 1, progress->border_left))
    gdk_draw_pixbuf (window,			 
		     get_clipping_gc (window, area),
		     progress->border_left,
		     0,0,
		     x - gdk_pixbuf_get_width (progress->border_left), 
		     y,
		     gdk_pixbuf_get_width (progress->border_left),
		     height - 1,
		     GDK_RGB_DITHER_NONE,0,0);

  if (check_sane_pixbuf_value (0,0, gdk_pixbuf_get_width (progress->border_right), height - 1, progress->border_right))

    gdk_draw_pixbuf (window,			 
		     get_clipping_gc (window, area),
		     progress->border_right,
		     0,0,
		     x + width + 1,
		     y,
		     gdk_pixbuf_get_width (progress->border_right),
		     height - 1,
		     GDK_RGB_DITHER_NONE,0,0);


  if (check_sane_pixbuf_value (0,0,width + 1,  gdk_pixbuf_get_height (progress->border_top),  progress->border_top))
    gdk_draw_pixbuf (window,			 
		     get_clipping_gc (window, area),
		     progress->border_top,
		     0,0,
		     x, 
		     y - gdk_pixbuf_get_height (progress->border_top),
		     width + 1,
		     gdk_pixbuf_get_height (progress->border_top),
		     GDK_RGB_DITHER_NONE,0,0);

  if (check_sane_pixbuf_value (0,0, width + 1,   gdk_pixbuf_get_height (progress->border_bottom), progress->border_bottom))
    gdk_draw_pixbuf (window,			 
		     get_clipping_gc (window, area),
		     progress->border_bottom,
		     0,0,
		     x, 
		     y + height - gdk_pixbuf_get_height (progress->border_bottom) + 1,
		     width + 1,
		     gdk_pixbuf_get_height (progress->border_bottom),
		     GDK_RGB_DITHER_NONE,0,0);
}


static void 
debug_print_corners (NimbusButtonCorner corners)
{
  printf ("corner present : ");
  if (corners & CORNER_TOP_LEFT)
    printf (" TOP_LEFT ");
  if (corners & CORNER_TOP_RIGHT)
    printf (" TOP_RIGHT ");
  if (corners & CORNER_BOTTOM_LEFT)
    printf (" BOTTOM_LEFT ");
  if (corners & CORNER_BOTTOM_RIGHT)
    printf (" BOTTOM_RIGHT ");
  printf ("\n");
}
typedef enum {
  MOUSE_OUTSIDE,
  MOUSE_STEPPER_A,
  MOUSE_STEPPER_B,
  MOUSE_STEPPER_C,
  MOUSE_STEPPER_D,
  MOUSE_TROUGH,
  MOUSE_SLIDER,
  MOUSE_WIDGET /* inside widget but not in any of the above GUI elements */
} MouseLocation;

/* typedef struct GtkRangeLayout NimbusGtkRangeLayout; */

struct _GtkRangeLayout
{
  /* These are in widget->window coordinates */
  GdkRectangle stepper_a;
  GdkRectangle stepper_b;
  GdkRectangle stepper_c;
  GdkRectangle stepper_d;
  /* The trough rectangle is the area the thumb can slide in, not the
   * entire range_rect
   */
  GdkRectangle trough;
  GdkRectangle slider;

  /* Layout-related state */
  
  MouseLocation mouse_location;
  /* last mouse coords we got, or -1 if mouse is outside the range */
  gint mouse_x;
  gint mouse_y;
  /* "grabbed" mouse location, OUTSIDE for no grab */
  MouseLocation grab_location;
  gint grab_button; /* 0 if none */

  /* Stepper sensitivity */
  GtkSensitivityType lower_sensitivity;
  GtkSensitivityType upper_sensitivity;
};

static void
draw_box (GtkStyle      *style,
	  GdkWindow     *window,
	  GtkStateType   state_type,
	  GtkShadowType  shadow_type,
	  GdkRectangle  *area,
	  GtkWidget     *widget,
	  const gchar   *detail,
	  gint           x,
	  gint           y,
	  gint           width,
	  gint           height)
{
  static gboolean should_draw_defaultbutton = FALSE;
  NimbusData* rc = NIMBUS_RC_STYLE (style->rc_style)->data;
  
  /* printf ("draw box state %s %s\n", state_names [state_type], state_names [GTK_WIDGET_STATE(widget)]);  */
  if (DETAIL ("button") || DETAIL ("optionmenu"))
    {
      NimbusButton *button_type = rc->button[state_type];

      if (widget && widget->parent &&
	  (GTK_IS_TREE_VIEW(widget->parent) ||
	   GTK_IS_CLIST (widget->parent) || get_ancestor_of_type (widget, "MessageList")))
	button_type = rc->header_button[state_type];
      
      if (get_ancestor_of_type (widget, "GtkCombo") || 
	  get_ancestor_of_type (widget, "GtkComboBoxEntry") ||
	  get_ancestor_of_type (widget, "GnomeEntry"))
	button_type = rc->combo_entry_button[state_type];
      
      if (should_draw_defaultbutton)
	{
	  x -= 1;  y -= 1; width += 2; height += 2;
	  button_type = rc->button_default[state_type];
	}
      
      draw_nimbus_box (style, window, state_type, shadow_type, area,
		       widget, detail, button_type, TRUE,
		       x, y, width, height, NIMBUS_SPIN_NONE,			     
		       GTK_ORIENTATION_HORIZONTAL);
      should_draw_defaultbutton = FALSE;
    }
  else if (DETAIL ("buttondefault")) /* needed as buttondefault is just an addon in gtkbutton.c */
    {
      should_draw_defaultbutton = TRUE;
      return;
    }
  else if ((detail) && (!strncmp("spinbutton", detail, strlen ("spinbutton"))))
    {
      static int spin_x, spin_y, spin_width, spin_height;
      static GtkStateType spin_up_state;
      if (DETAIL ("spinbutton"))
      { /* only kept location of the global button */ 
	spin_x = x; spin_y = y; spin_width = width; spin_height = height;
	
	if (area) gdk_gc_set_clip_rectangle (style->bg_gc[GTK_STATE_ACTIVE], area);
	gdk_draw_rectangle (window, style->bg_gc[GTK_STATE_ACTIVE], TRUE,
			      spin_x, spin_y, spin_width, spin_height);
	if (area) gdk_gc_set_clip_rectangle (style->bg_gc[GTK_STATE_ACTIVE], NULL);
	return;
      }
      if (DETAIL ("spinbutton_up")) 
	{
	  spin_up_state = state_type;
      	  /* draw up */
	  draw_nimbus_box (style, window, state_type, shadow_type, area,
			   widget, detail, rc->combo_entry_button[state_type], TRUE,
			   spin_x, spin_y, spin_width, spin_height, NIMBUS_SPIN_UP,
			   GTK_ORIENTATION_HORIZONTAL);
	}
      if (DETAIL ("spinbutton_down"))
	{ 
	  GtkStateType state_to_use;
	  /* draw  down*/
	  draw_nimbus_box (style, window, state_type, shadow_type, area,
			   widget, detail, rc->combo_entry_button[state_type], TRUE,
			   spin_x, spin_y, spin_width, spin_height, NIMBUS_SPIN_DOWN,
			   GTK_ORIENTATION_HORIZONTAL);

	  if (spin_up_state == GTK_STATE_INSENSITIVE)
	    {
	      spin_y++; /* draw the separation lines 1 pix up is the up spinner is insensitive */
	      spin_up_state = state_type;
	    }
		
	  gdk_draw_line (window, 
			 nimbus_realize_color (style,rc->spin_color[spin_up_state]->top, area),
			 spin_x + 1, spin_y + (spin_height/2) - 1, 
			 spin_x + spin_width - 2, spin_y + (spin_height/2) - 1);
	  gdk_draw_line (window, 
			 nimbus_realize_color (style,rc->spin_color[state_type]->bottom, area),
			 spin_x + 1, spin_y + (spin_height/2), 
			 spin_x + spin_width - 2, spin_y + (spin_height/2));
	}
    }
  else if (DETAIL ("trough"))
    {
      GdkPixbuf *tmp_pb;

      if (GTK_IS_SCALE (widget))
	{
	  NimbusScale* sc;
	  if (width > height)
	    {
	      nimbus_init_scale (rc, state_type, width, TRUE);
	      sc = rc->scale_h [state_type];
	      int center_offset = (height - gdk_pixbuf_get_height (sc->bkg_mid)) / 2 + 1;
	      gdk_draw_pixbuf (window,			 
			       get_clipping_gc (window, area),
			       sc->bkg_start,
			       0,0,
			       x,y + center_offset,
			       gdk_pixbuf_get_width (sc->bkg_start),
			       gdk_pixbuf_get_height (sc->bkg_start),
			       GDK_RGB_DITHER_NONE,0,0);
	      gdk_draw_pixbuf (window,			 
			       get_clipping_gc (window, area),
			       sc->bkg_mid,
			       0,0,
			       x+gdk_pixbuf_get_width (sc->bkg_start), y  + center_offset,
			       width - (gdk_pixbuf_get_width (sc->bkg_start) + gdk_pixbuf_get_width (sc->bkg_end)),
			       gdk_pixbuf_get_height (sc->bkg_mid),
			       GDK_RGB_DITHER_NONE,0,0);
	      gdk_draw_pixbuf (window,			 
			       get_clipping_gc (window, area),
			       sc->bkg_end,
			       0,0,
			       x+gdk_pixbuf_get_width (sc->bkg_start)+ (width - (gdk_pixbuf_get_width (sc->bkg_start) + gdk_pixbuf_get_width (sc->bkg_end))),
			       y + center_offset,
			       gdk_pixbuf_get_width (sc->bkg_end),
			       gdk_pixbuf_get_height (sc->bkg_end),
			       GDK_RGB_DITHER_NONE,0,0);
	    }
	  else
	    {
	      nimbus_init_scale (rc, state_type, height, FALSE);
	      sc = rc->scale_v [state_type];
	      int center_offset = (width - gdk_pixbuf_get_width (sc->bkg_mid)) / 2 + 1;
	      gdk_draw_pixbuf (window,			 
			       get_clipping_gc (window, area),
			       sc->bkg_start,
			       0,0,
			       x + center_offset,y,
			       gdk_pixbuf_get_width (sc->bkg_start),
			       gdk_pixbuf_get_height (sc->bkg_start),
			       GDK_RGB_DITHER_NONE,0,0);
	      gdk_draw_pixbuf (window,			 
			       get_clipping_gc (window, area),
			       sc->bkg_mid,
			       0,0,
			       x + center_offset, y + gdk_pixbuf_get_height (sc->bkg_start),
			       gdk_pixbuf_get_width (sc->bkg_mid),
			       height - (gdk_pixbuf_get_height (sc->bkg_start) + gdk_pixbuf_get_height (sc->bkg_end)) ,
			       GDK_RGB_DITHER_NONE,0,0);
	      gdk_draw_pixbuf (window,			 
			       get_clipping_gc (window, area),
			       sc->bkg_end,
			       0,0,
			       x + center_offset,
			       y + gdk_pixbuf_get_height (sc->bkg_start) + (height - (gdk_pixbuf_get_height (sc->bkg_start) + gdk_pixbuf_get_height (sc->bkg_end))),
			       gdk_pixbuf_get_width (sc->bkg_end),
			       gdk_pixbuf_get_height (sc->bkg_end),
			       GDK_RGB_DITHER_NONE,0,0);
	    }
	}
      else if (GTK_IS_PROGRESS_BAR (widget) || 
	       get_ancestor_of_type (widget, "GtkTreeView"))/* gaim cell_renderer_progress special case */
	{
	  GtkOrientation orientation = GTK_ORIENTATION_HORIZONTAL;
	  GtkProgressBarOrientation p_orientation = gtk_progress_bar_get_orientation (GTK_PROGRESS_BAR (widget));

	  if (p_orientation == GTK_PROGRESS_BOTTOM_TO_TOP ||
	      p_orientation == GTK_PROGRESS_TOP_TO_BOTTOM)
	    orientation = GTK_ORIENTATION_VERTICAL;
	 
	  gdk_gc_set_clip_rectangle (style->bg_gc[state_type], NULL);
	  if (area) gdk_gc_set_clip_rectangle (style->bg_gc[state_type], area);
	  gdk_draw_rectangle (window, style->bg_gc[state_type], TRUE,
			      x, y, width, height);
	  if (area) gdk_gc_set_clip_rectangle (style->bg_gc[state_type], NULL);

	  draw_nimbus_box (style, window, state_type, shadow_type, area,
			   widget, detail, rc->progress->bkg, FALSE,
			   x + 2, y + 2, width - 4, height - 4, NIMBUS_SPIN_NONE, orientation);
	}
      else
	{
	  GtkRange *range = GTK_RANGE (widget);
	  nimbus_init_scrollbar (rc, state_type, (width > height) ? width : height, (width > height) ? TRUE : FALSE);
	  tmp_pb = (width > height) ? rc->scroll_h[state_type]->bkg : rc->scroll_v[state_type]->bkg;
	  
	  gdk_draw_pixbuf (window,
			   get_clipping_gc (window, area),
			   tmp_pb,
			   0,0,
			   x,
			   y,		       
			   width,
			   height,
			   GDK_RGB_DITHER_NONE,0,0);

	  scroll_trough_x = x;
	  scroll_trough_y = y;
	  scroll_trough_width = width;
	  scroll_trough_height = height;

	/* curved bits */
	  if (width > height) /*horizontal*/
	    {
	      /* left */
	      GtkStateType state = state_type;
	      if (range->layout && 
		  range->layout->grab_location == MOUSE_STEPPER_A)
		state = GTK_STATE_ACTIVE;
	      else if (range->layout && 
		       range->layout->mouse_location == MOUSE_STEPPER_A)
		state = GTK_STATE_PRELIGHT;
	      else if (state != GTK_STATE_INSENSITIVE)
		state = GTK_STATE_NORMAL;
		
	      gdk_draw_pixbuf (window,
			       get_clipping_gc (window, area),
			       rc->scroll_h[state]->button_start,
			       0,0,
			       x,
			       y,		       
			       gdk_pixbuf_get_width (rc->scroll_h[state]->button_start),
			       gdk_pixbuf_get_height(rc->scroll_h[state]->button_start),
			       GDK_RGB_DITHER_NONE,0,0);

	      /* right */
	      if (range->layout && 
		  range->layout->grab_location == MOUSE_STEPPER_D)
		state = GTK_STATE_ACTIVE;
	      else if (range->layout && 
		       range->layout->mouse_location == MOUSE_STEPPER_D)
		state = GTK_STATE_PRELIGHT;
	      else if (state != GTK_STATE_INSENSITIVE)
		state = GTK_STATE_NORMAL;

	      gdk_draw_pixbuf (window,
			       get_clipping_gc (window, area),
			       rc->scroll_h[state]->button_end,
			       0,0,
			       x+width-gdk_pixbuf_get_width (rc->scroll_h[state]->button_end),
			       y,		       
			       gdk_pixbuf_get_width (rc->scroll_h[state]->button_end),
			       gdk_pixbuf_get_height(rc->scroll_h[state]->button_end),
			       GDK_RGB_DITHER_NONE,0,0);
	    }
	  else
	    {
	      GtkStateType state = state_type;
	      /* top */
	      if (range->layout && 
		  range->layout->grab_location == MOUSE_STEPPER_A)
		state = GTK_STATE_ACTIVE;
	      else if (range->layout && 
		       range->layout->mouse_location == MOUSE_STEPPER_A)
		state = GTK_STATE_PRELIGHT;
	      else if (state != GTK_STATE_INSENSITIVE)
		state = GTK_STATE_NORMAL;

	      gdk_draw_pixbuf (window,
			       get_clipping_gc (window, area),
			       rc->scroll_v[state]->button_start,
			       0,0,
			       x,
			       y,		       
			       gdk_pixbuf_get_width (rc->scroll_v[state]->button_start),
			       gdk_pixbuf_get_height(rc->scroll_v[state]->button_start),
			       GDK_RGB_DITHER_NONE,0,0);
	      /* bottom */
	      if (range->layout && 
		  range->layout->grab_location == MOUSE_STEPPER_D)
		state = GTK_STATE_ACTIVE;
	      else if (range->layout &&
		       range->layout->mouse_location == MOUSE_STEPPER_D)
		state = GTK_STATE_PRELIGHT;
	      else if (state != GTK_STATE_INSENSITIVE)
		state = GTK_STATE_NORMAL;

	      gdk_draw_pixbuf (window,
			       get_clipping_gc (window, area),
			       rc->scroll_v[state]->button_end,
			       0,0,
			       x,
			       y+height-gdk_pixbuf_get_height (rc->scroll_v[state]->button_end),		       
			       gdk_pixbuf_get_width (rc->scroll_v[state]->button_end),
			       gdk_pixbuf_get_height(rc->scroll_v[state]->button_end),
			       GDK_RGB_DITHER_NONE,0,0);
	    }
	}
    }
  else if (DETAIL ("hscrollbar") || DETAIL ("vscrollbar") || DETAIL ("stepper"))
    { /* save coord at the box will be drawn in draw_arrow as we don't have the direction of the box here */
      scroll_button_x = x - 1;
      scroll_button_y = y - 1;
      scroll_button_width = width + 2;
      scroll_button_height = height + 2;
    }
  else if (DETAIL ("bar"))
    draw_progress (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
  else if (DETAIL ("toolbar") || DETAIL ("dockitem_bin") || DETAIL ("handlebox_bin"))
    {
      GtkOrientation orientation = GTK_ORIENTATION_HORIZONTAL;
      if (GTK_IS_TOOLBAR (widget))
	  orientation = gtk_toolbar_get_orientation (widget);

      if (orientation == GTK_ORIENTATION_HORIZONTAL)
	  gdk_draw_line (window, nimbus_realize_color (style, rc->menubar_border, area), x,y+height-1,x+width-1,y+height-1); 
      else
	  {
	    gdk_draw_line (window, nimbus_realize_color (style, rc->menubar_border, area), x,y,x,y+height-1); 
	    gdk_draw_line (window, nimbus_realize_color (style, rc->menubar_border, area), x+width-1,y,x+width-1,y+height-1); 
	  }
    }
  else if (DETAIL ("menu"))
    {
      GdkGC *start, *mid_start, *mid_end, *end;
      sanitize_size (window, &width, &height);
      gdk_draw_rectangle (window, nimbus_realize_color (style, rc->menu->border, area), FALSE, x,y,width-1,height-1); 
      gdk_draw_line (window, nimbus_realize_color (style, rc->menu->shadow, area), x + width - 2,y+1,x + width - 2,height-2); 
      
      start = nimbus_realize_color (style, rc->menu->start, area);
      mid_start = nimbus_realize_color (style, rc->menu->mid_start, area);
      mid_end = nimbus_realize_color (style, rc->menu->mid_end, area);
      end = nimbus_realize_color (style, rc->menu->end, area);
      gdk_draw_line (window, start, x+1,y+1,x + width - 2,y+1); 
      gdk_draw_line (window, mid_start, x+1,y+2,x + width - 2,y+2); 
      gdk_draw_line (window, mid_end, x+1,y+3,x + width - 2,y+3); 
      gdk_draw_line (window, end, x+1,y+4,x + width - 2,y+4); 
      
      gdk_draw_line (window, start, x+1,y+height-2,x + width - 2,y+height-2); 
      gdk_draw_line (window, mid_start, x+1,y+height-3,x + width - 2,y+height-3); 
      gdk_draw_line (window, mid_end, x+1,y+height-4,x + width - 2,y+height-4); 
      gdk_draw_line (window, end, x+1,y+height-5,x + width - 2,y+height-5); 
      
    }
  else if (DETAIL ("menubar"))
    {
      nimbus_draw_gradient (window, style, area, rc->menubar,
			    x, y, width, height-1, -1, TRUE, 
			    GTK_ORIENTATION_HORIZONTAL, NO_TAB);

      gdk_draw_line (window, nimbus_realize_color (style, rc->menubar_border, area), 
		     x,y+height-1,x+width-1,y+height-1);
      
    }
  else
    {
	/* ensure the clipping area is reset needed of gtkruler in gimp and some progressbar */
	gdk_gc_set_clip_rectangle (style->bg_gc[state_type], NULL); 
	if (area) gdk_gc_set_clip_rectangle (style->bg_gc[state_type], area);
	gdk_draw_rectangle (window, style->bg_gc[state_type], TRUE,
			      x, y, width, height);
	if (area) gdk_gc_set_clip_rectangle (style->bg_gc[state_type], NULL);
    }
    /* parent_class->draw_box (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height); */
    
  verbose ("draw\t box \t\t-%s-\n", detail ? detail : "no detail");
}


/**************************************************************************/
static void 
draw_check (GtkStyle      *style,
	    GdkWindow     *window,
	    GtkStateType   state_type,
	    GtkShadowType  shadow_type,
	    GdkRectangle  *area,
	    GtkWidget     *widget,
	    const gchar   *detail,
	    gint           x,
	    gint           y,
	    gint           width,
	    gint           height)
{
  GdkPixbuf **image;
  NimbusData* rc = NIMBUS_RC_STYLE (style->rc_style)->data;
  
  if (shadow_type == GTK_SHADOW_IN) /* checked */
    image = rc->check_set;
  else
    image = rc->check_not_set;

  if (shadow_type == GTK_SHADOW_ETCHED_IN) /* insensitive */
    {
      if (GTK_IS_TOGGLE_BUTTON (widget))
	{
	  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
	    image = rc->check_inconsistent;
	}
      if (GTK_IS_TREE_VIEW (widget)) 
	    image = rc->check_inconsistent;

    }

  if (GTK_IS_MENU_ITEM (widget))
    {
      if (shadow_type != GTK_SHADOW_IN)
	image = NULL;
      else
	image = rc->check_menu_set;
    }
      
  if (image)
    gdk_draw_pixbuf (window,			 
		     get_clipping_gc (window, area),
		     image[state_type],
		     0,0,
		     x,y,
		     gdk_pixbuf_get_width (image[state_type]),
		     gdk_pixbuf_get_height (image[state_type]),
		     GDK_RGB_DITHER_NONE,0,0);
  
  /* parent_class->draw_check (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height); */

  verbose ("draw\t check \t-%s-\n", detail ? detail : "no detail");
}

/**************************************************************************/
static void
draw_slider (GtkStyle      *style,
	     GdkWindow     *window,
	     GtkStateType   state_type,
	     GtkShadowType  shadow_type,
	     GdkRectangle  *area,
	     GtkWidget     *widget,
	     const gchar   *detail,
	     gint           x,
	     gint           y,
	     gint           width,
	     gint           height,
	     GtkOrientation orientation)
{
  NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;

  if (DETAIL ("slider"))
    {
      NimbusScrollbar *sb;

      if (state_type == GTK_STATE_INSENSITIVE)
	return;

      scroll_slider_state = state_type;

      if (orientation == GTK_ORIENTATION_HORIZONTAL)
	{
	  nimbus_init_scrollbar (rc, state_type, width, TRUE);
	  sb = rc->scroll_h[state_type];
	  if (area)
	    area->height++; /* grow clip area for the slider shadow */
	    gdk_draw_pixbuf (window,			 
			     get_clipping_gc (window, area),
			     sb->slider_start,
			     0,0,
			     x,y-1,
			     gdk_pixbuf_get_width (sb->slider_start),
			     gdk_pixbuf_get_height (sb->slider_start),
			     GDK_RGB_DITHER_NONE,0,0);

	  if (check_sane_pixbuf_value (0,0, 
				       width - (gdk_pixbuf_get_width (sb->slider_start) + gdk_pixbuf_get_width (sb->slider_end)),
				       gdk_pixbuf_get_height (sb->slider_mid), sb->slider_mid))
	    gdk_draw_pixbuf (window,			 
			     get_clipping_gc (window, area),
			     sb->slider_mid,
			     0,0,
			     x+gdk_pixbuf_get_width (sb->slider_start), y -1,
			     width - (gdk_pixbuf_get_width (sb->slider_start) + gdk_pixbuf_get_width (sb->slider_end)),
			     gdk_pixbuf_get_height (sb->slider_mid),
			     GDK_RGB_DITHER_NONE,0,0);

	    gdk_draw_pixbuf (window,			 
			     get_clipping_gc (window, area),
			     sb->slider_end,
			     0,0,
			     x+gdk_pixbuf_get_width (sb->slider_start)+ (width - (gdk_pixbuf_get_width (sb->slider_start) + gdk_pixbuf_get_width (sb->slider_end))),
			     y - 1,
			     gdk_pixbuf_get_width (sb->slider_end),
			     gdk_pixbuf_get_height (sb->slider_end),
			     GDK_RGB_DITHER_NONE,0,0);
	}
      else
	{
	  nimbus_init_scrollbar (rc, state_type, height, FALSE);
	  sb = rc->scroll_v[state_type]; 
	  if (area)
	    area->width++; /* grow clip area for the slider shadow */
	    gdk_draw_pixbuf (window,			 
			     get_clipping_gc (window, area),
			     sb->slider_start,
			     0,0,
			     x - 1,y,
			     gdk_pixbuf_get_width (sb->slider_start),
			     gdk_pixbuf_get_height (sb->slider_start),
			     GDK_RGB_DITHER_NONE,0,0);

	  if (check_sane_pixbuf_value (0,0, gdk_pixbuf_get_width (sb->slider_mid),
				       height - (gdk_pixbuf_get_height (sb->slider_start) + gdk_pixbuf_get_height (sb->slider_end)) ,sb->slider_mid))
	    gdk_draw_pixbuf (window,			 
			     get_clipping_gc (window, area),
			     sb->slider_mid,
			     0,0,
			     x - 1, y + gdk_pixbuf_get_height (sb->slider_start),
			     gdk_pixbuf_get_width (sb->slider_mid),
			     height - (gdk_pixbuf_get_height (sb->slider_start) + gdk_pixbuf_get_height (sb->slider_end)) ,
			     GDK_RGB_DITHER_NONE,0,0);
	    gdk_draw_pixbuf (window,			 
			     get_clipping_gc (window, area),
			     sb->slider_end,
			     0,0,
			     x - 1,
			     y + gdk_pixbuf_get_height (sb->slider_start) + (height - (gdk_pixbuf_get_height (sb->slider_start) + gdk_pixbuf_get_height (sb->slider_end))),
			     gdk_pixbuf_get_width (sb->slider_end),
			     gdk_pixbuf_get_height (sb->slider_end),
			     GDK_RGB_DITHER_NONE,0,0);
	}
    }
  else if (DETAIL ("hscale") || DETAIL ("vscale"))
    {
      GdkPixbuf *button = DETAIL ("hscale") ? rc->scale_h[state_type]->button : rc->scale_v[state_type]->button;
	gdk_draw_pixbuf (window,			 
			 NULL, /* don't clip here as the button is 1 pix larger */
			 button,
			 0,0,
			 x, y,
			 gdk_pixbuf_get_width (button),
			 gdk_pixbuf_get_height (button),
			 GDK_RGB_DITHER_NONE,0,0);     
    }
  else
    parent_class->draw_slider (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, orientation);

  verbose ("draw\t slider \t-%s-\n", detail ? detail : "no detail");

}

/**************************************************************************/
static void 
draw_option (GtkStyle      *style,
	     GdkWindow     *window,
	     GtkStateType   state_type,
	     GtkShadowType  shadow_type,
	     GdkRectangle  *area,
	     GtkWidget     *widget,
	     const gchar   *detail,
	     gint           x,
	     gint           y,
	     gint           width,
	     gint           height)
{
  GdkPixbuf **image;
  NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;
  
  if (shadow_type == GTK_SHADOW_IN) /* checked */
    image = rc->radio_set;
  else
    image = rc->radio_not_set;
  
  if (shadow_type == GTK_SHADOW_ETCHED_IN) /* insensitive */
    {
      if (GTK_IS_CHECK_MENU_ITEM (widget) && 
	  gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (widget)))
	  image = rc->radio_inconsistent;

      if (GTK_IS_TOGGLE_BUTTON (widget) && 
	  gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
	  image = rc->radio_inconsistent;
      if (GTK_IS_TREE_VIEW (widget))
	  image = rc->radio_inconsistent;
    }

  if (GTK_IS_MENU_ITEM (widget))
    {
      if (shadow_type != GTK_SHADOW_IN)
	image = NULL;
      else
	image = rc->radio_menu_set;
    }

  if (image)
    gdk_draw_pixbuf (window,			 
		     get_clipping_gc (window, area),
		     image[state_type],
		     0,0,
		     x,y,
		     gdk_pixbuf_get_width (image[state_type]),
		     gdk_pixbuf_get_height (image[state_type]),
		     GDK_RGB_DITHER_NONE,0,0);
  /* parent_class->draw_option (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height); */
    verbose ("draw\t option \t-%s-\n", detail ? detail : "no detail");
}

/**************************************************************************/

static void 
draw_shadow_gap (GtkStyle       *style,
		 GdkWindow      *window,
		 GtkStateType    state_type,
		 GtkShadowType   shadow_type,
		 GdkRectangle   *area,
		 GtkWidget      *widget,
		 const gchar    *detail,
		 gint            x,
		 gint            y,
		 gint            width,
		 gint            height,
		 GtkPositionType gap_side,
		 gint            gap_x,
		 gint            gap_width)
{
  NimbusStyle *nimbus_style = NIMBUS_STYLE (style);
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  
  parent_class->draw_shadow_gap (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, gap_side, gap_x, gap_width);

  verbose ("draw\t shadow gap \t-%s-\n", detail ? detail : "no detail");
}

/**************************************************************************/

static void
draw_hline (GtkStyle     *style,
	    GdkWindow    *window,
	    GtkStateType  state_type,
	    GdkRectangle  *area,
	    GtkWidget     *widget,
	    const gchar   *detail,
	    gint          x1,
	    gint          x2,
	    gint          y)
{
  NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;

  gdk_draw_line (window, nimbus_realize_color (style, rc->hline, area), x1,y,x2,y);
  
  verbose ("draw\t hline \t-%s-\n", detail ? detail : "no detail");
}

/**************************************************************************/

static void
draw_vline (GtkStyle     *style,
	    GdkWindow    *window,
	    GtkStateType  state_type,
	    GdkRectangle  *area,
	    GtkWidget     *widget,
	    const gchar   *detail,
	    gint          y1,
	    gint          y2,
	    gint          x)
{
  NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;
  
  gdk_draw_line (window, nimbus_realize_color (style, rc->hline, area), x,y1,x,y2);

  verbose ("draw\t vline \t-%s-\n", detail ? detail : "no detail");
}

/**************************************************************************/

static void 
draw_focus (GtkStyle      *style,
	    GdkWindow     *window,
	    GtkStateType   state_type,
	    GdkRectangle  *area,
	    GtkWidget     *widget,
	    const gchar   *detail,
	    gint           x,
	    gint           y,
	    gint           width,
	    gint           height)
{
  NimbusStyle *nimbus_style = NIMBUS_STYLE (style);
  
  parent_class->draw_focus (style, window, state_type, area, widget, detail, x, y, width, height-1); 
  verbose ("draw\t focus \t-%s-\n", detail ? detail : "no detail");
}

/**************************************************************************/

static void
draw_resize_grip (GtkStyle       *style,
		  GdkWindow      *window,
		  GtkStateType    state_type,
		  GdkRectangle   *area,
		  GtkWidget      *widget,
		  const gchar    *detail,
		  GdkWindowEdge   edge,
		  gint            x,
		  gint            y,
		  gint            width,
		  gint            height)
{
  NimbusStyle *nimbus_style = NIMBUS_STYLE (style);
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);

  parent_class->draw_resize_grip (style, window, state_type,
                                                 area, widget, detail,
                                                 edge, x, y, width, height);

  verbose ("draw\t resize grip \t-%s-\n", detail ? detail : "no detail");
}

static void
draw_string (GtkStyle * style,
	     GdkWindow * window,
	     GtkStateType state,
	     GdkRectangle * area,
	     GtkWidget * widget,
	     const gchar *detail,
	     gint x,
	     gint y,
	     const gchar * string)
{
  g_return_if_fail(style != NULL);
  g_return_if_fail(window != NULL);

  parent_class->draw_string (style, window, state,
			     area, widget, detail,
			     x, y, string);
  verbose ("draw\t draw_string \t-%s-\n", detail ? detail : "no detail");

}

static void 
draw_layout (GtkStyle        *style,
	     GdkWindow       *window,
	     GtkStateType     state_type,
	     gboolean         use_text,
	     GdkRectangle    *area,
	     GtkWidget       *widget,
	     const gchar     *detail,
	     gint             x,
	     gint             y,
	     PangoLayout     *layout)
{
  GdkGC *gc;
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);

  gc = use_text ? style->text_gc[state_type] : style->fg_gc[state_type];

  if (area)
    gdk_gc_set_clip_rectangle (gc, area);

  if (state_type == GTK_STATE_INSENSITIVE)
    {
      gdk_draw_layout (window, style->text_gc[state_type], x, y, layout);
    }
  else if (state_type == GTK_STATE_ACTIVE)
    {
      GtkWidget *ans_widget = get_ancestor_of_type (widget, "GtkButton");
      if (ans_widget && GTK_WIDGET_HAS_DEFAULT (ans_widget))
	gdk_draw_layout (window, style->white_gc, x, y, layout);
      else
	gdk_draw_layout (window, gc, x, y, layout);
    }
  else
    gdk_draw_layout (window, gc, x, y, layout);

  if (area)
    gdk_gc_set_clip_rectangle (gc, NULL);

  verbose ("draw\t draw_layout \t-%s-\n", detail ? detail : "no detail");
}
/**************************************************************************/

static void
nimbus_style_init_from_rc (GtkStyle * style,
			       GtkRcStyle * rc_style)
{
  NimbusStyle *nimbus_style = NIMBUS_STYLE (style);
  parent_class->init_from_rc (style, rc_style);
}

static void
nimbus_style_realize (GtkStyle * style)
{
  parent_class->realize (style);
  verbose ("in nimbus_style_realize\n");
}

static void
nimbus_style_unrealize (GtkStyle * style)
{
  parent_class->unrealize (style);
  verbose ("in nimbus_style_unrealize\n");
}

static void
nimbus_style_init (NimbusStyle * style)
{
  verbose ("in nimbus_style_init\n");
}

static void
nimbus_style_class_init (NimbusStyleClass * klass)
{
  GtkStyleClass *style_class = GTK_STYLE_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  style_class->realize = nimbus_style_realize;
  style_class->unrealize = nimbus_style_unrealize;
  style_class->init_from_rc = nimbus_style_init_from_rc; 

  /*
  style_class->draw_polygon = draw_polygon;
  style_class->draw_diamond = draw_diamond; */
  style_class->draw_string = draw_string;
  style_class->draw_hline = draw_hline;
  style_class->draw_vline = draw_vline;
  style_class->draw_shadow = draw_shadow;
  
  style_class->draw_focus = draw_focus;
  style_class->draw_resize_grip = draw_resize_grip;
  style_class->draw_handle = draw_handle;
  style_class->draw_slider = draw_slider;
  style_class->draw_shadow_gap = draw_shadow_gap;
  style_class->draw_tab = draw_tab;
  style_class->draw_arrow = draw_arrow;
  style_class->draw_check = draw_check;
  style_class->draw_box = draw_box;
  style_class->draw_flat_box = draw_flat_box;
  style_class->draw_box_gap = draw_box_gap;
  style_class->draw_extension = draw_extension;
  style_class->draw_option = draw_option;
  style_class->draw_layout = draw_layout;
}

GType nimbus_type_style = 0;

void
nimbus_style_register_type (GTypeModule * module)
{
  static const GTypeInfo object_info =
  {
    sizeof (NimbusStyleClass),
    (GBaseInitFunc) NULL,
    (GBaseFinalizeFunc) NULL,
    (GClassInitFunc) nimbus_style_class_init,
    NULL,			/* class_finalize */
    NULL,			/* class_data */
    sizeof (NimbusStyle),
    0,				/* n_preallocs */
    (GInstanceInitFunc) nimbus_style_init,
    NULL
  };

  nimbus_type_style = g_type_module_register_type (module,
						  GTK_TYPE_STYLE,
						  "NimbusStyle",
						  &object_info, 0);
}
