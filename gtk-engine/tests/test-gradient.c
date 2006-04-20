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
/*  nimbus_draw_gradient (widget->window,
			widget->style,
			border_gradient,
			widget->allocation.x,
			widget->allocation.y,
			widget->allocation.width,
			widget->allocation.height,
			-1, 
			FALSE,
			GTK_ORIENTATION_VERTICAL);*/

  /* main button gradient */
  nimbus_draw_gradient (widget->window,
			widget->style,
			gradient,
			widget->allocation.x,
			widget->allocation.y,
			widget->allocation.width,
			widget->allocation.height,
			-1, FALSE,
			GTK_ORIENTATION_VERTICAL);

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
 
  gradient = nimbus_gradient_new (0,0,0,0, CORNER_NO_CORNER, 0, 0);
  nimbus_gradient_add_segment (gradient, "#983e00", "#a34900", 0, 10);
  nimbus_gradient_add_segment (gradient, "#a34900", "#a34900", 10, 60);
  nimbus_gradient_add_segment (gradient, "#a34900", "#e88e28", 60, 90);
  nimbus_gradient_add_segment (gradient, "#e88e28", "#89310d", 90, 95);
  nimbus_gradient_add_segment (gradient, "#89310d", "#89310d", 95, 100);

  border_gradient=  nimbus_gradient_new (0,0,0,0, CORNER_NO_CORNER, 0, 0);
  nimbus_gradient_add_segment (border_gradient, "#ecd1b3","#dba76b", 0, 6);
  nimbus_gradient_add_segment (border_gradient, "#dba76b","#cb7f2a", 6, 45);
  nimbus_gradient_add_segment (border_gradient, "#c06600","#e88e28", 45, 85);
  nimbus_gradient_add_segment (border_gradient, "#e88e28", "#ffb146", 85, 95);
  nimbus_gradient_add_segment (border_gradient, "#ffb146", "#ffb146", 95, 100);

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


