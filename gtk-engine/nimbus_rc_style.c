/* Nimbus theme engine
 * Copyright (C) 2003 Sun Microsystems, inc 
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
 * Author Erwann Chenede - <erwann.chenede@sun.com>
 * 
 */

#include "nimbus_style.h"
#include "nimbus_rc_style.h"
#include "nimbus_utils.h"
#include "images/images.h"
#include <string.h>

static void      nimbus_rc_style_init         (NimbusRcStyle      *style);
static void      nimbus_rc_style_class_init   (NimbusRcStyleClass *klass);
static GtkStyle *nimbus_rc_style_create_style (GtkRcStyle         *rc_style);
static guint     nimbus_rc_style_parse        (GtkRcStyle         *rc_style,
					       GtkSettings        *settings,
					       GScanner           *scanner);
static void      nimbus_rc_style_merge        (GtkRcStyle         *dest,
					       GtkRcStyle         *src);

static GtkRcStyleClass *parent_class;

GType nimbus_type_rc_style = 0;

void
nimbus_rc_style_register_type (GTypeModule *module)
{
  static const GTypeInfo object_info =
  {
    sizeof (NimbusRcStyleClass),
    (GBaseInitFunc) NULL,
    (GBaseFinalizeFunc) NULL,
    (GClassInitFunc) nimbus_rc_style_class_init,
    NULL,           /* class_finalize */
    NULL,           /* class_data */
    sizeof (NimbusRcStyle),
    0,              /* n_preallocs */
    (GInstanceInitFunc) nimbus_rc_style_init,
    NULL
  };
  
  nimbus_type_rc_style = g_type_module_register_type (module,
						     GTK_TYPE_RC_STYLE,
						     "NimbusRcStyle",
						     &object_info, 0);
}
static GdkPixbuf *
replicate_rows (GdkPixbuf    *src,
		gint          src_x,
		gint          src_y,
		gint          width,
		gint          height)
{
  guint n_channels = gdk_pixbuf_get_n_channels (src);
  guint src_rowstride = gdk_pixbuf_get_rowstride (src);
  guchar *pixels = (gdk_pixbuf_get_pixels (src) + src_y * src_rowstride + src_x * n_channels);
  guchar *dest_pixels;
  GdkPixbuf *result;
  guint dest_rowstride;
  int i;

  result = gdk_pixbuf_new (GDK_COLORSPACE_RGB, n_channels == 4, 8,
			   width, height);
  dest_rowstride = gdk_pixbuf_get_rowstride (result);
  dest_pixels = gdk_pixbuf_get_pixels (result);

  for (i = 0; i < height; i++)
    memcpy (dest_pixels + dest_rowstride * i, pixels, n_channels * width);

  return result;
}

static GdkPixbuf *
replicate_cols (GdkPixbuf    *src,
		gint          src_x,
		gint          src_y,
		gint          width,
		gint          height)
{
  guint n_channels = gdk_pixbuf_get_n_channels (src);
  guint src_rowstride = gdk_pixbuf_get_rowstride (src);
  guchar *pixels = (gdk_pixbuf_get_pixels (src) + src_y * src_rowstride + src_x * n_channels);
  guchar *dest_pixels;
  GdkPixbuf *result;
  guint dest_rowstride;
  int i, j;

  result = gdk_pixbuf_new (GDK_COLORSPACE_RGB, n_channels == 4, 8,
			   width, height);
  dest_rowstride = gdk_pixbuf_get_rowstride (result);
  dest_pixels = gdk_pixbuf_get_pixels (result);

  for (i = 0; i < height; i++)
    {
      guchar *p = dest_pixels + dest_rowstride * i;
      guchar *q = pixels + src_rowstride * i;

      guchar r = *(q++);
      guchar g = *(q++);
      guchar b = *(q++);
      guchar a = 0;
      
      if (n_channels == 4)
	a = *(q++);

      for (j = 0; j < width; j++)
	{
	  *(p++) = r;
	  *(p++) = g;
	  *(p++) = b;

	  if (n_channels == 4)
	    *(p++) = a;
	}
    }

  return result;
}

static void define_normal_button_states (NimbusData *nimbus_rc)
{
  NimbusButton *tmp;
  NimbusGradient *tmp_gradient;
  GError **error = NULL;

  /* button GTK_STATE_NORMAL */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_left = gdk_pixbuf_new_from_inline (-1, button_corner_normal_top_left, FALSE, error);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_corner_normal_top_right, FALSE, error);
  tmp->corner_bottom_left = gdk_pixbuf_new_from_inline (-1, button_corner_normal_bottom_left, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_corner_normal_bottom_right, FALSE, error);
 
  tmp_gradient = nimbus_gradient_new (0,0,1,0, 
				      CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
				      4, 4);
  
  nimbus_gradient_add_segment (tmp_gradient, "#95989e", "#95989e", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#95989e", "#55585e", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#55585e", "#55585e", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  tmp_gradient =  nimbus_gradient_new (1,1,3,2, 
				       CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
				       0, 0);
  
  nimbus_gradient_add_segment (tmp_gradient, "#fbfbfc","#f1f2f4", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#f1f2f4","#d6d9df", 6, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#d6d9df", "#d6d9df", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#d6d9df", "#f4f7fd", 70, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#f4f7fd", "#ffffff",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  nimbus_rc->button[GTK_STATE_NORMAL] = tmp;
  
  /* button GTK_STATE_PRELIGHT */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_left = gdk_pixbuf_new_from_inline (-1, button_corner_prelight_top_left, FALSE, error);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_corner_prelight_top_right, FALSE, error);
  tmp->corner_bottom_left = gdk_pixbuf_new_from_inline (-1, button_corner_prelight_bottom_left, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_corner_prelight_bottom_right, FALSE, error);
 
  tmp_gradient = nimbus_gradient_new (0,0,1,0,
				      CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
				      4, 4);
  
  nimbus_gradient_add_segment (tmp_gradient, "#7a7e86", "#7a7e86", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#7a7e86", "#2a2e36", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#2a2e36", "#2a2e36", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#fdfdfe","#f7f8fa", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#f7f8fa","#e9ecf2", 6, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#e9ecf2", "#e9ecf2", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#e9ecf2", "#ffffff", 70, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#ffffff", "#ffffff",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    
  nimbus_rc->button[GTK_STATE_PRELIGHT] = tmp;
  
  /* button GTK_STATE_ACTIVE */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_left = gdk_pixbuf_new_from_inline (-1, button_corner_active_top_left, FALSE, error);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_corner_active_top_right, FALSE, error);
  tmp->corner_bottom_left = gdk_pixbuf_new_from_inline (-1, button_corner_active_bottom_left, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_corner_active_bottom_right, FALSE, error);
 
  tmp_gradient = nimbus_gradient_new (0,0,1,0, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
4, 4);
  nimbus_gradient_add_segment (tmp_gradient, "#000007", "#000007", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#000007", "#60646c", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#60646c", "#60646c", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#cdd1d8","#c2c7cf", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#c2c7cf","#a4abb8", 6, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#a4abb8", "#ccd3e0", 60, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#ccd3e0", "#e7edfb",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
  
  nimbus_rc->button[GTK_STATE_ACTIVE] = tmp;

  /* button GTK_STATE_INSENSITIVE */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_left = gdk_pixbuf_new_from_inline (-1, button_corner_disabled_top_left, FALSE, error);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_corner_disabled_top_right, FALSE, error);
  tmp->corner_bottom_left = gdk_pixbuf_new_from_inline (-1, button_corner_disabled_bottom_left, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_corner_disabled_bottom_right, FALSE, error);
  
  tmp_gradient = nimbus_gradient_new (0,0,1,0, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
4, 4);
  nimbus_gradient_add_segment (tmp_gradient, "#c9ccd2", "#c9ccd2", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#c9ccd2", "#bcbfc5", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#bcbfc5", "#bcbfc5", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#e3e5e9","#dfe2e6", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#dfe2e6","#d6d9df", 6, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#d6d9df", "#d6d9df", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#d6d9df", "#d8dbe1", 70, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#d8dbe1", "#dadde3",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    
  nimbus_rc->button[GTK_STATE_INSENSITIVE] = tmp;
}
/* for buttondefault  special case */
static void define_default_button_states (NimbusData *nimbus_rc)
{
  NimbusButton *tmp;
  NimbusGradient *tmp_gradient;
  GError **error = NULL;

  memset (nimbus_rc->button_default, 0, sizeof (NimbusButton*));
  
  /* button GTK_STATE_NORMAL */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_left = gdk_pixbuf_new_from_inline (-1, button_default_corner_normal_top_left, FALSE, error);
  tmp->corner_bottom_left = gdk_pixbuf_new_from_inline (-1, button_default_corner_normal_bottom_left, FALSE, error);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_normal_top_right, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_normal_bottom_right, FALSE, error);
 
  tmp_gradient = nimbus_gradient_new (0,0,1,0, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
4, 4);
  nimbus_gradient_add_segment (tmp_gradient, "#62778a", "#62778a", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#62778a", "#22374a", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#22374a", "#22374a", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);


  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#f6f8fa","#dfe6ed", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#dfe6ed","#c0cedb", 6, 27);
  nimbus_gradient_add_segment (tmp_gradient, "#c0cedb","#a3b8cb", 27, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#a3b8cb", "#a3b8cb", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#a3b8cb", "#b0c5d8", 70, 86);
  nimbus_gradient_add_segment (tmp_gradient, "#b0c5d8", "#c1d6e9", 86, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#c1d6e9", "#d5eafd",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  nimbus_rc->button_default[GTK_STATE_NORMAL] = tmp;
  
  /* button GTK_STATE_PRELIGHT */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_left = gdk_pixbuf_new_from_inline (-1, button_default_corner_prelight_top_left, FALSE, error);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_prelight_top_right, FALSE, error);
  tmp->corner_bottom_left = gdk_pixbuf_new_from_inline (-1, button_default_corner_prelight_bottom_left, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_prelight_bottom_right, FALSE, error);
 
  tmp_gradient = nimbus_gradient_new (0,0,1,0, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
4, 4);
  nimbus_gradient_add_segment (tmp_gradient, "#3b556d", "#3b556d", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#3b556d", "#00051d", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#00051d", "#00051d", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#f8fafc","#e6edf3", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#e6edf3","#cddbe8", 6, 27);
  nimbus_gradient_add_segment (tmp_gradient, "#cddbe8","#b6cbde", 27, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#b6cbde", "#b6cbde", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#b6cbde", "#c3d8ec", 70, 86);
  nimbus_gradient_add_segment (tmp_gradient, "#c3d8ec", "#d4e9fc", 86, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#d4e9fc", "#e8fdff",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    
  nimbus_rc->button_default[GTK_STATE_PRELIGHT] = tmp;
  
  /* button GTK_STATE_ACTIVE */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_left = gdk_pixbuf_new_from_inline (-1, button_default_corner_active_top_left, FALSE, error);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_active_top_right, FALSE, error);
  tmp->corner_bottom_left = gdk_pixbuf_new_from_inline (-1, button_default_corner_active_bottom_left, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_active_bottom_right, FALSE, error);
 
  tmp_gradient = nimbus_gradient_new (0,0,1,0, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
4, 4);
  nimbus_gradient_add_segment (tmp_gradient, "#000000", "#000000", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#000000", "#1c3851", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#1c3851", "#1c3851", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT | CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#8fa9c0", "#7695b2", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#7695b2", "#51789c", 6, 27);
  nimbus_gradient_add_segment (tmp_gradient, "#51789c", "#33628c", 27, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#33628c", "#4978a3", 60, 86);
  nimbus_gradient_add_segment (tmp_gradient, "#4978a3", "#5b89b4", 86, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#5b89b4", "#76a4ce",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    
  nimbus_rc->button_default[GTK_STATE_ACTIVE] = tmp;

  /* button GTK_STATE_INSENSITIVE */
  
  nimbus_rc->button_default[GTK_STATE_INSENSITIVE] = nimbus_rc->button[GTK_STATE_INSENSITIVE];
}
void nimbus_init_progress (NimbusData* rc,
			   int height,
			   int width)
{
  GError **error = NULL;
  GdkPixbuf* tmp_pb;
  
  if (rc->progress->border_top == NULL || gdk_pixbuf_get_width (rc->progress->border_top) < width)
    {
      if (rc->progress->border_top)
	gdk_pixbuf_unref (rc->progress->border_top);
      tmp_pb = gdk_pixbuf_new_from_inline (-1, progress_shadow_top, FALSE, error);
      rc->progress->border_top = replicate_cols (tmp_pb, 0,0, width, gdk_pixbuf_get_height (tmp_pb));
      gdk_pixbuf_unref (tmp_pb);     
    }
  if (rc->progress->border_bottom == NULL || gdk_pixbuf_get_width (rc->progress->border_bottom) < width)
    {
      if (rc->progress->border_bottom)
	gdk_pixbuf_unref (rc->progress->border_bottom);
      tmp_pb = gdk_pixbuf_new_from_inline (-1, progress_shadow_bottom, FALSE, error);
      rc->progress->border_bottom  = replicate_cols (tmp_pb, 0,0, width, gdk_pixbuf_get_height (tmp_pb));
      gdk_pixbuf_unref (tmp_pb);     
    }
  if (rc->progress->border_left == NULL || gdk_pixbuf_get_width (rc->progress->border_left) < width)
    {
      if (rc->progress->border_left)
	gdk_pixbuf_unref (rc->progress->border_left);
      tmp_pb = gdk_pixbuf_new_from_inline (-1, progress_shadow_left, FALSE, error);
      rc->progress->border_left = replicate_rows (tmp_pb, 0,0, gdk_pixbuf_get_width (tmp_pb), height);
      gdk_pixbuf_unref (tmp_pb);
    }
  if (rc->progress->border_right == NULL || gdk_pixbuf_get_width (rc->progress->border_right) < width)
    {
      if (rc->progress->border_right)
	gdk_pixbuf_unref (rc->progress->border_right);
      tmp_pb = gdk_pixbuf_new_from_inline (-1, progress_shadow_right, FALSE, error);
      rc->progress->border_right= replicate_rows (tmp_pb, 0,0, gdk_pixbuf_get_width (tmp_pb), height);
      gdk_pixbuf_unref (tmp_pb);
    }
}

static void define_progressbar (NimbusData *rc, int scr_w, int scr_h)
{
  NimbusButton *tmp;
  NimbusGradient *tmp_gradient;
  GError **error = NULL;
  GdkPixbuf* tmp_pb;

  rc->progress = g_new0 (NimbusProgress, 1);

  tmp = g_new0 (NimbusButton, 1);
 
  tmp_gradient = nimbus_gradient_new (0,0,0,0, CORNER_NO_CORNER, 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#983e00", "#a34900", 0, 10);
  nimbus_gradient_add_segment (tmp_gradient, "#a34900", "#a34900", 10, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#a34900", "#e88e28", 60, 90);
  nimbus_gradient_add_segment (tmp_gradient, "#e88e28", "#89310d", 90, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#89310d", "#89310d", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  tmp_gradient =  nimbus_gradient_new (1,1,2,2, CORNER_NO_CORNER, 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#ecd1b3","#dba76b", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#dba76b","#cb7f2a", 6, 45);
  nimbus_gradient_add_segment (tmp_gradient, "#c06600","#e88e28", 45, 85);
  nimbus_gradient_add_segment (tmp_gradient, "#e88e28", "#ffb146", 85, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#ffb146", "#ffb146", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  rc->progress->bar = tmp;

  tmp = g_new0 (NimbusButton, 1);

  tmp_gradient = nimbus_gradient_new (0,0,0,0, CORNER_NO_CORNER, 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#888b91", "#888b91", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#888b91", "#aeb1b7", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#aeb1b7", "#aeb1b7", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  tmp_gradient =  nimbus_gradient_new (1,1,2,2, CORNER_NO_CORNER, 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#ffffff","#eff0f2", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#eff0f2","#dadbde", 6, 45);
  nimbus_gradient_add_segment (tmp_gradient, "#ced0d4","#e3e5e9", 45, 85);
  nimbus_gradient_add_segment (tmp_gradient, "#e3e5e9", "#f9fbff", 85, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#f9fbff", "#f9fbff", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
  
  rc->progress->bkg = tmp;

  /* shadow */
  rc->progress->corner_top_left = gdk_pixbuf_new_from_inline (-1, progress_shadow_corner_top_left, FALSE, error);
  rc->progress->corner_top_right = gdk_pixbuf_new_from_inline (-1, progress_shadow_corner_top_right, FALSE, error);
  rc->progress->corner_bottom_left = gdk_pixbuf_new_from_inline (-1, progress_shadow_corner_bottom_left, FALSE, error);
  rc->progress->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, progress_shadow_corner_bottom_right, FALSE, error);
}

static void define_arrow_button_states (NimbusData *nimbus_rc, gboolean combo_entry)
{
  NimbusButton *tmp;
  NimbusGradient *tmp_gradient;
  GError **error = NULL;

  if (combo_entry)
    memset (nimbus_rc->combo_entry_button, 0, sizeof (NimbusButton*));
  else
    memset (nimbus_rc->arrow_button, 0, sizeof (NimbusButton*));
  
  /* button GTK_STATE_NORMAL */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_normal_top_right, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_normal_bottom_right, FALSE, error);
 
  tmp_gradient = nimbus_gradient_new (0,0,1,0, CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
4, 4);
  nimbus_gradient_add_segment (tmp_gradient, "#62778a", "#62778a", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#62778a", "#22374a", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#22374a", "#22374a", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  if (!combo_entry)
    {
      
      tmp_gradient =  nimbus_gradient_new (0,1,gdk_pixbuf_get_width (tmp->corner_top_right),2, 
					   CORNER_NO_CORNER, 0, 0);
      
      nimbus_gradient_add_segment (tmp_gradient, "#f5f7f9","#dee5eb", 0, 6);
      nimbus_gradient_add_segment (tmp_gradient, "#dee5eb","#b3c1ce", 6, 27);
      nimbus_gradient_add_segment (tmp_gradient, "#b3c1ce","#8da2b5", 27, 60);
      nimbus_gradient_add_segment (tmp_gradient, "#8da2b5", "#91a7ba", 60, 86);
      nimbus_gradient_add_segment (tmp_gradient, "#91a7ba", "#a0b5c8", 86, 100);
      tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    }

  tmp_gradient =  nimbus_gradient_new (1,1,3,2,  CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#f6f8fa","#dfe6ed", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#dfe6ed","#c0cedb", 6, 27);
  nimbus_gradient_add_segment (tmp_gradient, "#c0cedb","#a3b8cb", 27, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#a3b8cb", "#a3b8cb", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#a3b8cb", "#b0c5d8", 70, 86);
  nimbus_gradient_add_segment (tmp_gradient, "#b0c5d8", "#c1d6e9", 86, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#c1d6e9", "#d5eafd",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    
  if (combo_entry)
    nimbus_rc->combo_entry_button[GTK_STATE_NORMAL] = tmp;
  else
    nimbus_rc->arrow_button[GTK_STATE_NORMAL] = tmp;
  
  /* button GTK_STATE_PRELIGHT */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_prelight_top_right, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_prelight_bottom_right, FALSE, error);
 
  tmp_gradient = nimbus_gradient_new (0,0,1,0,  CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT, 4, 4);
  nimbus_gradient_add_segment (tmp_gradient, "#3b556d", "#3b556d", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#3b556d", "#00051d", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#00051d", "#00051d", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  if (!combo_entry)
    {
      tmp_gradient =  nimbus_gradient_new (0,1,gdk_pixbuf_get_width (tmp->corner_top_right),2, 
					   CORNER_NO_CORNER, 0, 0);
      
      nimbus_gradient_add_segment (tmp_gradient, "#f5f7f9","#e0e7ec", 0, 6);
      nimbus_gradient_add_segment (tmp_gradient, "#e0e7ec","#b6c5d3", 6, 27);
      nimbus_gradient_add_segment (tmp_gradient, "#b6c5d3","#92a8bc", 27, 60);
      nimbus_gradient_add_segment (tmp_gradient, "#92a8bc", "#98adc2", 60, 86);
      nimbus_gradient_add_segment (tmp_gradient, "#98adc2", "#a8bace", 86, 100);
      tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    }
  
  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT, 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#f8fafc","#e6edf3", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#e6edf3","#cddbe8", 6, 27);
  nimbus_gradient_add_segment (tmp_gradient, "#cddbe8","#b6cbde", 27, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#b6cbde", "#b6cbde", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#b6cbde", "#c3d8ec", 70, 86);
  nimbus_gradient_add_segment (tmp_gradient, "#c3d8ec", "#d4e9fc", 86, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#d4e9fc", "#e8fdff",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
  
  if (combo_entry)
    nimbus_rc->combo_entry_button[GTK_STATE_PRELIGHT] = tmp;
  else
    nimbus_rc->arrow_button[GTK_STATE_PRELIGHT] = tmp;
  
  /* button GTK_STATE_ACTIVE */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_active_top_right, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_default_corner_active_bottom_right, FALSE, error);
 
  tmp_gradient = nimbus_gradient_new (0,0,1,0, CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT, 4, 4);
  nimbus_gradient_add_segment (tmp_gradient, "#000000", "#000000", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#000000", "#1c3851", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#1c3851", "#1c3851", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  if (!combo_entry)
    {
      tmp_gradient =  nimbus_gradient_new (0,1,gdk_pixbuf_get_width (tmp->corner_top_right),2, 
					   CORNER_NO_CORNER, 0, 0);
      nimbus_gradient_add_segment (tmp_gradient, "#8a9eb1","#738ba2", 0, 6);
      nimbus_gradient_add_segment (tmp_gradient, "#738ba2","#4b6b8a", 6, 27);
      nimbus_gradient_add_segment (tmp_gradient, "#4b6b8a","#295279", 27, 60);
      nimbus_gradient_add_segment (tmp_gradient, "#295279", "#356088", 60, 86);
      nimbus_gradient_add_segment (tmp_gradient, "#356088", "#4c779d", 86, 100);
      tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    }
  
  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT, 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#8fa9c0", "#7695b2", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#7695b2", "#51789c", 6, 27);
  nimbus_gradient_add_segment (tmp_gradient, "#51789c", "#33628c", 27, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#33628c", "#4978a3", 60, 86);
  nimbus_gradient_add_segment (tmp_gradient, "#4978a3", "#5b89b4", 86, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#5b89b4", "#76a4ce",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  if (combo_entry)
    nimbus_rc->combo_entry_button[GTK_STATE_ACTIVE] = tmp;
  else
    nimbus_rc->arrow_button[GTK_STATE_ACTIVE] = tmp;

  /* button GTK_STATE_INSENSITIVE */
  tmp = g_new0 (NimbusButton, 1);
  tmp->corner_top_right = gdk_pixbuf_new_from_inline (-1, button_corner_disabled_top_right, FALSE, error);
  tmp->corner_bottom_right = gdk_pixbuf_new_from_inline (-1, button_corner_disabled_bottom_right, FALSE, error);
  
  tmp_gradient = nimbus_gradient_new (0,0,1,0, CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT, 4, 4);
  nimbus_gradient_add_segment (tmp_gradient, "#c9ccd2", "#c9ccd2", 0, 5);
  nimbus_gradient_add_segment (tmp_gradient, "#c9ccd2", "#bcbfc5", 5, 95);
  nimbus_gradient_add_segment (tmp_gradient, "#bcbfc5", "#bcbfc5", 95, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT, 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#e3e5e9","#dfe2e6", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#dfe2e6","#d6d9df", 6, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#d6d9df", "#d6d9df", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#d6d9df", "#d8dbe1", 70, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#d8dbe1", "#dadde3",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    
  nimbus_rc->arrow_button[GTK_STATE_INSENSITIVE] = tmp;
  nimbus_rc->combo_entry_button[GTK_STATE_INSENSITIVE] = tmp;
}

gboolean my_expose_event_h (GtkWidget        *widget,
			  GdkEventExpose   *event,
			  gpointer          data)
{
  GdkGC *gc;
  int i, x, width;
  NimbusData *rc = ((NimbusRcStyle*) data)->data;
  gboolean tmpbool = FALSE;


  gc = gdk_gc_new (widget->window);

  x = widget->allocation.x;
  width = 20;

  for (i = 0; i < 5; i++)
    {
      if (rc->button[i])
	{
	  GSList *tmp = rc->button[i]->gradients;
	  
	  while (tmp)
	    {
	      NimbusGradient *tmp_gradient = (NimbusGradient*) tmp->data;
	      tmpbool = !tmpbool;
	      nimbus_draw_gradient (widget->window,
				    gtk_widget_get_style (widget),
				    tmp_gradient,
				    x,
				    widget->allocation.y,
				    width,
				    widget->allocation.height,
				    widget->allocation.height / 2, tmpbool,
				    GTK_ORIENTATION_HORIZONTAL,
				    NO_TAB);
	      x += 30;
	      tmp = tmp->next;
	    }
	  x += 20;
	}
    }
   for (i = 0; i < 5; i++)
    {
      if (rc->button_default[i])
	{
	  GSList *tmp = rc->button_default[i]->gradients;
	  
	  while (tmp)
	    {
	      NimbusGradient *tmp_gradient = (NimbusGradient*) tmp->data;
	      tmpbool = !tmpbool;
	      nimbus_draw_gradient (widget->window,
				    gtk_widget_get_style (widget),
				    tmp_gradient,
				    x,
				    widget->allocation.y,
				    width,
				    widget->allocation.height,
				    widget->allocation.height / 2, tmpbool,
				    GTK_ORIENTATION_HORIZONTAL,
				    NO_TAB);
	      x += 30;
	      tmp = tmp->next;
	    }
	  x += 20;
	}
    }
}

static void debug_gradients (NimbusRcStyle *nimbus_rc)
{
  static gboolean started = FALSE;
  GtkWidget *window;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size (GTK_WINDOW (window), 200, 600);

  g_signal_connect (G_OBJECT (window), 
		    "expose_event", 
		    G_CALLBACK (my_expose_event_h),
		    nimbus_rc);
  
  g_signal_connect (G_OBJECT (window), 
		    "delete_event", 
		    G_CALLBACK (gtk_main_quit),
		    NULL);
 
  gtk_widget_show_all (window);



}
void nimbus_init_scrollbar (NimbusData* rc,
			    GtkStateType state, 
			    int size,
			    gboolean horizontal)
{
  GdkPixbuf* tmp_pb , *tmp_pb_bis;
  GError **error = NULL;

  if (horizontal)
    {
      gboolean init_bkg = FALSE;
      gboolean init_slider = FALSE;

      if (rc->scroll_h[state]->bkg == NULL)
	init_bkg = TRUE;
      else if (gdk_pixbuf_get_height (rc->scroll_h[state]->bkg) < size)
	{
	  init_bkg = TRUE;
	  gdk_pixbuf_unref (rc->scroll_h[state]->bkg);
	}
      
      if (rc->scroll_h[state]->slider_mid == NULL)
	init_slider = TRUE;
      else if (gdk_pixbuf_get_height (rc->scroll_h[state]->slider_mid) < size)
	{
	  init_slider = TRUE;
	  gdk_pixbuf_unref (rc->scroll_h[state]->slider_mid);
	}
      
      if (init_bkg)
	switch (state) {
	case GTK_STATE_NORMAL:
	case GTK_STATE_PRELIGHT:
	case GTK_STATE_ACTIVE:
	case GTK_STATE_SELECTED:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_bkg_normal, FALSE, error);
	  rc->scroll_h[GTK_STATE_NORMAL]->bkg = replicate_cols (tmp_pb, 0,0, size, gdk_pixbuf_get_height (tmp_pb));
	  gdk_pixbuf_unref (tmp_pb);
	  rc->scroll_h[GTK_STATE_PRELIGHT]->bkg = rc->scroll_h[GTK_STATE_NORMAL]->bkg;
	  rc->scroll_h[GTK_STATE_ACTIVE]->bkg = rc->scroll_h[GTK_STATE_NORMAL]->bkg;
	break;
	case GTK_STATE_INSENSITIVE:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_bkg_disable, FALSE, error);
	  rc->scroll_h[GTK_STATE_INSENSITIVE]->bkg = replicate_cols (tmp_pb, 0,0, size, gdk_pixbuf_get_height (tmp_pb));
	  gdk_pixbuf_unref (tmp_pb);
	break;
	}
      if (init_slider)
	switch (state) {
	case GTK_STATE_NORMAL:
	case GTK_STATE_INSENSITIVE:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_mid_normal, FALSE, error);
	  rc->scroll_h[GTK_STATE_NORMAL]->slider_mid = replicate_cols (tmp_pb, 0,0, size, gdk_pixbuf_get_height (tmp_pb));
	  gdk_pixbuf_unref (tmp_pb);
	  rc->scroll_h[GTK_STATE_INSENSITIVE]->slider_mid = rc->scroll_h[GTK_STATE_NORMAL]->slider_mid;
	  break;
	case GTK_STATE_PRELIGHT:
	case GTK_STATE_ACTIVE:
	case GTK_STATE_SELECTED:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_mid_prelight, FALSE, error);
	  rc->scroll_h[GTK_STATE_PRELIGHT]->slider_mid = replicate_cols (tmp_pb, 0,0, size, gdk_pixbuf_get_height (tmp_pb));
	  gdk_pixbuf_unref (tmp_pb);
	  rc->scroll_h[GTK_STATE_ACTIVE]->slider_mid = rc->scroll_h[GTK_STATE_PRELIGHT]->slider_mid;
	  break;
	}
    }
  else
    {
     gboolean init_bkg = FALSE;
     gboolean init_slider = FALSE;

      if (rc->scroll_v[state]->bkg == NULL)
	init_bkg = TRUE;
      else if (gdk_pixbuf_get_width (rc->scroll_v[state]->bkg) < size)
	{
	  init_bkg = TRUE;
	  gdk_pixbuf_unref (rc->scroll_v[state]->bkg);
	}
      
      if (rc->scroll_v[state]->slider_mid == NULL)
	init_slider = TRUE;
      else if (gdk_pixbuf_get_width (rc->scroll_v[state]->slider_mid) < size)
	{
	  init_slider = TRUE;
	  gdk_pixbuf_unref (rc->scroll_v[state]->slider_mid);
	}
      
      if (init_bkg)
	switch (state) {
	case GTK_STATE_NORMAL:
	case GTK_STATE_PRELIGHT:
	case GTK_STATE_ACTIVE:
	case GTK_STATE_SELECTED:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_bkg_normal, FALSE, error);
	  tmp_pb_bis = rotate_simple (tmp_pb, ROTATE_COUNTERCLOCKWISE);
	  rc->scroll_v[GTK_STATE_NORMAL]->bkg = replicate_rows (tmp_pb_bis, 0, 0, gdk_pixbuf_get_width (tmp_pb_bis), size);
	  gdk_pixbuf_unref (tmp_pb);  
	  gdk_pixbuf_unref (tmp_pb_bis); 
	  
	  rc->scroll_v[GTK_STATE_PRELIGHT]->bkg = rc->scroll_v[GTK_STATE_NORMAL]->bkg;
	  rc->scroll_v[GTK_STATE_ACTIVE]->bkg = rc->scroll_v[GTK_STATE_NORMAL]->bkg;
	break;
	case GTK_STATE_INSENSITIVE:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_bkg_disable, FALSE, error);
	  tmp_pb_bis = rotate_simple (tmp_pb, ROTATE_COUNTERCLOCKWISE);
	  rc->scroll_v[GTK_STATE_INSENSITIVE]->bkg = replicate_rows (tmp_pb_bis, 0, 0, gdk_pixbuf_get_width (tmp_pb_bis), size);
	  gdk_pixbuf_unref (tmp_pb);  
	  gdk_pixbuf_unref (tmp_pb_bis); 
	break;
	}
      
      if (init_slider)
	switch (state) {
	case GTK_STATE_NORMAL:
	case GTK_STATE_INSENSITIVE:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_mid_normal, FALSE, error);
	  tmp_pb_bis = rotate_simple (tmp_pb, ROTATE_COUNTERCLOCKWISE);
	  rc->scroll_v[GTK_STATE_NORMAL]->slider_mid = replicate_rows (tmp_pb_bis, 0, 0, gdk_pixbuf_get_width (tmp_pb_bis), size);
	  gdk_pixbuf_unref (tmp_pb);  
	  gdk_pixbuf_unref (tmp_pb_bis); 
	  rc->scroll_v[GTK_STATE_INSENSITIVE]->slider_mid = rc->scroll_v[GTK_STATE_NORMAL]->slider_mid;
	  break;
	case GTK_STATE_PRELIGHT:
	case GTK_STATE_ACTIVE:
	case GTK_STATE_SELECTED:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_mid_prelight, FALSE, error);
	  tmp_pb_bis = rotate_simple (tmp_pb, ROTATE_COUNTERCLOCKWISE);
	  rc->scroll_v[GTK_STATE_PRELIGHT]->slider_mid = replicate_rows (tmp_pb_bis, 0, 0, gdk_pixbuf_get_width (tmp_pb_bis), size);
	  gdk_pixbuf_unref (tmp_pb);  
	  gdk_pixbuf_unref (tmp_pb_bis); 
	  rc->scroll_v[GTK_STATE_ACTIVE]->slider_mid = rc->scroll_v[GTK_STATE_PRELIGHT]->slider_mid;
	  break;
	}
    }
}

void nimbus_init_scale (NimbusData* rc,
			GtkStateType state, 
			int size,
			gboolean horizontal)
{

  GdkPixbuf* tmp_pb , *tmp_pb_bis;
  GError **error = NULL;
  gboolean init_bkg = FALSE;
  gboolean init_slider = FALSE;

  if (horizontal)
    {
      if (rc->scale_h[state]->bkg_mid == NULL)
	init_bkg = TRUE;
      else if (gdk_pixbuf_get_width (rc->scale_h[state]->bkg_mid) >= size)
	{
	  init_bkg = TRUE;
	  gdk_pixbuf_unref (rc->scale_h[state]->bkg_mid);
	}
      if (init_bkg)
	switch (state) {
	case GTK_STATE_NORMAL:
	case GTK_STATE_PRELIGHT:
	case GTK_STATE_ACTIVE:
	case GTK_STATE_SELECTED:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scale_corner_mid_normal, FALSE, error);
	  rc->scale_h[GTK_STATE_NORMAL]->bkg_mid = replicate_cols (tmp_pb, 0,0, size, gdk_pixbuf_get_height (tmp_pb));
	  gdk_pixbuf_unref (tmp_pb);
	  rc->scale_h[GTK_STATE_PRELIGHT]->bkg_mid = rc->scale_h[GTK_STATE_NORMAL]->bkg_mid;
	  rc->scale_h[GTK_STATE_ACTIVE]->bkg_mid = rc->scale_h[GTK_STATE_NORMAL]->bkg_mid;
	  break;
	case GTK_STATE_INSENSITIVE:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scale_corner_mid_disable, FALSE, error);
	  rc->scale_h[GTK_STATE_INSENSITIVE]->bkg_mid = replicate_cols (tmp_pb, 0,0, size, gdk_pixbuf_get_height (tmp_pb));
	  gdk_pixbuf_unref (tmp_pb);
	  break;
	}
    }
  else
    {
      if (rc->scale_v[state]->bkg_mid == NULL)
	init_bkg = TRUE;
      else if (gdk_pixbuf_get_height (rc->scale_v[state]->bkg_mid) >= size)
	{
	  init_bkg = TRUE;
	  gdk_pixbuf_unref (rc->scale_v[state]->bkg_mid);
	}
      if (init_bkg)
	switch (state) {
	case GTK_STATE_NORMAL:
	case GTK_STATE_PRELIGHT:
	case GTK_STATE_ACTIVE:
	case GTK_STATE_SELECTED:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scale_corner_mid_normal, FALSE, error);
	  tmp_pb_bis = rotate_simple (tmp_pb, ROTATE_COUNTERCLOCKWISE);
	  rc->scale_v[GTK_STATE_NORMAL]->bkg_mid = replicate_rows (tmp_pb_bis, 0,0, gdk_pixbuf_get_width (tmp_pb_bis), size);
	  gdk_pixbuf_unref (tmp_pb);
	  gdk_pixbuf_unref (tmp_pb_bis);
	  rc->scale_v[GTK_STATE_PRELIGHT]->bkg_mid = rc->scale_v[GTK_STATE_NORMAL]->bkg_mid;
	  rc->scale_v[GTK_STATE_ACTIVE]->bkg_mid = rc->scale_v[GTK_STATE_NORMAL]->bkg_mid;
	  break;
	case GTK_STATE_INSENSITIVE:
	  tmp_pb = gdk_pixbuf_new_from_inline (-1, scale_corner_mid_disable, FALSE, error);
	  tmp_pb_bis = rotate_simple (tmp_pb, ROTATE_COUNTERCLOCKWISE);  
	  rc->scale_v[GTK_STATE_INSENSITIVE]->bkg_mid = replicate_rows (tmp_pb_bis, 0,0, gdk_pixbuf_get_width (tmp_pb_bis), size);
	  gdk_pixbuf_unref (tmp_pb);
	  gdk_pixbuf_unref (tmp_pb_bis);
	  break;
	}
    }
}

void nimbus_init_button_drop_shadow (NimbusData* rc,
				     GtkStateType state, 
				     int size)
{
  if (rc->drop_shadow[state] && gdk_pixbuf_get_width (rc->drop_shadow[state]) >= size)
    return;

  if (rc->drop_shadow[state])
    gdk_pixbuf_unref (rc->drop_shadow[state]);

  rc->drop_shadow[state] =  gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, size + 10, 1);

  /* button shadow */

  if (state == GTK_STATE_ACTIVE) /* white, opacity 60% */
    gdk_pixbuf_fill (rc->drop_shadow[state], 0xffffff99);
  else
    { /* black, opacity 20% */
      gdk_pixbuf_fill (rc->drop_shadow[state], 0x00000033);
      rc->drop_shadow[GTK_STATE_NORMAL] = rc->drop_shadow[state];
      rc->drop_shadow[GTK_STATE_PRELIGHT] = rc->drop_shadow[state];
      rc->drop_shadow[GTK_STATE_SELECTED] = rc->drop_shadow[state];
      rc->drop_shadow[GTK_STATE_INSENSITIVE] = rc->drop_shadow[state];
    }
}


static void nimbus_data_rc_style_init (NimbusRcStyle* nimbus_rc)
{
  GdkPixbuf *tmp_pb, *tmp_pb_bis;
  GError **error = NULL;
  static NimbusData *rc = NULL;
  int screen_w = gdk_screen_get_width (gdk_display_get_default_screen (gdk_display_get_default ()));
  int screen_h = gdk_screen_get_height (gdk_display_get_default_screen (gdk_display_get_default ()));

  if (rc)
    {
      nimbus_rc->data = rc;
      return;
    }
  rc = g_new0 (NimbusData, 1);
  
  define_normal_button_states (rc);
  define_default_button_states (rc);
  define_arrow_button_states (rc, TRUE);
  define_arrow_button_states (rc, FALSE);
  define_progressbar (rc, screen_w, screen_h);

  rc->combo_arrow[GTK_STATE_NORMAL] =  gdk_pixbuf_new_from_inline (-1, combo_caret_normal, FALSE, error);
  rc->combo_arrow[GTK_STATE_PRELIGHT] =  gdk_pixbuf_new_from_inline (-1, combo_caret_prelight, FALSE, error);
  rc->combo_arrow[GTK_STATE_ACTIVE] =  gdk_pixbuf_new_from_inline (-1, combo_caret_active, FALSE, error);
  rc->combo_arrow[GTK_STATE_SELECTED] =  rc->combo_arrow[GTK_STATE_ACTIVE];
  rc->combo_arrow[GTK_STATE_INSENSITIVE] =  gdk_pixbuf_new_from_inline (-1, combo_caret_disable, FALSE, error);

  /* textfield spec */

  rc->textfield_color[GTK_STATE_NORMAL] = g_new0 (NimbusTextfield, 1);
  rc->textfield_color[GTK_STATE_NORMAL]->gradient_line1 = color_cache_get_color ("#8d8e8f");
  rc->textfield_color[GTK_STATE_NORMAL]->gradient_line2 = color_cache_get_color ("#cbcbcc");
  rc->textfield_color[GTK_STATE_NORMAL]->gradient_line3 = color_cache_get_color ("#f4f4f4");
  rc->textfield_color[GTK_STATE_NORMAL]->vertical_line_gradient1= color_cache_get_color ("#989899");
  rc->textfield_color[GTK_STATE_NORMAL]->vertical_line_gradient2 = color_cache_get_color ("#b0b0b1");
  rc->textfield_color[GTK_STATE_NORMAL]->bottom_line = color_cache_get_color ("#c0c0c1");
  rc->textfield_color[GTK_STATE_NORMAL]->vertical_line = color_cache_get_color ("#b8b8b9");

  rc->textfield_color[GTK_STATE_PRELIGHT] = rc->textfield_color[GTK_STATE_NORMAL];
  rc->textfield_color[GTK_STATE_ACTIVE] = rc->textfield_color[GTK_STATE_NORMAL];
  rc->textfield_color[GTK_STATE_SELECTED] = rc->textfield_color[GTK_STATE_NORMAL];

  rc->textfield_color[GTK_STATE_INSENSITIVE] = g_new0 (NimbusTextfield, 1);
  rc->textfield_color[GTK_STATE_INSENSITIVE]->gradient_line1 = color_cache_get_color ("#c7c9ce");
  rc->textfield_color[GTK_STATE_INSENSITIVE]->gradient_line2 = color_cache_get_color ("#d3d6db");
  rc->textfield_color[GTK_STATE_INSENSITIVE]->gradient_line3 = color_cache_get_color ("#dcdee3");
  rc->textfield_color[GTK_STATE_INSENSITIVE]->vertical_line_gradient1= color_cache_get_color ("#c9cbd0");
  rc->textfield_color[GTK_STATE_INSENSITIVE]->vertical_line_gradient2 = color_cache_get_color ("#ced0d5");
  rc->textfield_color[GTK_STATE_INSENSITIVE]->bottom_line = color_cache_get_color ("#d1d3d8");
  rc->textfield_color[GTK_STATE_INSENSITIVE]->vertical_line = color_cache_get_color ("#cfd2d7");

  /* spin separator spec */

  rc->spin_color[GTK_STATE_NORMAL] = g_new0 (NimbusSpinSeparator, 1);
  rc->spin_color[GTK_STATE_NORMAL]->top = color_cache_get_color ("#5d6f80");
  rc->spin_color[GTK_STATE_NORMAL]->bottom = color_cache_get_color ("#ccd7e2");
  
  rc->spin_color[GTK_STATE_PRELIGHT] = g_new0 (NimbusSpinSeparator, 1);
  rc->spin_color[GTK_STATE_PRELIGHT]->top = color_cache_get_color ("#6d8091");
  rc->spin_color[GTK_STATE_PRELIGHT]->bottom = color_cache_get_color ("#dde9f4");
  
  rc->spin_color[GTK_STATE_ACTIVE] = g_new0 (NimbusSpinSeparator, 1);
  rc->spin_color[GTK_STATE_ACTIVE]->top = color_cache_get_color ("#0f2b52");
  rc->spin_color[GTK_STATE_ACTIVE]->bottom = color_cache_get_color ("#6b8dac");

  rc->spin_color[GTK_STATE_SELECTED] = rc->spin_color[GTK_STATE_ACTIVE];

  rc->spin_color[GTK_STATE_INSENSITIVE] = g_new0 (NimbusSpinSeparator, 1);
  rc->spin_color[GTK_STATE_INSENSITIVE]->top = color_cache_get_color ("#bcc2cb");
  rc->spin_color[GTK_STATE_INSENSITIVE]->bottom = color_cache_get_color ("#d4d9e0");

  /* arrows */
  rc->arrow_up[GTK_STATE_NORMAL] = gdk_pixbuf_new_from_inline (-1, arrow_top_normal, FALSE, error);
  rc->arrow_up[GTK_STATE_PRELIGHT] = rc->arrow_up[GTK_STATE_NORMAL];
  rc->arrow_up[GTK_STATE_ACTIVE] = gdk_pixbuf_new_from_inline (-1, arrow_top_active, FALSE, error);
  rc->arrow_up[GTK_STATE_SELECTED] = rc->arrow_up[GTK_STATE_ACTIVE];
  rc->arrow_up[GTK_STATE_INSENSITIVE] = gdk_pixbuf_new_from_inline (-1, arrow_top_disable, FALSE, error);

  rc->arrow_down[GTK_STATE_NORMAL] = gdk_pixbuf_new_from_inline (-1, arrow_bottom_normal, FALSE, error);
  rc->arrow_down[GTK_STATE_PRELIGHT] = rc->arrow_down[GTK_STATE_NORMAL];
  rc->arrow_down[GTK_STATE_ACTIVE] = gdk_pixbuf_new_from_inline (-1, arrow_bottom_active, FALSE, error);
  rc->arrow_down[GTK_STATE_SELECTED] = rc->arrow_down[GTK_STATE_ACTIVE];
  rc->arrow_down[GTK_STATE_INSENSITIVE] = gdk_pixbuf_new_from_inline (-1, arrow_bottom_disable, FALSE, error);

  /* check buttons */

  rc->check_not_set[GTK_STATE_NORMAL] = gdk_pixbuf_new_from_inline (-1, check_normal_not_set, FALSE, error);
  rc->check_not_set[GTK_STATE_PRELIGHT] = gdk_pixbuf_new_from_inline (-1, check_prelight_not_set, FALSE, error);
  rc->check_not_set[GTK_STATE_ACTIVE] = gdk_pixbuf_new_from_inline (-1, check_active_not_set, FALSE, error);
  rc->check_not_set[GTK_STATE_SELECTED] = rc->check_not_set[GTK_STATE_ACTIVE];
  rc->check_not_set[GTK_STATE_INSENSITIVE] = gdk_pixbuf_new_from_inline (-1, check_disable_not_set, FALSE, error);

  rc->check_set[GTK_STATE_NORMAL] = gdk_pixbuf_new_from_inline (-1, check_normal_set, FALSE, error);
  rc->check_set[GTK_STATE_PRELIGHT] = gdk_pixbuf_new_from_inline (-1, check_prelight_set, FALSE, error);
  rc->check_set[GTK_STATE_ACTIVE] = gdk_pixbuf_new_from_inline (-1, check_active_set, FALSE, error);
  rc->check_set[GTK_STATE_SELECTED] = rc->check_set[GTK_STATE_ACTIVE];
  rc->check_set[GTK_STATE_INSENSITIVE] = gdk_pixbuf_new_from_inline (-1, check_disable_set, FALSE, error);

  rc->radio_not_set[GTK_STATE_NORMAL] = gdk_pixbuf_new_from_inline (-1, radio_normal_not_set, FALSE, error);
  rc->radio_not_set[GTK_STATE_PRELIGHT] = gdk_pixbuf_new_from_inline (-1, radio_prelight_not_set, FALSE, error);
  rc->radio_not_set[GTK_STATE_ACTIVE] = gdk_pixbuf_new_from_inline (-1, radio_active_not_set, FALSE, error);
  rc->radio_not_set[GTK_STATE_SELECTED] = rc->radio_not_set[GTK_STATE_ACTIVE];
  rc->radio_not_set[GTK_STATE_INSENSITIVE] = gdk_pixbuf_new_from_inline (-1, radio_disable_not_set, FALSE, error);

  rc->radio_set[GTK_STATE_NORMAL] = gdk_pixbuf_new_from_inline (-1, radio_normal_set, FALSE, error);
  rc->radio_set[GTK_STATE_PRELIGHT] = gdk_pixbuf_new_from_inline (-1, radio_prelight_set, FALSE, error);
  rc->radio_set[GTK_STATE_ACTIVE] = gdk_pixbuf_new_from_inline (-1, radio_active_set, FALSE, error);
  rc->radio_set[GTK_STATE_SELECTED] = rc->radio_set[GTK_STATE_ACTIVE];
  rc->radio_set[GTK_STATE_INSENSITIVE] = gdk_pixbuf_new_from_inline (-1, radio_disable_set, FALSE, error);

  /* horizontal scrollbar */

  rc->scroll_h[GTK_STATE_NORMAL] = g_new0 (NimbusScrollbar, 1);
  rc->scroll_h[GTK_STATE_NORMAL]->button_start = gdk_pixbuf_new_from_inline (-1, scroll_button_h_left_normal, FALSE, error);
  rc->scroll_h[GTK_STATE_NORMAL]->button_end = gdk_pixbuf_new_from_inline (-1, scroll_button_h_right_normal, FALSE, error);
  rc->scroll_h[GTK_STATE_NORMAL]->slider_start = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_left_normal, FALSE, error);
  rc->scroll_h[GTK_STATE_NORMAL]->slider_end = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_right_normal, FALSE, error);

  rc->scroll_h[GTK_STATE_PRELIGHT] = g_new0 (NimbusScrollbar, 1);
  rc->scroll_h[GTK_STATE_PRELIGHT]->button_start = gdk_pixbuf_new_from_inline (-1, scroll_button_h_left_prelight, FALSE, error);
  rc->scroll_h[GTK_STATE_PRELIGHT]->button_end = gdk_pixbuf_new_from_inline (-1, scroll_button_h_right_prelight, FALSE, error);
  rc->scroll_h[GTK_STATE_PRELIGHT]->slider_start = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_left_prelight, FALSE, error);
  rc->scroll_h[GTK_STATE_PRELIGHT]->slider_end = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_right_prelight, FALSE, error);
  
  rc->scroll_h[GTK_STATE_ACTIVE] = g_new0 (NimbusScrollbar, 1);
  rc->scroll_h[GTK_STATE_ACTIVE]->button_start = gdk_pixbuf_new_from_inline (-1, scroll_button_h_left_active, FALSE, error);
  rc->scroll_h[GTK_STATE_ACTIVE]->button_end = gdk_pixbuf_new_from_inline (-1, scroll_button_h_right_active, FALSE, error);
  rc->scroll_h[GTK_STATE_ACTIVE]->slider_start = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_left_active, FALSE, error);
  rc->scroll_h[GTK_STATE_ACTIVE]->slider_end = gdk_pixbuf_new_from_inline (-1, scroll_bar_h_right_active, FALSE, error);
  
  rc->scroll_h[GTK_STATE_SELECTED] = rc->scroll_h[GTK_STATE_ACTIVE];

  rc->scroll_h[GTK_STATE_INSENSITIVE] = g_new0 (NimbusScrollbar, 1);
  rc->scroll_h[GTK_STATE_INSENSITIVE]->button_start =  rc->scroll_h[GTK_STATE_NORMAL]->button_start;
  rc->scroll_h[GTK_STATE_INSENSITIVE]->button_end =  rc->scroll_h[GTK_STATE_NORMAL]->button_end;
  rc->scroll_h[GTK_STATE_INSENSITIVE]->slider_start = rc->scroll_h[GTK_STATE_NORMAL]->slider_end;
  rc->scroll_h[GTK_STATE_INSENSITIVE]->slider_mid = rc->scroll_h[GTK_STATE_NORMAL]->slider_mid;

 /* vertical scrollbar */

  rc->scroll_v[GTK_STATE_NORMAL] = g_new0 (NimbusScrollbar, 1);
  rc->scroll_v[GTK_STATE_NORMAL]->button_start = rotate_simple (rc->scroll_h[GTK_STATE_NORMAL]->button_end, 
									   ROTATE_COUNTERCLOCKWISE);
  rc->scroll_v[GTK_STATE_NORMAL]->button_end = rotate_simple (rc->scroll_h[GTK_STATE_NORMAL]->button_start, 
									 ROTATE_COUNTERCLOCKWISE);
  rc->scroll_v[GTK_STATE_NORMAL]->slider_start = rotate_simple (rc->scroll_h[GTK_STATE_NORMAL]->slider_end,
									 ROTATE_COUNTERCLOCKWISE);
  rc->scroll_v[GTK_STATE_NORMAL]->slider_end = rotate_simple (rc->scroll_h[GTK_STATE_NORMAL]->slider_start,
									 ROTATE_COUNTERCLOCKWISE);

  rc->scroll_v[GTK_STATE_PRELIGHT] = g_new0 (NimbusScrollbar, 1);
  rc->scroll_v[GTK_STATE_PRELIGHT]->button_start = rotate_simple (rc->scroll_h[GTK_STATE_PRELIGHT]->button_end,
									 ROTATE_COUNTERCLOCKWISE);
  rc->scroll_v[GTK_STATE_PRELIGHT]->button_end = rotate_simple (rc->scroll_h[GTK_STATE_PRELIGHT]->button_start,
									 ROTATE_COUNTERCLOCKWISE);
  rc->scroll_v[GTK_STATE_PRELIGHT]->slider_start = rotate_simple (rc->scroll_h[GTK_STATE_PRELIGHT]->slider_end,
									 ROTATE_COUNTERCLOCKWISE);
  rc->scroll_v[GTK_STATE_PRELIGHT]->slider_end = rotate_simple (rc->scroll_h[GTK_STATE_PRELIGHT]->slider_start,
									 ROTATE_COUNTERCLOCKWISE);
  
  rc->scroll_v[GTK_STATE_ACTIVE] = g_new0 (NimbusScrollbar, 1);
  rc->scroll_v[GTK_STATE_ACTIVE]->button_start = rotate_simple (rc->scroll_h[GTK_STATE_ACTIVE]->button_end,
									 ROTATE_COUNTERCLOCKWISE);
  rc->scroll_v[GTK_STATE_ACTIVE]->button_end = rotate_simple (rc->scroll_h[GTK_STATE_ACTIVE]->button_start,
									 ROTATE_COUNTERCLOCKWISE);
  rc->scroll_v[GTK_STATE_ACTIVE]->slider_start = rotate_simple (rc->scroll_h[GTK_STATE_ACTIVE]->slider_end,
									 ROTATE_COUNTERCLOCKWISE);
  rc->scroll_v[GTK_STATE_ACTIVE]->slider_end = rotate_simple (rc->scroll_h[GTK_STATE_ACTIVE]->slider_start,
									 ROTATE_COUNTERCLOCKWISE);
  
  rc->scroll_v[GTK_STATE_SELECTED] = rc->scroll_v[GTK_STATE_ACTIVE];

  rc->scroll_v[GTK_STATE_INSENSITIVE] = g_new0 (NimbusScrollbar, 1);
  rc->scroll_v[GTK_STATE_INSENSITIVE]->button_start =  rc->scroll_v[GTK_STATE_NORMAL]->button_start;
  rc->scroll_v[GTK_STATE_INSENSITIVE]->button_end =  rc->scroll_v[GTK_STATE_NORMAL]->button_end;
  rc->scroll_v[GTK_STATE_INSENSITIVE]->slider_start = rc->scroll_v[GTK_STATE_NORMAL]->slider_end;

  /* panes */

  rc->pane = g_new0 (NimbusPane, 1);
  rc->pane->pane_h = gdk_pixbuf_new_from_inline (-1, pane_h, FALSE, error);
  rc->pane->pane_v = gdk_pixbuf_new_from_inline (-1, pane_v, FALSE, error);
  rc->pane->outline = color_cache_get_color ("#9297a1");
  rc->pane->innerline = color_cache_get_color ("#f4f4f6");

  /* scale horizontal */

  rc->scale_h[GTK_STATE_NORMAL] = g_new0 (NimbusScale, 1);
  rc->scale_h[GTK_STATE_NORMAL]->button = gdk_pixbuf_new_from_inline (-1, scale_button_normal, FALSE, error);
  rc->scale_h[GTK_STATE_NORMAL]->bkg_start = gdk_pixbuf_new_from_inline (-1, scale_corner_left_normal, FALSE, error);
  rc->scale_h[GTK_STATE_NORMAL]->bkg_end = gdk_pixbuf_new_from_inline (-1, scale_corner_right_normal, FALSE, error);

  rc->scale_h[GTK_STATE_PRELIGHT] = g_new0 (NimbusScale, 1);
  rc->scale_h[GTK_STATE_PRELIGHT]->button = gdk_pixbuf_new_from_inline (-1, scale_button_prelight, FALSE, error);
  rc->scale_h[GTK_STATE_PRELIGHT]->bkg_start = rc->scale_h[GTK_STATE_NORMAL]->bkg_start;
  rc->scale_h[GTK_STATE_PRELIGHT]->bkg_end = rc->scale_h[GTK_STATE_NORMAL]->bkg_end;
  
  rc->scale_h[GTK_STATE_ACTIVE] = g_new0 (NimbusScale, 1);
  rc->scale_h[GTK_STATE_ACTIVE]->button = gdk_pixbuf_new_from_inline (-1, scale_button_active, FALSE, error);
  rc->scale_h[GTK_STATE_ACTIVE]->bkg_start = rc->scale_h[GTK_STATE_NORMAL]->bkg_start;
  rc->scale_h[GTK_STATE_ACTIVE]->bkg_end = rc->scale_h[GTK_STATE_NORMAL]->bkg_end;
  
  rc->scale_h[GTK_STATE_SELECTED] = rc->scale_h[GTK_STATE_ACTIVE];

  rc->scale_h[GTK_STATE_INSENSITIVE] = g_new0 (NimbusScale, 1);
  rc->scale_h[GTK_STATE_INSENSITIVE]->button = gdk_pixbuf_new_from_inline (-1, scale_button_disable, FALSE, error);
  rc->scale_h[GTK_STATE_INSENSITIVE]->bkg_start = gdk_pixbuf_new_from_inline (-1, scale_corner_left_disable, FALSE, error);
  rc->scale_h[GTK_STATE_INSENSITIVE]->bkg_end = gdk_pixbuf_new_from_inline (-1, scale_corner_right_disable, FALSE, error);

  /* scale vertical */

  rc->scale_v[GTK_STATE_NORMAL] = g_new0 (NimbusScale, 1);
  rc->scale_v[GTK_STATE_NORMAL]->button = rc->scale_h[GTK_STATE_NORMAL]->button;
  rc->scale_v[GTK_STATE_NORMAL]->bkg_start = rotate_simple (rc->scale_h[GTK_STATE_NORMAL]->bkg_end,
								       ROTATE_COUNTERCLOCKWISE);
  rc->scale_v[GTK_STATE_NORMAL]->bkg_end = rotate_simple (rc->scale_h[GTK_STATE_NORMAL]->bkg_start,
								       ROTATE_COUNTERCLOCKWISE);
 
  rc->scale_v[GTK_STATE_PRELIGHT] = g_new0 (NimbusScale, 1);
  rc->scale_v[GTK_STATE_PRELIGHT]->button = rc->scale_h[GTK_STATE_PRELIGHT]->button;
  rc->scale_v[GTK_STATE_PRELIGHT]->bkg_start = rc->scale_v[GTK_STATE_NORMAL]->bkg_start;
  rc->scale_v[GTK_STATE_PRELIGHT]->bkg_end = rc->scale_v[GTK_STATE_NORMAL]->bkg_end;

  rc->scale_v[GTK_STATE_ACTIVE] = g_new0 (NimbusScale, 1);
  rc->scale_v[GTK_STATE_ACTIVE]->button = rc->scale_h[GTK_STATE_ACTIVE]->button;
  rc->scale_v[GTK_STATE_ACTIVE]->bkg_start = rc->scale_v[GTK_STATE_NORMAL]->bkg_start;
  rc->scale_v[GTK_STATE_ACTIVE]->bkg_end = rc->scale_v[GTK_STATE_NORMAL]->bkg_end;

  rc->scale_v[GTK_STATE_SELECTED] = rc->scale_v[GTK_STATE_ACTIVE];
 
  rc->scale_v[GTK_STATE_INSENSITIVE] = g_new0 (NimbusScale, 1);
  rc->scale_v[GTK_STATE_INSENSITIVE]->button = rc->scale_h[GTK_STATE_INSENSITIVE]->button;
  rc->scale_v[GTK_STATE_INSENSITIVE]->bkg_start = rotate_simple (rc->scale_h[GTK_STATE_INSENSITIVE]->bkg_end,
									    ROTATE_COUNTERCLOCKWISE);
  rc->scale_v[GTK_STATE_INSENSITIVE]->bkg_end = rotate_simple (rc->scale_h[GTK_STATE_INSENSITIVE]->bkg_start,
									  ROTATE_COUNTERCLOCKWISE);
  
  /* tab mini gradient */
  rc->tab[GTK_STATE_NORMAL] = g_new0 (NimbusTab, 1);
  rc->tab[GTK_STATE_NORMAL]->start = color_cache_get_color ("#b5cadd");
  rc->tab[GTK_STATE_NORMAL]->mid = color_cache_get_color ("#bbd0e3");
  rc->tab[GTK_STATE_NORMAL]->end = color_cache_get_color ("#c0d5e8");
  rc->tab[GTK_STATE_NORMAL]->junction = color_cache_get_color ("#bbd0e3");

  rc->tab[GTK_STATE_ACTIVE] = g_new0 (NimbusTab, 1);
  rc->tab[GTK_STATE_ACTIVE]->start = color_cache_get_color ("#406f99");
  rc->tab[GTK_STATE_ACTIVE]->mid = color_cache_get_color ("#4776a0");
  rc->tab[GTK_STATE_ACTIVE]->end = color_cache_get_color ("#4b7aa4");
  rc->tab[GTK_STATE_ACTIVE]->junction = color_cache_get_color ("#406f99");

  rc->tab[GTK_STATE_PRELIGHT] = rc->tab[GTK_STATE_NORMAL];
  rc->tab[GTK_STATE_INSENSITIVE] = rc->tab[GTK_STATE_NORMAL];
  rc->tab[GTK_STATE_SELECTED] = rc->tab[GTK_STATE_ACTIVE];

  /* menubar toolbar border */
  rc->menubar_border = color_cache_get_color ("#9ea3ad");

  /* menubar gradient */

  rc->menubar = nimbus_gradient_new (0,0,1,0, CORNER_NO_CORNER, 0, 0);
  nimbus_gradient_add_segment (rc->menubar, "#f9fafb", "#f4f4f6", 0, 8);
  nimbus_gradient_add_segment (rc->menubar, "#f4f4f6", "#e8e9ed", 8, 16);
  nimbus_gradient_add_segment (rc->menubar, "#e8e9ed", "#dedfe4", 16, 40);
  nimbus_gradient_add_segment (rc->menubar, "#dedfe4", "#d6d9df", 40, 75);
  nimbus_gradient_add_segment (rc->menubar, "#d6d9df", "#d6d9df", 75, 100);

  /*menu border and shadow */

  rc->menu = g_new0 (NimbusMenu, 1);
  rc->menu->border = color_cache_get_color ("#595959");
  rc->menu->shadow = color_cache_get_color ("#eaebee");
  rc->menu->start = color_cache_get_color ("white");
  rc->menu->mid_start = color_cache_get_color ("#fbfcfc");
  rc->menu->mid_end= color_cache_get_color ("#f6f7f9");
  rc->menu->end = color_cache_get_color ("#f1f2f5");

  /* horizontal line */
  rc->hline = color_cache_get_color ("#c7c7c7");
  /* vertical line */
  rc->vline = color_cache_get_color ("#46494f");

  nimbus_rc->data = rc;
}

static void
nimbus_rc_style_init (NimbusRcStyle *nimbus_rc)
{
  nimbus_data_rc_style_init (nimbus_rc);
}




static void
nimbus_rc_style_class_init (NimbusRcStyleClass *klass)
{
  GtkRcStyleClass *rc_style_class = GTK_RC_STYLE_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  rc_style_class->parse = nimbus_rc_style_parse;
  rc_style_class->create_style = nimbus_rc_style_create_style;
  rc_style_class->merge = nimbus_rc_style_merge;
}

static guint
nimbus_rc_style_parse (GtkRcStyle *rc_style,
			   GtkSettings  *settings,
			   GScanner   *scanner)
		     
{
  static GQuark scope_id = 0;
  NimbusRcStyle *nimbus_style = NIMBUS_RC_STYLE (rc_style);

  guint old_scope;
  guint token;
  guint i;
  
  /* Set up a new scope in this scanner. */

  if (!scope_id)
    scope_id = g_quark_from_string("nimbus_theme_engine");

  /* If we bail out due to errors, we *don't* reset the scope, so the
   * error messaging code can make sense of our tokens.
   */
  old_scope = g_scanner_set_scope(scanner, scope_id);

  /* Now check if we already added our symbols to this scope
   * (in some previous call to nimbus_rc_style_parse for the
   * same scanner.
   */

/*  if (!g_scanner_lookup_symbol(scanner, theme_symbols[0].name))
    {
      g_scanner_freeze_symbol_table(scanner);
      for (i = 0; i < G_N_ELEMENTS (theme_symbols); i++)
	g_scanner_scope_add_symbol(scanner, scope_id,
				   theme_symbols[i].name,
				   GINT_TO_POINTER(theme_symbols[i].token));
      g_scanner_thaw_symbol_table(scanner);
    }
*/
  /* We're ready to go, now parse the top level */

  token = g_scanner_peek_next_token(scanner);
  while (token != G_TOKEN_RIGHT_CURLY)
    {
      if (token != G_TOKEN_NONE)
	return token;

      token = g_scanner_peek_next_token(scanner);
    }

  g_scanner_get_next_token(scanner);

  g_scanner_set_scope(scanner, old_scope);

  /* debug_gradients (nimbus_style);  */

  return G_TOKEN_NONE;
}

static void
nimbus_rc_style_merge (GtkRcStyle *dest,
			   GtkRcStyle *src)
{
  parent_class->merge (dest, src);
}


/* Create an empty style suitable to this RC style
 */
static GtkStyle *
nimbus_rc_style_create_style (GtkRcStyle *rc_style)
{
  return GTK_STYLE (g_object_new (NIMBUS_TYPE_STYLE, NULL));
}
