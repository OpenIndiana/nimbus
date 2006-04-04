/* Nimbus Theme Engine
 */

#include <gtk/gtk.h>
#include <glib.h>

typedef struct _NimbusGradient NimbusGradient;
typedef struct _NimbusGradientSegment NimbusGradientSegment;

struct _NimbusGradientSegment 
{ 
  GdkColor *start_color;
  GdkColor *end_color;
  /* in percentages */
  int start_location;
  int end_location;
};

struct _NimbusGradient 
{
  GSList   *segments;
  /* offsets */
  int	   w_start_offset;
  int	   h_start_offset;
  int	   w_end_offset;
  int	   h_end_offset;
  /* corner info */
  gboolean start_corner;
  gboolean end_corner;
  int	   corner_width; /* in pixels */  
  int	   corner_height; /* in pixels */  
};


NimbusGradient* 
nimbus_gradient_new (int       w_start_offset,
		     int       h_start_offset,
		     int       w_end_offset,
		     int       h_end_offset,
		     gboolean  start_corner,
		     gboolean  end_corner,
		     int       corner_width,
		     int       corner_height);

void nimbus_gradient_add_segment (NimbusGradient *gradient,
				  char *start_color,
				  char *end_color,
				  int start_location,
				  int end_location);
void 
nimbus_draw_gradient (GdkWindow *window,
		      GdkGC	   *gc,
		      NimbusGradient *gradient,
		      int	    x,
		      int	    y,
		      int	    width,
		      int	    height);

void 
nimbus_draw_horizontal_gradient (GdkWindow *window,
				 GdkGC	   *gc,
				 GdkColor  *from,
				 GdkColor  *to,
				 int	    x,
				 int	    y,
				 int	    width,
				 int	    height,
				 gboolean   start_corner,
				 gboolean   end_corner,
				 int	    corner_width,
				 int	    corner_height);

