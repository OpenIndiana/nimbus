#include <gtk/gtk.h>
#include <gdk/gdkevents.h>


void main (int argc, char **argv)
{
  GtkWidget *window, *radio, *check, *hbox, *vbox, *label, *label2, *l3, *l4, *hb, *vb, *vb2;
  GtkAdjustment *spinner_adj;

  gtk_init (&argc, &argv);

  gtk_settings_set_string_property (gtk_settings_get_default (),
				    "gtk-theme-name", "Nimbus", "gdm");


  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  g_signal_connect (G_OBJECT (window),
                    "delete_event",
                    G_CALLBACK (gtk_main_quit),
                    NULL);

  
  hb = gtk_hbox_new (FALSE, 10);
  vb = gtk_vbox_new (FALSE, 10);
  vb2 = gtk_vbox_new (FALSE, 10);

  check = gtk_check_button_new_with_label ("Active Checked");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), TRUE);
  gtk_container_add (GTK_CONTAINER (vb), check);

  check = gtk_check_button_new_with_label ("Active Not Checked");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), FALSE);
  gtk_container_add (GTK_CONTAINER (vb), check);

  check = gtk_check_button_new_with_label ("Inconsistent Checked");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), TRUE);
  gtk_toggle_button_set_inconsistent (GTK_TOGGLE_BUTTON (check), TRUE);
  gtk_container_add (GTK_CONTAINER (vb), check);

  check = gtk_check_button_new_with_label ("Inconsistent Not Checked");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check), FALSE);
  gtk_toggle_button_set_inconsistent (GTK_TOGGLE_BUTTON (check), TRUE);
  gtk_container_add (GTK_CONTAINER (vb), check);

  /* radio */

  radio = gtk_radio_button_new_with_label (NULL, "Active Checked");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio), TRUE);
  gtk_container_add (GTK_CONTAINER (vb2), radio);
  
  radio = gtk_radio_button_new_with_label (gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio)), 
								       "Active Not Checked");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio), FALSE);
  gtk_container_add (GTK_CONTAINER (vb2), radio);
 
  radio = gtk_radio_button_new_with_label (NULL, "Inconsistent Checked");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio), TRUE);
  gtk_toggle_button_set_inconsistent (GTK_TOGGLE_BUTTON (radio), TRUE);
  gtk_container_add (GTK_CONTAINER (vb2), radio);
  
  radio = gtk_radio_button_new_with_label (gtk_radio_button_get_group (GTK_RADIO_BUTTON (radio)), 
								       "Inconsistent Not Checked");
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (radio), FALSE);
  gtk_toggle_button_set_inconsistent (GTK_TOGGLE_BUTTON (radio), TRUE);
  gtk_container_add (GTK_CONTAINER (vb2), radio);

  gtk_container_add (GTK_CONTAINER (hb), vb);
  gtk_container_add (GTK_CONTAINER (hb), vb2);

  
  

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
  gtk_container_add (GTK_CONTAINER (hbox), hb);
  gtk_container_add (GTK_CONTAINER (hbox), l4);

  gtk_widget_show_all (window);

  gtk_main ();
}
