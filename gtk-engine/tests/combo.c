#include <gtk/gtk.h>
#include <gdk/gdkevents.h>


void main (int argc, char **argv)
{
  GtkWidget *window, *combo, *hbox, *vbox, *top, *bottom, *left, *right;;
  GtkTreeModel *model;
  GtkCellRenderer *renderer;
  GList *items = NULL;

  gtk_init (&argc, &argv);

  gtk_settings_set_string_property (gtk_settings_get_default (),
				    "gtk-theme-name", "dark-nimbus", "gdm");

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  g_signal_connect (G_OBJECT (window),
                    "delete_event",
                    G_CALLBACK (gtk_main_quit),
                    NULL);


  items = g_list_append (items, "First Item");
  items = g_list_append (items, "Second Item");
  items = g_list_append (items, "Third Item");
  items = g_list_append (items, "Fourth Item");
  items = g_list_append (items, "Fifth Item");

  combo = gtk_combo_new ();
  gtk_combo_set_popdown_strings (GTK_COMBO (combo), items);

  /* gtk_widget_set_sensitive (combo, FALSE); */

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
  gtk_container_add (GTK_CONTAINER (hbox), combo);
  gtk_container_add (GTK_CONTAINER (hbox), right);

  
  gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_widget_show_all (window);

  gtk_main ();
}




