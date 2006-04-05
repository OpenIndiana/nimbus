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

typedef enum
{
  CORNER_NO_CORNER = 1 << 0,
  CORNER_TOP_LEFT= 1 << 1,
  CORNER_BOTTOM_LEFT = 1 << 2,
  CORNER_TOP_RIGHT = 1 << 3,
  CORNER_BOTTOM_RIGHT= 1 << 4,
} NimbusButtonCorner;


struct _NimbusGradient 
{
  GSList   *segments;
  /* offsets */
  int	   w_start_offset;
  int	   h_start_offset;
  int	   w_end_offset;
  int	   h_end_offset;
  /* corner info */
  NimbusButtonCorner corners;
  int	   corner_width; /* in pixels */  
  int	   corner_height; /* in pixels */  
};


NimbusGradient* 
nimbus_gradient_new (int		w_start_offset,
		     int		h_start_offset,
		     int		w_end_offset,
		     int		h_end_offset,
		     NimbusButtonCorner corners,
		     int		corner_width,
		     int		corner_height);

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
				 NimbusButtonCorner corners,
				 gboolean   start_corner,
				 gboolean   end_corner,
				 int	    corner_width,
				 int	    corner_height);

