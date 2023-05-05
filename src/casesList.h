/**
 * File              : casesList.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 05.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CASES_LIST_H
#define CASES_LIST_H

#include <gtk/gtk.h>
#include "prozubilib/cases.h"
#include "support.h"
#include "casesEdit.h"

#include "prozubilib/prozubilib.h"

enum {
  CASES_LIST_COLUMN_TITLE,
  CASES_LIST_COLUMN_KEY,
  CASES_LIST_COLUMN_TYPE,
  CASES_LIST_COLUMN_COMBOBOX_ARRAY,
  CASES_LIST_POINTER,
  CASES_LIST_N_COLUMNS
};

static GtkTreeStore *
cases_list_table_model_new(){
	GtkTreeStore *store = gtk_tree_store_new(CASES_LIST_N_COLUMNS, 
			G_TYPE_STRING, // title
			G_TYPE_INT, // key
			G_TYPE_INT, // type
			G_TYPE_POINTER, //combobox array
			G_TYPE_POINTER
	);

	return store;
}

static gboolean 
cases_list_table_model_free(GtkTreeModel* model, GtkTreePath* path, 
		GtkTreeIter* iter, gpointer data) 
{
	struct case_t * c;
	gtk_tree_model_get(model, iter, CASES_LIST_POINTER, &c, -1);	
	prozubi_case_free(c);
	return FALSE;
}

static void * 
cases_list_fill_table(
		void *user_data,
		struct case_t *c,
		void * parent,
		bool has_children,
		char * title,
		enum tagCASES key,
		enum tagCASES_LIST_TYPE type,
		char ** array
		)
{
	GObject *delegate = user_data;
	GtkTreeStore *store = g_object_get_data(delegate, "casesListStore");	

	GtkTreeIter * parent_iter = parent;
	GtkTreeIter iter;
	gtk_tree_store_append(store, &iter, parent);
	gtk_tree_store_set(store, &iter, 
			CASES_LIST_COLUMN_TITLE,          title,
			CASES_LIST_COLUMN_KEY,            key,
			CASES_LIST_COLUMN_TYPE,           type,
			CASES_LIST_COLUMN_COMBOBOX_ARRAY, array,
			CASES_LIST_POINTER,               c,
	-1);

	return gtk_tree_iter_copy(&iter);
}

static int 
cases_list_get_list(void *userdata, struct case_t * c){

	GObject *delegate = userdata;
	GtkTreeStore *store = g_object_get_data(delegate, "casesListStore");	

	prozubi_cases_list_foreach(c, delegate, cases_list_fill_table);	
	return 0;
}

static void 
cases_list_update(GObject * delegate, prozubi_t *p, char patientid[37]){
	g_print("Update casesList\n");
	GtkTreeStore *store = g_object_get_data(delegate, "casesListStore");	

	if (!p){
		g_print("prozubi_t pointer is NULL\n");
		return;
	}

	/* set remove button insensitive */
	//gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(delegate, "petientRemoveButton")), FALSE);		

	/* set selected cases to NULL */
	g_object_set_data(delegate, "selectedcases", NULL);	

	/* clear store */
	gtk_tree_model_foreach (GTK_TREE_MODEL(store), 
			cases_list_table_model_free, NULL);
	gtk_tree_store_clear(store);
	
	/* get list of casess */
	prozubi_cases_foreach(p, patientid, delegate, cases_list_get_list);
}

static void 
cases_list_row_activated(
		GtkTreeView *treeview, 
		GtkTreePath *path, 
		GtkTreeViewColumn *col, 
		gpointer userdata
		)
{
	g_print("Row activated\n");
	GObject *delegate = userdata;	
	
	prozubi_t *p = g_object_get_data(G_OBJECT(treeview), "prozubi");

	GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	GtkTreeIter iter;

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		/* expand/collapse row */
		if (gtk_tree_model_iter_has_child(model, &iter)){
			if(gtk_tree_view_row_expanded(treeview, path))
				gtk_tree_view_collapse_row(treeview, path);
			else
				gtk_tree_view_expand_row(treeview, path, FALSE);
			return;
		}		
		CASES key;
		CASES_LIST_TYPE type;
		char **combobox_array;
		struct case_t *c;

		gtk_tree_model_get(model, &iter, 
				CASES_LIST_POINTER, &c,
				CASES_LIST_COLUMN_TYPE, &type,
				CASES_LIST_COLUMN_COMBOBOX_ARRAY, &combobox_array,
				CASES_LIST_COLUMN_KEY, &key, -1); 

		/* todo: refresh casesEditWindow */
		cases_edit_refresh(GTK_WIDGET(delegate), p, c, key, type, combobox_array);
	}
}

static void 
cases_list_ask_to_remove_responce(
		GtkDialog *dialog, 
		gint responce, 
		gpointer userdata
		)
{
	if (responce == GTK_RESPONSE_DELETE_EVENT) {
		g_print("Remove commited\n");

		GObject *delegate = userdata;
		struct cases * cases =  g_object_get_data(delegate, "casesToRemove"); 
		
		if (!cases){
			g_print("cases is NULL\n");
			/*gtk_window_destroy(GTK_WINDOW(dialog));*/
			gtk_widget_destroy(GTK_WIDGET(dialog));
			return;
		}		

		/* remove cases and update table */
		/*! TODO: remove parient
		*  \todo remove parient
		*/
	}
	/*gtk_window_destroy(GTK_WINDOW(dialog));*/
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void 
cases_list_ask_to_remove(GObject *delegate, struct case_t * c) {
	if (!c){
		g_print("cases is NULL\n");
		return;
	}
	
	g_object_set_data(delegate, "casesToRemove", c);
	
	char *t = NULL;
	cJSON *json = c->case_list;
	cJSON *title = cJSON_GetObjectItem(json, "parent");
	
	GtkWidget * mainWindow = GTK_WIDGET(delegate); 
	GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow),
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_NONE,
			"Удалить из базы: %s?", t);
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
			G_CALLBACK (cases_list_ask_to_remove_responce), delegate);

	gtk_widget_show_all(dialog);
}

static GtkWidget *
cases_list_new(GtkWidget *casesWindow, prozubi_t *p, char patientid[37]){
	/* set delegate */
	GObject *delegate = G_OBJECT(casesWindow);

	/* get treeView */
	GtkWidget * casesListView = lookup_widget(casesWindow, "casesListView");
	if (!casesListView){
		g_print("Error! Can't find casesListView\n");
		return NULL;
	}
	gtk_container_remove (GTK_CONTAINER (casesListView), 
				gtk_bin_get_child (GTK_BIN (casesListView)));

	GtkWidget *treeView = gtk_tree_view_new();
	if (!treeView){
		g_print("Error! Can't create treeView\n");
		return NULL;
	}

	/* create new model */
	GtkTreeStore *store = cases_list_table_model_new();
	g_object_set_data(delegate, "casesListStore", store);
	g_object_set_data(delegate, "patientid", patientid);
	g_object_set_data(delegate, "prozubi", p);

	cases_list_update(delegate, p, patientid);

	/* set tree model for view */
	//GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), GTK_TREE_MODEL(store));

	//gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(view), TRUE);
	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView)),
			GTK_SELECTION_SINGLE);

	g_object_set_data(G_OBJECT(treeView), "prozubi", p);
	g_signal_connect(treeView, "row-activated", 
			(GCallback) cases_list_row_activated, delegate);

	const char *column_titles[] = {
		""
	};
	
	/* fill tableView */
	int i;
	for (i = 0; i < 1; ++i) {
		
		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", FALSE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", 300, NULL);	
		
		//g_signal_connect(renderer, "edited", 
				//(GCallback) cases_list_table_cell_edited_callback, treeView);
		
		g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
		g_object_set_data(G_OBJECT(renderer), "delegate", delegate);
		
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
				column_titles[i], renderer, "text", i,  NULL);

		switch (i) {
			case CASES_LIST_COLUMN_TITLE:
				{
					gtk_cell_renderer_set_fixed_size(renderer, -1, 40);
					g_object_set(column, "expand", TRUE, NULL);	
					break;
				}			
			
			default: break;
		}
		g_object_set(column, "resizable", TRUE, NULL);	
		
		gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);	
	}

	gtk_widget_show(treeView);
	return treeView;	
}

#endif /* ifndef CASES_LIST_H */
