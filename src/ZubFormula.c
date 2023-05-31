/**
 * File              : ZubFormula.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 31.05.2023
 * Last Modified Date: 31.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "ZubFormula.h"

//ZUBFORMULA_COORD(tooth, left, right, top, bottom, x, y)
#define ZUBFORMULA_COORDS \
	ZUBFORMULA_COORD(Z18, 10,  36,  86,  115, 17,  102)\
	ZUBFORMULA_COORD(Z17, 50,  80,  86,  115, 58,  102)\
	ZUBFORMULA_COORD(Z16, 90,  120, 86,  115, 98,  102)\
	ZUBFORMULA_COORD(Z15, 130, 150, 86,  115, 134, 102)\
	ZUBFORMULA_COORD(Z14, 160, 185, 86,  115, 168, 102)\
	ZUBFORMULA_COORD(Z13, 195, 222, 86,  115, 203, 102)\
	ZUBFORMULA_COORD(Z12, 230, 250, 86,  115, 236, 102)\
	ZUBFORMULA_COORD(Z11, 260, 290, 86,  115, 269, 102)\
	ZUBFORMULA_COORD(Z21, 305, 330, 86,  115, 314, 102)\
	ZUBFORMULA_COORD(Z22, 342, 360, 86,  115, 348, 102)\
	ZUBFORMULA_COORD(Z23, 370, 400, 86,  115, 379, 102)\
	ZUBFORMULA_COORD(Z24, 410, 430, 86,  115, 416, 102)\
	ZUBFORMULA_COORD(Z25, 440, 465, 86,  115, 447, 102)\
	ZUBFORMULA_COORD(Z26, 475, 505, 86,  115, 485, 102)\
	ZUBFORMULA_COORD(Z27, 510, 545, 86,  115, 522, 102)\
	ZUBFORMULA_COORD(Z28, 560, 580, 86,  115, 566, 102)\
	ZUBFORMULA_COORD(Z48, 20,  45,  160, 200, 25,  165)\
	ZUBFORMULA_COORD(Z47, 60,  90,  160, 200, 69,  165)\
	ZUBFORMULA_COORD(Z46, 105, 130, 160, 200, 112, 165)\
	ZUBFORMULA_COORD(Z45, 145, 165, 160, 200, 151, 165)\
	ZUBFORMULA_COORD(Z44, 180, 200, 160, 200, 185, 165)\
	ZUBFORMULA_COORD(Z43, 210, 230, 160, 200, 216, 165)\
	ZUBFORMULA_COORD(Z42, 245, 260, 160, 200, 249, 165)\
	ZUBFORMULA_COORD(Z41, 270, 290, 160, 200, 278, 165)\
	ZUBFORMULA_COORD(Z31, 300, 320, 160, 200, 308, 165)\
	ZUBFORMULA_COORD(Z32, 330, 350, 160, 200, 336, 165)\
	ZUBFORMULA_COORD(Z33, 360, 385, 160, 200, 367, 165)\
	ZUBFORMULA_COORD(Z34, 390, 415, 160, 200, 399, 165)\
	ZUBFORMULA_COORD(Z35, 425, 450, 160, 200, 432, 165)\
	ZUBFORMULA_COORD(Z36, 460, 490, 160, 200, 469, 165)\
	ZUBFORMULA_COORD(Z37, 500, 530, 160, 200, 511, 165)\
	ZUBFORMULA_COORD(Z38, 545, 575, 160, 200, 555, 165)

struct zubformula_tooth_t {
	CASES num;
	int left;
	int rigth;
	int top;
	int bottom;
	int x;
	int y;
	} zubformula[] =
{
#define ZUBFORMULA_COORD(num, left, right, top, bottom, x, y)\
	{num, left, right, top, bottom, x, y},
	
	ZUBFORMULA_COORDS
#undef ZUBFORMULA_COORD
	-1
};

struct zubformula_tooth_t *
zubformula_tooth_from_event(GdkEventButton *event){
	/* pointer to array */
	struct zubformula_tooth_t *ptr = zubformula;

	/* first to check upper or lower jawl */
	if (event->y > 200 || event->y < 86)
		return NULL;
	if (event->y > 115 && event->y < 160)
		return NULL;
	
	if (event->y > 115) /* lower jawl */
		ptr += 16;

	/* second check right or left */
	if (event->x > 580 || event->x < 10)
		return NULL;
	if (event->x > 290 && event->x < 300)
		return NULL;
	
	if (event->x > 290) /* left */
		ptr += 8;

	int i;
	for (i = 0; i < 8; ++i) {
		if (event->x > ptr[i].left && event->x < ptr[i].rigth &&
				event->y > ptr[i].top && event->y < ptr[i].bottom)
			return &ptr[i];
	}

	return NULL;
};

void 
zubformula_draw(
	struct case_t *c,
	GtkWidget *image)
{
	const char *file = "images/zubFormula.jpg"; 
	GdkPixbuf *pixbuf = 
			gdk_pixbuf_new_from_file(file, NULL);
	if (!pixbuf){
		g_print("can't load image: %s\n", file);
		return;
	}

#if GTK_CHECK_VERSION(3, 0, 0)
	cairo_surface_t *surface = 
		gdk_cairo_surface_create_from_pixbuf(pixbuf, 0, NULL);
	gtk_image_set_from_surface(GTK_IMAGE(image), surface);
	
	cairo_t *cr = cairo_create(surface);
	GdkRGBA color; 
	gdk_rgba_parse(&color, "#ff0000");
	gdk_cairo_set_source_rgba (cr, &color);\

#define ZUBFORMULA_COORD(num, left, right, top, bottom, x, y)\
	{\
		const char *value = prozubi_case_get(c, num);\
		if (!value)\
			value = "";\
		PangoLayout *layout =\
			gtk_widget_create_pango_layout(image, value);\
		cairo_move_to (cr, x, y);\
		pango_cairo_show_layout (cr, layout);\
	}
	
	ZUBFORMULA_COORDS
#undef ZUBFORMULA_COORD
	cairo_destroy (cr);
	cairo_surface_destroy(surface);

#else
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	gdk_pixbuf_render_pixmap_and_mask(pixbuf, &pixmap, &mask, 0);
	gtk_image_set_from_pixmap(GTK_IMAGE(image), pixmap, mask);

	GdkColormap *cm = gdk_window_get_colormap(GDK_DRAWABLE(pixmap));
	GdkColor color;
	gdk_color_parse("#ff0000", &color);
	gdk_colormap_alloc_color(cm, &color, TRUE, FALSE);

	GdkGCValues values;
	values.foreground = color;
	GdkGC *gc = 
			gdk_gc_new_with_values(GDK_DRAWABLE(pixmap), &values, GDK_GC_FOREGROUND);


#define ZUBFORMULA_COORD(num, left, right, top, bottom, x, y)\
	{\
		const char *value = prozubi_case_get(c, num);\
		if (!value)\
			value = "";\
		PangoLayout *layout =\
			gtk_widget_create_pango_layout(image, value);\
		gdk_draw_layout(GDK_DRAWABLE(pixmap),\
		   	gc, x, y, layout);\
	}
	
	ZUBFORMULA_COORDS
#undef ZUBFORMULA_COORD
	gdk_gc_unref(gc);
	gdk_pixmap_unref(pixmap);
	gdk_bitmap_unref(mask);

#endif
}

void 
zubformula_redraw(
		struct case_t *c, 
		GtkWidget *image)
{
	GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
	g_object_unref(pixbuf);
	zubformula_draw(c, image);
}

void
zubformula_set_tooth (
		GtkWidget *widget,
		gpointer   user_data)
{
	GtkWidget *image = g_object_get_data(G_OBJECT(widget), "image");
	prozubi_t *p = g_object_get_data(G_OBJECT(widget), "prozubi");
	struct case_t *c = g_object_get_data(G_OBJECT(widget), "case");
	CASES num = 
		GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "index"));
	
	const char *attr = user_data;
	g_print("TOOTH: %s SET TO: %s\n", getStringCASES(num), attr);	
				
	prozubi_case_set_text(num, p, c, attr);

	zubformula_redraw(c, image);
}

gboolean
zubformula_tooth_press_callback (
		GtkWidget *event_box, 
        GdkEventButton *event,
        gpointer userdata)
{
	struct case_t *c = userdata;
	prozubi_t *p = g_object_get_data(G_OBJECT(event_box), "prozubi");
	GtkWidget *image = g_object_get_data(G_OBJECT(event_box), "image");
    
	g_print ("Event box clicked at coordinates %f,%f\n", 
             event->x, event->y);

	struct zubformula_tooth_t *z = zubformula_tooth_from_event(event); 
	if (z){
		g_print("HAVE TOOTH: %s\n", getStringCASES(z->num));	
		/* get thooth value */
		const char *value = prozubi_case_get(c, z->num);
		if (!value)
			value = "";

		GtkWidget *pmenu = gtk_menu_new();
		int i;
		for (i = 0; i < ZUBFORMULA_TYPE_COLS_NUM; ++i) {
			GtkWidget *item = 
					gtk_menu_item_new_with_label(ZUBFORMULA_TYPE_ARRAY[i].title);
			if (strcmp(value, ZUBFORMULA_TYPE_ARRAY[i].attr) == 0)
				gtk_menu_item_select(GTK_MENU_ITEM(item));
			gtk_widget_show(item);
			gtk_menu_shell_append(GTK_MENU_SHELL(pmenu), item);
			g_object_set_data(G_OBJECT(item), "index",   GINT_TO_POINTER(z->num));
			g_object_set_data(G_OBJECT(item), "prozubi", p);
			g_object_set_data(G_OBJECT(item), "image",   image);
			g_object_set_data(G_OBJECT(item), "case",    c);
			
			g_signal_connect(G_OBJECT(item), "activate",
					G_CALLBACK(zubformula_set_tooth), (void *)(ZUBFORMULA_TYPE_ARRAY[i].attr));  
		}

		gtk_menu_popup(GTK_MENU(pmenu), NULL, NULL, NULL, NULL,
              0, 0);
	}

    /* Returning TRUE means we handled the event, so the signal 
     * emission should be stopped (don't call any further 
     * callbacks that may be connected). Return FALSE
     * to continue invoking callbacks.
     */
    return TRUE;
}

GtkWidget *
zubformula_new(
		GtkWidget *parent,
		prozubi_t *p, 
		struct case_t *c		
		)
{
	GtkWidget *box;
#if GTK_CHECK_VERSION(3, 2, 0)
	box = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#else
	box = gtk_vbutton_box_new();
#endif
	gtk_button_box_set_layout(GTK_BUTTON_BOX(box), GTK_BUTTONBOX_CENTER);
	gtk_container_add(GTK_CONTAINER(parent), box);
	gtk_widget_show(box);
	
	GtkWidget *event_box = gtk_event_box_new ();
	gtk_container_add(GTK_CONTAINER (box), event_box);	
	gtk_widget_show(event_box);

	GtkWidget *image = gtk_image_new();
	gtk_container_add (GTK_CONTAINER (event_box), image);
	gtk_misc_set_alignment(GTK_MISC(image), 0, 0);

	g_object_set_data(G_OBJECT(event_box), "prozubi", p);
	g_object_set_data(G_OBJECT(event_box), "image", image);
	g_signal_connect (G_OBJECT(event_box), 
		  "button_press_event",
		  G_CALLBACK (zubformula_tooth_press_callback),
		  c);

	gtk_widget_show(image);

	zubformula_draw(c, image);

	return image;
}
