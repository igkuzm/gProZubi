/**
 * File              : casesEdit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.05.2023
 * Last Modified Date: 05.08.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CASES_EDIT_H
#define CASES_EDIT_H

#include <gtk/gtk.h>
#include <string.h>
#include "cairo.h"
#include "glibconfig.h"
#include "prozubilib/passport.h"
#include "prozubilib/cases.h"
#include "prozubilib/planlecheniya.h"
#include "planLecheniya.h"

#include "Calendar.h"
#include "ZubFormula.h"
#include "Xray.h"
#include "TextEdit.h"
#include "ComboBox.h"


#include "prozubilib/prozubilib.h"

static GtkWidget *
cases_edit_refresh(
		GtkWidget *casesWindow, 
		prozubi_t *p, 
		struct passport_t *patient,
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
				return text_edit_new(casesEditFrame, p, c, key);
			}
		case CASES_LIST_TYPE_COMBOBOX:
			{
				return combo_box_new(casesEditFrame, p, c, key, combobox_array);
			}			
		case CASES_LIST_TYPE_DATE:
			{
				return calendar_new(casesEditFrame, p, c, key);
			}			

		case CASES_LIST_TYPE_ZFORMULA:
			{
				return zubformula_new(casesEditFrame, p, c);
			}			

		case CASES_LIST_TYPE_PLANLECHENIYA:
			{
				return plan_lecheniya_new(casesEditFrame, c->planlecheniya, p, patient, c);	
			}			

		case CASES_LIST_TYPE_XRAY:
			{
				return xray_new(GTK_WINDOW(casesWindow), casesEditFrame, p, c);
			}			

		default:
			return NULL;
	}
}
#endif /* ifndef CASES_EDIT_H */
