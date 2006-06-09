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
  GtkWidget *box1;
  GtkWidget *box2;
  GtkWidget *button;
  GtkWidget *scrollbar;
  GtkWidget *scale;
  GtkWidget *separator;
  GtkWidget *progress;
  GtkObject *adjustment;
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

  adjustment = gtk_adjustment_new (5.0, 0.0, 101.0, 0.1, 1.0, 1.0);

  progress = gtk_progress_bar_new ();
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress), 0.5);

  box1 = gtk_vbox_new (FALSE, 0);
  scale = gtk_hscale_new (GTK_ADJUSTMENT (adjustment));
  gtk_widget_set_size_request (GTK_WIDGET (scale), 150, -1);
  gtk_range_set_update_policy (GTK_RANGE (scale), GTK_UPDATE_DELAYED);
  gtk_scale_set_digits (GTK_SCALE (scale), 1);
  gtk_scale_set_draw_value (GTK_SCALE (scale), TRUE);
  gtk_box_pack_start (GTK_BOX (box1), scale, TRUE, TRUE, 0); 
  gtk_box_pack_start (GTK_BOX (box1), progress, TRUE, TRUE, 0);

  g_signal_connect (G_OBJECT (scale), "value_changed",
                    G_CALLBACK (on_scale_value_changed),
                    progress);

  
  gtk_widget_set_sensitive (progress, FALSE);  
      
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
  gtk_container_add (GTK_CONTAINER (hbox), box1);
  gtk_container_add (GTK_CONTAINER (hbox), l4);

  gtk_widget_show_all (window);

  gtk_main ();
}
