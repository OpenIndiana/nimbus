#include <gtk/gtk.h>
#include <gdk/gdkevents.h>

static GtkWidget*
create_menu (GdkScreen *screen, gint depth, gint length, gboolean tearoff)
{
  GtkWidget *menu;
  GtkWidget *menuitem;
  GtkWidget *image;
  GSList *group;
  char buf[32];
  int i, j;

  if (depth < 1)
    return NULL;

  menu = gtk_menu_new ();
  gtk_menu_set_screen (GTK_MENU (menu), screen);

  group = NULL;

  if (tearoff)
    {
      menuitem = gtk_tearoff_menu_item_new ();
      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
      gtk_widget_show (menuitem);
    }

  image = gtk_image_new_from_stock (GTK_STOCK_OPEN,
                                    GTK_ICON_SIZE_MENU);
  gtk_widget_show (image);
  menuitem = gtk_image_menu_item_new_with_label ("Image item");
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), image);
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
  gtk_widget_show (menuitem);
  
  for (i = 0, j = 1; i < length; i++, j++)
    {
      sprintf (buf, "item %2d - %d", depth, j);

      menuitem = gtk_radio_menu_item_new_with_label (group, buf);
      group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (menuitem));

#if 0
      if (depth % 2)
	gtk_check_menu_item_set_show_toggle (GTK_CHECK_MENU_ITEM (menuitem), TRUE);
#endif

      gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);
      gtk_widget_show (menuitem);
      if (i == 3)
	gtk_widget_set_sensitive (menuitem, FALSE);

      if (i == 5)
        gtk_check_menu_item_set_inconsistent (GTK_CHECK_MENU_ITEM (menuitem),
                                              TRUE);

      if (i < 5)
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), 
				   create_menu (screen, depth - 1, 5,  TRUE));
    }

  return menu;
}

void blah ()
{
  printf ("blah");
}
  

void main (int argc, char **argv)
{
  GtkWidget *window, *option_menu, *hbox, *vbox, *top, *bottom, *left, *right;
  GtkWidget *menu;

  gtk_init (&argc, &argv);

  gtk_settings_set_string_property (gtk_settings_get_default (),
				    "gtk-theme-name", "nimbus", "gdm");

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  g_signal_connect (G_OBJECT (window),
                    "delete_event",
                    G_CALLBACK (gtk_main_quit),
                    NULL);

  menu = create_menu (gdk_display_get_default_screen (gdk_display_get_default ()), 2, 10, TRUE);

  option_menu = gtk_option_menu_new ();

  g_signal_connect (G_OBJECT (gtkbutton), "clicked",
		    G_CALLBACK (blah),
		    NULL);


  /* gtk_widget_set_sensitive (option_menu, FALSE);  */

  gtk_option_menu_set_menu (GTK_OPTION_MENU (option_menu), menu);
  
  gtk_option_menu_set_history (GTK_OPTION_MENU (option_menu), 3);

  hbox = gtk_hbox_new (FALSE, 10);
  vbox = gtk_vbox_new (FALSE, 10);

  top = gtk_label_new ("top");
  bottom = gtk_label_new ("bottom");
  left = gtk_label_new ("left");
  right = gtk_label_new ("right");

  gtk_container_add (GTK_CONTAINER (vbox), top);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_container_add (GTK_CONTAINER (vbox), bottom);
  
  gtk_container_add (GTK_CONTAINER (hbox), left);
  gtk_container_add (GTK_CONTAINER (hbox), option_menu);
  gtk_container_add (GTK_CONTAINER (hbox), right);

  
  gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_widget_show_all (window);

  gtk_main ();
}




