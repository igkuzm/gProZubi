/**
 * File              : Calendar.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.06.2023
 * Last Modified Date: 02.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "Calendar.h"

void
cases_edit_date_entry_insert_text(
		GtkEditable     *editable,
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

void 
cases_edit_calendar_changed (GtkCalendar *widget, gpointer user_data) {
	struct case_t *c = g_object_get_data(G_OBJECT(widget), "case");
	prozubi_t *p = g_object_get_data(G_OBJECT(widget), "prozubi");
	CASES key = 
		GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "key"));

	guint year, month, day; 
	gtk_calendar_get_date(widget, &year, &month, &day);
	/*GDateTime *date = g_date_time_new_local(year, month, day, 12, 0, 0.0);*/

	GtkWidget *entry = user_data;
	/*gtk_entry_set_text(GTK_ENTRY(entry), g_date_time_format(date, "%d.%m.%Y"));	*/
	
	/*prozubi_case_set_date(key, p, c, g_date_time_to_unix(date));*/

	/*g_date_time_unref(date);*/
}

void 
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

	/*GDateTime *date = */
			/*g_date_time_new_local(year, month, day, 12, 0, 0.0);*/
	/*if (!date)*/
		/*return;*/

	/*char *str = g_date_time_format(date, "%d.%m.%Y"); */
	/*gtk_entry_set_text(widget, str);*/
	/*free(str);*/
	
	/*gtk_calendar_select_month(GTK_CALENDAR(calendar), */
			/*g_date_time_get_month(date), */
					/*g_date_time_get_year(date));*/
	/*gtk_calendar_select_day(GTK_CALENDAR(calendar), */
			/*g_date_time_get_day_of_month(date));*/
	
	/*g_date_time_unref(date);*/
}

GtkWidget *
calendar_new(
		GtkWidget *parent,
		prozubi_t *p,
		struct case_t *c,
		CASES key)
{
	GtkWidget *bbox;
#if GTK_CHECK_VERSION(3, 2, 0)
	bbox = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#else
	bbox = gtk_vbutton_box_new();
#endif
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_CENTER);
	gtk_container_add(GTK_CONTAINER(parent), bbox);
	gtk_widget_show(bbox);
	
	time_t *value = prozubi_case_get(c, key);
	/*GDateTime *d = g_date_time_new_from_unix_local(*value); */

	GtkWidget *entry = gtk_entry_new();
	/*char *str = g_date_time_format(d, "%d.%m.%Y"); */
	/*gtk_entry_set_text(GTK_ENTRY(entry), str);*/
	/*gtk_container_add(GTK_CONTAINER (bbox), entry);	*/
	/*free(str);*/
	/*gtk_widget_show(entry);*/

	g_object_set_data(G_OBJECT(entry), "prozubi", p);
	g_object_set_data(G_OBJECT(entry), "case", c);
	g_object_set_data(G_OBJECT(entry), "key", GINT_TO_POINTER(key));				

	
	GtkWidget *calendar = gtk_calendar_new();
	/*gtk_calendar_select_month(GTK_CALENDAR(calendar), */
			/*g_date_time_get_month(d), */
					/*g_date_time_get_year(d));*/
	/*gtk_calendar_select_day(GTK_CALENDAR(calendar), */
			/*g_date_time_get_day_of_month(d));*/
	
	/*g_date_time_unref(d);*/
	
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

