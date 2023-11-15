/**
 * File              : Xray.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 31.05.2023
 * Last Modified Date: 05.08.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "Xray.h"
#include "openfile.h"

struct xray_image_t {
	struct image_t *image;
	GtkWidget *eventBox;
	GtkWidget *frame;
	bool selected;
};

struct xray_image_t *
xray_image_t_new(
		struct image_t *image 
		)
{
	struct xray_image_t *ximage = malloc(sizeof(struct xray_image_t));
	if (!ximage){
		g_print("can't allocate memory for struct xray_image_t\n");
		return NULL;
	}	
	ximage->image = image;
	ximage->frame = NULL;
	ximage->eventBox = NULL;
	ximage->selected = false;
	return ximage;
}

void
xray_image_t_free(struct xray_image_t *ximage)
{
	if (ximage->image)
		prozubi_image_free(ximage->image);	
	free(ximage);
}

struct xray_t{
	prozubi_t *p;
	struct case_t *c;
	GtkWindow *window;
	GtkWidget *grid;
	struct xray_image_t **images;
	int count;
};

struct xray_t * 
xray_t_new(prozubi_t *p, struct case_t *c, GtkWindow *window, GtkWidget *grid){
	struct xray_t *x = malloc(sizeof(struct xray_t));
	if (!x){
		g_print("can't allocate memory for struct xray_t\n");
		return NULL;
	}	
	x->grid = grid;
	x->p = p;
	x->c = c;
	x->window = window;
	x->count = 0;
	x->images = malloc(BUFSIZ);
	if (!x->images){
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
		xray_image_t_free(x->images[i]);
	}
	free(x->images);
	free(x);
}

void 
xray_on_destroy(GtkWidget *grid, gpointer userdata) {
	g_print("xray_on_destroy\n");
	xray_t_free(userdata);
}

gboolean
xray_image_press_callback (
		GtkWidget *event_box, 
        GdkEventButton *event,
        gpointer userdata)
{
	struct xray_t *x = userdata;
	struct xray_image_t *ximage = g_object_get_data(G_OBJECT(event_box), "ximage");	

	if (event->type == GDK_BUTTON_PRESS){
		ximage->selected = !ximage->selected;


		if (!ximage->frame)
			return TRUE;

		if (ximage->selected){

#if GTK_CHECK_VERSION(3, 0, 0)
		GdkRGBA color; 
		gdk_rgba_parse(&color, "#0000ff");	
		gtk_widget_override_background_color(ximage->frame, 0, &color);
			
#else	
		GdkColor color;
		gdk_color_parse("#0000ff", &color);
		gtk_widget_modify_bg(ximage->frame, 0, &color);	
		
#endif		

		} else {

#if GTK_CHECK_VERSION(3, 0, 0)
		GdkRGBA color; 
		gdk_rgba_parse(&color, "#000000ff");	
		gtk_widget_override_background_color(ximage->frame, 0, &color);
			
#else	
		GdkColor color;
		gdk_color_parse("#ffffff", &color);
		gtk_widget_modify_bg(ximage->frame, 0, &color);	
		
#endif		
		}
	
	} else if (event->type == GDK_2BUTTON_PRESS){
		const char *file = "out.jpg";
		FILE *fp = fopen(file, "w+"); 
		if (!fp){
			g_print("can't write to disk\n");
			return TRUE;
		}
		int ret = fwrite(ximage->image->data, ximage->image->len_data, 1, fp); 
		if (ret != 1){
			g_print("error to write image data to disk\n");
			return TRUE;
		}
		fclose(fp);
		openfile(file);
	}

	return TRUE;
}

void
xray_grid_add_image(struct xray_t *x, struct xray_image_t *ximage)
{
	if (x->count > BUFSIZ/8){
		g_print("buffer overload\n");
		return;
	}

	GError *error = NULL;
	GdkPixbufLoader *l = gdk_pixbuf_loader_new();
	gdk_pixbuf_loader_write(l, ximage->image->data, ximage->image->len_data, &error);
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
	GdkPixbuf *src = gdk_pixbuf_loader_get_pixbuf(l);
	GdkPixbuf *pixbuf = gdk_pixbuf_scale_simple(src, 180, 180, GDK_INTERP_TILES);	

	int col = x->count % 3;
	int row = x->count / 3;	

	//event box
	GtkWidget *event_box = gtk_event_box_new ();
	gtk_widget_show(event_box);	
	ximage->eventBox = event_box;
	g_object_set_data(G_OBJECT(event_box), "ximage", ximage);	
	g_signal_connect (G_OBJECT(event_box), 
		  "button_press_event",
		  G_CALLBACK (xray_image_press_callback),
		  x);

	//box
	GtkWidget *box;
#if GTK_CHECK_VERSION(3, 2, 0)
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
	box = gtk_vbox_new(FALSE, 0);
#endif				
	gtk_container_add(GTK_CONTAINER(event_box), box);
	gtk_widget_show(box);	

	//label
	GtkWidget *label = gtk_label_new(ximage->image->title);
	gtk_box_pack_end(GTK_BOX(box), label, FALSE, FALSE, 0);
	gtk_widget_show(label);	

	//frame
	GtkWidget *frame = gtk_frame_new(NULL);
	gtk_widget_set_size_request(frame, 200, 200);
	gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_ETCHED_OUT);
	gtk_widget_show(frame);	
	//gtk_container_add(GTK_CONTAINER (event_box), frame);	
	gtk_box_pack_start(GTK_BOX(box), frame, TRUE, TRUE, 0);
	ximage->frame = frame;

	//image
	GtkWidget *imageView = gtk_image_new_from_pixbuf(pixbuf);
	gtk_container_add(GTK_CONTAINER (frame), imageView);	

#if GTK_CHECK_VERSION(3, 0, 0)
	gtk_grid_attach(GTK_GRID(x->grid), event_box, col, row, 1, 1);

	GdkRGBA color; 
	gdk_rgba_parse(&color, "#000000ff");	
	gtk_widget_override_background_color(frame, 0, &color);

#else	
	gtk_table_attach(GTK_TABLE(x->grid), event_box, 
			col, col + 1, 
			row, row + 1, 
			GTK_FILL | GTK_EXPAND | GTK_SHRINK,
			GTK_FILL | GTK_EXPAND | GTK_SHRINK, 
			10, 10
			);

	GdkColor color;
	gdk_color_parse("#ffffff", &color);
	gtk_widget_modify_bg(frame, 0, &color);	

#endif	
	gtk_widget_show(imageView);
	g_object_set_data(G_OBJECT(imageView), "ximage", ximage);	
	x->images[x->count++] = ximage;
}

int 
xray_images_foreach(void *userdata, struct image_t *image)
{
	g_print("Image\n");
	struct xray_t *x = userdata;
	struct xray_image_t *ximage = xray_image_t_new(image);
	if (ximage)
		xray_grid_add_image(x, ximage);
	return 0;
}

void
xray_reload(
	struct xray_t *x		
	)
{
	g_print("Xray reload for: %s\n", x->c->id);
	prozubi_image_foreach(x->p, x->c->id, NULL, x, xray_images_foreach);
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
			struct xray_image_t *ximage =
					xray_image_t_new(image);
			if (ximage)
				xray_grid_add_image(x, ximage);
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

void 
xray_images_ask_to_remove_responce(
		GtkDialog *dialog, 
		gint responce, 
		gpointer userdata
		)
{
	if (responce == GTK_RESPONSE_DELETE_EVENT) {
		g_print("Remove commited\n");

		struct xray_t *x = userdata;
		
		/* remove images and update table */
		int i;
		for (i = 0; i < x->count; ++i) {
			if (x->images[i]->selected)
				prozubi_image_remove(x->p, x->images[i]->image);
			
			if (x->images[i]->eventBox){
				gtk_container_remove(GTK_CONTAINER(x->grid), x->images[i]->eventBox);
				gtk_widget_destroy(x->images[i]->eventBox);
			}
			xray_image_t_free(x->images[i]);
		}
		x->count = 0;
		xray_reload(x);
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void
xray_images_ask_to_remove(
		GtkWidget *sender,
		struct xray_t *x
		)
{		
	char title[BUFSIZ] = "Удалить выделенные изображения:\n";
	bool hasSelected = false;
	int i;
	for (i = 0; i < x->count; ++i) {
		if (x->images[i]->selected){
			hasSelected = true;
			if (x->images[i]->image->title){
				strcat(title, x->images[i]->image->title);	
				strcat(title, "\n");	
			} 
		}
	}
	if (!hasSelected)
		return;
	
	strcat(title, "?");	

	GtkWidget *dialog = gtk_message_dialog_new(NULL,
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_NONE,
			"%s", title);
	gtk_window_set_title(GTK_WINDOW(dialog), "Удалить?");

	//add remove button
	GtkWidget *button = gtk_button_new_with_label("УДАЛИТЬ");
#if GTK_CHECK_VERSION(3, 0, 0)	
	GtkStyleContext *context = gtk_widget_get_style_context(button);
	gtk_style_context_add_class(context, "destructive-action");
#else 
	GdkColor color;
	gdk_color_parse("#ff0000", &color);
	gtk_widget_modify_bg(button, 0, &color);	
#endif
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog), button, GTK_RESPONSE_DELETE_EVENT);
	
	//add cancel button
	gtk_dialog_add_button(GTK_DIALOG(dialog), "Отмена", GTK_RESPONSE_CANCEL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
	g_signal_connect (dialog, "response", 
			G_CALLBACK (xray_images_ask_to_remove_responce), x);

	gtk_widget_show_all(dialog);
}

GtkWidget *
xray_new(
		GtkWindow *window,
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
	gtk_box_pack_start(GTK_BOX(box), w, TRUE, TRUE, 0);
	gtk_widget_show(w);

	//grid
	GtkWidget *grid;
#if GTK_CHECK_VERSION(3, 0, 0)
	grid = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
#else		   
	grid = gtk_table_new(10, 3, TRUE);
#endif
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(w), grid);
	gtk_widget_show(grid);

	struct xray_t *x = xray_t_new(p, c, window, grid);
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
	g_signal_connect(del, "clicked", 
			(GCallback) xray_images_ask_to_remove, x);
	gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(del));
	gtk_widget_show(GTK_WIDGET(del));
	
	xray_reload(x);

	return grid;
}
