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
#include "prozubilib/cases.h"
#include "support.h"

#include "prozubilib/planlecheniya.h"

enum {
  PLAN_LECHENIYA_COLUMN_TITLE,
  PLAN_LECHENIYA_COLUMN_COUNT,
  PLAN_LECHENIYA_COLUMN_PRICE,
  PLAN_LECHENIYA_COLUMN_TOTAL,
  PLAN_LECHENIYA_BUTTON,
  PLAN_LECHENIYA_POINTER,
  PLAN_LECHENIYA_N_COLUMNS
};

static GtkTreeStore *
plan_lecheniya_table_model_new(){
	GtkTreeStore *store = gtk_tree_store_new(PLAN_LECHENIYA_N_COLUMNS, 
			G_TYPE_STRING, // title
			G_TYPE_INT, // count
			G_TYPE_INT, // price
			G_TYPE_INT, // total
			G_TYPE_POINTER, //button
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

	GtkTreeIter * parent_iter = parent;
	GtkTreeIter iter;
	gtk_tree_store_append(store, &iter, parent);
	gtk_tree_store_set(store, &iter, 
			PLAN_LECHENIYA_COLUMN_TITLE, title,
			PLAN_LECHENIYA_COLUMN_COUNT, count,
			PLAN_LECHENIYA_COLUMN_PRICE, price,
			PLAN_LECHENIYA_COLUMN_TOTAL, total,
			PLAN_LECHENIYA_BUTTON,       NULL,
			PLAN_LECHENIYA_POINTER,      json,
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
	for (i = 0; i < PLAN_LECHENIYA_N_COLUMNS -1; ++i) {
		
		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", TRUE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", 300, NULL);	
		
		//g_signal_connect(renderer, "edited", 
				//(GCallback) plan_lecheniya_table_cell_edited_callback, treeView);
		
		g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
		g_object_set_data(G_OBJECT(renderer), "delegate", delegate);
		
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
				column_titles[i], renderer, "text", i,  NULL);

		switch (i) {
			case PLAN_LECHENIYA_COLUMN_TITLE:
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

	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(parent), treeView);	
	/*gtk_container_add(GTK_CONTAINER(parent), treeView);*/

	gtk_tree_view_expand_all(GTK_TREE_VIEW(treeView));
	gtk_widget_show(treeView);
	return treeView;	
}

#endif /* ifndef PLAN_LECHENIYA_H */

