#include <gtk/gtk.h>
#include <gdk/gdkevents.h>


void main (int argc, char **argv)
{
  GtkWidget *window,  *hbox, *vbox, *label, *label2, *l3, *l4, *la;
  GtkWidget *frame;
  GtkWidget *hpaned;
  GtkWidget *vpaned;
  GtkWidget *button;
  GtkWidget *vbox_pane;
  int i,j;

  gtk_init (&argc, &argv);

  gtk_settings_set_string_property (gtk_settings_get_default (),
				    "gtk-theme-name", "Nimbus", "gdm");


  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);
  
  g_signal_connect (G_OBJECT (window),
                    "delete_event",
                    G_CALLBACK (gtk_main_quit),
                    NULL);

      vbox_pane= gtk_vbox_new (FALSE, 0);
      
      vpaned = gtk_vpaned_new ();
      gtk_box_pack_start (GTK_BOX (vbox_pane), vpaned, TRUE, TRUE, 0);
      gtk_container_set_border_width (GTK_CONTAINER(vpaned), 5);

      hpaned = gtk_hpaned_new ();
      gtk_paned_add1 (GTK_PANED (vpaned), hpaned);

      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_NONE);
      gtk_widget_set_size_request (frame, 60, 60);
      gtk_paned_add1 (GTK_PANED (hpaned), frame);
      
      button = gtk_button_new_with_label ("Hi there");
      gtk_container_add (GTK_CONTAINER(frame), button);

      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_NONE);
      gtk_widget_set_size_request (frame, 80, 60);
      gtk_paned_add2 (GTK_PANED (hpaned), frame);

      frame = gtk_frame_new (NULL);
      gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_NONE);
      gtk_widget_set_size_request (frame, 60, 80);
      gtk_paned_add2 (GTK_PANED (vpaned), frame);
 
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
  gtk_container_add (GTK_CONTAINER (hbox), vbox_pane);
  gtk_container_add (GTK_CONTAINER (hbox), l4);

  gtk_widget_show_all (window);

  gtk_main ();
}
