/**
 * File              : Xray.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 31.05.2023
 * Last Modified Date: 31.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "Xray.h"

struct xray_t{
	prozubi_t *p;
	struct case_t *c;
	GtkWidget *grid;
	struct image_t **images;
	int count;
};

struct xray_t * 
xray_t_new(prozubi_t *p, struct case_t *c, GtkWidget *grid){
	struct xray_t *x = malloc(sizeof(struct xray_t));
	if (!x){
		g_print("can't allocate memory for struct xray_t\n");
		return NULL;
	}	
	x->grid = grid;
	x->p = p;
	x->c = c;
	x->count = 0;
	x->images = malloc(BUFSIZ);
	if (!x){
		g_print("can't allocate memory for images array\n");
		free(x);
		return NULL;
	}	

	return x;
}

void
xray_t_free(struct xray_t *x){
	int i;
	for (i = 0; i < x->count; ++i) {
		if (x->images[i])
			prozubi_image_free(x->images[i]);
	}
	free(x);
}

void 
xray_on_destroy(GtkWidget *grid, gpointer userdata) {
	g_print("xray_on_destroy\n");
	xray_t_free(userdata);
}

void
xray_grid_add_image(struct xray_t *x, struct image_t *image)
{
	if (x->count > BUFSIZ/8){
		g_print("buffer overload\n");
		return;
	}

	GError *error = NULL;
	GdkPixbufLoader *l = gdk_pixbuf_loader_new();
	gdk_pixbuf_loader_write(l, image->data, image->len_data, &error);
	if (error){
		g_error("%s\n", error->message);	
		g_error_free(error);
		return;
	}
	gdk_pixbuf_loader_close(l, &error);
	if (error){
		g_error("%s\n", error->message);	
		g_error_free(error);
		return;
	}	
	GdkPixbuf *pixbuf = gdk_pixbuf_loader_get_pixbuf(l);

	int col = x->count % 3;
	int row = x->count / 3;	

	GtkWidget *imageView = gtk_image_new_from_pixbuf(pixbuf);
	gtk_widget_set_size_request(imageView, 80, 80);
	//gtk_container_add(GTK_CONTAINER(grid), GTK_WIDGET(imageView));
	gtk_table_attach(GTK_TABLE(x->grid), imageView, 
			col, col + 1, 
			row, row + 1, 
			GTK_EXPAND | GTK_SHRINK,
			GTK_EXPAND | GTK_SHRINK, 
			//GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			//GTK_FILL | GTK_EXPAND | GTK_SHRINK, 
			10, 10
			);
	gtk_widget_show(GTK_WIDGET(imageView));
	g_object_set_data(G_OBJECT(imageView), "image", image);	
	x->images[x->count++] = image;
}

int 
xray_images_foreach(void *userdata, struct image_t *image)
{
	g_print("Image\n");
	struct xray_t *x = userdata;
	xray_grid_add_image(x, image);
	return 0;
}

void
xray_reload(
	struct xray_t *x		
	)
{
	g_print("Xray reload for: %s\n", x->c->id);
	prozubi_image_foreach(x->p, x->c->id, x, xray_images_foreach);
}

void
xray_add_file(
		gchar *file,
		struct xray_t *x
		)
{
	g_print("add file %s\n", file);
	
	GError *error = NULL;
	GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(file, &error);
	if (error){
		g_error("%s\n", error->message);	
		g_error_free(error);
		return;
	}

	gchar *buffer = NULL;
	gsize len = 0;
	gdk_pixbuf_save_to_buffer(pixbuf, &buffer, &len, "jpeg", &error, "quality", "100", NULL);

	if (buffer && len){
		struct image_t *image = 
				prozubi_image_new(x->p, time(NULL), "снимок", x->c->id, buffer, len, NULL);
		if (image){
			xray_grid_add_image(x, image);
		}
	}
}

void
xray_file_chooser_response (
		GtkDialog *dialog,
        int       response,
		gpointer  userdata)
{
	struct xray_t *x = userdata;	
	if (response == GTK_RESPONSE_ACCEPT){
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		gchar *file = gtk_file_chooser_get_filename(chooser);
		xray_add_file (file, x);
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void
xray_file_chooser(
		GtkWidget *sender,
		struct xray_t *x
		)
{		
	GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;	
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File",
                                        NULL,
                                        action,
                                        "Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        "Open",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);

	GtkFileFilter *filter = gtk_file_filter_new();
	
	gtk_file_filter_add_pattern(filter, "*.[jJ][pP][gG]");
	gtk_file_filter_add_pattern(filter, "*.[jJ][pP][eE][gG]");
	gtk_file_filter_add_pattern(filter, "*.[pP][nN][gG]");
	gtk_file_filter_add_pattern(filter, "*.[bB][mM][pP]");
	gtk_file_filter_add_pattern(filter, "*.[tT][iI][fF]");
	gtk_file_filter_add_pattern(filter, "*.[tT][iI][fF][fF]");
	gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), filter);
	gtk_widget_show (dialog);

	g_signal_connect (dialog, "response",
					G_CALLBACK (xray_file_chooser_response),
					x);	
}

GtkWidget *
xray_new(
		GtkWidget *parent,
		prozubi_t *p, 
		struct case_t *c		
		)
{
	GtkWidget *box;
#if GTK_CHECK_VERSION(3, 2, 0)
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
	box = gtk_vbox_new(FALSE, 0);
#endif				
	gtk_container_add(GTK_CONTAINER(parent), box);
	gtk_widget_show(box);

	//toolbar
	GtkWidget *toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_MENU);
	gtk_box_pack_start(GTK_BOX(box), toolbar, FALSE, FALSE, 0);
	gtk_widget_show(toolbar);

	//scrolled window
	GtkWidget *w = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(box), w);
	gtk_widget_show(w);

	//grid
	GtkWidget *grid = gtk_table_new(10, 3, TRUE);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(w), grid);
	gtk_widget_show(grid);

	struct xray_t *x = xray_t_new(p, c, grid);
	if (!x)
		return NULL;

	g_signal_connect ((gpointer) grid, "destroy",
                    G_CALLBACK (xray_on_destroy),
                    x);	
		
	//toolbar buttons
	GtkToolItem *add = gtk_tool_button_new_from_stock(GTK_STOCK_ADD);
	g_signal_connect(add, "clicked", 
			(GCallback) xray_file_chooser, x);
	gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(add));
	gtk_widget_show(GTK_WIDGET(add));

	GtkToolItem *del = gtk_tool_button_new_from_stock(GTK_STOCK_REMOVE);
	//g_signal_connect(del, "clicked", 
			//(GCallback) plan_lecheniya_remove_clicked, treeView);
	gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(del));
	gtk_widget_show(GTK_WIDGET(del));
	
	xray_reload(x);

	return grid;
}
