/**
 * File              : callbacks.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 23.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "patientList.h"
#include "doctorList.h"

void clear_columns(GtkWidget *mainWindow){
	GtkWidget * treeView = lookup_widget(mainWindow, "mainTreeView");
	GList *columns = 
			gtk_tree_view_get_columns(GTK_TREE_VIEW(treeView));
	
	while (columns){
		GtkTreeViewColumn *column = columns->data;
		if (column)
			gtk_tree_view_remove_column(GTK_TREE_VIEW(treeView), column);

		columns = columns->next;
	}
}

void
on_patientListButton_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *mainWindow = lookup_widget(user_data, "mainWindow");;
	prozubi_t *p = g_object_get_data(G_OBJECT(mainWindow), "prozubi");
	clear_columns(mainWindow);
	patient_list_new(mainWindow, p);
}


void
on_doctorListButton_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *mainWindow = lookup_widget(user_data, "mainWindow");;
	prozubi_t *p = g_object_get_data(G_OBJECT(mainWindow), "prozubi");
	clear_columns(mainWindow);
	doctor_list_new(mainWindow, p);
}


void
on_priceListButton_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_configButton_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{

}

