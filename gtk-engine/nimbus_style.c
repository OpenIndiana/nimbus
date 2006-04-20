#include <gtk/gtk.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "nimbus_style.h"
#include "nimbus_rc_style.h"
#include "nimbus_utils.h"

char *state_names[5] = {
    "Normal",
    "Active",
    "Prelight",
    "Selected",
    "Insensitive"};

typedef enum
{
  NIMBUS_SPIN_NONE,
  NIMBUS_SPIN_UP,
  NIMBUS_SPIN_DOWN
} NimbusSpinButton;

void
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

GtkWidget *get_ancestor_of_type (GtkWidget *widget,
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
      GdkPixbuf *tmp_pb = NULL;
      GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (widget));

      if (state_type == GTK_STATE_INSENSITIVE)
	return;

      if (arrow_type == GTK_ARROW_DOWN)
	{
	  tmp_pb = rc->scroll_v[state_type]->button_end;
	  offset_y -=  gdk_pixbuf_get_height (tmp_pb) - scroll_button_height;
	  offset_height += gdk_pixbuf_get_height (tmp_pb) - scroll_button_height;
	}
      if (arrow_type == GTK_ARROW_UP)
	{
	  tmp_pb = rc->scroll_v[state_type]->button_start;
	  offset_height += gdk_pixbuf_get_height (tmp_pb) - scroll_button_height;
	}
      if (arrow_type == GTK_ARROW_LEFT)
	{
	  tmp_pb = rc->scroll_h[state_type]->button_start;
	  offset_width += gdk_pixbuf_get_width (tmp_pb) - scroll_button_width;
	}
      if (arrow_type == GTK_ARROW_RIGHT)
	{
	  tmp_pb = rc->scroll_h[state_type]->button_end;
	  offset_x -= gdk_pixbuf_get_width (tmp_pb) - scroll_button_width;
	  offset_width += gdk_pixbuf_get_width (tmp_pb) - scroll_button_width;
	}

      if (tmp_pb)
 	gdk_draw_pixbuf (window,
			 NULL,
			 tmp_pb,
			 0,0,
			 scroll_button_x +  offset_x,
			 scroll_button_y + offset_y,
			 scroll_button_width + offset_width,
			 scroll_button_height + offset_height,
			 GDK_RGB_DITHER_NONE,0,0);


      /* draw the button and slider merge properly as the slider is drawn before 
       * the button.... */
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
	      offset_y += 17;
	      offset_width = -1;
	    }
	  if (arrow_type == GTK_ARROW_LEFT)
	    {
	      tmp_pb = rc->scroll_h[scroll_slider_state]->slider_start;
	      offset_x += 17;
	      offset_height = -1;
	    }
	}
      if (adj->value + adj->page_size == adj->upper)
	{ /* slider and end button merged */
	  if (arrow_type == GTK_ARROW_DOWN)
	    {
	      tmp_pb = rc->scroll_v[scroll_slider_state]->slider_end;
	      offset_width = -1;
	    }
	  if (arrow_type == GTK_ARROW_RIGHT)
	    {
	      tmp_pb = rc->scroll_h[scroll_slider_state]->slider_end;
	      offset_height = -1;
	    }
	}

      if (tmp_pb)   
	  gdk_draw_pixbuf (window,
			   NULL,
			   tmp_pb,
			   0,0,
			   scroll_button_x +  offset_x,
			   scroll_button_y + offset_y,
			   gdk_pixbuf_get_width (tmp_pb) + offset_width,
			   gdk_pixbuf_get_height (tmp_pb) + offset_height,
			   GDK_RGB_DITHER_NONE,0,0);
      
    }
  else if (get_ancestor_of_type (widget, "GtkComboBox"))
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

      draw_nimbus_box (style, window, state_type, shadow_type, area, widget, "combobox_arrow",
		       rc->arrow_button[state_type],
		       FALSE,
		       x - vsep_offset,
		       widget->parent->parent->allocation.y,
		       widget->parent->parent->allocation.x + widget->parent->parent->allocation.width - x + vsep_offset,
		       widget->parent->parent->allocation.height,
		       NIMBUS_SPIN_NONE, GTK_ORIENTATION_HORIZONTAL);

      if (rc->combo_arrow[state_type])
	gdk_draw_pixbuf (window,
			 NULL,
			 rc->combo_arrow[state_type],
			 0,0,
			 (x - vsep_offset) + (widget->parent->parent->allocation.x + widget->parent->parent->allocation.width - x + vsep_offset - gdk_pixbuf_get_width (rc->combo_arrow[state_type])) / 2,
			 widget->parent->parent->allocation.y + (widget->parent->parent->allocation.height - gdk_pixbuf_get_height (rc->combo_arrow[state_type])) / 2 ,
			 gdk_pixbuf_get_width (rc->combo_arrow[state_type]),
			 gdk_pixbuf_get_height (rc->combo_arrow[state_type]),
			 GDK_RGB_DITHER_NONE,0,0);
    }
  else if (get_ancestor_of_type (widget, "GtkCombo") || get_ancestor_of_type (widget, "GtkComboBoxEntry"))
    {
      if (rc->combo_arrow[state_type])
	gdk_draw_pixbuf (window,
			 NULL,
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
      if (arrow_type == GTK_ARROW_UP)
	arrow = rc->arrow_up[state_type];
      if (arrow_type == GTK_ARROW_DOWN)
	arrow = rc->arrow_down[state_type];

      if (arrow)
	gdk_draw_pixbuf (window,
			 NULL,
			 arrow,
			 0,0,
			 x, y,
			 gdk_pixbuf_get_width (arrow),
			 gdk_pixbuf_get_height (arrow),
			 GDK_RGB_DITHER_NONE,0,0);
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
		     rc->arrow_button[state_type],
		     FALSE,
		     tab_x, button_area.y,
		     indicator_size.width + indicator_spacing.right + indicator_spacing.left + widget->style->xthickness,
		     button_area.height, 
		     NIMBUS_SPIN_NONE,
		     GTK_ORIENTATION_HORIZONTAL);

    if (rc->combo_arrow[state_type])
      gdk_draw_pixbuf (window,
		       NULL,
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
      if (get_ancestor_of_type (widget, "GtkCombo") || get_ancestor_of_type (widget, "GtkComboBoxEntry") || get_ancestor_of_type (widget, "GtkSpinButton"))
	general_case = FALSE; /*combo case */
      
      /* work around for a bug in gtkentry were the state isn't set */
      state_type = GTK_WIDGET_STATE(widget);

      /* border gradients points */
      gdk_draw_line (window, 
		     realize_color (style,rc->textfield_color[state_type]->vertical_line_gradient1), 
		     x, y+1, x, y+1);
      
      if (general_case)
	{
	  gdk_draw_line (window, 
			 realize_color (style,rc->textfield_color[state_type]->vertical_line_gradient1), 
			 x+width-1, y+1, x+width-1, y+1);
	    
	  gdk_draw_line (window, 
			 realize_color (style,rc->textfield_color[state_type]->vertical_line_gradient2), 
			 x+width-1, y+2, x+width-1, y+2);
	  

	}

      gdk_draw_line (window, 
		     realize_color (style,rc->textfield_color[state_type]->vertical_line_gradient2), 
		     x, y+2, x, y+2);
      

      /* third gradient line end points as they can't be drawn in draw_flat_box */
      
      gdk_draw_line (window, 
		     realize_color (style,rc->textfield_color[state_type]->gradient_line3), 
		     x+1, y+2, x+1, y+2);

      gdk_draw_line (window, 
		     realize_color (style,rc->textfield_color[state_type]->gradient_line3), 
		     x+width-2, y+2, x+width-2, y+2);

      /* horizontal gradient */
      gdk_draw_line (window, 
		     realize_color (style,rc->textfield_color[state_type]->gradient_line1), 
		     x, y, x + width - 1, y);

      gdk_draw_line (window, 
		     realize_color (style,rc->textfield_color[state_type]->gradient_line2), 
		     x+1, y+1, x+width- (general_case ? 2 :1), y+1);
     
      /* vertical borders */
      gdk_draw_line (window, 
		     realize_color (style,rc->textfield_color[state_type]->vertical_line), 
		     x, y+3, x, y+height-1);
      
      if (general_case)
	gdk_draw_line (window, 
		       realize_color (style,rc->textfield_color[state_type]->vertical_line), 
		       x+width-1, y+3, x+width-1, y+height-1);

      /* bottom line */
      gdk_draw_line (window, 
		     realize_color (style,rc->textfield_color[state_type]->vertical_line), 
		     x+1, 
		     y+height - (general_case ? 1 : 2),
		     x+width - (general_case ? 2 : 1), 
		     y+height - (general_case ? 1 : 2));

      if (!general_case)
	gdk_draw_line (window, 
		       style->bg_gc[GTK_STATE_ACTIVE],  /* hack to get #d6d9df color */
		       x, 
		       y+height - 1,
		       x+width, 
		       y+height -  1);
    }
  else
    parent_class->draw_shadow (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
  verbose ("draw\t shadow \t-%s-\n", detail ? detail : "no detail");
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
      nimbus_draw_gradient (window, style, (NimbusGradient*)tmp->data,
			    x, y, width, height, 
			    -1, TRUE, GTK_ORIENTATION_HORIZONTAL, position);
      tmp = tmp->next;
    }
     
  if (button->corner_top_left && position != TAB_POS_LEFT && position != TAB_POS_TOP)
    gdk_draw_pixbuf (window,
    		 NULL,
    		 button->corner_top_left,
    		 0,0,
    		 x,y,
    		 gdk_pixbuf_get_width (button->corner_top_left),
    		 gdk_pixbuf_get_height (button->corner_top_left),
    		 GDK_RGB_DITHER_NONE,0,0);
  
  if (button->corner_top_right && position != TAB_POS_RIGHT && position != TAB_POS_TOP)
    gdk_draw_pixbuf (window,
    		 NULL,
    		 button->corner_top_right,
    		 0,0,
    		 x+ width - gdk_pixbuf_get_width (button->corner_top_right),
    		 y,
    		 gdk_pixbuf_get_width (button->corner_top_right),
    		 gdk_pixbuf_get_height (button->corner_top_right),
    		 GDK_RGB_DITHER_NONE,0,0);

  if (button->corner_bottom_left && position != TAB_POS_BOTTOM && position != TAB_POS_LEFT)
      gdk_draw_pixbuf (window,
    		   NULL,
    		   button->corner_bottom_left,
    		   0,0,
    		   x, 
    		   y + height - gdk_pixbuf_get_height (button->corner_bottom_left),
    		   gdk_pixbuf_get_width (button->corner_bottom_left),
    		   gdk_pixbuf_get_height (button->corner_bottom_left),
    		   GDK_RGB_DITHER_NONE,0,0);
  if (button->corner_bottom_right && position != TAB_POS_BOTTOM && position != TAB_POS_RIGHT)
      gdk_draw_pixbuf (window,
    		   NULL,
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
      
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->start), 
		     x+1, y + 1, x + width-1, y + 1);
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->mid),
		     x+1, y + 2, x + width-1, y + 2);
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->end),
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
      
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->start), 
		     x+1, y + height - 1, x + width-1, y + height - 1);
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->mid),
		     x+1, y + height - 2, x + width-1, y + height - 2);
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->end),
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
      
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->start), 
		     x+1, y + 1, x+1, y+height-1);
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->mid),
		     x+2, y + 1, x+2, y+height-1);
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->end),
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
      
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->start), 
		     x+width-1, y + 1, x+width-1, y+height-1);
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->mid),
		     x+width-2, y + 1, x+width-2, y+height-1);
      gdk_draw_line (window, realize_color (style, rc->tab[state_type]->end),
		     x+width-3, y + 1, x+width-3, y+height-1);
      gdk_draw_line (window, style->black_gc, x+width-4, y + 1, x+width-4, y+height-1);
          break;
    }

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
    gc = realize_color (style, rc->tab[state_type]->junction);

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

  if (DETAIL ("handlebox") && get_ancestor_of_type (widget, "PanelToplevel"))
    draw_outline = FALSE;

/*  if (DETAIL ("handlebox"))
    {
      GList *list = gtk_container_get_children (GTK_CONTAINER (widget));
      
      for (list; list; list = list->next)
	{
	  printf ("type %s\n", g_type_name (G_OBJECT_TYPE (list->data)));
	  if (GTK_IS_TOOLBAR (list->data))
	    printf ("got a toolbar\n");
	  if (GTK_IS_MENU_BAR (list->data))
	    printf ("got a menubar\n");
	}


    }
  if (get_ancestor_of_type (widget, "GtkToolbar"))
    printf ("got a handlebar in a toolbar\n"); */


  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      pane = rc->pane->pane_h;
      if (draw_outline)
	{
	  gdk_draw_line (window, realize_color (style, rc->pane->outline),
			 x+1, y, x+width-1, y);      
	  gdk_draw_line (window, realize_color (style, rc->pane->outline),
			 x+1, y+height, x+width-1, y+height);      
	  gdk_draw_line (window, realize_color (style, rc->pane->innerline),
			 x+1, y+1, x+width-1, y+1);      
	  gdk_draw_line (window, realize_color (style, rc->pane->innerline),
			 x+1, y+height-1, x+width-1, y+height-1);      
	}
	    
      c_y = 1;
    }
  else
    {
      pane = rc->pane->pane_v;
      if (draw_outline)
	{
	  gdk_draw_line (window, realize_color (style, rc->pane->outline),
			 x, y, x, y+height-1);      
	  gdk_draw_line (window, realize_color (style, rc->pane->outline),
			 x+width-1, y, x+width-1, y+height-1);      
	  gdk_draw_line (window, realize_color (style, rc->pane->innerline),
			 x+1, y, x+1, y+height-1);      
	  gdk_draw_line (window, realize_color (style, rc->pane->innerline),
			 x+width-2, y, x+width-2, y+height-1);      
	}
    }
  
  c_x = (width - gdk_pixbuf_get_width (pane)) / 2;
  c_y += (height - gdk_pixbuf_get_height (pane)) /2;

  gdk_draw_pixbuf (window,
		   NULL,
		   pane,
		   0,0,
		   x + c_x, y + c_y,
		   gdk_pixbuf_get_width (pane),
		   gdk_pixbuf_get_height (pane),
		   GDK_RGB_DITHER_NONE,0,0);
  
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
  if (DETAIL ("entry_bg"))
    {
      NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;

      gdk_draw_line (window, realize_color (style, rc->textfield_color[state_type]->gradient_line3),
		     x, y, x+width, y);
		     
      gdk_draw_rectangle (window, style->bg_gc[state_type], TRUE,
			  x, y+1, width, height-1);
    }
  else
    parent_class->draw_box (style, window, state_type, shadow_type, area,
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
      partial_height = (height - drop_shadow_offset) / 2 + 1;
    }
  if (spin_type == NIMBUS_SPIN_UP)
    {
      draw_bottom = FALSE;
      partial_height = (height - drop_shadow_offset) / 2 + 1;
    }
  
  tmp = button->gradients;
  while (tmp)
    {
      nimbus_draw_gradient (window, style, (NimbusGradient*)tmp->data,
    			x, y, width, height - drop_shadow_offset, 
    			partial_height, draw_partial_from_start,
    			orientation, NO_TAB);
      tmp = tmp->next;
    }
     
  if (button->corner_top_left && draw_top)
    gdk_draw_pixbuf (window,
    		 NULL,
    		 button->corner_top_left,
    		 0,0,
    		 x,y,
    		 gdk_pixbuf_get_width (button->corner_top_left),
    		 gdk_pixbuf_get_height (button->corner_top_left),
    		 GDK_RGB_DITHER_NONE,0,0);
  
  if (button->corner_top_right && draw_top)
    gdk_draw_pixbuf (window,
    		 NULL,
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
    		   NULL,
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
    		   NULL,
    		   button->corner_bottom_right,
    		   0,0,
    		   x+ width - bottom_right_c_w, 
    		   y + height - gdk_pixbuf_get_height (button->corner_bottom_right),			 
    		   bottom_right_c_w,
    		   gdk_pixbuf_get_height (button->corner_bottom_right),
    		   GDK_RGB_DITHER_NONE,0,0);
    }
  
  if ((state_type != GTK_STATE_INSENSITIVE) && drop_shadow && draw_bottom)
    gdk_draw_pixbuf (window,
		     NULL,
		     rc->drop_shadow[state_type],
		     0,0,
		     x + bottom_left_c_w, 
		     y + height-1,
		     width - (bottom_left_c_w + bottom_right_c_w),
		     gdk_pixbuf_get_height (rc->drop_shadow[state_type]),
		     GDK_RGB_DITHER_NONE,0,0);
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
  GtkProgressBarOrientation p_orientation = gtk_progress_bar_get_orientation (GTK_PROGRESS_BAR (widget));
  
  if (p_orientation == GTK_PROGRESS_BOTTOM_TO_TOP ||
      p_orientation == GTK_PROGRESS_TOP_TO_BOTTOM)
    orientation = GTK_ORIENTATION_VERTICAL;

  NimbusProgress *progress = NIMBUS_RC_STYLE (style->rc_style)->data->progress;

  draw_nimbus_box (style, window, state_type, shadow_type, area,
		       widget, detail, progress->bar, FALSE,
		       x, y, width, height, NIMBUS_SPIN_NONE,
		       orientation);
  /* corners */
  gdk_draw_pixbuf (window,			 
		   NULL,
		   progress->corner_top_left,
		   0,0,
		   x - gdk_pixbuf_get_width (progress->corner_top_left),
		   y - gdk_pixbuf_get_height (progress->corner_top_left),
		   gdk_pixbuf_get_width (progress->corner_top_left),
		   gdk_pixbuf_get_height (progress->corner_top_left),
		   GDK_RGB_DITHER_NONE,0,0);
  gdk_draw_pixbuf (window,			 
		   NULL,
		   progress->corner_top_right,
		   0,0,
		   x + width + 1,
		   y - gdk_pixbuf_get_height (progress->corner_top_right),
		   gdk_pixbuf_get_width (progress->corner_top_right),
		   gdk_pixbuf_get_height (progress->corner_top_right),
		   GDK_RGB_DITHER_NONE,0,0);
  gdk_draw_pixbuf (window,			 
		   NULL,
		   progress->corner_bottom_left,
		   0,0,
		   x - gdk_pixbuf_get_width (progress->corner_bottom_left),
		   y + height + 1 - gdk_pixbuf_get_height (progress->corner_bottom_left),
		   gdk_pixbuf_get_width (progress->corner_bottom_left),
		   gdk_pixbuf_get_height (progress->corner_bottom_left),
		   GDK_RGB_DITHER_NONE,0,0);

  gdk_draw_pixbuf (window,			 
		   NULL,
		   progress->corner_bottom_right,
		   0,0,
		   x + width + 1,
		   y + height + 1 - gdk_pixbuf_get_height (progress->corner_bottom_right),
		   gdk_pixbuf_get_width (progress->corner_bottom_right),
		   gdk_pixbuf_get_height (progress->corner_bottom_right),
		   GDK_RGB_DITHER_NONE,0,0);
  /* lines */
  
  gdk_draw_pixbuf (window,			 
		   NULL,
		   progress->border_left,
		   0,0,
		   x - gdk_pixbuf_get_width (progress->border_left), 
		   y,
		   gdk_pixbuf_get_width (progress->border_left),
		   height,
		   GDK_RGB_DITHER_NONE,0,0);

  gdk_draw_pixbuf (window,			 
		   NULL,
		   progress->border_right,
		   0,0,
		   x + width + 1,
		   y,
		   gdk_pixbuf_get_width (progress->border_right),
		   height,
		   GDK_RGB_DITHER_NONE,0,0);
 
  gdk_draw_pixbuf (window,			 
		   NULL,
		   progress->border_top,
		   0,0,
		   x, 
		   y - gdk_pixbuf_get_height (progress->border_top),
		   width + 1,
		   gdk_pixbuf_get_height (progress->border_top),
		   GDK_RGB_DITHER_NONE,0,0);

  gdk_draw_pixbuf (window,			 
		   NULL,
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

  /* printf ("draw box state %s %s\n", state_names [state_type], state_names [GTK_WIDGET_STATE(widget)]); */
      
  if (DETAIL ("button") || DETAIL ("optionmenu"))
    {
      
      if (get_ancestor_of_type (widget, "GtkCombo") || get_ancestor_of_type (widget, "GtkComboBoxEntry"))
	{
	  draw_nimbus_box (style, window, state_type, shadow_type, area,
			   widget, detail, rc->combo_entry_button[state_type], TRUE,
			   x, y, width, height, NIMBUS_SPIN_NONE,
			   GTK_ORIENTATION_HORIZONTAL);
	}
      else
	{
	  if (should_draw_defaultbutton)
	    {
	      x -= 1;  y -= 1; width += 2; height += 2;
	      draw_nimbus_box (style, window, state_type, shadow_type, area,
			       widget, detail, rc->button_default[state_type], TRUE,
			       x, y, width, height, NIMBUS_SPIN_NONE, 
			       GTK_ORIENTATION_HORIZONTAL);
	    }
	  else
	    draw_nimbus_box (style, window, state_type, shadow_type, area,
			     widget, detail, rc->button[state_type], TRUE,
			     x, y, width, height, NIMBUS_SPIN_NONE,
			     GTK_ORIENTATION_HORIZONTAL);
	}
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
	gdk_draw_rectangle (window, style->bg_gc[GTK_STATE_ACTIVE], TRUE,
			      spin_x, spin_y, spin_width, spin_height);
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

	  gdk_draw_line (window, 
			 realize_color (style,rc->spin_color[spin_up_state]->top),
			 spin_x + 1, spin_y + (spin_height/2) - 1, 
			 spin_x + spin_width - 2, spin_y + (spin_height/2) - 1);
	  gdk_draw_line (window, 
			 realize_color (style,rc->spin_color[state_type]->bottom),
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
	      sc = rc->scale_h [state_type];
	      int center_offset = (height - gdk_pixbuf_get_height (sc->bkg_mid)) / 2 + 1;
	      gdk_draw_pixbuf (window,			 
			       NULL,
			       sc->bkg_start,
			       0,0,
			       x,y + center_offset,
			       gdk_pixbuf_get_width (sc->bkg_start),
			       gdk_pixbuf_get_height (sc->bkg_start),
			       GDK_RGB_DITHER_NONE,0,0);
	      gdk_draw_pixbuf (window,			 
			       NULL,
			       sc->bkg_mid,
			       0,0,
			       x+gdk_pixbuf_get_width (sc->bkg_start), y  + center_offset,
			       width - (gdk_pixbuf_get_width (sc->bkg_start) + gdk_pixbuf_get_width (sc->bkg_end)),
			       gdk_pixbuf_get_height (sc->bkg_mid),
			       GDK_RGB_DITHER_NONE,0,0);
	      gdk_draw_pixbuf (window,			 
			       NULL,
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
	      sc = rc->scale_v [state_type];
	      int center_offset = (width - gdk_pixbuf_get_width (sc->bkg_mid)) / 2 + 1;
	      gdk_draw_pixbuf (window,			 
			       NULL,
			       sc->bkg_start,
			       0,0,
			       x + center_offset,y,
			       gdk_pixbuf_get_width (sc->bkg_start),
			       gdk_pixbuf_get_height (sc->bkg_start),
			       GDK_RGB_DITHER_NONE,0,0);
	      gdk_draw_pixbuf (window,			 
			       NULL,
			       sc->bkg_mid,
			       0,0,
			       x + center_offset, y + gdk_pixbuf_get_height (sc->bkg_start),
			       gdk_pixbuf_get_width (sc->bkg_mid),
			       height - (gdk_pixbuf_get_height (sc->bkg_start) + gdk_pixbuf_get_height (sc->bkg_end)) ,
			       GDK_RGB_DITHER_NONE,0,0);
	      gdk_draw_pixbuf (window,			 
			       NULL,
			       sc->bkg_end,
			       0,0,
			       x + center_offset,
			       y + gdk_pixbuf_get_height (sc->bkg_start) + (height - (gdk_pixbuf_get_height (sc->bkg_start) + gdk_pixbuf_get_height (sc->bkg_end))),
			       gdk_pixbuf_get_width (sc->bkg_end),
			       gdk_pixbuf_get_height (sc->bkg_end),
			       GDK_RGB_DITHER_NONE,0,0);
	    }
	}
      else if (GTK_IS_PROGRESS_BAR (widget))
	{
	  GtkOrientation orientation = GTK_ORIENTATION_HORIZONTAL;
	  GtkProgressBarOrientation p_orientation = gtk_progress_bar_get_orientation (GTK_PROGRESS_BAR (widget));

	  if (p_orientation == GTK_PROGRESS_BOTTOM_TO_TOP ||
	      p_orientation == GTK_PROGRESS_TOP_TO_BOTTOM)
	    orientation = GTK_ORIENTATION_VERTICAL;
	  
	  gdk_draw_rectangle (window, style->bg_gc[state_type], TRUE,
			  x, y, width, height);

	  draw_nimbus_box (style, window, state_type, shadow_type, area,
			   widget, detail, rc->progress->bkg, FALSE,
			   x + 2, y + 2, width - 4, height - 4, NIMBUS_SPIN_NONE, orientation);
	}
      else
	{
	  tmp_pb = (width > height) ? rc->scroll_h[state_type]->bkg : rc->scroll_v[state_type]->bkg;
	  
	  gdk_draw_pixbuf (window,
			   NULL,
			   tmp_pb,
			   0,0,
			   x,
			   y,		       
			   width,
			   height,
			   GDK_RGB_DITHER_NONE,0,0);
	}
    }
  else if (DETAIL ("hscrollbar") || DETAIL ("vscrollbar"))
    { /* save coord at the box will be drawn in draw_arrow as we don't have the direction of the box here */
      scroll_button_x = x - 1;
      scroll_button_y = y - 1;
      scroll_button_width = width + 2;
      scroll_button_height = height + 2;
    }
  else if (DETAIL ("bar"))
    draw_progress (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
  else if (DETAIL ("toolbar"))
      gdk_draw_line (window, realize_color (style, rc->menubar_border), x,y+height-1,x+width-1,y+height-1);
  else if (DETAIL ("menu"))
    {
      GdkGC *start, *mid_start, *mid_end, *end;
      sanitize_size (window, &width, &height);
      gdk_draw_rectangle (window, realize_color (style, rc->menu->border), FALSE, x,y,width-1,height-1); 
      gdk_draw_line (window, realize_color (style, rc->menu->shadow), x + width - 2,y+1,x + width - 2,height-2); 
      
      start = realize_color (style, rc->menu->start);
      mid_start = realize_color (style, rc->menu->mid_start);
      mid_end = realize_color (style, rc->menu->mid_end);
      end = realize_color (style, rc->menu->end);
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
      nimbus_draw_gradient (window, style, rc->menubar,
			    x, y, width, height-1, -1, TRUE, GTK_ORIENTATION_HORIZONTAL, NO_TAB);
      gdk_draw_line (window, realize_color (style, rc->menubar_border), x,y+height-1,x+width-1,y+height-1);
    }
  else
    parent_class->draw_box (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
    
  verbose ("draw\t box \t-%s-\n", detail ? detail : "no detail");
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
      state_type = GTK_STATE_INSENSITIVE;
      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
	  image = rc->radio_set;
    }

  gdk_draw_pixbuf (window,			 
		   NULL,
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
	  sb = rc->scroll_h[state_type];
	  gdk_draw_pixbuf (window,			 
			   NULL,
			   sb->slider_start,
			   0,0,
			   x,y-1,
			   gdk_pixbuf_get_width (sb->slider_start),
			   gdk_pixbuf_get_height (sb->slider_start),
			   GDK_RGB_DITHER_NONE,0,0);
	  gdk_draw_pixbuf (window,			 
			   NULL,
			   sb->slider_mid,
			   0,0,
			   x+gdk_pixbuf_get_width (sb->slider_start), y -1,
			   width - (gdk_pixbuf_get_width (sb->slider_start) + gdk_pixbuf_get_width (sb->slider_end)),
			   gdk_pixbuf_get_height (sb->slider_mid),
			   GDK_RGB_DITHER_NONE,0,0);
	  gdk_draw_pixbuf (window,			 
			   NULL,
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
	  sb = rc->scroll_v[state_type];
	  gdk_draw_pixbuf (window,			 
			   NULL,
			   sb->slider_start,
			   0,0,
			   x - 1,y,
			   gdk_pixbuf_get_width (sb->slider_start),
			   gdk_pixbuf_get_height (sb->slider_start),
			   GDK_RGB_DITHER_NONE,0,0);
	  gdk_draw_pixbuf (window,			 
			   NULL,
			   sb->slider_mid,
			   0,0,
			   x - 1, y + gdk_pixbuf_get_height (sb->slider_start),
			   gdk_pixbuf_get_width (sb->slider_mid),
			   height - (gdk_pixbuf_get_height (sb->slider_start) + gdk_pixbuf_get_height (sb->slider_end)) ,
			   GDK_RGB_DITHER_NONE,0,0);
	  gdk_draw_pixbuf (window,			 
			   NULL,
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
		       NULL,
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
      state_type = GTK_STATE_INSENSITIVE;
      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
	  image = rc->radio_set;
    }

  gdk_draw_pixbuf (window,			 
		   NULL,
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

  gdk_draw_line (window, realize_color (style, rc->hline), x1,y,x2,y);
  /* parent_class->draw_hline (style, window, state_type, area, widget, detail, x1, x2, y);   */
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
  int i;
  NimbusData *rc = NIMBUS_RC_STYLE (style->rc_style)->data;
  
  if (!get_ancestor_of_type (widget, "GtkComboBox"))
    {
      GdkGC *color = realize_color (style, rc->vline);
      for (i=0; i <= (y2 - y1); i += 3)
	  gdk_draw_line (window, color, x, y1+i, x, y1+i);
	  
      /* parent_class->draw_vline (style, window, state_type, area, widget, detail, y1, y2, x); */
    }
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
