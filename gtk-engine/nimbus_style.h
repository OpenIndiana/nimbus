/* Nimbus Engine
 * Copyright (C) 2001 Red Hat, Inc.
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
 * Written by Owen Taylor <otaylor@redhat.com>
 * modified by Alexander Larsson <alexl@redhat.com>
 */
#include <gtk/gtkstyle.h>

typedef struct _NimbusStyle NimbusStyle;
typedef struct _NimbusStyleClass NimbusStyleClass;

extern GType nimbus_type_style;

#define NIMBUS_TYPE_STYLE              nimbus_type_style
#define NIMBUS_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), NIMBUS_TYPE_STYLE, NimbusStyle))
#define NIMBUS_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), NIMBUS_TYPE_STYLE, NimbusStyleClass))
#define NIMBUS_IS_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), NIMBUS_TYPE_STYLE))
#define NIMBUS_IS_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), NIMBUS_TYPE_STYLE))
#define NIMBUS_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), NIMBUS_TYPE_STYLE, NimbusStyleClass))

struct _NimbusStyle
{
  GtkStyle parent_instance;

  GdkColor gray[8];

  GdkColor spot_color;
  GdkColor spot1;
  GdkColor spot2;
  GdkColor spot3;

  /* from light to dark */
  GdkGC *gray_gc[8];

  GdkGC *spot1_gc;
  GdkGC *spot2_gc;
  GdkGC *spot3_gc;
  
  GdkPixmap *radio_pixmap_nonactive[5];
  GdkPixmap *radio_pixmap_active[5];
  GdkPixmap *radio_pixmap_inconsistent[5];
  GdkBitmap *radio_pixmap_mask; /* All masks are the same */
  
  GdkPixmap *check_pixmap_nonactive[5];
  GdkPixmap *check_pixmap_active[5];
  GdkPixmap *check_pixmap_inconsistent[5];
};

struct _NimbusStyleClass
{
  GtkStyleClass parent_class;
};

void nimbus_style_register_type (GTypeModule *module);
