#include <stdio.h>
#include <string.h>
#include "nimbus_utils.h"

static int x_derivation, y_derivation;

static void 
nimbus_draw_horizontal_gradient (GdkWindow *window,
				 GtkStyle  *style,
				 GdkRectangle *clip,
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
				 int	    corner_height,
				 int	    partial_height,
				 gboolean   draw_partial_from_start);

static void 
nimbus_draw_vertical_gradient (GdkWindow *window,
			       GtkStyle  *style,
			       GdkRectangle *clip,
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
			       int	    corner_height,
			       int	    partial_height,
			       gboolean   draw_partial_from_start);



NimbusGradient* 
nimbus_gradient_new (int		w_start_offset,
		     int		h_start_offset,
		     int		w_end_offset,
		     int		h_end_offset,
		     NimbusButtonCorner corners,
		     int		corner_width,
		     int		corner_height)
{
  NimbusGradient* gradient = g_new0 (NimbusGradient, 1);
  gradient->segments = NULL;
  gradient->w_start_offset = w_start_offset;
  gradient->w_end_offset = w_end_offset;
  gradient->h_start_offset = h_start_offset;
  gradient->h_end_offset = h_end_offset;
  gradient->corners = corners;
  gradient->corner_width = corner_width;
  gradient->corner_height = corner_height;
  return gradient;
}

typedef struct _CachedColor CachedColor;

struct _CachedColor 
{
  GdkColor *color;
  char *name;
};

static GSList *colors_list = NULL;

static gint color_cache_compare (CachedColor *tmp, char *color)
{
  return strcmp (color, tmp->name);
}


GdkColor *
nimbus_color_cache_get (char *color_name)
{
  CachedColor *color;
  GSList *cached_color = g_slist_find_custom (colors_list, 
					      color_name,
					      (GCompareFunc)color_cache_compare);
  if (cached_color)
    return ((CachedColor*) cached_color->data)->color;

  color = g_new (CachedColor, 1);

  color->color = g_new0 (GdkColor, 1);

  gdk_color_parse (color_name, color->color);

  color->name = g_strdup (color_name);

  colors_list = g_slist_append (colors_list, color);

  return color->color;
}

void 
nimbus_gradient_add_segment (NimbusGradient *gradient,
			     char *start_color,
			     char *end_color,
			     int start_location,
			     int end_location)
{
  NimbusGradientSegment* seg = g_new (NimbusGradientSegment, 1);

  seg->start_color = nimbus_color_cache_get (start_color);
  seg->end_color = nimbus_color_cache_get(end_color);
  seg->start_location = start_location;
  seg->end_location = end_location;
  gradient->segments = g_slist_append (gradient->segments, seg);
}

static void 
draw_gradient_segment (GdkWindow	     *window,
		       GtkStyle		     *style,
		       GdkRectangle	     *clip,
		       NimbusGradient	     *gradient,
		       NimbusGradientSegment *seg,
		       int		      x,
		       int		      y,
		       int		      width,
		       int		      height,
		       int		      partial_height,
		       gboolean		      draw_partial_from_start,
		       GtkOrientation	      orientation,
		       NimbusTabPosition      tab_position)
{
  int start_length = 0;
  int end_length = 0;
  int length = 0;
  int rendering_length = 0;
  int segment_corner_height = 0;
  int segment_corner_width = 0;
  gboolean start_corner = FALSE;
  gboolean end_corner = FALSE;

  /* determine the length of the segment in pixel */

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    rendering_length = height;
  else
    rendering_length = width;
  
  if (seg->start_location != 0)
    start_length = (seg->start_location * rendering_length) / 100;
  if (seg->end_location != 0)
    end_length = (seg->end_location * rendering_length) / 100;

   length = end_length - start_length;

   if (partial_height != -1)
     {
       /* don't draw the gradient if the start_length is after partial_height */
       if ((start_length >= partial_height) && draw_partial_from_start)
	 return;
       /* same for partial height ending at the bottom of the gradient */
       if ((end_length <= partial_height) && !draw_partial_from_start)
	 return;
	 
	if (!draw_partial_from_start)
	  {/* remove the h_start_offset as the partial draw doesn't
	      beginning at the start of the segment */
	    if (gradient->h_start_offset)
	      {
		y -= gradient->h_start_offset; 
		if (length == 1)
		  length++;
	      }
	  }
     }

   /* determine how many lines should be truncated for this segment */
   if (orientation == GTK_ORIENTATION_HORIZONTAL)
     {
       if ((((gradient->corners & CORNER_TOP_RIGHT) != 0 ) ||
	    ((gradient->corners & CORNER_TOP_LEFT) != 0 )) && 
	   (start_length < gradient->corner_height))
	 
	 {
	   segment_corner_height = gradient->corner_height - start_length;
	   start_corner = TRUE;
	 }
       
       if ((((gradient->corners & CORNER_BOTTOM_RIGHT) != 0 ) ||
	    ((gradient->corners & CORNER_BOTTOM_LEFT) != 0 )) && 
	   ((height - end_length) < gradient->corner_height))
	 {
	   segment_corner_height = gradient->corner_height - (height - end_length);
	   end_corner = TRUE;
	 }
       nimbus_draw_horizontal_gradient (window, 
					style,
					clip,
					seg->start_color, seg->end_color, 
					x,
					y + start_length, 
					width,
					length,
					gradient->corners,
					start_corner,
					end_corner,
					gradient->corner_width,
					segment_corner_height,
					partial_height,
					draw_partial_from_start);
     }
   else
     {
       if ((((gradient->corners & CORNER_TOP_LEFT) != 0 ) ||
	    ((gradient->corners & CORNER_BOTTOM_LEFT) != 0 )) && 
	   (start_length < gradient->corner_width))
	 
	 {
	   segment_corner_width = gradient->corner_width - start_length;
	   start_corner = TRUE;
	 }
       
       if ((((gradient->corners & CORNER_TOP_RIGHT) != 0 ) ||
	    ((gradient->corners & CORNER_BOTTOM_RIGHT) != 0 )) && 
	   ((width - end_length) < gradient->corner_width))
	 {
	   segment_corner_width = gradient->corner_width - (width - end_length);
	   end_corner = TRUE;
	 }

       
       nimbus_draw_vertical_gradient (window, 
				      style,
				      clip,
				      seg->start_color, seg->end_color, 
				      x + start_length,
				      y, 
				      length,
				      height,
				      gradient->corners,
				      start_corner,
				      end_corner,
				      segment_corner_width,
				      gradient->corner_height,
				      partial_height,
				      draw_partial_from_start);       
     }
}

void 
nimbus_draw_gradient (GdkWindow*	window,
		      GtkStyle*		style,
		      GdkRectangle     *clip,
		      NimbusGradient*	gradient,
		      int		x,
		      int		y,
		      int		width,
		      int		height,
		      int		partial_height,
		      gboolean		draw_partial_from_start,
		      GtkOrientation	orientation,
		      NimbusTabPosition	tab_position)
{
 GSList *tmp = gradient->segments;
 NimbusButtonCorner corners = gradient->corners;

 x_derivation = x;
 y_derivation = y;

 if (tab_position != NO_TAB)
   {
     if (tab_position == TAB_POS_LEFT)
       gradient->corners = CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT;
     if (tab_position == TAB_POS_RIGHT)
       gradient->corners = CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT;
     if (tab_position == TAB_POS_TOP)
       gradient->corners = CORNER_BOTTOM_RIGHT | CORNER_BOTTOM_LEFT;
     if (tab_position == TAB_POS_BOTTOM)
       gradient->corners = CORNER_TOP_RIGHT | CORNER_TOP_LEFT;
   }
     
 
 while (tmp)
   {
     draw_gradient_segment (window, style, clip, gradient,
			    (NimbusGradientSegment*) tmp->data,
			    x + gradient->w_start_offset, 
			    y + gradient->h_start_offset,
			    width - gradient->w_end_offset,
			    height - gradient->h_end_offset,
			    partial_height,
			    draw_partial_from_start,
			    orientation,
			    tab_position);
      tmp = tmp->next;
    }
 gradient->corners = corners;
}

static void 
nimbus_draw_horizontal_gradient (GdkWindow *window,
				 GtkStyle  *style,
				 GdkRectangle *clip,
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
				 int	    corner_height,
				 int	    partial_height,
				 gboolean   draw_partial_from_start)
{
  GdkColormap *sys_lut = gdk_colormap_get_system ();
  long delta_r;
  long delta_g;
  long delta_b;
  int i;

  if (to->red == 0 && to->green && to->blue)
    {
      delta_r = 65535 - from->red;
      delta_g = 65535 - from->green;
      delta_b = 65535 - from->blue;
    }

  delta_r = to->red - from->red;
  delta_g = to->green - from->green;
  delta_b = to->blue - from->blue;

  /* printf ("window (%x) gc (%x)\n", window, gc); 
   printf ("delta rgb (%d,%d,%d)\n", delta_r, delta_g, delta_b);  
   printf ("x,y,width, height = (%d,%d,%d,%d)\n", x,y,width, height); */

  for (i=0; i<height; i++)
    {
      GdkGC *gc;
      int offset_x = 0, offset_w = 0;
      GdkColor color = { 0, 
			from->red + (delta_r * i) / height,
			from->green + (delta_g * i) / height,
			from->blue + (delta_b * i) / height};
     /* GdkColor color = { 0, 0, 0, 60000};  */

      gc = nimbus_realize_color (style, &color, clip);
		     
      if (i < corner_height && start_corner) /* troncate start of this gradient */
	{
	  if (corners & CORNER_TOP_LEFT)
	    offset_x = corner_width;
	  if (corners & CORNER_TOP_RIGHT)
	    offset_w = corner_width;
	}
      else if (((height - (i+1)) < corner_height) && end_corner) /* troncate end of this gradient */
	{
	  if (corners & CORNER_BOTTOM_LEFT) 
	    offset_x = corner_width;
	  if (corners & CORNER_BOTTOM_RIGHT) 
	    offset_w = corner_width;
	}
      else {/* don't troncate this gradient */}

      if (partial_height == -1)
	gdk_draw_line (window, gc, x + offset_x, y+i, x + width - offset_w, y+i);
      else
	{
	  /* spin button upper part case where a gradient can be drawn partially from the start*/
	  if (draw_partial_from_start)
	    {
	      if (y-y_derivation+i >= partial_height) 
	        return;
	      gdk_draw_line (window, gc, x + offset_x, y+i, x + width - offset_w, y+i);
	    }
	  else
	    {
	      /* spin button lower part case where a gradient can be drawn partially from the bottom*/
	      if (y-y_derivation+i >= partial_height)
		{
 	  	  gdk_draw_line (window, gc, x + offset_x, y+i, x + width - offset_w, y+i);
		}
	    }
	}
      gdk_gc_set_clip_rectangle (gc, NULL);

      /* printf ("color is (%d,%d,%d) x, y, x_end, y_end  = (%d,%d,%d,%d) \n", 
	      color.red >> 8, color.green >> 8 , color.blue >> 8,
	       x, y+i, x+width, y+i); */
    }
}

static void 
nimbus_draw_vertical_gradient (GdkWindow *window,
			       GtkStyle  *style,
			       GdkRectangle *clip,
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
			       int	    corner_height,
			       int	    partial_height,
			       gboolean   draw_partial_from_start)
{
  GdkColormap *sys_lut = gdk_colormap_get_system ();
  long delta_r;
  long delta_g;
  long delta_b;
  int i;

  if (to->red == 0 && to->green && to->blue)
    {
      delta_r = 65535 - from->red;
      delta_g = 65535 - from->green;
      delta_b = 65535 - from->blue;
    }

  delta_r = to->red - from->red;
  delta_g = to->green - from->green;
  delta_b = to->blue - from->blue;

/*   printf ("delta rgb (%d,%d,%d)\n", delta_r, delta_g, delta_b);  
   printf ("x,y,width, height = (%d,%d,%d,%d)\n", x,y,width, height); */

  for (i=0; i<width; i++)
    {
      GdkGC *gc;
      int offset_y = 0, offset_h = 0;
      GdkColor color = { 0, 
			from->red + (delta_r * i) / width,
			from->green + (delta_g * i) / width,
			from->blue + (delta_b * i) / width};
     /* GdkColor color = { 0, 0, 0, 60000};  */

      gc = nimbus_realize_color (style, &color, clip);
		     
      if (i < corner_width && start_corner) /* troncate start of this gradient */
	{
	  if (corners & CORNER_TOP_LEFT)
	    offset_y = corner_width;
	  if (corners & CORNER_BOTTOM_LEFT)
	    offset_h = corner_width;
	}
      else if (((width - (i+1)) < corner_width) && end_corner) /* troncate end of this gradient */
	{
	  if (corners & CORNER_TOP_RIGHT) 
	    offset_y = corner_width;
	  if (corners & CORNER_BOTTOM_RIGHT) 
	    offset_h = corner_width;
	}
      else {/* don't troncate this gradient */}

      if (partial_height == -1)
	gdk_draw_line (window, gc, x+i, y+offset_y, x+i, y+ height - offset_h);
      else
	{
	  /* partial_height not implemented for vertical gradient */
	}
      gdk_gc_set_clip_rectangle (gc, NULL);

/*      printf ("color is (%d,%d,%d) x, y, x_end, y_end  = (%d,%d,%d,%d) \n", 
	      color.red >> 8, color.green >> 8 , color.blue >> 8,
	       x+i, y+offset_y, x+i, y+ height - offset_h);*/
    }
}


GdkGC *
nimbus_realize_color (GtkStyle * style,
		      GdkColor * color,
		      GdkRectangle *clip)
{
  GdkGCValues gc_values;
  GdkGC *tmp = NULL;

  gdk_colormap_alloc_color (style->colormap, color,
			    FALSE, TRUE);

  gc_values.foreground = *color;

  tmp = gtk_gc_get (style->depth, style->colormap, &gc_values, GDK_GC_FOREGROUND);
  if (clip) 
    gdk_gc_set_clip_rectangle (tmp, clip);
  else
    gdk_gc_set_clip_rectangle (tmp, NULL);

  return tmp;
}

/* modified copy of gdk_pixbuf_nimbus_rotate_simple for compatibility with gtk 2.4
 */

#define OFFSET(pb, x, y, n_channel, rowstride) ((x) * (n_channel) + (y) * (rowstride))

GdkPixbuf *
nimbus_rotate_simple (const GdkPixbuf   *src,
	       NimbusRotation     angle)
{
  GdkPixbuf *dest;
  guchar *p, *q;
  gint x, y;
  gint src_height, src_width;
  gint n_channels_src, n_channels_dest;
  gint rowstride_src, rowstride_dest;
  guchar* pixels_dest, *pixels_src;

  src_height = gdk_pixbuf_get_height (src);
  src_width = gdk_pixbuf_get_width (src);
  

  switch (angle % 360)
    {
    case 0:
      dest = gdk_pixbuf_copy (src);
      break;
    case 90:
      dest = gdk_pixbuf_new (gdk_pixbuf_get_colorspace (src), 
			     gdk_pixbuf_get_has_alpha (src), 
			     gdk_pixbuf_get_bits_per_sample (src), 
			     src_height, 
			     src_width);
      if (!dest)
	return NULL;

      n_channels_dest = gdk_pixbuf_get_n_channels (dest);
      n_channels_src = gdk_pixbuf_get_n_channels (src);

      rowstride_src = gdk_pixbuf_get_rowstride (src);
      rowstride_dest = gdk_pixbuf_get_rowstride (dest);

      pixels_dest = gdk_pixbuf_get_pixels (dest);
      pixels_src = gdk_pixbuf_get_pixels (src);

      for (y = 0; y < src_height; y++) 
	{ 
	  for (x = 0; x < src_width; x++) 
	    { 
	      p = pixels_src + OFFSET (src, x, y, n_channels_src, rowstride_src); 
	      q = pixels_dest + OFFSET (dest, y, src_width- x - 1, n_channels_dest, rowstride_dest); 
	      memcpy (q, p, n_channels_dest);
	    }
	} 
      break;
    case 180:
      dest = gdk_pixbuf_new (gdk_pixbuf_get_colorspace (src), 
			     gdk_pixbuf_get_has_alpha (src), 
			     gdk_pixbuf_get_bits_per_sample (src),
			     src_width, 
			     src_height);
      if (!dest)
	return NULL;

      n_channels_dest = gdk_pixbuf_get_n_channels (dest);
      n_channels_src = gdk_pixbuf_get_n_channels (src);

      rowstride_src = gdk_pixbuf_get_rowstride (src);
      rowstride_dest = gdk_pixbuf_get_rowstride (dest);

      pixels_dest = gdk_pixbuf_get_pixels (dest);
      pixels_src = gdk_pixbuf_get_pixels (src);
      
      for (y = 0; y < src_height; y++) 
	{ 
	  for (x = 0; x < src_width; x++) 
	    { 
	      p = pixels_src + OFFSET (src, x, y, n_channels_src, rowstride_src); 
	      q = pixels_dest + OFFSET (dest, src_width- x - 1, src_height- y - 1, n_channels_dest, rowstride_dest); 
	      memcpy (q, p, n_channels_dest);
	    }
	} 
      break;
    case 270:
      dest = gdk_pixbuf_new (gdk_pixbuf_get_colorspace (src), 
			     gdk_pixbuf_get_has_alpha (src), 
			     gdk_pixbuf_get_bits_per_sample (src),
			     src_height, 
			     src_width);
      if (!dest)
	return NULL;

      n_channels_dest = gdk_pixbuf_get_n_channels (dest);
      n_channels_src = gdk_pixbuf_get_n_channels (src);

      rowstride_src = gdk_pixbuf_get_rowstride (src);
      rowstride_dest = gdk_pixbuf_get_rowstride (dest);

      pixels_dest = gdk_pixbuf_get_pixels (dest);
      pixels_src = gdk_pixbuf_get_pixels (src);

      for (y = 0; y < src_height; y++) 
	{ 
	  for (x = 0; x < src_width; x++) 
	    { 
	      p = pixels_src + OFFSET (src, x, y, n_channels_src, rowstride_src); 
	      q = pixels_dest + OFFSET (dest, src_height- y - 1, x, n_channels_dest, rowstride_dest); 
	      memcpy (q, p, n_channels_dest);
	    }
	} 
      break;
    default:
      dest = NULL;
      g_warning ("nimbus_rotate_simple() can only rotate "
		 "by multiples of 90 degrees");
      g_assert_not_reached ();
  } 

  return dest;
}

