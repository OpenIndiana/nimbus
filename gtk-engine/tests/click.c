#include <gtk/gtk.h>
#include <gdk/gdkevents.h>


void clicked ()
{
  static gboolean dark = TRUE;

  if (dark)
    gtk_settings_set_string_property (gtk_settings_get_default (),
				      "gtk-theme-name", "nimbus", "blah");
  else
    gtk_settings_set_string_property (gtk_settings_get_default (),
				      "gtk-theme-name", "dark-nimbus", "blah");

    
  dark = !dark;

  printf ("in clicked\n");
}


void main (int argc, char **argv)
{
  GtkWidget *window, *button, *b2, *hbox, *vbox, *label, *label2, *l3, *l4;

  gtk_init (&argc, &argv);

  gtk_settings_set_string_property (gtk_settings_get_default (),
				    "gtk-theme-name", "dark-nimbus", "blah");


  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  g_signal_connect (G_OBJECT (window),
                    "delete_event",
                    G_CALLBACK (gtk_main_quit),
                    NULL);

  button = gtk_button_new_with_label ("Click Me !");
  b2 = gtk_button_new_with_label ("Click Me !");
  g_signal_connect (G_OBJECT (b2),
                    "clicked",
                    G_CALLBACK (clicked),
                    NULL);

  gtk_widget_set_sensitive (button, FALSE);
  GTK_WIDGET_SET_FLAGS (b2, GTK_CAN_DEFAULT);
  gtk_widget_grab_default (b2);

  hbox = gtk_hbox_new (FALSE, 10);
  vbox = gtk_vbox_new (FALSE, 10);

  gtk_container_add (GTK_CONTAINER (window), vbox);

  label = gtk_label_new ("top");
  label2 = gtk_label_new ("bottom");
  l3 = gtk_label_new ("left");
  l4 = gtk_label_new ("right");
  
  gtk_container_add (GTK_CONTAINER (vbox), label);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_container_add (GTK_CONTAINER (vbox), label2);
  /* gtk_container_add (GTK_CONTAINER (vbox), b2); */
  
  gtk_container_add (GTK_CONTAINER (hbox), l3);
  gtk_container_add (GTK_CONTAINER (hbox), button);
  gtk_container_add (GTK_CONTAINER (hbox), b2);

  gtk_widget_show_all (window);

  gtk_main ();
}
