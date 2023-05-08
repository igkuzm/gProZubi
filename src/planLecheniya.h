/**
 * File              : planLecheniya.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 08.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PLAN_LECHENIYA_H
#define PLAN_LECHENIYA_H

#include <gtk/gtk.h>
#include "pango/pango-font.h"
#include "prozubilib/cases.h"
#include "support.h"

#include "prozubilib/planlecheniya.h"

enum {
  PLAN_LECHENIYA_COLUMN_TITLE,
  PLAN_LECHENIYA_COLUMN_COUNT,
  PLAN_LECHENIYA_COLUMN_PRICE,
  PLAN_LECHENIYA_COLUMN_TOTAL,
  PLAN_LECHENIYA_COLUMN_REMOVE,
  PLAN_LECHENIYA_N_COLUMNS,
	PLAN_LECHENIYA_IS_EDITABLE,
	PLAN_LECHENIYA_PANGO_STYLE,
  PLAN_LECHENIYA_POINTER,
  PLAN_LECHENIYA_N_VALUES
};

static GtkTreeStore *
plan_lecheniya_table_model_new(){
	GtkTreeStore *store = gtk_tree_store_new(PLAN_LECHENIYA_N_VALUES, 
			G_TYPE_STRING, // title
			G_TYPE_INT, // count
			G_TYPE_INT, // price
			G_TYPE_INT, // total
			G_TYPE_POINTER, //remove
			G_TYPE_POINTER, //ncols
			G_TYPE_BOOLEAN, //editable
			G_TYPE_INT,     //style
			G_TYPE_POINTER //cJSON
	);

	return store;
}

static gboolean 
plan_lecheniya_table_model_free(GtkTreeModel* model, GtkTreePath* path, 
		GtkTreeIter* iter, gpointer data) 
{
	//struct case_t * c;
	//gtk_tree_model_get(model, iter, PLAN_LECHENIYA_ALLOCATED_PTR, &c, -1);	
	//prozubi_case_free(c);
	return FALSE;
}

static void * 
plan_lecheniya_fill_table(
			void *userdata,
			void *parent,
			cJSON *json,
			PLANLECHENIYA_TYPE type,
			int index,
			char * title,
			char * kod,
			int price,
			int count,
			int total
		)
{
	GObject *delegate = userdata;
	GtkTreeStore *store = g_object_get_data(delegate, "planLecheniyaStore");	

	gboolean editable = TRUE;
	gint style = PANGO_STYLE_NORMAL;
	if (type == PLANLECHENIYA_TYPE_STAGE ||
			type == PLANLECHENIYA_TYPE_STAGE_PRICE ||
			type == PLANLECHENIYA_TYPE_STAGE_DURATION ||
			type == PLANLECHENIYA_TYPE_TOTAL_PRICE ||
			type == PLANLECHENIYA_TYPE_TOTAL_DURATION
			)
	{
		style = PANGO_STYLE_OBLIQUE;
		editable = FALSE;
	}

	GtkTreeIter * parent_iter = parent;
	GtkTreeIter iter;
	gtk_tree_store_append(store, &iter, parent);
	gtk_tree_store_set(store, &iter, 
			PLAN_LECHENIYA_COLUMN_TITLE,  title,
			PLAN_LECHENIYA_COLUMN_COUNT,  count,
			PLAN_LECHENIYA_COLUMN_PRICE,  price,
			PLAN_LECHENIYA_COLUMN_TOTAL,  total,
			PLAN_LECHENIYA_COLUMN_REMOVE, NULL,
			PLAN_LECHENIYA_N_COLUMNS,     NULL,
			PLAN_LECHENIYA_IS_EDITABLE,   editable,
			PLAN_LECHENIYA_PANGO_STYLE,   style,
			PLAN_LECHENIYA_POINTER,       json,
	-1);

	return gtk_tree_iter_copy(&iter);
}

static int 
plan_lecheniya_get_list(void *userdata, cJSON * json){

	GObject *delegate = userdata;
	GtkTreeStore *store = g_object_get_data(delegate, "planLecheniyaStore");	

	prozubi_planlecheniya_foreach(json, delegate, plan_lecheniya_fill_table);	
	return 0;
}

static void 
plan_lecheniya_update(GObject * delegate, cJSON * json){
	g_print("Update planLecheniya\n");
	GtkTreeStore *store = g_object_get_data(delegate, "planLecheniyaStore");	

	if (!json){
		g_print("pointer is NULL\n");
		return;
	}

	/* set remove button insensitive */
	//gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(delegate, "petientRemoveButton")), FALSE);		

	/* clear store */
	gtk_tree_model_foreach (GTK_TREE_MODEL(store), 
			plan_lecheniya_table_model_free, NULL);
	gtk_tree_store_clear(store);
	
	/* get list of items */
	plan_lecheniya_get_list(delegate, json);
}

static void 
plan_lecheniya_row_activated(
		GtkTreeView *treeview, 
		GtkTreePath *path, 
		GtkTreeViewColumn *col, 
		gpointer userdata
		)
{
	g_print("Row activated\n");
	//GObject *delegate = userdata;	
	
	//prozubi_t *p = g_object_get_data(G_OBJECT(treeview), "prozubi");

	//GtkTreeModel *model = gtk_tree_view_get_model(treeview);
	//GtkTreeIter iter;

	//if (gtk_tree_model_get_iter(model, &iter, path)) {
		//[> expand/collapse row <]
		//if (gtk_tree_model_iter_has_child(model, &iter)){
			//if(gtk_tree_view_row_expanded(treeview, path))
				//gtk_tree_view_collapse_row(treeview, path);
			//else
				//gtk_tree_view_expand_row(treeview, path, FALSE);
			//return;
		//}		
		//CASES key;
		//PLAN_LECHENIYA_TYPE type;
		//char **combobox_array;
		//struct case_t *c;

		//gtk_tree_model_get(model, &iter, 
				//PLAN_LECHENIYA_POINTER, &c,
				//PLAN_LECHENIYA_COLUMN_TYPE, &type,
				//PLAN_LECHENIYA_COLUMN_COMBOBOX_ARRAY, &combobox_array,
				//PLAN_LECHENIYA_COLUMN_KEY, &key, -1); 

		//[> todo: refresh casesEditWindow <]
		//cases_edit_refresh(GTK_WIDGET(delegate), p, c, key, type, combobox_array);
	//}
}

static void 
plan_lecheniya_cursor_changed(
		GtkTreeView *treeview, 
		gpointer userdata
		)
{
	g_print("CURSOR CHANGED!\n");
	//GObject *delegate = userdata;	
	
	//prozubi_t *p = g_object_get_data(G_OBJECT(treeview), "prozubi");

	//GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
	//if (!selection){
		//g_print("Error! Can't get selection\n");
		//return;
	//}
	
	//GtkTreeModel *model = gtk_tree_view_get_model(treeview); 
	//if (!model){
		//g_print("Error! Can't get model\n");
		//return;
	//}	

	//GList *rows = 
			//gtk_tree_selection_get_selected_rows(selection, &model);
	//if (!rows){
		//g_print("Error! Can't get rows\n");
		//return;
	//}	

	//GtkTreePath *path = rows->data;
	//if (!path){
		//g_print("Error! Can't get path\n");
		//return;
	//}	

	//GtkTreeIter iter;
	//if (gtk_tree_model_get_iter(model, &iter, path)) {
		//[> expand/collapse row <]
		//if (gtk_tree_model_iter_has_child(model, &iter))
			//return;
				
		//CASES key;
		//PLAN_LECHENIYA_TYPE type;
		//char **combobox_array;
		//struct case_t *c;

		//gtk_tree_model_get(model, &iter, 
				//PLAN_LECHENIYA_POINTER, &c,
				//PLAN_LECHENIYA_COLUMN_TYPE, &type,
				//PLAN_LECHENIYA_COLUMN_COMBOBOX_ARRAY, &combobox_array,
				//PLAN_LECHENIYA_COLUMN_KEY, &key, -1); 

		//[> todo: refresh casesEditWindow <]
		//cases_edit_refresh(GTK_WIDGET(delegate), p, c, key, type, combobox_array);
	//}	
}

static void 
plan_lecheniya_ask_to_remove_responce(
		GtkDialog *dialog, 
		gint responce, 
		gpointer userdata
		)
{
	//if (responce == GTK_RESPONSE_DELETE_EVENT) {
		//g_print("Remove commited\n");

		//GObject *delegate = userdata;
		//struct case_t * c =  g_object_get_data(delegate, "casesToRemove"); 
		//prozubi_t * p =  g_object_get_data(delegate, "prozubi"); 
		//char * patientid =  g_object_get_data(delegate, "patientid"); 
		
		//if (!c){
			//g_print("cases is NULL\n");
			//[>gtk_window_destroy(GTK_WINDOW(dialog));<]
			//gtk_widget_destroy(GTK_WIDGET(dialog));
			//return;
		//}		

		//[> remove cases and update table <]
		//prozubi_case_remove(p, c);
		//plan_lecheniya_update(delegate, p, patientid);
	//}
	//[>gtk_window_destroy(GTK_WINDOW(dialog));<]
	//gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void 
plan_lecheniya_ask_to_remove(GObject *delegate, struct case_t * c) {
	//if (!c){
		//g_print("cases is NULL\n");
		//return;
	//}

	//g_object_set_data(delegate, "casesToRemove", c);
	
	//cJSON *json = c->case_list;
	//cJSON *title = cJSON_GetObjectItem(json, "parent");
	//char *t = cJSON_GetStringValue(title);
	
	//GtkWidget * mainWindow = GTK_WIDGET(delegate); 
	//GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(mainWindow),
			//GTK_DIALOG_MODAL,
			//GTK_MESSAGE_QUESTION,
			//GTK_BUTTONS_NONE,
			//"Удалить из базы: %s?", t);
	//gtk_window_set_title(GTK_WINDOW(dialog), "Удалить?");

	////add remove button
	//GtkWidget *button = gtk_button_new_with_label("УДАЛИТЬ");
	////GtkStyleContext *context = gtk_widget_get_style_context(button);
	////gtk_style_context_add_class(context, "destructive-action");
	//gtk_dialog_add_action_widget(GTK_DIALOG(dialog), button, GTK_RESPONSE_DELETE_EVENT);
	
	////add cancel button
	//gtk_dialog_add_button(GTK_DIALOG(dialog), "Отмена", GTK_RESPONSE_CANCEL);
	//gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
	//g_signal_connect (dialog, "response", 
			//G_CALLBACK (plan_lecheniya_ask_to_remove_responce), delegate);

	//gtk_widget_show_all(dialog);
}

static void
plan_lecheniya_remove_clicked(gpointer user_data){
	
	/* get mainWindow */
	//GtkWidget * casesWindow = 
			//lookup_widget(GTK_WIDGET(user_data), "casesWindow");
	//if (!casesWindow){
		//g_print("Error! Can't find casesWindow\n");
		//return;
	//}	
	//GObject *delegate = G_OBJECT(casesWindow);	
	
	//[> get treeView <]
	//GtkWidget * treeView = g_object_get_data(delegate, "treeView");
	//if (!treeView){
		//g_print("Error! Can't get treeView\n");
		//return;
	//}

	//GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));
	//if (!selection){
		//g_print("Error! Can't get selection\n");
		//return;
	//}
	
	//GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView)); 
	//if (!model){
		//g_print("Error! Can't get model\n");
		//return;
	//}	

	//GList *rows = 
			//gtk_tree_selection_get_selected_rows(selection, &model);
	//if (!rows){
		//g_print("Error! Can't get rows\n");
		//return;
	//}	

	//GtkTreePath *path = rows->data;
	//if (!path){
		//g_print("Error! Can't get path\n");
		//return;
	//}	

	//GtkTreeIter iter;
	//if (gtk_tree_model_get_iter(model, &iter, path)) {
		//struct case_t *c;
		//gtk_tree_model_get(model, &iter, PLAN_LECHENIYA_POINTER, &c, -1); 			
		
		//if (c){
			//plan_lecheniya_ask_to_remove(delegate, c);
		//}
	//}
}

static void 
plan_lecheniya_table_cell_edited_callback (
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

	//struct passport_t * c;
	//gtk_tree_model_get(treeModel, &iter_rawModel, PATIENT_LIST_POINTER, &c, -1); 			
	//prozubi_t *p = g_object_get_data(G_OBJECT(cell), "prozubi");

	//if (!c || !p)
		//return;
	
	//switch (column_number) {
		//case PATIENT_LIST_COLUMN_COMMENT:
			//{
				//if (!prozubi_passport_set_PASSPORTCOMMENT(p, c, new_text, true))
					//gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);				
				//break;
			//}			
		//default: break;
	//}
}


static GtkWidget *
plan_lecheniya_new(GtkWidget *parent, cJSON *json){
	/* set delegate */
	GObject *delegate = G_OBJECT(parent);

	/* get treeView */
	GtkWidget *treeView = gtk_tree_view_new();
	if (!treeView){
		g_print("Error! Can't create treeView\n");
		return NULL;
	}
	g_object_set_data(delegate, "treeView", treeView);

	/* create new model */
	GtkTreeStore *store = plan_lecheniya_table_model_new();
	g_object_set_data(delegate, "planLecheniyaStore", store);
	//g_object_set_data(delegate, "prozubi", p);

	plan_lecheniya_update(delegate, json);

	/* set tree model for view */
	//GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), GTK_TREE_MODEL(store));

	//gtk_tree_view_set_activate_on_single_click(GTK_TREE_VIEW(view), TRUE);
	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView)),
			GTK_SELECTION_SINGLE);

	//g_object_set_data(G_OBJECT(treeView), "prozubi", p);
	g_signal_connect(treeView, "row-activated", 
			(GCallback) plan_lecheniya_row_activated, delegate);
	
	g_signal_connect(treeView, "cursor-changed", 
			(GCallback) plan_lecheniya_cursor_changed, delegate);

	const char *column_titles[] = {
		"Наименование работы (услуги)",
		"Количество",
		"Цена",
		"Сумма",
		""
	};
	
	/* fill tableView */
	int i;
	for (i = 0; i < PLAN_LECHENIYA_N_COLUMNS; ++i) {
		
		GtkTreeViewColumn *column; 

		switch (i) {
			case PLAN_LECHENIYA_COLUMN_TITLE:
				{
					GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
					g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
					g_object_set(renderer, "wrap-width", 300, NULL);	
					g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
					g_object_set_data(G_OBJECT(renderer), "delegate", delegate);
					g_signal_connect(renderer, "edited", 
							(GCallback) plan_lecheniya_table_cell_edited_callback, treeView);
		
					//gtk_cell_renderer_set_fixed_size(renderer, -1, 40);
		
					column = gtk_tree_view_column_new_with_attributes(
							column_titles[i], renderer, 
							"text", i,
							"editable", PLAN_LECHENIYA_IS_EDITABLE,
							"style", PLAN_LECHENIYA_PANGO_STYLE,
							NULL);
					g_object_set(column, "expand", TRUE, NULL);	
					g_object_set(column, "resizable", TRUE, NULL);	
					break;
				}			
			case PLAN_LECHENIYA_COLUMN_COUNT:
				{
					GtkCellRenderer	*renderer = gtk_cell_renderer_spin_new();
					g_object_set(renderer, "editable", TRUE, NULL);
					g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
					g_object_set_data(G_OBJECT(renderer), "delegate", delegate);
					g_signal_connect(renderer, "edited", 
							(GCallback) plan_lecheniya_table_cell_edited_callback, treeView);

					GtkObject *adjustment = gtk_adjustment_new(0, 0, 100, 1, 10, 0);
					g_object_set(renderer, "adjustment", adjustment, NULL);
		
					gtk_cell_renderer_set_fixed_size(renderer, -1, 40);
		
					column = gtk_tree_view_column_new_with_attributes(
							column_titles[i], renderer, 
							"text", i, 
							"editable", PLAN_LECHENIYA_IS_EDITABLE,
							"style", PLAN_LECHENIYA_PANGO_STYLE,
							NULL);
					g_object_set(column, "resizable", TRUE, NULL);	
					break;
				}
			case PLAN_LECHENIYA_COLUMN_PRICE:
				{
					GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
					g_object_set(renderer, "editable", TRUE, NULL);
					g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
					g_object_set_data(G_OBJECT(renderer), "delegate", delegate);
					g_signal_connect(renderer, "edited", 
							(GCallback) plan_lecheniya_table_cell_edited_callback, treeView);
		
					//gtk_cell_renderer_set_fixed_size(renderer, -1, 40);
		
					column = gtk_tree_view_column_new_with_attributes(
							column_titles[i], renderer, 
							"text", i, 
							"editable", PLAN_LECHENIYA_IS_EDITABLE,
							"style", PLAN_LECHENIYA_PANGO_STYLE,
							NULL);
					g_object_set(column, "resizable", TRUE, NULL);	
					break;
				}
			case PLAN_LECHENIYA_COLUMN_TOTAL:
				{
					GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
					g_object_set(renderer, "editable", FALSE, NULL);
		
					//gtk_cell_renderer_set_fixed_size(renderer, -1, 40);
		
					column = gtk_tree_view_column_new_with_attributes(
							column_titles[i], renderer, "text", i,  NULL);
					g_object_set(column, "resizable", TRUE, NULL);	
					break;
				}
				case PLAN_LECHENIYA_COLUMN_REMOVE:
				{
					column = gtk_tree_view_column_new();
					gtk_tree_view_column_set_title(column, column_titles[i]);
					g_object_set(column, "resizable", TRUE, NULL);	
					break;
				}
				

			default: break;
		}
		
		gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);	
	}

	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(parent), treeView);	
	/*gtk_container_add(GTK_CONTAINER(parent), treeView);*/

	gtk_tree_view_expand_all(GTK_TREE_VIEW(treeView));
	gtk_widget_show(treeView);
	return treeView;	
}

#endif /* ifndef PLAN_LECHENIYA_H */

