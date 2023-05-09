/**
 * File              : casesEdit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.05.2023
 * Last Modified Date: 09.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CASES_EDIT_H
#define CASES_EDIT_H

#include <gtk/gtk.h>
#include <string.h>
#include "prozubilib/cases.h"
#include "prozubilib/planlecheniya.h"
#include "support.h"
#include "planLecheniya.h"

#include "prozubilib/prozubilib.h"

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
	ZUBFORMULA_COORD(Z35, 425, 450, 160, 200, 342, 165)\
	ZUBFORMULA_COORD(Z36, 460, 490, 160, 200, 469, 165)\
	ZUBFORMULA_COORD(Z37, 500, 530, 160, 200, 511, 165)\
	ZUBFORMULA_COORD(Z38, 545, 575, 160, 200, 555, 165)

struct zubformula_t {
	CASES num;
	int left;
	int rigth;
	int top;
	int bottom;
	int x;
	int y;
};

static struct zubformula_t zubformula[] =
{
#define ZUBFORMULA_COORD(num, left, right, top, bottom, x, y)\
	{num, left, right, top, bottom, x, y},
	
	ZUBFORMULA_COORDS
#undef ZUBFORMULA_COORD
	-1
};

static struct zubformula_t *
zubformulaFromEvent(GdkEventButton *event){
	/* pointer to array */
	struct zubformula_t *ptr = 	zubformula;

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

static void draw_zubformula(
			struct case_t *c,
			GtkWidget *image	
		)
{
	GdkPixbuf *pixbuf = 
			gdk_pixbuf_new_from_file("src/images/zubFormula.jpg", NULL);
	if (!pixbuf){
		g_print("can't load image: %s", "src/images/zubFormula.jpg");
		return;
	}

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
	}\
	
	ZUBFORMULA_COORDS
#undef ZUBFORMULA_COORD

	gdk_gc_unref(gc);
}

static void redraw_zubformula(struct case_t *c, GtkWidget *image){
	
	GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
	g_object_unref(pixbuf);
	draw_zubformula(c, image);
}

static void
set_tooth (GtkWidget *widget,
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

	redraw_zubformula(c, image);
}

static gboolean
button_press_callback (GtkWidget      *event_box, 
                       GdkEventButton *event,
                       gpointer        data)
{
	struct case_t *c = data;
	prozubi_t *p = g_object_get_data(G_OBJECT(event_box), "prozubi");
	GtkWidget *image = g_object_get_data(G_OBJECT(event_box), "image");
    
	g_print ("Event box clicked at coordinates %f,%f\n", 
             event->x, event->y);

	struct zubformula_t *z = zubformulaFromEvent(event); 
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
			g_object_set_data(G_OBJECT(item), "index", GINT_TO_POINTER(z->num));
			g_object_set_data(G_OBJECT(item), "prozubi", p);
			g_object_set_data(G_OBJECT(item), "image", image);
			g_object_set_data(G_OBJECT(item), "case", c);
			g_signal_connect(G_OBJECT(item), "activate",
					G_CALLBACK(set_tooth), (void *)(ZUBFORMULA_TYPE_ARRAY[i].attr));  
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

static void 
cases_edit_combobox_changed (GtkComboBox *widget, gpointer user_data) {
	GtkComboBox *combo_box = widget;
	struct case_t *c = g_object_get_data(G_OBJECT(widget), "case");
	prozubi_t *p = g_object_get_data(G_OBJECT(widget), "prozubi");
	CASES key = 
		GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "key"));
	gint id = gtk_combo_box_get_active(combo_box);
	char **ptr = user_data;
	prozubi_case_set_text(key, p, c, ptr[id]);
}

static void 
cases_edit_calendar_changed (GtkCalendar *widget, gpointer user_data) {
	struct case_t *c = g_object_get_data(G_OBJECT(widget), "case");
	prozubi_t *p = g_object_get_data(G_OBJECT(widget), "prozubi");
	CASES key = 
		GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "key"));

	guint year, month, day; 
	gtk_calendar_get_date(widget, &year, &month, &day);
	GDateTime *date = g_date_time_new_local(year, month, day, 12, 0, 0.0);

	GtkWidget *entry = user_data;
	gtk_entry_set_text(GTK_ENTRY(entry), g_date_time_format(date, "%d.%m.%Y"));	
	
	prozubi_case_set_date(key, p, c, g_date_time_to_unix(date));

	g_date_time_unref(date);
}

static void 
cases_edit_date_entry_changed(GtkEntry *widget, gpointer   data){
	GtkCalendar *calendar = data;
	struct case_t *c = g_object_get_data(G_OBJECT(widget), "case");
	prozubi_t *p = g_object_get_data(G_OBJECT(widget), "prozubi");
	CASES key = 
		GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "key"));

	const char *text = gtk_entry_get_text(widget);	
	
	/* guess time format */
	guint day, month, year;
	if (sscanf(text, "%d.%d.%d", &day, &month, &year) != 3)
		/*if (sscanf(text, "%d/%d/%d", &day, &month, &year) != 3)*/
			/*if (sscanf(text, "%d-%d-%d", &day, &month, &year) != 3)*/
				gtk_calendar_get_date(calendar, &year, &month, &day);

	GDateTime *date = 
			g_date_time_new_local(year, month, day, 12, 0, 0.0);
	if (!date)
		return;

	char *str = g_date_time_format(date, "%d.%m.%Y"); 
	gtk_entry_set_text(widget, str);
	free(str);
	
	gtk_calendar_select_month(GTK_CALENDAR(calendar), 
			g_date_time_get_month(date), 
					g_date_time_get_year(date));
	gtk_calendar_select_day(GTK_CALENDAR(calendar), 
			g_date_time_get_day_of_month(date));
	
	g_date_time_unref(date);
}

static void
cases_edit_date_entry_insert_text        (GtkEditable     *editable,
                                        gchar           *new_text,
                                        gint             new_text_length,
                                        gpointer         position,
                                        gpointer         user_data)
{
	/* format input like date dd.mm.yyyy */
	GtkEntry *entry = GTK_ENTRY(editable);
	const gchar * old_text = gtk_entry_get_text(entry);	

	int *p = position;
	
	g_print("POSITION: %d\n", *p);

	if (*p > 9){
		g_signal_stop_emission_by_name (editable, "insert_text");
		return;
	} 

	if (*p == 0 && new_text[0] > '3'){
		g_signal_stop_emission_by_name (editable, "insert_text");
		return;
	}	

	if (*p == 4 && new_text[0] > '2' && old_text[3] == '1'){
		g_signal_stop_emission_by_name (editable, "insert_text");
		return;
	}	

	char *text = MALLOC(BUFSIZ, ERR("%s", "can't allocate text"), return);
	int i = 0;
	text[i] = 0;

	char *ptr = new_text;
	char c = *ptr++;
	
	if (*p == 1 || *p == 4){
		if (c >= '0' && c<= '9'){
			text[i++] = c;		
			text[i++] = '.';		
		}
	} else if (*p == 3){
		if (c < '2'){
			text[i++] = c;		
		} else {
			text[i++] = '0';		
			text[i++] = c;		
		}
	} else if (*p == 2 || *p == 5){
		if (c == '/' || c == ' ' || c == '.')
			text[i++] = '.';		

	} else {
		if (c >= '0' && c<= '9')
			text[i++] = c;		
	}

	while(*ptr){
		c = *ptr++; 
		if ((c >= '0' && c<= '9') || c == '.')
			text[i++] = c;
	}				
	
	text[i] = 0;

	/* stop entry */
	g_signal_handlers_block_by_func (editable, (gpointer) 
			cases_edit_date_entry_insert_text, user_data);
  
	/* white text */
	/*gtk_entry_set_text(entry, text);*/
	gtk_editable_insert_text(editable, text, strlen(text), position);
	free(text);
  
	/* start entry */
	g_signal_handlers_unblock_by_func (editable, (gpointer) 
			cases_edit_date_entry_insert_text, user_data);
	
	/* stop callback */
	g_signal_stop_emission_by_name (editable, "insert_text");
}

static void 
cases_edit_text_edit_changed(GtkTextBuffer *buffer, gpointer   data){
	struct case_t *c = g_object_get_data(G_OBJECT(buffer), "case");
	prozubi_t *p = g_object_get_data(G_OBJECT(buffer), "prozubi");
	CASES key = 
		GPOINTER_TO_INT(g_object_get_data(G_OBJECT(buffer), "key"));

	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	const char *text = 
			gtk_text_buffer_get_text(buffer, &start, &end, FALSE);	

	prozubi_case_set_text(key, p, c, text);
}


static GtkWidget *
cases_edit_refresh(
		GtkWidget *casesWindow, 
		prozubi_t *p, 
		struct case_t *c, 
		CASES key, 
		CASES_LIST_TYPE type,
		char **combobox_array
		)
{
	if (!c){
		g_print("case_t is NULL\n");
		return NULL;
	}

	/* set delegate */
	GObject *delegate = G_OBJECT(casesWindow);
	
	/* get casesEditWindow */
	GtkWidget * casesEditFrame = lookup_widget(casesWindow, "casesEditFrame");
	if (!casesEditFrame){
		g_print("Error! Can't find casesEditFrame\n");
		return NULL;
	}	

	/* get and remove child */
	GtkWidget *child = gtk_bin_get_child(GTK_BIN(casesEditFrame));
	if (child)
		gtk_container_remove(GTK_CONTAINER (casesEditFrame), child);

	switch (type) {
		case CASES_LIST_TYPE_TEXT:
			{
				// vertical box
				GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
				gtk_container_add(GTK_CONTAINER(casesEditFrame), vbox);
				gtk_widget_show(vbox);
				
				// toolbar
				GtkWidget *toolbar = gtk_toolbar_new();
				gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
				gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_MENU);
				gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
				gtk_widget_show(toolbar);

				//toolbar buttons
				GtkToolItem *share = gtk_tool_button_new_from_stock(GTK_STOCK_GO_UP);
				gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(share));
				gtk_widget_show(GTK_WIDGET(share));

				//scrolled vindow
				GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
				gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), 
						GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
				gtk_box_pack_end(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
				gtk_widget_show(scrolled);

				// text edit
				GtkWidget *textEdit = gtk_text_view_new();
				gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textEdit), GTK_WRAP_WORD);
				gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),
						textEdit);
				const char *value = prozubi_case_get(c, key);
				GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textEdit));
				if (value){
					gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buf), value, -1);
				}
				
				g_object_set_data(G_OBJECT(buf), "prozubi", p);
				g_object_set_data(G_OBJECT(buf), "case", c);
				g_object_set_data(G_OBJECT(buf), "key", GINT_TO_POINTER(key));
				
				g_signal_connect (buf, "changed", 
						G_CALLBACK (cases_edit_text_edit_changed), NULL);
				gtk_widget_show(textEdit);
				return textEdit;
			}
		case CASES_LIST_TYPE_COMBOBOX:
			{
				GtkWidget *bbox = gtk_vbutton_box_new();
				gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_CENTER);
				gtk_container_add(GTK_CONTAINER(casesEditFrame), bbox);
				gtk_widget_show(bbox);
				
				const char *value = prozubi_case_get(c, key);
				GtkWidget *combobox = gtk_combo_box_text_new();
				char **ptr = combobox_array;
				int i = 0;
				while (ptr[i]){
					char *str = ptr[i];	
					gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combobox), str);
					if (value)
						if (strcmp(value, str) == 0)
							gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), i);
					i++;
				}
				g_object_set_data(G_OBJECT(combobox), "prozubi", p);
				g_object_set_data(G_OBJECT(combobox), "case", c);
				g_object_set_data(G_OBJECT(combobox), "key", GINT_TO_POINTER(key));
				g_signal_connect (combobox, "changed", 
						G_CALLBACK (cases_edit_combobox_changed), combobox_array);				
				
				gtk_container_add(GTK_CONTAINER (bbox), combobox);	
				gtk_widget_show(combobox);
				return combobox;
			}			
		case CASES_LIST_TYPE_DATE:
			{
				GtkWidget *bbox = gtk_vbutton_box_new();
				gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_CENTER);
				gtk_container_add(GTK_CONTAINER(casesEditFrame), bbox);
				gtk_widget_show(bbox);
				
				time_t *value = prozubi_case_get(c, key);
				GDateTime *d = g_date_time_new_from_unix_local(*value); 

				GtkWidget *entry = gtk_entry_new();
				char *str = g_date_time_format(d, "%d.%m.%Y"); 
				gtk_entry_set_text(GTK_ENTRY(entry), str);
				gtk_container_add(GTK_CONTAINER (bbox), entry);	
				free(str);
				gtk_widget_show(entry);

				g_object_set_data(G_OBJECT(entry), "prozubi", p);
				g_object_set_data(G_OBJECT(entry), "case", c);
				g_object_set_data(G_OBJECT(entry), "key", GINT_TO_POINTER(key));				

				
				GtkWidget *calendar = gtk_calendar_new();
				gtk_calendar_select_month(GTK_CALENDAR(calendar), 
						g_date_time_get_month(d), 
								g_date_time_get_year(d));
				gtk_calendar_select_day(GTK_CALENDAR(calendar), 
						g_date_time_get_day_of_month(d));
				
				g_date_time_unref(d);
				
				g_object_set_data(G_OBJECT(calendar), "prozubi", p);
				g_object_set_data(G_OBJECT(calendar), "case", c);
				g_object_set_data(G_OBJECT(calendar), "key", GINT_TO_POINTER(key));
				
				g_signal_connect (calendar, "month-changed", 
						G_CALLBACK (cases_edit_calendar_changed), entry);				
				g_signal_connect (calendar, "day-selected", 
						G_CALLBACK (cases_edit_calendar_changed), entry);				
				
				g_signal_connect (entry, "activate", 
						G_CALLBACK (cases_edit_date_entry_changed), calendar);
				g_signal_connect (entry, "insert_text", 
						G_CALLBACK (cases_edit_date_entry_insert_text), calendar);				
				
				gtk_container_add(GTK_CONTAINER (bbox), calendar);	
				gtk_widget_show(calendar);
				return calendar;
			}			

		case CASES_LIST_TYPE_ZFORMULA:
			{
				GtkWidget *bbox = gtk_vbutton_box_new();
				gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_CENTER);
				gtk_container_add(GTK_CONTAINER(casesEditFrame), bbox);
				gtk_widget_show(bbox);
				
				GtkWidget *event_box = gtk_event_box_new ();
				gtk_container_add(GTK_CONTAINER (bbox), event_box);	
				gtk_widget_show(event_box);

				GtkWidget *image = gtk_image_new();
				gtk_container_add (GTK_CONTAINER (event_box), image);
				gtk_misc_set_alignment(GTK_MISC(image), 0, 0);

				g_object_set_data(G_OBJECT(event_box), "prozubi", p);
				g_object_set_data(G_OBJECT(event_box), "image", image);
				g_signal_connect (G_OBJECT (event_box), 
                      "button_press_event",
                      G_CALLBACK (button_press_callback),
                      c);

				gtk_widget_show(image);

				draw_zubformula(c, image);

				return image;
			}			

		case CASES_LIST_TYPE_PLANLECHENIYA:
			{
				GtkWidget *pl = plan_lecheniya_new(casesEditFrame, c->planlecheniya, p, c);	
				return pl;
			}			

		default:
			return NULL;
	}
}
#endif /* ifndef CASES_EDIT_H */
