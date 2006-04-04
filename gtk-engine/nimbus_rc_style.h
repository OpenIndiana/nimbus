/* Nimbus Theme Engine
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

#include <gtk/gtkrc.h>

typedef struct _NimbusRcStyle NimbusRcStyle;
typedef struct _NimbusRcStyleClass NimbusRcStyleClass;
typedef struct _NimbusButton NimbusButton;

extern GType nimbus_type_rc_style;

#define NIMBUS_TYPE_RC_STYLE              nimbus_type_rc_style
#define NIMBUS_RC_STYLE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), NIMBUS_TYPE_RC_STYLE, NimbusRcStyle))
#define NIMBUS_RC_STYLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), NIMBUS_TYPE_RC_STYLE, NimbusRcStyleClass))
#define NIMBUS_IS_RC_STYLE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), NIMBUS_TYPE_RC_STYLE))
#define NIMBUS_IS_RC_STYLE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), NIMBUS_TYPE_RC_STYLE))
#define NIMBUS_RC_STYLE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), NIMBUS_TYPE_RC_STYLE, NimbusRcStyleClass))

struct _NimbusButton
{
  GdkPixbuf *corner_top_left;
  GdkPixbuf *corner_top_right;
  GdkPixbuf *corner_bottom_left;
  GdkPixbuf *corner_bottom_right;
  GSList    *gradients;
};

struct _NimbusRcStyle
{
  GtkRcStyle parent_instance; /* must be first */

  NimbusButton* button[5]; 
  NimbusButton* button_default[5]; 
  GdkPixbuf*	drop_shadow[5];
};


struct _NimbusRcStyleClass
{
  GtkRcStyleClass parent_class;
};

void nimbus_rc_style_register_type (GTypeModule *module);
