/**
 * File              : patientEdit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 03.05.2023
 * Last Modified Date: 05.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PATIENT_EDIT_H
#define PATIENT_EDIT_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "glibconfig.h"
#include "support.h"

#include "casesList.h"
#include "interface.h"
#include "configFile.h"
#include "patientList.h"

#include "prozubilib/prozubilib.h"

#define PATIENT_EDIT_TYPES\
	PATIENT_EDIT_TYPE_ENTRY(familiya,    familiyaEntry,    PASSPORTFAMILIYA)\
	PATIENT_EDIT_TYPE_ENTRY(imia,        imiaEntry,        PASSPOTIMIA)\
	PATIENT_EDIT_TYPE_ENTRY(otchestvo,   otchestvoEntry,   PASSPORTOTCHESTVO)\
	PATIENT_EDIT_TYPE_ENTRY(email,       emailEntry,       PASSPORTEMAIL)\
	PATIENT_EDIT_TYPE_ENTRY(tel,         telEntry,         PASSPORTTEL)\
	PATIENT_EDIT_TYPE_ENTRY(comment,     commentEntry,     PASSPORTCOMMENT)\
	PATIENT_EDIT_TYPE_EDATE(dateofbirth, dateOfBirthEntry, PASSPORTDATEOFBIRTH)\
	PATIENT_EDIT_TYPE_TEXTV(address,     addressTextview,  PASSPORTADDRESS)\
	PATIENT_EDIT_TYPE_TEXTV(document,    passportTextview, PASSPORTDOCUMENT)

static void
patient_edit_save_button_pushed(gpointer data){
	GObject *delegate = G_OBJECT(data);

	prozubi_t *p = g_object_get_data(delegate, "prozubi");
	struct passport_t *patient = g_object_get_data(delegate, "patient");
	GtkWidget *mainWindow = g_object_get_data(delegate, "mainWindow");
	gboolean isNew = GPOINTER_TO_INT(g_object_get_data(delegate, "isNew"));
	
	GtkWidget *patientEditWindow = GTK_WIDGET(delegate);

#define PATIENT_EDIT_TYPE_ENTRY(member, name, key)\
	GtkWidget * name = lookup_widget(patientEditWindow, #name);\
	if (!name){\
		g_print("Error! Can't find " #name "\n");\
		return;\
	}\
	{\
		const gchar *text = gtk_entry_get_text(GTK_ENTRY(name));\
		prozubi_passport_set_text(key, p, patient, text, true);\
	}

#define PATIENT_EDIT_TYPE_EDATE(member, name, key)\
	GtkWidget * name = lookup_widget(patientEditWindow, #name);\
	if (!name){\
		g_print("Error! Can't find " #name "\n");\
		return;\
	}\
	{\
		const gchar *text = gtk_entry_get_text(GTK_ENTRY(name));\
		guint day, month, year;\
		bool guess_date = true;\
		if (sscanf(text, "%d.%d.%d", &day, &month, &year) == 3){\
			GDateTime *date = \
					g_date_time_new_local(year, month, day, 12, 0, 0.0);\
			if (date){\
				prozubi_passport_set_date(key, p, patient,\
						g_date_time_to_unix(date), true);\
			}\
		}\
	}
	
#define PATIENT_EDIT_TYPE_TEXTV(member, name, key)\
	GtkWidget * name = lookup_widget(patientEditWindow, #name);\
	if (!name){\
		g_print("Error! Can't find " #name "\n");\
		return;\
	}\
	{\
		GtkTextIter start, end;\
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(GTK_TEXT_VIEW(name)->buffer),\
			   	&start, &end);\
		const char *text = \
				gtk_text_buffer_get_text(GTK_TEXT_BUFFER(GTK_TEXT_VIEW(name)->buffer),\
					   	&start, &end, FALSE);\
		prozubi_passport_set_text(key, p, patient, text, true);\
	}

PATIENT_EDIT_TYPES

#undef PATIENT_EDIT_TYPE_ENTRY 
#undef PATIENT_EDIT_TYPE_EDATE 
#undef PATIENT_EDIT_TYPE_TEXTV 
	if (isNew)
		prozubi_passport_free(patient);
	patient_list_update(G_OBJECT(mainWindow), p);
	gtk_widget_destroy(patientEditWindow);
}

static GtkWidget *
patient_edit_new(
		GtkWidget *mainWindow, 
		prozubi_t *p, 
		struct passport_t *patient,
		gboolean isNew
		)
{
	if (!patient){
		g_print("patient is NULL\n");
		return NULL;
	}

	if (!patient){
		g_print("prozubi pointer is NULL\n");
		return NULL;
	}
		
	GtkWidget *patientEditWindow = create_patientEditWindow(); 
  /*widget_restore_state_from_config(patientEditWindow, "patientEditWindow", 300, 450); */
	char title[BUFSIZ];
	sprintf(title, "%s %s %s", patient->familiya,
			patient->imia, patient->otchestvo);
	gtk_window_set_title(GTK_WINDOW(patientEditWindow), title);
	
	GObject *delegate = G_OBJECT(patientEditWindow);
	g_object_set_data(delegate, "patient", patient);
	g_object_set_data(delegate, "prozubi", p);
	g_object_set_data(delegate, "mainWindow", mainWindow);
	g_object_set_data(delegate, "isNew", GINT_TO_POINTER(isNew));


#define PATIENT_EDIT_TYPE_ENTRY(member, name, key)\
	GtkWidget * name = lookup_widget(patientEditWindow, #name);\
	if (!name){\
		g_print("Error! Can't find " #name "\n");\
		return NULL;\
	}\
	gtk_entry_set_text(GTK_ENTRY(name), patient->member);

#define PATIENT_EDIT_TYPE_EDATE(member, name, key)\
	GtkWidget * name = lookup_widget(patientEditWindow, #name);\
	if (!name){\
		g_print("Error! Can't find " #name "\n");\
		return NULL;\
	}\
	{\
		GDateTime *date = g_date_time_new_from_unix_local(patient->member);\
		if (date){\
			char *str = g_date_time_format(date, "%d.%m.%Y");\
			if (str){\
				gtk_entry_set_text(GTK_ENTRY(name), str);\
				free(str);\
			}\
			g_date_time_unref(date);\
		}\
	}

#define PATIENT_EDIT_TYPE_TEXTV(member, name, key)\
	GtkWidget * name = lookup_widget(patientEditWindow, #name);\
	if (!name){\
		g_print("Error! Can't find " #name "\n");\
		return NULL;\
	}\
	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(GTK_TEXT_VIEW(name)->buffer),\
			patient->member, -1);

PATIENT_EDIT_TYPES

#undef PATIENT_EDIT_TYPE_ENTRY 
#undef PATIENT_EDIT_TYPE_EDATE 
#undef PATIENT_EDIT_TYPE_TEXTV 

	gtk_widget_show(patientEditWindow);
	return patientEditWindow;
}


#endif /* ifndef PATIENT_EDIT_H */
