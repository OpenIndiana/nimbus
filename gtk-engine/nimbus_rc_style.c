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

static void define_normal_button_states (NimbusRcStyle *nimbus_rc)
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
  nimbus_gradient_add_segment (tmp_gradient, "#e3efe9","#dfe2e6", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#dfe2e6","#d6d9df", 6, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#d6d9df", "#d6d9df", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#d6d9df", "#d8dbe1", 70, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#d8dbe1", "#dadde3",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    
  nimbus_rc->button[GTK_STATE_INSENSITIVE] = tmp;
}
/* for buttondefault  special case */
static void define_default_button_states (NimbusRcStyle *nimbus_rc)
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

static void define_arrow_button_states (NimbusRcStyle *nimbus_rc)
{
  NimbusButton *tmp;
  NimbusGradient *tmp_gradient;
  GError **error = NULL;

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

  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT, 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#f8fafc","#e6edf3", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#e6edf3","#cddbe8", 6, 27);
  nimbus_gradient_add_segment (tmp_gradient, "#cddbe8","#b6cbde", 27, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#b6cbde", "#b6cbde", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#b6cbde", "#c3d8ec", 70, 86);
  nimbus_gradient_add_segment (tmp_gradient, "#c3d8ec", "#d4e9fc", 86, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#d4e9fc", "#e8fdff",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
  
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

  tmp_gradient =  nimbus_gradient_new (1,1,3,2, CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT, 0, 0);
  nimbus_gradient_add_segment (tmp_gradient, "#8fa9c0", "#7695b2", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#7695b2", "#51789c", 6, 27);
  nimbus_gradient_add_segment (tmp_gradient, "#51789c", "#33628c", 27, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#33628c", "#4978a3", 60, 86);
  nimbus_gradient_add_segment (tmp_gradient, "#4978a3", "#5b89b4", 86, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#5b89b4", "#76a4ce",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);

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
  nimbus_gradient_add_segment (tmp_gradient, "#e3efe9","#dfe2e6", 0, 6);
  nimbus_gradient_add_segment (tmp_gradient, "#dfe2e6","#d6d9df", 6, 60);
  nimbus_gradient_add_segment (tmp_gradient, "#d6d9df", "#d6d9df", 60, 70);
  nimbus_gradient_add_segment (tmp_gradient, "#d6d9df", "#d8dbe1", 70, 96);
  nimbus_gradient_add_segment (tmp_gradient, "#d8dbe1", "#dadde3",96, 100);
  tmp->gradients = g_slist_append (tmp->gradients, tmp_gradient);
    
  nimbus_rc->button[GTK_STATE_INSENSITIVE] = tmp;
}

gboolean my_expose_event (GtkWidget        *widget,
			  GdkEventExpose   *event,
			  gpointer          data)
{
  GdkGC *gc;
  int i, x, width;
  NimbusRcStyle *rc = (NimbusRcStyle*) data;

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
	      nimbus_draw_gradient (widget->window,
				    gc,
				    tmp_gradient,
				    x,
				    widget->allocation.y,
				    width,
				    widget->allocation.height);
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
	      nimbus_draw_gradient (widget->window,
				    gc,
				    tmp_gradient,
				    x,
				    widget->allocation.y,
				    width,
				    widget->allocation.height);
	      x += 30;
	      tmp = tmp->next;
	    }
	  x += 20;
	}
    }

}


static void debug_gradients (NimbusRcStyle *nimbus_rc)
{
    GtkWidget *window;
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);

  g_signal_connect (G_OBJECT (window), 
		    "expose_event", 
		    G_CALLBACK (my_expose_event),
		    nimbus_rc);
  
  g_signal_connect (G_OBJECT (window), 
		    "delete_event", 
		    G_CALLBACK (gtk_main_quit),
		    NULL);
 
  gtk_widget_show_all (window);



}

static void
nimbus_rc_style_init (NimbusRcStyle *nimbus_rc)
{
  GdkPixbuf *black_drop_shadow, *white_drop_shadow;
  GError **error = NULL;
  
  define_normal_button_states (nimbus_rc);
  define_default_button_states(nimbus_rc);
  define_arrow_button_states (nimbus_rc);

  black_drop_shadow = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, 600, 1);
  white_drop_shadow = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, 600, 1);

  /* black, opacity 20% */
  gdk_pixbuf_fill (black_drop_shadow, 0x00000033);
  /* white, opacity 60% */
  gdk_pixbuf_fill (white_drop_shadow, 0xffffff99);

  nimbus_rc->drop_shadow[GTK_STATE_NORMAL] = black_drop_shadow;
  nimbus_rc->drop_shadow[GTK_STATE_PRELIGHT] = black_drop_shadow;
  nimbus_rc->drop_shadow[GTK_STATE_ACTIVE] = white_drop_shadow;
  nimbus_rc->drop_shadow[GTK_STATE_SELECTED] = black_drop_shadow;
  nimbus_rc->drop_shadow[GTK_STATE_INSENSITIVE] = black_drop_shadow;

  nimbus_rc->combo_arrow[GTK_STATE_NORMAL] =  gdk_pixbuf_new_from_inline (-1, combo_caret_normal, FALSE, error);
  nimbus_rc->combo_arrow[GTK_STATE_PRELIGHT] =  gdk_pixbuf_new_from_inline (-1, combo_caret_prelight, FALSE, error);
  nimbus_rc->combo_arrow[GTK_STATE_ACTIVE] =  gdk_pixbuf_new_from_inline (-1, combo_caret_active, FALSE, error);
  nimbus_rc->combo_arrow[GTK_STATE_SELECTED] =  nimbus_rc->combo_arrow[GTK_STATE_ACTIVE];
  nimbus_rc->combo_arrow[GTK_STATE_INSENSITIVE] =  gdk_pixbuf_new_from_inline (-1, combo_caret_disable, FALSE, error);
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

  /* debug_gradients (nimbus_style); */

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
