/**
 * File              : patientList.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 05.08.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PATIENT_LIST_H
#define PATIENT_LIST_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "casesList.h"
#include "interface.h"
#include "configFile.h"

#include "prozubilib/prozubilib.h"

enum {
  PATIENT_LIST_COLUMN_FAMILIYA,
  PATIENT_LIST_COLUMN_IMIA,
  PATIENT_LIST_COLUMN_OTCHESTVO,
  PATIENT_LIST_COLUMN_DATEOFBIRTH,
  PATIENT_LIST_COLUMN_TEL,
  PATIENT_LIST_COLUMN_EMAIL,
  PATIENT_LIST_COLUMN_COMMENT,
  PATIENT_LIST_POINTER,
  PATIENT_LIST_N_COLUMNS
};

static GtkListStore 
*patient_list_table_model_new(){
	GtkListStore *store = gtk_list_store_new(PATIENT_LIST_N_COLUMNS, 
			G_TYPE_STRING, // familiya
			G_TYPE_STRING, // imia
			G_TYPE_STRING, // otchestvo
			G_TYPE_STRING,  // dateofbirth
			G_TYPE_STRING, // tel
			G_TYPE_STRING, // email
			G_TYPE_STRING, // comment
			G_TYPE_POINTER
	);

	return store;
}

static void 
patient_list_store_add(GtkListStore *store, struct passport_t * patient){
	char date[11];
	struct tm *tm = localtime(&patient->dateofbirth);
	strftime(date, 11, "%d.%m.%Y", tm);

	GtkTreeIter iter;
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 
			PATIENT_LIST_COLUMN_FAMILIYA,    patient->familiya,
			PATIENT_LIST_COLUMN_IMIA,        patient->imia,
			PATIENT_LIST_COLUMN_OTCHESTVO,   patient->otchestvo,
			PATIENT_LIST_COLUMN_DATEOFBIRTH, date,
			PATIENT_LIST_COLUMN_TEL,         patient->tel,
			PATIENT_LIST_COLUMN_EMAIL,       patient->email,
			PATIENT_LIST_COLUMN_COMMENT,     patient->comment,
			PATIENT_LIST_POINTER,            patient,
	-1);
}

static int 
patient_list_fill_table(void *userdata, struct passport_t * patient){

	GObject *delegate = userdata;
	GtkListStore *store = g_object_get_data(delegate, "patientListStore");	
		
	patient_list_store_add(store, patient);

	return 0;
}

static gboolean 
patient_list_table_model_free(GtkTreeModel* model, GtkTreePath* path, 
		GtkTreeIter* iter, gpointer data) 
{
	struct passport_t * patient;
	gtk_tree_model_get(model, iter, PATIENT_LIST_POINTER, &patient, -1);	
	prozubi_passport_free(patient);
	return FALSE;
}

static void 
patient_list_update(GObject * delegate, prozubi_t *p){
	g_print("Update patientList\n");
	GtkListStore *store = g_object_get_data(delegate, "patientListStore");	

	/* set remove button insensitive */
	//gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(delegate, "petientRemoveButton")), FALSE);		

	/* clear store */
	gtk_tree_model_foreach (GTK_TREE_MODEL(store), 
			patient_list_table_model_free, NULL);
	gtk_list_store_clear(store);
	
	/* get list of patients */
	prozubi_passport_foreach(p, NULL, delegate, patient_list_fill_table);
}

static void 
patient_list_row_activated(
		GtkTreeView *treeview, 
		GtkTreePath *path, 
		GtkTreeViewColumn *col, 
		gpointer userdata
		)
{
	g_print("Row activated\n");
	g_print("Path: %s\n", gtk_tree_path_to_string(path));

	GObject *delegate = userdata;

	/* set remove button insensitive */
	//gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "petientRemoveButton")), FALSE);		

	/* set selected patient to NULL */
	g_object_set_data(delegate, "selectedPatient", NULL);

	GtkTreeModel *model;
	GtkTreeIter   iter;

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		struct passport_t * patient;
		gtk_tree_model_get(model, &iter, PATIENT_LIST_POINTER, &patient, -1); 			
		
		if (patient){
			/* set remove button sensitive */
			//gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(app, "petientRemoveButton")), TRUE);			
			g_print("PATIENTID: %s\n", patient->id);
				
			/* set selected patient */
			g_object_set_data(delegate, "selectedPatient", patient);

			/* get window */
			GtkWidget *casesWindow = create_casesWindow();
			char title[BUFSIZ];
			sprintf(title, "%s %s %s", patient->familiya, patient->imia, patient->otchestvo);
			gtk_window_set_title(GTK_WINDOW(casesWindow), title);
			
			gtk_widget_show(casesWindow);

			/* start cases list */
			prozubi_t *p = g_object_get_data(delegate, "prozubi");
			cases_list_new(casesWindow, p, patient);
		}
	}
}

static void 
patient_list_table_cell_edited_callback (
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

	struct passport_t * c;
	gtk_tree_model_get(treeModel, &iter_rawModel, PATIENT_LIST_POINTER, &c, -1); 			
	prozubi_t *p = g_object_get_data(G_OBJECT(cell), "prozubi");

	if (!c || !p)
		return;
	
	switch (column_number) {
		case PATIENT_LIST_COLUMN_COMMENT:
			{
				if (!prozubi_passport_set_PASSPORTCOMMENT(p, c, new_text, true))
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);				
				break;
			}			
		default: break;
	}
}

static void 
patient_list_ask_to_remove_responce(
		GtkDialog *dialog, 
		gint responce, 
		gpointer userdata
		)
{
	if (responce == GTK_RESPONSE_DELETE_EVENT) {
		g_print("Remove commited\n");

		GObject *delegate = userdata;
		struct passport_t * patient =  g_object_get_data(delegate, "patientToRemove"); 
		prozubi_t * p =  g_object_get_data(delegate, "prozubi"); 
		
		if (!patient){
			g_print("Patient is NULL\n");
			/*gtk_window_destroy(GTK_WINDOW(dialog));*/
			gtk_widget_destroy(GTK_WIDGET(dialog));
			return;
		}		

		/* remove patient and update table */
		prozubi_passport_remove(p, patient);
		patient_list_update(delegate, p);
	}
	/*gtk_window_destroy(GTK_WINDOW(dialog));*/
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void 
patient_list_ask_to_remove(GObject *delegate, struct passport_t * patient) {
	if (!patient){
		g_print("Patient is NULL\n");
		return;
	}
	
	g_object_set_data(delegate, "patientToRemove", patient);
	
	char title[BUFSIZ] = "";
	sprintf(title, 
			"Удалить из базы пациенты: %s?", 
			patient->familiya
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
			G_CALLBACK (patient_list_ask_to_remove_responce), delegate);

	gtk_widget_show_all(dialog);
}

static void
patient_list_remove_clicked(gpointer user_data){
	
	GtkWidget * mainWindow = user_data; 
	
	/* get mainWindow */
	if (!mainWindow){
		g_print("Error! Can't find mainWindow\n");
		return;
	}	
	GObject *delegate = G_OBJECT(mainWindow);	
	
	/* get treeView */
	GtkWidget * treeView = g_object_get_data(delegate, "mainTreeView");
	if (!treeView){
		g_print("Error! Can't get treeView\n");
		return;
	}

	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));
	if (!selection){
		g_print("Error! Can't get selection\n");
		return;
	}
	
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView)); 
	if (!model){
		g_print("Error! Can't get model\n");
		return;
	}	

	GList *rows = 
			gtk_tree_selection_get_selected_rows(selection, &model);
	if (!rows){
		g_print("Error! Can't get rows\n");
		return;
	}	

	GtkTreePath *path = rows->data;
	if (!path){
		g_print("Error! Can't get path\n");
		return;
	}	

	GtkTreeIter iter;
	if (gtk_tree_model_get_iter(model, &iter, path)) {
		struct passport_t *c;
		gtk_tree_model_get(model, &iter, PATIENT_LIST_POINTER, &c, -1); 			
		
		if (c){
			patient_list_ask_to_remove(delegate, c);
		}
	}
}

static void
patient_list_resize_column (GtkTreeViewColumn       *column,
                            gpointer                 w,
                            gpointer                 user_data)
{
	gint width = gtk_tree_view_column_get_width(column);
	gint col = GPOINTER_TO_INT(user_data);
	save_colummn_state(width - 4, col, "patientList");
}

static gboolean
patient_list_SearchEqualFunc(
		GtkTreeModel* model,
		int column,
		const char* key,
		GtkTreeIter* iter,
		gpointer search_data
		)
{
	struct passport_t * patient;
	gtk_tree_model_get(model, iter, PATIENT_LIST_POINTER, &patient, -1); 			
	if (!patient)
		return 1;
	
	char *needle = g_utf8_strup(key, BUFSIZ); 

#define PASSPORT_COLUMN_TEXT(member, number, title)\
	{\
		char *haystack = g_utf8_strup(patient->member, BUFSIZ);\
		if (g_strrstr(haystack, needle) != NULL){\
			free(needle);\
			free(haystack);\
			return 0;\
		}\
	}	
#define PASSPORT_COLUMN_DATE(member, number, title)
	//\
	{\
		GDateTime *d = g_date_time_new_from_unix_local(patient->member);\
		gchar *haystack = g_date_time_format(d,  "%d.%m.%Y");\
		if (g_strrstr(haystack, needle) != NULL){\
			free(needle);\
			free(haystack);\
			return 0;\
		}\
	}
	PASSPORT_COLUMNS
#undef PASSPORT_COLUMN_DATE
#undef PASSPORT_COLUMN_TEXT

	free(needle);
	return 1;
}


static GtkWidget *
patient_list_new(GtkWidget *mainWindow, prozubi_t *p){
	/* set delegate */
	GObject *delegate = G_OBJECT(mainWindow);
	g_object_set_data(delegate, "selectedItem", "patients");

	GtkWidget *mainView = g_object_get_data(delegate, "mainView");
	
	GtkWidget * oldWidget = gtk_bin_get_child (GTK_BIN (mainView)); 
	if (oldWidget)
		gtk_container_remove (GTK_CONTAINER (mainView), oldWidget); 

	/* get treeView */
	GtkWidget * treeView = gtk_tree_view_new();
	if (!treeView){
		g_print("Error! Can't create treeView\n");
		return NULL;
	}
	g_object_set_data(delegate, "mainTreeView", treeView);

	/* create new model */
	GtkListStore *store = patient_list_table_model_new();
	g_object_set_data(delegate, "patientListStore", store);

	patient_list_update(delegate, p);

	/* set tree model for view */
	//GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), GTK_TREE_MODEL(store));
	
	GtkWidget *search = g_object_get_data(delegate, "searchEntry");
	gtk_tree_view_set_search_entry(GTK_TREE_VIEW(treeView), GTK_ENTRY(search));
	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeView), true);	
	gtk_tree_view_set_search_equal_func(GTK_TREE_VIEW(treeView), patient_list_SearchEqualFunc, NULL, NULL);

	//gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(view), TRUE);
	g_signal_connect(treeView, "row-activated", 
			(GCallback) patient_list_row_activated, delegate);

	const char *column_titles[] = {
		"Фамилия", 
		"Имя", 
		"Отчество", 
		"Дата рождения", 
		"Телефон", 
		"Email", 
		"Коментарий"
	};
	
	/* fill tableView */
	int i;
	for (i = 0; i < PATIENT_LIST_N_COLUMNS -1; ++i) {

		/* get width */
		gint width = get_colummn_state(i, "patientList"); 
		if (width < 40)
			width = 40;

		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", FALSE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", width, NULL);	
		g_object_set(renderer, "width", width, NULL);	
		
		g_object_set_data(G_OBJECT(renderer), "prozubi", p);
		
		g_signal_connect(renderer, "edited", 
				(GCallback) patient_list_table_cell_edited_callback, treeView);

		g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
		g_object_set_data(G_OBJECT(renderer), "delegate", delegate);
		
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
				column_titles[i], renderer, "text", i,  NULL);

		switch (i) {
			case PATIENT_LIST_COLUMN_COMMENT:
				{
					gtk_cell_renderer_set_fixed_size(renderer, -1, 40);
					g_object_set(column, "expand", TRUE, NULL);	
					g_object_set(renderer, "wrap-width", 300, NULL);	
					g_object_set(renderer, "editable", TRUE, NULL);
					break;
				}			
			default: break;
		}
		g_object_set(column, "resizable", TRUE, NULL);	
		
		gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);	
		
		g_signal_connect ((gpointer) column, "notify::width",
										G_CALLBACK (patient_list_resize_column),
										GINT_TO_POINTER(i));		
	}

#if !GTK_CHECK_VERSION(3, 8, 0)
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(mainView), treeView);	
#else
	gtk_container_add(GTK_CONTAINER(mainView), treeView);
#endif

	gtk_widget_show(treeView);
	return treeView;	
}

#endif /* ifndef PATIENT_LIST_H */
