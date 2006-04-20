#include <gtk/gtk.h>
#include <gdk/gdkevents.h>


void main (int argc, char **argv)
{
  GtkWidget *window, *hscroll, *hbox, *vbox, *label, *label2, *l3, *l4;
  GtkAdjustment *spinner_adj;

  gtk_init (&argc, &argv);

  gtk_settings_set_string_property (gtk_settings_get_default (),
				    "gtk-theme-name", "Nimbus", "gdm");


  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  g_signal_connect (G_OBJECT (window),
                    "delete_event",
                    G_CALLBACK (gtk_main_quit),
                    NULL);

  spinner_adj = (GtkAdjustment *) gtk_adjustment_new (50.0, 0.0, 100.0, 1.0, 5.0, 5.0);
  
  hscroll= gtk_hscrollbar_new (spinner_adj);

  /* gtk_widget_set_sensitive (entry, FALSE);  */

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
  gtk_container_add (GTK_CONTAINER (hbox), hscroll);
  gtk_container_add (GTK_CONTAINER (hbox), l4);

  gtk_widget_show_all (window);

  gtk_main ();
}
