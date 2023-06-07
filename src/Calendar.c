/**
 * File              : Calendar.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.06.2023
 * Last Modified Date: 07.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "Calendar.h"
#include "prozubilib/date-time/time_local.h"

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
	struct tm tm = {
		.tm_year = year - 1900,
		.tm_mon = month,
		.tm_mday = day,
		.tm_hour = 12,
		.tm_min = 0,
		.tm_sec = 0,
	};
	time_t date = ya_mktime(&tm);

	GtkWidget *entry = user_data;
	char datestr[16];
	ya_strftime(datestr, 16, "%d.%m.%Y", &tm);
	gtk_entry_set_text(GTK_ENTRY(entry), datestr);	
	
	prozubi_case_set_date(key, p, c, date);
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
		gtk_calendar_get_date(calendar, &year, &month, &day);
	else
		month -= 1;

	struct tm tm = {
		.tm_year = year - 1900,
		.tm_mon = month,
		.tm_mday = day,
		.tm_hour = 12,
		.tm_min = 0,
		.tm_sec = 0,
	};
	time_t date = ya_mktime(&tm);
	
	char datestr[16];
	ya_strftime(datestr, 16, "%d.%m.%Y", &tm);
	gtk_entry_set_text(GTK_ENTRY(widget), datestr);	

	gtk_calendar_select_month(GTK_CALENDAR(calendar), tm.tm_mon, tm.tm_year + 1900); 
	gtk_calendar_select_day(GTK_CALENDAR(calendar), tm.tm_mday); 

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
	
	time_t *date = prozubi_case_get(c, key);
	struct tm tm;
	sec_to_tm(*date, &tm);

	GtkWidget *entry = gtk_entry_new();
	char datestr[16];
	ya_strftime(datestr, 16, "%d.%m.%Y", &tm);
	gtk_entry_set_text(GTK_ENTRY(entry), datestr);
	gtk_container_add(GTK_CONTAINER (bbox), entry);	
	gtk_widget_show(entry);

	g_object_set_data(G_OBJECT(entry), "prozubi", p);
	g_object_set_data(G_OBJECT(entry), "case", c);
	g_object_set_data(G_OBJECT(entry), "key", GINT_TO_POINTER(key));				

	
	GtkWidget *calendar = gtk_calendar_new();
	gtk_calendar_select_month(GTK_CALENDAR(calendar), tm.tm_mon, tm.tm_year + 1900); 
	gtk_calendar_select_day(GTK_CALENDAR(calendar), tm.tm_mday); 
	
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

