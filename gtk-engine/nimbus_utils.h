#ifndef __NIMBUS_UTILS_H__
#define __NIMBUS_UTILS_H__

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

typedef enum
{
  TAB_POS_LEFT,
  TAB_POS_RIGHT,
  TAB_POS_TOP,
  TAB_POS_BOTTOM,
  NO_TAB,
} NimbusTabPosition;

typedef enum {
        ROTATE_NONE             =   0,
        ROTATE_COUNTERCLOCKWISE =  90,
        ROTATE_UPSIDEDOWN       = 180,
        ROTATE_CLOCKWISE        = 270
} NimbusRotation;

GdkPixbuf *
nimbus_rotate_simple (const GdkPixbuf   *src,
	       NimbusRotation     angle);

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
nimbus_draw_gradient (GdkWindow	       *window,
		      GtkStyle	       *style,
		      GdkRectangle     *clip,
		      NimbusGradient   *gradient,
		      int		x,
		      int		y,
		      int		width,
		      int		height,
		      int		partial_height,
		      gboolean		draw_partial_from_start,
		      GtkOrientation	orientation,
		      NimbusTabPosition	tab_position);



GdkColor *
nimbus_color_cache_get (char *color_name);

GdkGC * 
nimbus_realize_color (GtkStyle * style,
		      GdkColor * color,
		      GdkRectangle *clip);


#endif /* __NIMBUS_UTILS_H__ */
