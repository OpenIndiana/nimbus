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

static void draw_horizontal_box	   (GtkStyle      *style,
				    GdkWindow     *window,
				    GtkStateType   state_type,
				    GtkShadowType  shadow_type,
				    GdkRectangle  *area,
				    GtkWidget     *widget,
				    const gchar   *detail,
				    NimbusButton  *button,
				    gint           x,
				    gint           y,
				    gint           width,
				    gint           height);

static GtkStyleClass *parent_class;

GtkWidget *get_ancestor_of_type (GtkWidget *widget,
				 gchar	 *ancestor_type_s)
{
  GtkWidget *tmp;
  GType ancestor_type = G_TYPE_INVALID;
  g_return_val_if_fail (GTK_WIDGET (widget),FALSE);
  g_return_val_if_fail (ancestor_type_s != NULL, FALSE);

  tmp = widget;
  ancestor_type = g_type_from_name (ancestor_type_s);
  /* printf ("looking for %s\n", ancestor_type_s); */
  
  while (tmp)
    {
      if (G_OBJECT_TYPE (tmp) == ancestor_type)
	  return tmp;
      tmp = tmp->parent;
   /*   if (tmp)
	printf ("get_ancestor_of_type %s\n", g_type_name (G_OBJECT_TYPE (tmp))); */
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
  NimbusRcStyle *rc; 
  
  g_return_if_fail (GTK_IS_STYLE (style));

  rc = NIMBUS_RC_STYLE (style->rc_style);

  if (get_ancestor_of_type (widget, "GtkComboBox"))
    {
      GList *tmp_list;
      int vsep_offset = 0;
      GType researched_type = g_type_from_name ("GtkVSeparator");
      GdkGC *gc = gdk_gc_new (window);
      
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

      draw_horizontal_box (style, window, state_type, shadow_type, area, widget, "combobox_arrow",
			   rc->arrow_button[state_type],
			   x - vsep_offset,
			   widget->parent->parent->allocation.y,
			   widget->parent->parent->allocation.x + widget->parent->parent->allocation.width - x + vsep_offset,
			   widget->parent->parent->allocation.height);

      if (rc->combo_arrow[state_type])
	gdk_draw_pixbuf (window,
			 gc,
			 rc->combo_arrow[state_type],
			 0,0,
			 (x - vsep_offset) + (widget->parent->parent->allocation.x + widget->parent->parent->allocation.width - x + vsep_offset - gdk_pixbuf_get_width (rc->combo_arrow[state_type])) / 2,
			 (widget->parent->parent->allocation.height - gdk_pixbuf_get_height (rc->combo_arrow[state_type])) / 2 ,
			 gdk_pixbuf_get_width (rc->combo_arrow[state_type]),
			 gdk_pixbuf_get_height (rc->combo_arrow[state_type]),
			 GDK_RGB_DITHER_NONE,0,0);

      printf ("combo arrow (%d,%d)  (%d,%d)\n",
	      x - vsep_offset,
	      widget->parent->parent->allocation.y,
	      (x - vsep_offset) + (widget->parent->parent->allocation.x + widget->parent->parent->allocation.width - x + vsep_offset - gdk_pixbuf_get_width (rc->combo_arrow[state_type])) / 2,
	      (widget->parent->parent->allocation.height - gdk_pixbuf_get_height (rc->combo_arrow[state_type])) / 2);
	    
      /*gdk_draw_rectangle (window, style->white_gc, TRUE, 
			  x - vsep_offset,
			  widget->parent->parent->allocation.y,
			  widget->parent->parent->allocation.x + widget->parent->parent->allocation.width - x + vsep_offset,
			  widget->parent->parent->allocation.height);*/
    }
  else
    parent_class->draw_arrow (style, window, state_type, shadow_type, area, widget, detail, arrow_type, fill, x, y, width, height);

  verbose ("draw\t arrow\n");

}

/**************************************************************************/

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
  parent_class->draw_shadow (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);
  verbose ("draw\t shadow \t-%s-\n", detail ? detail : "no detail");
}

/**************************************************************************/

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
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  parent_class->draw_box_gap (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, gap_side, gap_x, gap_width);
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
  g_return_if_fail (GTK_IS_STYLE (style));
  
  parent_class->draw_extension (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, gap_side);

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
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  
  parent_class->draw_handle (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height, orientation);
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
  NimbusStyle *nimbus_style = NIMBUS_STYLE (style);
  
  parent_class->draw_box (style, window, state_type, shadow_type, area,
			  widget, detail, x, y, width, height);
  verbose ("draw\t flat box \t-%s-\n", detail ? detail : "no detail");
}
/**************************************************************************/
static void
draw_horizontal_box (GtkStyle      *style,
		     GdkWindow     *window,
		     GtkStateType   state_type,
		     GtkShadowType  shadow_type,
		     GdkRectangle  *area,
		     GtkWidget     *widget,
		     const gchar   *detail,
		     NimbusButton  *button,
		     gint           x,
		     gint           y,
		     gint           width,
		     gint           height)
{
  NimbusRcStyle *rc; 
  
  g_return_if_fail (GTK_IS_STYLE (style));

  rc = NIMBUS_RC_STYLE (style->rc_style);
    
  if (button)
    {
      GSList *tmp;
      int drop_shadow_offset = (state_type == GTK_STATE_INSENSITIVE) ? 0 : 1;
      GdkGC *gc = gdk_gc_new (window);
      int bottom_left_c_w = 0, bottom_right_c_w = 0;
      
      tmp = button->gradients;
      while (tmp)
	{
	  nimbus_draw_gradient (window, gc, (NimbusGradient*)tmp->data,
				x, y, width, height - drop_shadow_offset);
	  tmp = tmp->next;
	}
	 
      if (button->corner_top_left)
	gdk_draw_pixbuf (window,
			 gc,
			 button->corner_top_left,
			 0,0,
			 x,y,
			 gdk_pixbuf_get_width (button->corner_top_left),
			 gdk_pixbuf_get_height (button->corner_top_left),
			 GDK_RGB_DITHER_NONE,0,0);
      
      if (button->corner_top_right)
	gdk_draw_pixbuf (window,
			 gc,
			 button->corner_top_right,
			 0,0,
			 x+ width - gdk_pixbuf_get_width (button->corner_top_right),
			 y,
			 gdk_pixbuf_get_width (button->corner_top_right),
			 gdk_pixbuf_get_height (button->corner_top_right),
			 GDK_RGB_DITHER_NONE,0,0);

      if (button->corner_bottom_left)
	{
	  bottom_left_c_w = gdk_pixbuf_get_width (button->corner_bottom_left);
	  gdk_draw_pixbuf (window,
			 gc,
			 button->corner_bottom_left,
			 0,0,
			 x, 
			 y + height - gdk_pixbuf_get_height (button->corner_bottom_left),
			 bottom_left_c_w,
			 gdk_pixbuf_get_height (button->corner_bottom_left),
			 GDK_RGB_DITHER_NONE,0,0);
	}
      if (button->corner_bottom_right)
	{
	  bottom_right_c_w = gdk_pixbuf_get_width (button->corner_bottom_right);
	  gdk_draw_pixbuf (window,
			   gc,
			   button->corner_bottom_right,
			   0,0,
			   x+ width - bottom_right_c_w, 
			   y + height - gdk_pixbuf_get_height (button->corner_bottom_right),			 
			   bottom_right_c_w,
			   gdk_pixbuf_get_height (button->corner_bottom_right),
			   GDK_RGB_DITHER_NONE,0,0);
	}
      
      if (drop_shadow_offset)
	gdk_draw_pixbuf (window,
			 gc,
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
  NimbusRcStyle *rc;
  static gboolean should_draw_defaultbutton = FALSE;
  
  g_return_if_fail (GTK_IS_STYLE (style));

  rc = NIMBUS_RC_STYLE (style->rc_style);
  
  if (DETAIL ("buttondefault")) /* needed as buttondefault is just an addon in gtkbutton.c */
    {
      should_draw_defaultbutton = TRUE;
      return;
    }
      
  if (DETAIL ("button"))
    {
      if (should_draw_defaultbutton)
	draw_horizontal_box (style, window, state_type, shadow_type, area,
			     widget, detail, rc->button_default[state_type],
			     x, y, width, height);
      else
	draw_horizontal_box (style, window, state_type, shadow_type, area,
			     widget, detail, rc->button[state_type],
			     x, y, width, height);
      
      should_draw_defaultbutton = FALSE;
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
  g_return_if_fail (GTK_IS_STYLE (style));
  
  parent_class->draw_check (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);

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
  NimbusStyle *nimbus_style = NIMBUS_STYLE (style);
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  
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
  NimbusStyle *nimbus_style = NIMBUS_STYLE (style);
  
  parent_class->draw_option (style, window, state_type, shadow_type, area, widget, detail, x, y, width, height);

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
  g_return_if_fail (GTK_IS_STYLE (style));
  
  parent_class->draw_hline (style, window, state_type, area, widget, detail, x1, x2, y);  
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
  NimbusStyle *nimbus_style = NIMBUS_STYLE (style);
  g_return_if_fail (GTK_IS_STYLE (style));
  g_return_if_fail (window != NULL);
  
  if (!get_ancestor_of_type (widget, "GtkComboBox"))
    {
      parent_class->draw_vline (style, window, state_type, area, widget, detail, y1, y2, x);
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
  
  /* parent_class->draw_focus (style, window, state_type, area, widget, detail, x, y, width, height-1); */
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
  /*style_class->draw_flat_box = draw_flat_box;*/
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
