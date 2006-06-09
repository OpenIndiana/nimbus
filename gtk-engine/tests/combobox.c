#include <gtk/gtk.h>
#include <gdk/gdkevents.h>

static GtkTreeModel *
create_list_blaat (void)
{
        GdkPixbuf *pixbuf;
        GtkWidget *cellview;
        GtkTreeIter iter;
        GtkListStore *store;

        cellview = gtk_cell_view_new ();

        store = gtk_list_store_new (2, GDK_TYPE_PIXBUF, G_TYPE_STRING);

        pixbuf = gtk_widget_render_icon (cellview, GTK_STOCK_DIALOG_WARNING,
                                         GTK_ICON_SIZE_BUTTON, NULL);
        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter,
                            0, pixbuf,
                            1, "gtk-stock-dialog-warning",
                            -1);

        pixbuf = gtk_widget_render_icon (cellview, GTK_STOCK_STOP,
                                         GTK_ICON_SIZE_BUTTON, NULL);
        gtk_list_store_append (store, &iter);			       
        gtk_list_store_set (store, &iter,
                            0, pixbuf,
                            1, "gtk-stock-stop",
                            -1);

        pixbuf = gtk_widget_render_icon (cellview, GTK_STOCK_NEW,
                                         GTK_ICON_SIZE_BUTTON, NULL);
        gtk_list_store_append (store, &iter);			       
        gtk_list_store_set (store, &iter,
                            0, pixbuf,
                            1, "gtk-stock-new",
                            -1);

        pixbuf = gtk_widget_render_icon (cellview, GTK_STOCK_CLEAR,
                                         GTK_ICON_SIZE_BUTTON, NULL);
        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter,
                            0, pixbuf,
                            1, "gtk-stock-clear",
                            -1);

        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter,
                            0, NULL,
                            1, "separator",
                            -1);

        pixbuf = gtk_widget_render_icon (cellview, GTK_STOCK_OPEN,
                                         GTK_ICON_SIZE_BUTTON, NULL);
        gtk_list_store_append (store, &iter);
        gtk_list_store_set (store, &iter,
                            0, pixbuf,
                            1, "gtk-stock-open",
                            -1);

        gtk_widget_destroy (cellview);

        return GTK_TREE_MODEL (store);
}
static gboolean
is_separator (GtkTreeModel *model,
	      GtkTreeIter  *iter,
	      gpointer      data)
{
  GtkTreePath *path;
  gboolean result;

  path = gtk_tree_model_get_path (model, iter);
  result = gtk_tree_path_get_indices (path)[0] == 4;
  gtk_tree_path_free (path);

  return result;
  
}

static void
set_sensitive (GtkCellLayout   *cell_layout,
	       GtkCellRenderer *cell,
	       GtkTreeModel    *tree_model,
	       GtkTreeIter     *iter,
	       gpointer         data)
{
  GtkTreePath *path;
  gint *indices;
  gboolean sensitive;

  path = gtk_tree_model_get_path (tree_model, iter);
  indices = gtk_tree_path_get_indices (path);
  sensitive = indices[0] != 1;
  gtk_tree_path_free (path);

  g_object_set (cell, "sensitive", sensitive, NULL);
}


void main (int argc, char **argv)
{
  GtkWidget *window, *combobox, *hbox, *vbox, *top, *bottom, *left, *right;;
  GtkTreeModel *model;
  GtkCellRenderer *renderer;

  gtk_init (&argc, &argv);

  gtk_settings_set_string_property (gtk_settings_get_default (),
				    "gtk-theme-name", "nimbus", "gdm");

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
  g_signal_connect (G_OBJECT (window),
                    "delete_event",
                    G_CALLBACK (gtk_main_quit),
                    NULL);

  model = create_list_blaat();

  combobox = gtk_combo_box_new_with_model (model);

  gtk_combo_box_set_add_tearoffs (GTK_COMBO_BOX (combobox), TRUE);

  renderer = gtk_cell_renderer_pixbuf_new ();
  
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox),
                                    renderer,
                                    FALSE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer,
                                  "pixbuf", 0,
                                  NULL);
  gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combobox),
  				    renderer,
  				    set_sensitive,
  				    NULL, NULL);

  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox),
                              renderer,
                              TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer,
                                  "text", 1,
                                  NULL);
  gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT (combobox),
  				    renderer,
  				    set_sensitive,
  				    NULL, NULL);
  gtk_combo_box_set_row_separator_func (GTK_COMBO_BOX (combobox), 
  				      is_separator, NULL, NULL);
  					
  gtk_combo_box_set_active (GTK_COMBO_BOX (combobox), 0);

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
  gtk_container_add (GTK_CONTAINER (hbox), combobox);
  gtk_container_add (GTK_CONTAINER (hbox), right);

  
  gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_widget_show_all (window);

  gtk_main ();
}




