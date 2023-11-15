/**
 * File              : planLecheniya.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 05.08.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PLAN_LECHENIYA_H
#define PLAN_LECHENIYA_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "prozubilib/passport.h"
#include "prozubilib/prozubilib.h"
#include "prozubilib/cases.h"
#include "prozubilib/documents.h"

#include "prozubilib/planlecheniya.h"

enum {
  PLAN_LECHENIYA_COLUMN_TITLE,
  PLAN_LECHENIYA_COLUMN_COUNT,
  PLAN_LECHENIYA_COLUMN_PRICE,
  PLAN_LECHENIYA_COLUMN_TOTAL,
  PLAN_LECHENIYA_COLUMN_BUTTON,
  PLAN_LECHENIYA_N_COLUMNS,
	PLAN_LECHENIYA_IS_TITLE_PRICE_EDITABLE,
	PLAN_LECHENIYA_IS_COUNT_EDITABLE,
	PLAN_LECHENIYA_PANGO_STYLE,
  PLAN_LECHENIYA_TYPE,
  PLAN_LECHENIYA_KOD,
  PLAN_LECHENIYA_N_VALUES
};

static GtkTreeStore *
plan_lecheniya_table_model_new(){
	GtkTreeStore *store = gtk_tree_store_new(PLAN_LECHENIYA_N_VALUES, 
			G_TYPE_STRING, // title
			G_TYPE_STRING, // count
			G_TYPE_STRING, // price
			G_TYPE_STRING, // total
			G_TYPE_STRING, //remove
			G_TYPE_POINTER, //ncols
			G_TYPE_BOOLEAN, //title/price editable
			G_TYPE_BOOLEAN, //count editable
			G_TYPE_INT,     //style
			G_TYPE_INT,     //type
			G_TYPE_STRING   //kod
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
			struct planlecheniya_t *t
		)
{
	GObject *delegate = userdata;
	GtkTreeStore *store = g_object_get_data(delegate, "planLecheniyaStore");	

	char * stock_id = NULL;
	gboolean tp_editable = TRUE, count_editable = TRUE;
	gint style = PANGO_STYLE_NORMAL;
	
	if (t->type == PLANLECHENIYA_TYPE_ITEM){
		stock_id = GTK_STOCK_REMOVE; 
	}
	if (t->type == PLANLECHENIYA_TYPE_STAGE){
		stock_id = GTK_STOCK_ADD; 
	}

	if (t->type == PLANLECHENIYA_TYPE_STAGE ||
			t->type == PLANLECHENIYA_TYPE_STAGE_PRICE ||
			t->type == PLANLECHENIYA_TYPE_STAGE_DURATION ||
			t->type == PLANLECHENIYA_TYPE_TOTAL_PRICE ||
			t->type == PLANLECHENIYA_TYPE_TOTAL_DURATION
			)
	{
		style = PANGO_STYLE_OBLIQUE;
		tp_editable = FALSE;
		count_editable = FALSE;
	}
	if (t->type == PLANLECHENIYA_TYPE_STAGE_DURATION) {
		count_editable = TRUE;
	}

	GtkTreeIter * parent_iter = parent;
	GtkTreeIter iter;
	gtk_tree_store_append(store, &iter, parent);
	gtk_tree_store_set(store, &iter, 
			PLAN_LECHENIYA_COLUMN_TITLE,  t->title,
			PLAN_LECHENIYA_COLUMN_COUNT,  t->count,
			PLAN_LECHENIYA_COLUMN_PRICE,  t->price,
			PLAN_LECHENIYA_COLUMN_TOTAL,  t->total,
			PLAN_LECHENIYA_COLUMN_BUTTON, stock_id,
			PLAN_LECHENIYA_N_COLUMNS,     NULL,
			PLAN_LECHENIYA_IS_TITLE_PRICE_EDITABLE,   tp_editable,
			PLAN_LECHENIYA_IS_COUNT_EDITABLE,   count_editable,
			PLAN_LECHENIYA_PANGO_STYLE,   style,
			PLAN_LECHENIYA_TYPE,          t->type,
			PLAN_LECHENIYA_KOD,           t->kod,
	-1);

	return gtk_tree_iter_copy(&iter);
}

static int 
plan_lecheniya_get_list(void *userdata, cJSON * json){

	GObject *delegate = userdata;
	GtkTreeStore *store = g_object_get_data(delegate, "planLecheniyaStore");	
	prozubi_t * p =  g_object_get_data(delegate, "prozubi"); 

	prozubi_planlecheniya_foreach(p, json, delegate, plan_lecheniya_fill_table);	
	return 0;
}

static void 
plan_lecheniya_update(GObject * delegate, cJSON * json){
	g_print("Update planLecheniya\n");
	GtkTreeStore *store = g_object_get_data(delegate, "planLecheniyaStore");	
	GtkTreeView *treeView = GTK_TREE_VIEW(delegate);

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

	gtk_tree_view_expand_all(treeView);
}

struct plan_lecheniya_tree_model_to_json {
	cJSON *root;
	cJSON *stage;
	cJSON *array;
	int parent;
	int index;
};

static gboolean 
plan_lecheniya_tree_model_to_json_cb(
		GtkTreeModel* model, 
		GtkTreePath* path, 
		GtkTreeIter* iter, 
		gpointer data) 
{
	struct plan_lecheniya_tree_model_to_json *s = data;
	PLANLECHENIYA_TYPE type;
	gtk_tree_model_get(model, iter, PLAN_LECHENIYA_TYPE, &type, -1); 			
	switch (type) {
		case PLANLECHENIYA_TYPE_STAGE:
			{
				s->stage = NULL;
				s->array = NULL;
				if (!gtk_tree_model_iter_has_child(model, iter))
					break;

				// create new json object
				s->index = 0;
				s->parent++;
				cJSON *stage = cJSON_CreateObject();
				s->stage = stage;
				cJSON_AddItemToArray(s->root, stage);
				cJSON *array = cJSON_CreateArray();
				s->array = array;
				cJSON_AddItemToObject(stage, "array", array);
			
				break;
			}
		case PLANLECHENIYA_TYPE_ITEM:
			{
				if (!s->array)
					break;
				// add item to stage
				cJSON *item = cJSON_CreateObject();
				{
					char str[32];
					sprintf(str, "%d", s->index++);
					cJSON_AddItemToObject(item, "childIndex", cJSON_CreateString(str));
				}

				{
					char str[32];
					sprintf(str, "%d", s->index);
					cJSON_AddItemToObject(item, "number", cJSON_CreateString(str));
				}
				{
					char str[32];
					sprintf(str, "%d", s->parent);
					cJSON_AddItemToObject(item, "parentIndex", cJSON_CreateString(str));
				}

				char *title = NULL;
				gtk_tree_model_get(model, iter, PLAN_LECHENIYA_COLUMN_TITLE, &title, -1); 			
				if (title)
					cJSON_AddItemToObject(item, "title", cJSON_CreateString(title));
					
				cJSON_AddItemToObject(item, "isItem", cJSON_CreateString("1"));
				
				char *kod = NULL;
				gtk_tree_model_get(model, iter, PLAN_LECHENIYA_COLUMN_TITLE, &kod, -1); 			
				if (kod)
					cJSON_AddItemToObject(item, "kod", cJSON_CreateString(kod));
				
				char *price = NULL;
				gtk_tree_model_get(model, iter, PLAN_LECHENIYA_COLUMN_PRICE, &price, -1); 			
				if (price)
					cJSON_AddItemToObject(item, "price", cJSON_CreateString(price));
				
				char *count = NULL;
				gtk_tree_model_get(model, iter, PLAN_LECHENIYA_COLUMN_COUNT, &count, -1); 			
				if (count)
					cJSON_AddItemToObject(item, "count", cJSON_CreateString(count));
				
				char *total = NULL;
				gtk_tree_model_get(model, iter, PLAN_LECHENIYA_COLUMN_TOTAL, &total, -1); 			
				if (total)
					cJSON_AddItemToObject(item, "total", cJSON_CreateString(total));
				
				cJSON_AddItemToArray(s->array, item);
				break;
			}
		case PLANLECHENIYA_TYPE_STAGE_DURATION:
			{
				if (!s->stage)
					break;
				// add time to stage
				char *time = NULL;
				gtk_tree_model_get(model, iter, PLAN_LECHENIYA_COLUMN_COUNT, &time, -1); 			
				if (time)
					cJSON_AddItemToObject(s->stage, "time", cJSON_CreateString(time));
				break;
			}
	
		default:
			break;
	}
	
	return FALSE;
}

static void
plan_lecheniya_tree_model_to_json(
			GtkTreeStore *store,
			prozubi_t *p,
			struct case_t *c
		)
{
	cJSON *json = cJSON_CreateArray();
	struct plan_lecheniya_tree_model_to_json s = {json, NULL, NULL, 0, -1};
	gtk_tree_model_foreach (GTK_TREE_MODEL(store), 
			plan_lecheniya_tree_model_to_json_cb, &s);

	g_print("JSON: %s\n", cJSON_Print(json));
	prozubi_case_set_CASEPLANLECHENIYA(p, c, json, 0);
}

static void
plan_lecheniya_set_price_total(
		GtkTreeModel *model,
		GtkTreeIter *iter,
		int old_total,
		int new_total
		)
{
		//get stage_price iter
		while (gtk_tree_model_iter_next(model, iter)){
			
			//get iter with type PLANLECHENIYA_TYPE_STAGE_PRICE
			PLANLECHENIYA_TYPE type;
			gtk_tree_model_get(model, iter, PLAN_LECHENIYA_TYPE, &type, -1); 			
			if (type == PLANLECHENIYA_TYPE_STAGE_PRICE){
				char *stage_total_str;
				gtk_tree_model_get(model, 
						iter, PLAN_LECHENIYA_COLUMN_TOTAL, &stage_total_str, -1); 			
				
				int stage_total = atoi(stage_total_str);
				stage_total -= old_total;
				stage_total += new_total;

				char new_stage_total[32];
				sprintf(new_stage_total, "%d", stage_total);

				gtk_tree_store_set(GTK_TREE_STORE(model), 
						iter, PLAN_LECHENIYA_COLUMN_TOTAL, new_stage_total, -1);				

				break;
			}
		}
		
		//set plan_lecheniya total 
		GtkTreeIter parent;
		if (gtk_tree_model_iter_parent(model, &parent, iter)){
			while (gtk_tree_model_iter_next(model, &parent)){
				//get iter with type PLANLECHENIYA_TYPE_TOTAL_PRICE
				PLANLECHENIYA_TYPE type;
				gtk_tree_model_get(model, &parent, PLAN_LECHENIYA_TYPE, &type, -1); 			
				if (type == PLANLECHENIYA_TYPE_TOTAL_PRICE){
					char *total_price_str;
					gtk_tree_model_get(model, 
							&parent, PLAN_LECHENIYA_COLUMN_TOTAL, &total_price_str, -1); 			
					
					int total_price = atoi(total_price_str);
					total_price -= old_total;
					total_price += new_total;

					char new_stage_total[32];
					sprintf(new_stage_total, "%d", total_price);

					gtk_tree_store_set(GTK_TREE_STORE(model), 
							&parent, PLAN_LECHENIYA_COLUMN_TOTAL, new_stage_total, -1);				

					break;
				}
		}
	}
}

static void
plan_lecheniya_add_stage(
		GtkWidget *sender,
		GObject *delegate
		)
{		
	GtkTreeStore *store = g_object_get_data(delegate, "planLecheniyaStore");
	prozubi_t * p =  g_object_get_data(delegate, "prozubi"); 
	struct case_t *c=  g_object_get_data(delegate, "case"); 

	if (c->planlecheniya == NULL)
		prozubi_planlecheniya_new(c);

	prozubi_planlecheniya_add_stage(p, c->planlecheniya);
	plan_lecheniya_update(delegate, c->planlecheniya);
}

static void
plan_lecheniya_add_item(
		GObject *delegate,
		int index,
		const char *title,
		const char *kod,
		int price,
		int count
		)
{		
	GtkTreeStore *store = g_object_get_data(delegate, "planLecheniyaStore");
	prozubi_t * p =  g_object_get_data(delegate, "prozubi"); 
	struct case_t *c=  g_object_get_data(delegate, "case"); 

	prozubi_planlecheniya_add_item(
			p, c->planlecheniya, index, title, kod, price, count
			);
	plan_lecheniya_update(delegate, c->planlecheniya);
}

static void
plan_lecheniya_set_duration_total(
		GtkTreeModel *model,
		GtkTreeIter *iter,
		int old_duration,
		int new_duration
		)
{
		GtkTreeIter parent;
		if (gtk_tree_model_iter_parent(model, &parent, iter)){
			while (gtk_tree_model_iter_next(model, &parent)){
				//get iter with type PLANLECHENIYA_TYPE_TOTAL_DURATION
				PLANLECHENIYA_TYPE type;
				gtk_tree_model_get(model, &parent, PLAN_LECHENIYA_TYPE, &type, -1); 			
				if (type == PLANLECHENIYA_TYPE_TOTAL_DURATION){
					char *total_duration_str;
					gtk_tree_model_get(model, 
							&parent, PLAN_LECHENIYA_COLUMN_COUNT, &total_duration_str, -1); 			
					
					int total_duration = atoi(total_duration_str);
					total_duration -= old_duration;
					total_duration += new_duration;

					char new_total[32];
					sprintf(new_total, "%d", total_duration);

					gtk_tree_store_set(GTK_TREE_STORE(model), 
							&parent, PLAN_LECHENIYA_COLUMN_COUNT, new_total, -1);				

					break;
				}
		}
	}
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
	if (responce == GTK_RESPONSE_DELETE_EVENT) {
		g_print("Remove commited\n");
	
		GObject *delegate = userdata;
		GtkTreeStore *store = g_object_get_data(delegate, "planLecheniyaStore");
		gchar *path = g_object_get_data(delegate, "toRemove");
		prozubi_t * p =  g_object_get_data(delegate, "prozubi"); 
		struct case_t *c=  g_object_get_data(delegate, "case"); 

		GtkTreeIter iter;
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(store),
					&iter, path);

		//check type
		PLANLECHENIYA_TYPE type;
		gtk_tree_model_get(GTK_TREE_MODEL(store), 
			&iter, PLAN_LECHENIYA_TYPE, &type, -1); 			
		
		/*if (type == PLANLECHENIYA_TYPE_ITEM){*/
			/*//update prices*/
			/*char *total;*/
			/*gtk_tree_model_get(GTK_TREE_MODEL(store), */
					/*&iter, PLAN_LECHENIYA_COLUMN_TOTAL, &total, -1); 			*/
			/*plan_lecheniya_set_price_total(GTK_TREE_MODEL(store), */
					/*&iter, atoi(total), 0);*/
		/*} */
		
		gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(store),
					&iter, path);
		gtk_tree_store_remove(store,  &iter);
		plan_lecheniya_tree_model_to_json(store, p, c);
		plan_lecheniya_update(delegate, c->planlecheniya);
	}
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void 
plan_lecheniya_ask_to_remove(GObject *delegate, gchar *path) {

	GtkTreeStore *store = g_object_get_data(delegate, "planLecheniyaStore");
	g_object_set_data(delegate, "toRemove", path);
	
	GtkTreeIter iter;
	gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(store),
			&iter, path);
	
	// check type
	PLANLECHENIYA_TYPE type;
	gtk_tree_model_get(GTK_TREE_MODEL(store), 
			&iter, PLAN_LECHENIYA_TYPE, &type, -1); 			

	if (type != PLANLECHENIYA_TYPE_STAGE && type != PLANLECHENIYA_TYPE_ITEM)
		return;
	
	// get title
	char *title; 
	gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 
			PLAN_LECHENIYA_COLUMN_TITLE, &title,
			-1); 			

	GtkWidget *dialog = gtk_message_dialog_new(NULL,
			GTK_DIALOG_MODAL,
			GTK_MESSAGE_QUESTION,
			GTK_BUTTONS_NONE,
			"Удалить из плана %s?", title);
	gtk_window_set_title(GTK_WINDOW(dialog), "Удалить?");

	//add remove button
	GtkWidget *button = gtk_button_new_with_label("УДАЛИТЬ");
	gtk_dialog_add_action_widget(GTK_DIALOG(dialog), button, GTK_RESPONSE_DELETE_EVENT);
#if GTK_CHECK_VERSION(3, 0, 0)	
	GtkStyleContext *context = gtk_widget_get_style_context(button);
	gtk_style_context_add_class(context, "destructive-action");
#else 
	GdkColor color;
	gdk_color_parse("#ff0000", &color);
	gtk_widget_modify_bg(button, 0, &color);	
#endif	
	
	//add cancel button
	gtk_dialog_add_button(GTK_DIALOG(dialog), "Отмена", GTK_RESPONSE_CANCEL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
	g_signal_connect (dialog, "response", 
			G_CALLBACK (plan_lecheniya_ask_to_remove_responce), delegate);

	gtk_widget_show_all(dialog);
}

static void
plan_lecheniya_print_clicked(GtkWidget *sender, gpointer user_data)
{
	g_print("print clicked\n");	
	GObject *delegate = G_OBJECT(user_data);
	prozubi_t *p = g_object_get_data(delegate, "prozubi");
	struct case_t *c = g_object_get_data(delegate, "case");
	struct passport_t *patient = g_object_get_data(delegate, "patient");
	const char *file =
			documents_get_plan_lecheniya(
				"Templates/planLecheniyaTemplate.rtf", 
				p, patient, c);
	if (file){
		char command[BUFSIZ];
		sprintf(command, "xdg-open %s", file);
		system(command);
	}
}

static void
plan_lecheniya_remove_clicked(GtkWidget *sender, gpointer user_data){
	g_print("remove clicked\n");	
	
	//get treeView
	GtkTreeView * treeView = user_data;
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
	
	plan_lecheniya_ask_to_remove(G_OBJECT(treeView), gtk_tree_path_to_string(path));
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
	GtkTreeStore *model;   
	GtkTreeIter iter_rawModel;

	// check if we're working on the raw model or on a sorted version
	//  of it
	if(GTK_IS_TREE_STORE(treeModel)){
		// just use the model as is    
		model = GTK_TREE_STORE(treeModel);

		// retrieve the iterator for the cell, that should be changed
		gtk_tree_model_get_iter_from_string((GtkTreeModel*)model, &iter_rawModel, path_string);

	} else { // we're working on a sorted model   
		// We need to change to a usual model.
		GtkTreeModelSort *sortedModel = GTK_TREE_MODEL_SORT(treeModel);
		model = GTK_TREE_STORE(gtk_tree_model_sort_get_model(sortedModel));

		// get the iterator within the sorted model
		GtkTreeIter iter_sortedModel;
		gtk_tree_model_get_iter_from_string((GtkTreeModel*)sortedModel, &iter_sortedModel, path_string);  

		// convert the iterator to one of the raw model.
		// (Otherwise the wrong cell will change)
		gtk_tree_model_sort_convert_iter_to_child_iter(sortedModel, &iter_rawModel, &iter_sortedModel);
  }
		
		//get old value
		char *old_value;
		gtk_tree_model_get(GTK_TREE_MODEL(model), 
					&iter_rawModel, column_number, &old_value, -1); 			

		switch (column_number) {
			case PLAN_LECHENIYA_COLUMN_PRICE:
				{
					// filter numbers only
					int price;
					sscanf(new_text, "%d", &price);
					char price_str[32];
					sprintf(price_str, "%d", price);
					gtk_tree_store_set(GTK_TREE_STORE(model), 
								&iter_rawModel, column_number, price_str, -1);				
					
					// set total price
					char *count_str;
					gtk_tree_model_get(GTK_TREE_MODEL(model), 
								&iter_rawModel, PLAN_LECHENIYA_COLUMN_COUNT, &count_str, -1); 			
					int total = price * atoi(count_str);
					char total_str[32];
					sprintf(total_str, "%d", total);
					gtk_tree_store_set(GTK_TREE_STORE(model), 
								&iter_rawModel, PLAN_LECHENIYA_COLUMN_TOTAL, total_str, -1);				

					plan_lecheniya_set_price_total(GTK_TREE_MODEL(model), 
							&iter_rawModel, atoi(old_value) * atoi(count_str), total);

					break;
				}			

			case PLAN_LECHENIYA_COLUMN_COUNT:
				{
					// filter numbers only
					int count;
					sscanf(new_text, "%d", &count);
					char count_str[32];
					sprintf(count_str, "%d", count);
					gtk_tree_store_set(GTK_TREE_STORE(model), 
								&iter_rawModel, column_number, count_str, -1);				

					// check type
					PLANLECHENIYA_TYPE type;
					gtk_tree_model_get(GTK_TREE_MODEL(model), 
								&iter_rawModel, PLAN_LECHENIYA_TYPE, &type, -1); 			
					if (type == PLANLECHENIYA_TYPE_STAGE_DURATION){
						int duration;
						sscanf(new_text, "%d", &duration);
						char duration_str[32];
						sprintf(duration_str, "%d", duration);
						gtk_tree_store_set(GTK_TREE_STORE(model), 
								&iter_rawModel, column_number, duration_str, -1);				

						plan_lecheniya_set_duration_total(GTK_TREE_MODEL(model), 
								&iter_rawModel, atoi(old_value), duration);
						break;
					}

					// set total price
					char *price_str;
					gtk_tree_model_get(GTK_TREE_MODEL(model), 
								&iter_rawModel, PLAN_LECHENIYA_COLUMN_PRICE, &price_str, -1); 			
					int total = count * atoi(price_str);
					char total_str[32];
					sprintf(total_str, "%d", total);
					gtk_tree_store_set(GTK_TREE_STORE(model), 
								&iter_rawModel, PLAN_LECHENIYA_COLUMN_TOTAL, total_str, -1);				

					plan_lecheniya_set_price_total(GTK_TREE_MODEL(model), 
							&iter_rawModel, atoi(old_value) * atoi(price_str), total);
					
					break;
				}
			case PLAN_LECHENIYA_COLUMN_TITLE:
				{
					gtk_tree_store_set(GTK_TREE_STORE(model), 
								&iter_rawModel, column_number, new_text, -1);				
				}
		default: break;
		}

		// save json
		prozubi_t *p = g_object_get_data(delegate, "prozubi");
		struct case_t *c = g_object_get_data(delegate, "case");
		plan_lecheniya_tree_model_to_json(model, p, c);
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
	GObject *delegate = userdata;	
	
	prozubi_t *p = g_object_get_data(delegate, "prozubi");
  struct case_t *c = g_object_get_data(delegate, "case");
  gint column_number = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(col), "column_number"));
	g_print("Column: %d\n", column_number);

	GtkTreeModel *treeModel = gtk_tree_view_get_model(treeview);
	
	GtkTreeStore *model;   
	GtkTreeIter iter_rawModel;
	if(GTK_IS_TREE_STORE(treeModel)){
		// just use the model as is    
		model = GTK_TREE_STORE(treeModel);

		// retrieve the iterator for the cell, that should be changed
		gtk_tree_model_get_iter_from_string((GtkTreeModel*)model, &iter_rawModel, 
				gtk_tree_path_to_string(path));

	} else { // we're working on a sorted model   
		// We need to change to a usual model.
		GtkTreeModelSort *sortedModel = GTK_TREE_MODEL_SORT(treeModel);
		model = GTK_TREE_STORE(gtk_tree_model_sort_get_model(sortedModel));

		// get the iterator within the sorted model
		GtkTreeIter iter_sortedModel;
		gtk_tree_model_get_iter_from_string((GtkTreeModel*)sortedModel, &iter_sortedModel, 
				gtk_tree_path_to_string(path));

		// convert the iterator to one of the raw model.
		// (Otherwise the wrong cell will change)
		gtk_tree_model_sort_convert_iter_to_child_iter(sortedModel, &iter_rawModel, &iter_sortedModel);
  }
		
		//get type
		PLANLECHENIYA_TYPE type;
		gtk_tree_model_get(GTK_TREE_MODEL(model), 
				&iter_rawModel, PLAN_LECHENIYA_TYPE, &type, -1); 			
		
		//get column number
		if (column_number == PLAN_LECHENIYA_COLUMN_BUTTON){
			if (type == PLANLECHENIYA_TYPE_ITEM){
				//ask for remove row
				plan_lecheniya_ask_to_remove(delegate, gtk_tree_path_to_string(path));
			} else if (type == PLANLECHENIYA_TYPE_STAGE){
				// add item to stage
				// get index of stage
				int index = atoi(gtk_tree_path_to_string(path));
				g_print("Index of stage: %d\n", index);
				/*! TODO: Todo open window with prices and add item
				*  \todo Todo open window with prices and add item
				*/
			}
		}
}

static GtkWidget *
plan_lecheniya_new(
		GtkWidget *parent, 
		cJSON *json,
		prozubi_t *p,
		struct passport_t *patient,
		struct case_t *c
		)
{
	// vertical box
#if GTK_CHECK_VERSION(3, 2, 0)
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
  GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
#endif
	gtk_container_add(GTK_CONTAINER(parent), vbox);
	gtk_widget_show(vbox);
	
	// toolbar
	GtkWidget *toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_MENU);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
	gtk_widget_show(toolbar);

	
	//scrolled vindow
	GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), 
			GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_end(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
	gtk_widget_show(scrolled);

	/* get treeView */
	GtkWidget *treeView = gtk_tree_view_new();
	GObject *delegate = G_OBJECT(treeView);
	g_object_set_data(delegate, "planLecheniyaTreeView", treeView);

	/* create new model */
	GtkTreeStore *store = plan_lecheniya_table_model_new();
	g_object_set_data(delegate, "planLecheniyaStore", store);
	g_object_set_data(delegate, "prozubi", p);
	g_object_set_data(delegate, "patient", patient);
	g_object_set_data(delegate, "case", c);

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
							"editable", PLAN_LECHENIYA_IS_TITLE_PRICE_EDITABLE,
							"style", PLAN_LECHENIYA_PANGO_STYLE,
							NULL);
					g_object_set(column, "expand", TRUE, NULL);	
					g_object_set(column, "resizable", TRUE, NULL);	
					g_object_set_data(G_OBJECT(column), "column_number", GUINT_TO_POINTER(i));
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

#if GTK_CHECK_VERSION(3, 2, 0)
					GtkAdjustment
#else
					GtkObject 
#endif
					*adjustment = gtk_adjustment_new(0, 0, 100, 1, 10, 0);
					g_object_set(renderer, "adjustment", adjustment, NULL);
		
					gtk_cell_renderer_set_fixed_size(renderer, -1, 40);
		
					column = gtk_tree_view_column_new_with_attributes(
							column_titles[i], renderer, 
							"text", i, 
							"editable", PLAN_LECHENIYA_IS_COUNT_EDITABLE,
							"style", PLAN_LECHENIYA_PANGO_STYLE,
							NULL);
					g_object_set(column, "resizable", TRUE, NULL);	
					g_object_set_data(G_OBJECT(column), "column_number", GUINT_TO_POINTER(i));
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
							"editable", PLAN_LECHENIYA_IS_TITLE_PRICE_EDITABLE,
							"style", PLAN_LECHENIYA_PANGO_STYLE,
							NULL);
					g_object_set(column, "resizable", TRUE, NULL);	
					g_object_set_data(G_OBJECT(column), "column_number", GUINT_TO_POINTER(i));
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
					g_object_set_data(G_OBJECT(column), "column_number", GUINT_TO_POINTER(i));
					break;
				}
				case PLAN_LECHENIYA_COLUMN_BUTTON:
				{
					GtkCellRenderer	*renderer = gtk_cell_renderer_pixbuf_new();
					column = gtk_tree_view_column_new_with_attributes(
							column_titles[i], renderer, "stock-id", i,  NULL);
					g_object_set(column, "resizable", FALSE, NULL);	
					g_object_set_data(G_OBJECT(column), "column_number", GUINT_TO_POINTER(i));
					
					break;
				}
				

			default: break;
		}
		
		gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);	
	}

#if GTK_CHECK_VERSION(3, 8, 0)
	gtk_container_add(GTK_CONTAINER(scrolled), treeView);
#else
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), treeView);	
#endif
	gtk_tree_view_expand_all(GTK_TREE_VIEW(treeView));
	gtk_widget_show(treeView);

	//toolbar buttons
	GtkToolItem *add = gtk_tool_button_new_from_stock(GTK_STOCK_ADD);
	g_signal_connect(add, "clicked", 
			(GCallback) plan_lecheniya_add_stage, delegate);
	gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(add));
	gtk_widget_show(GTK_WIDGET(add));

	GtkToolItem *del = gtk_tool_button_new_from_stock(GTK_STOCK_REMOVE);
	g_signal_connect(del, "clicked", 
			(GCallback) plan_lecheniya_remove_clicked, treeView);
	gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(del));
	gtk_widget_show(GTK_WIDGET(del));
	
	GtkToolItem *print = gtk_tool_button_new_from_stock(GTK_STOCK_PRINT);
	g_signal_connect(print, "clicked", 
			(GCallback) plan_lecheniya_print_clicked, treeView);
	gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(print));
	gtk_widget_show(GTK_WIDGET(print));

	return treeView;	
}

#endif /* ifndef PLAN_LECHENIYA_H */

