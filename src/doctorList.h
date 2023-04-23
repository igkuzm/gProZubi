/**
 * File              : doctorList.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 01.04.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef DOCTOR_LIST_H
#define DOCTOR_LIST_H

#include <gtk/gtk.h>
#include "support.h"

#include "/Users/kuzmich/src/prozubilib/prozubilib.h"

enum {
  DOCTOR_LIST_COLUMN_FIO,
  DOCTOR_LIST_COLUMN_TEL,
  DOCTOR_LIST_COLUMN_EMAIL,
  DOCTOR_LIST_POINTER,
  DOCTOR_LIST_N_COLUMNS
};

static
GtkListStore *doctor_list_table_model_new(){
	GtkListStore *store = gtk_list_store_new(DOCTOR_LIST_N_COLUMNS, 
			G_TYPE_STRING, // fio
			G_TYPE_UINT64, // tel
			G_TYPE_STRING, // email
			G_TYPE_POINTER
	);

	return store;
}

static
void doctor_list_store_add(GtkListStore *store, struct doctor * doctor){
	GtkTreeIter iter;
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 
			DOCTOR_LIST_COLUMN_FIO,   doctor->fio,
			DOCTOR_LIST_COLUMN_TEL,   doctor->tel,
			DOCTOR_LIST_COLUMN_EMAIL, doctor->calendar,
			DOCTOR_LIST_POINTER,      doctor,
	-1);
}

static
int doctor_list_fill_table(struct doctor * doctor, void *userdata, char *error){
	if (error) {
		g_print("ERROR: %s\n", error);
		return 0;
	}

	GObject *delegate = userdata;
	GtkListStore *store = g_object_get_data(delegate, "doctorListStore");	
		
	doctor_list_store_add(store, doctor);

	return 0;
}

static
gboolean doctor_list_table_model_free(GtkTreeModel* model, GtkTreePath* path, 
		GtkTreeIter* iter, gpointer data) 
{
	struct doctor * doctor;
	gtk_tree_model_get(model, iter, DOCTOR_LIST_POINTER, &doctor, -1);	
	free(doctor);
	return FALSE;
}

static
void doctor_list_update(GObject * delegate){
	g_print("Update doctorList\n");
	GtkListStore *store = g_object_get_data(delegate, "doctorListStore");	

	/* set remove button insensitive */
	//gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(delegate, "petientRemoveButton")), FALSE);		

	/* set selected patient to NULL */
	g_object_set_data(delegate, "selectedDoctor", NULL);	

	/* clear store */
	gtk_tree_model_foreach (GTK_TREE_MODEL(store), 
			doctor_list_table_model_free, NULL);
	gtk_list_store_clear(store);
	
	/* get list of patients */
	prozubilib_get_doctors(delegate, doctor_list_fill_table);
}

static
void doctor_list_row_activated(
		GtkTreeView *treeview, 
		GtkTreePath *path, 
		GtkTreeViewColumn *col, 
		gpointer userdata
		)
{
	g_print("Row activated\n");

	GObject *delegate = userdata;

	/* set remove button insensitive */
	//gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "petientRemoveButton")), FALSE);		

	/* set selected patient to NULL */
	g_object_set_data(delegate, "selectedDoctor", NULL);

	GtkTreeModel *model;
	GtkTreeIter   iter;

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		struct doctor * doctor;
		gtk_tree_model_get(model, &iter, DOCTOR_LIST_POINTER, &doctor, -1); 			
		
		if (doctor){
			/* set remove button sensitive */
			//gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "petientRemoveButton")), TRUE);		
				
			/* set selected patient */
			g_object_set_data(delegate, "selectedDoctor", doctor);
		}
	}
}

static
void doctor_list_table_cell_edited_callback (
		GtkCellRendererText *cell, 
		gchar *path_string, 
		gchar *new_text, 
		gpointer user_data
		)
{
	guint column_number = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "column_number"));
	g_print("EDITED path: %s, col: %d\n", path_string, column_number);
	
	//get application delegate
	GObject *delegate = g_object_get_data(G_OBJECT(cell), "delegate");

	// we HAVE TO use GtkTreeView within gpointer!
	//  otherwise we could not differntiate the model type!
	GtkTreeView  *treeview  = GTK_TREE_VIEW(user_data);
	GtkTreeModel *treeModel = gtk_tree_view_get_model(treeview);

	// we need to use GtkListStore, because GtkTreeModel does not
	//  provide an interface for cell changing.
	GtkListStore *model;   
	GtkTreeIter iter_rawModel;

	// check if we're working on the raw model or on a sorted version
	//  of it
	if(GTK_IS_LIST_STORE(treeModel)){
		// just use the model as is    
		model = GTK_LIST_STORE(treeModel);

		// retrieve the iterator for the cell, that should be changed
		gtk_tree_model_get_iter_from_string((GtkTreeModel*)model, &iter_rawModel, path_string);

	} else { // we're working on a sorted model   
		// We need to change to a usual model.
		GtkTreeModelSort *sortedModel = GTK_TREE_MODEL_SORT(treeModel);
		model = GTK_LIST_STORE(gtk_tree_model_sort_get_model(sortedModel));

		// get the iterator within the sorted model
		GtkTreeIter iter_sortedModel;
		gtk_tree_model_get_iter_from_string((GtkTreeModel*)sortedModel, &iter_sortedModel, path_string);  

		// convert the iterator to one of the raw model.
		// (Otherwise the wrong cell will change)
		gtk_tree_model_sort_convert_iter_to_child_iter(sortedModel, &iter_rawModel, &iter_sortedModel);
    }

	struct doctor * doctor;
	gtk_tree_model_get(treeModel, &iter_rawModel, DOCTOR_LIST_POINTER, &doctor, -1); 			
	
	switch (column_number) {
		default: break;
	}
}

static
void doctor_list_ask_to_remove_responce(
		GtkDialog *dialog, 
		gint responce, 
		gpointer userdata
		)
{
	if (responce == GTK_RESPONSE_DELETE_EVENT) {
		g_print("Remove commited\n");

		GObject *delegate = userdata;
		struct patient * patient =  g_object_get_data(delegate, "doctorToRemove"); 
		
		if (!patient){
			g_print("Patient is NULL\n");
			/*gtk_window_destroy(GTK_WINDOW(dialog));*/
			gtk_widget_destroy(GTK_WIDGET(dialog));
			return;
		}		

		/* remove patient and update table */
		/*! TODO: remove parient
		*  \todo remove parient
		*/
	}
	/*gtk_window_destroy(GTK_WINDOW(dialog));*/
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static
void doctor_list_ask_to_remove(GObject *delegate, struct doctor * doctor) {
	if (!doctor){
		g_print("doctor is NULL\n");
		return;
	}
	
	g_object_set_data(delegate, "doctorToRemove", doctor);
	
	char title[BUFSIZ] = "";
	sprintf(title, 
			"Удалить из базы сотрудника: %s?", 
			doctor->fio
	);
	
	GtkWidget * mainWindow = GTK_WIDGET(delegate); 
	GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_NONE,
			"%s", title);
	gtk_window_set_title(GTK_WINDOW(dialog), "Удалить?");

	//add remove button
	GtkWidget *button = gtk_button_new_with_label("УДАЛИТЬ");
	//GtkStyleContext *context = gtk_widget_get_style_context(button);
	//gtk_style_context_add_class(context, "destructive-action");
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog), button, GTK_RESPONSE_DELETE_EVENT);
	
	//add cancel button
	gtk_dialog_add_button(GTK_DIALOG(dialog), "Отмена", GTK_RESPONSE_CANCEL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
	g_signal_connect (dialog, "response", 
			G_CALLBACK (doctor_list_ask_to_remove_responce), delegate);

	gtk_widget_show_all(dialog);
}

static
GtkWidget *doctor_list_new(GtkWidget *mainWindow){
	/* set delegate */
	GObject *delegate = G_OBJECT(mainWindow);

	/* get treeView */
	GtkWidget * treeView = lookup_widget(mainWindow, "mainTreeView");
	if (!treeView){
		g_print("Error! Can't find mainTreeView\n");
		return NULL;
	}

	/* create new model */
	GtkListStore *store = doctor_list_table_model_new();
	g_object_set_data(delegate, "doctorListStore", store);

	doctor_list_update(delegate);

	/* set tree model for view */
	//GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), GTK_TREE_MODEL(store));

	//gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(view), TRUE);
	g_signal_connect(treeView, "row-activated", 
			(GCallback) doctor_list_row_activated, delegate);

	const char *column_titles[] = {
		"ФИО", 
		"Телефон", 
		"Email" 
	};
	
	/* fill tableView */
	int i;
	for (i = 0; i < DOCTOR_LIST_N_COLUMNS -1; ++i) {
		
		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", TRUE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", 60, NULL);	
		
		g_signal_connect(renderer, "edited", 
				(GCallback) doctor_list_table_cell_edited_callback, treeView);
		
		g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
		g_object_set_data(G_OBJECT(renderer), "delegate", delegate);
		
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
				column_titles[i], renderer, "text", i,  NULL);

		switch (i) {
			default: break;
		}
		g_object_set(column, "resizable", TRUE, NULL);	
		
		gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);	
	}

	gtk_widget_show(treeView);
	return treeView;	
}

#endif /* ifndef DOCTOR_LIST_H */
