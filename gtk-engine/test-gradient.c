#include <gtk/gtk.h>
#include <gdk/gdkevents.h>
#include "nimbus_utils.h"
#include "images/images.h"
 
NimbusGradient *gradient, *border_gradient;

GdkPixbuf *tl,*tr,*bl,*br;
  
gboolean my_expose_event (GtkWidget        *widget,
			  GdkEventExpose   *event,
			  gpointer          data)
{
  GdkGC *gc;

  gc = gdk_gc_new (widget->window);
  
  /* border gradient */
  nimbus_draw_gradient (widget->window,
			gc,
			border_gradient,
			widget->allocation.x,
			widget->allocation.y,
			widget->allocation.width,
			widget->allocation.height);

  /* main button gradient */
  nimbus_draw_gradient (widget->window,
			gc,
			gradient,
			widget->allocation.x,
			widget->allocation.y,
			widget->allocation.width,
			widget->allocation.height);

/*  gdk_draw_pixbuf (widget->window,
		   gc,
		   tl,
		   0,0,
		   0,0,
		   gdk_pixbuf_get_width (tl),
		   gdk_pixbuf_get_height (tl),
		   GDK_RGB_DITHER_NONE,
		   0,
		   0);
  
  gdk_draw_pixbuf (widget->window,
		   gc,
		   tr,
		   0,0,
		   widget->allocation.width - gdk_pixbuf_get_width (tr),
		   0,
		   gdk_pixbuf_get_width (tr),
		   gdk_pixbuf_get_height (tr),
		   GDK_RGB_DITHER_NONE,
		   0,
		   0);
 
  gdk_draw_pixbuf (widget->window,
		   gc,
		   bl,
		   0,0,
		   0,widget->allocation.height - gdk_pixbuf_get_height (bl),
		   gdk_pixbuf_get_width (bl),
		   gdk_pixbuf_get_height (bl),
		   GDK_RGB_DITHER_NONE,
		   0,
		   0);

  gdk_draw_pixbuf (widget->window,
		   gc,
		   br,
		   0,0,
		   widget->allocation.width - gdk_pixbuf_get_width (bl),
		   widget->allocation.height - gdk_pixbuf_get_height (bl),
		   gdk_pixbuf_get_width (bl),
		   gdk_pixbuf_get_height (bl),
		   GDK_RGB_DITHER_NONE,
		   0,
		   0);*/
  printf ("height = %d\n", widget->allocation.height);

  return TRUE;
}

void main (int argc, char **argv)
{
  GtkWidget *window;
  GError **error = NULL;
 
  gtk_init (&argc, &argv);
 
  gradient = nimbus_gradient_new (0,0,1,0, TRUE, TRUE,4, 4);
  nimbus_gradient_add_segment (gradient, "#62778a", "#62778a", 0, 5);
  nimbus_gradient_add_segment (gradient, "#62778a", "#22374a", 5, 95);
  nimbus_gradient_add_segment (gradient, "#22374a", "#22374a", 95, 100);

  border_gradient=  nimbus_gradient_new (1,1,3,2, TRUE, TRUE, 3, 3);
  nimbus_gradient_add_segment (border_gradient, "#f6f8fa","#dfe6ed", 0, 6);
  nimbus_gradient_add_segment (border_gradient, "#dfe6ed","#c0cedb", 6, 27);
  nimbus_gradient_add_segment (border_gradient, "#c0cedb","#a3b8cb", 27, 60);
  nimbus_gradient_add_segment (border_gradient, "#a3b8cb", "#a3b8cb", 60, 70);
  nimbus_gradient_add_segment (border_gradient, "#a3b8cb", "#b0c5d8", 70, 86);
  nimbus_gradient_add_segment (border_gradient, "#b0c5d8", "#c1d6e9", 86, 96);
  nimbus_gradient_add_segment (border_gradient, "#c1d6e9", "#d5eafd",96, 100);

  tl =  gdk_pixbuf_new_from_inline (-1, button_corner_normal_top_left, FALSE, error);
  tr =  gdk_pixbuf_new_from_inline (-1, button_corner_normal_top_right, FALSE, error);
  bl =  gdk_pixbuf_new_from_inline (-1, button_corner_normal_bottom_left, FALSE, error);
  br =  gdk_pixbuf_new_from_inline (-1, button_corner_normal_bottom_right, FALSE, error);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size (GTK_WINDOW (window), 100, 20);

  g_signal_connect (G_OBJECT (window), 
		    "expose_event", 
		    G_CALLBACK (my_expose_event),
		    gradient);
  
  g_signal_connect (G_OBJECT (window), 
		    "delete_event", 
		    G_CALLBACK (gtk_main_quit),
		    NULL);
 
  gtk_widget_show_all (window);
 
  gtk_main ();
}


