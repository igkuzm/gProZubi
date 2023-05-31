/**
 * File              : casesEdit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.05.2023
 * Last Modified Date: 31.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CASES_EDIT_H
#define CASES_EDIT_H

#include <gtk/gtk.h>
#include <string.h>
#include "cairo.h"
#include "glibconfig.h"
#include "prozubilib/cases.h"
#include "prozubilib/planlecheniya.h"
#include "planLecheniya.h"

#include "ZubFormula.h"
#include "Xray.h"


#include "prozubilib/prozubilib.h"

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

	char *text = MALLOC(BUFSIZ, g_print("%s", "can't allocate text"), return);
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
	GtkWidget * casesEditFrame = g_object_get_data(delegate, "casesEditFrame");
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
#if GTK_CHECK_VERSION(3, 2, 0)
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
#endif
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
#if GTK_CHECK_VERSION(3, 8, 0)
	gtk_container_add(GTK_CONTAINER(scrolled), textEdit);
#else
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),
						textEdit);
#endif
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
#if GTK_CHECK_VERSION(3, 2, 0)
	GtkWidget *bbox = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#else
	GtkWidget *bbox = gtk_vbutton_box_new();
#endif
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
#if GTK_CHECK_VERSION(3, 2, 0)
	GtkWidget *bbox = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#else
	GtkWidget *bbox = gtk_vbutton_box_new();
#endif
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
				return zubformula_new(casesEditFrame, p, c);
			}			

		case CASES_LIST_TYPE_PLANLECHENIYA:
			{
				GtkWidget *pl = plan_lecheniya_new(casesEditFrame, c->planlecheniya, p, c);	
				return pl;
			}			

		case CASES_LIST_TYPE_XRAY:
			{
				return xray_new(casesEditFrame, p, c);
			}			

		default:
			return NULL;
	}
}
#endif /* ifndef CASES_EDIT_H */
