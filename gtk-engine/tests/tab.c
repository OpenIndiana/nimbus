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
  GtkWidget *notebook, *page1, *page2;
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

  notebook = gtk_notebook_new  ();
  /* gtk_container_set_border_width(GTK_CONTAINER (notebook), 10); */
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (notebook), GTK_POS_TOP);
  page1 = gtk_label_new ("pagepagepagepaga\n\n\n\n\n\n\n\n\n\nepagepagepagepagepagepagepagepagepagepagepagepagepagepagepage 1");
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), page1, NULL);
  page2 = gtk_label_new ("page 2");
  gtk_notebook_append_page (GTK_NOTEBOOK (notebook), page2, NULL);

  
  
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
  gtk_container_add (GTK_CONTAINER (hbox), notebook);
  gtk_container_add (GTK_CONTAINER (hbox), l4);

  gtk_widget_show_all (window);

  gtk_main ();
}
