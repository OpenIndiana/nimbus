#include <gtk/gtk.h>
#include <gdk/gdkevents.h>

void
on_scale_value_changed (GtkRange        *range,
			GtkProgressBar  *pb)
{
  gtk_progress_bar_set_fraction (pb, gtk_range_get_value (range) / 100.0);
}


void main (int argc, char **argv)
{
  GtkWidget *window,  *hbox, *vbox, *label, *label2, *l3, *l4, *la;
  GtkWidget *menubar, *menuitem, *menuitem2, *menu;
  int i,j;

  gtk_init (&argc, &argv);

  gtk_settings_set_string_property (gtk_settings_get_default (),
				    "gtk-theme-name", "nimbus", "gdm");


  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);
  
  g_signal_connect (G_OBJECT (window),
                    "delete_event",
                    G_CALLBACK (gtk_main_quit),
                    NULL);

  menubar = gtk_menu_bar_new ();
  menuitem = gtk_menu_item_new_with_label ("test");
  gtk_menu_shell_append (GTK_MENU_SHELL (menubar), menuitem);

  menu = gtk_menu_new ();
  menuitem2 = gtk_image_menu_item_new_with_label ("item");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem2);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menu);

  
  /* gtk_widget_set_sensitive (progress, FALSE);   */
      
  hbox = gtk_hbox_new (FALSE, 10);
  vbox = gtk_vbox_new (FALSE, 10);

  gtk_container_add (GTK_CONTAINER (window), vbox);

  label = gtk_label_new ("        ");
  label2 = gtk_label_new ("        ");
  l3 = gtk_label_new ("       ");
  l4 = gtk_label_new ("         ");
  
  gtk_container_add (GTK_CONTAINER (vbox), label);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_container_add (GTK_CONTAINER (vbox), label2);
  
  gtk_container_add (GTK_CONTAINER (hbox), l3);
  gtk_container_add (GTK_CONTAINER (hbox), menubar);
  gtk_container_add (GTK_CONTAINER (hbox), l4);

  gtk_widget_show_all (window);

  gtk_main ();
}
