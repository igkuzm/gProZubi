/**
 * File              : pricesList.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 23.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef PRICES_LIST_H
#define PRICES_LIST_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#include "casesList.h"
#include "interface.h"
#include "configFile.h"

#include "prozubilib/prozubilib.h"

enum {
  PRICES_LIST_COLUMN_TITLE,
  PRICES_LIST_COLUMN_PRICE,
  PRICES_LIST_COLUMN_KOD,
  PRICES_LIST_COLUMN_ABOUT,
  PRICES_LIST_COLUMN_CATEGORY,
  PRICES_LIST_N_COLUMNS,
  PRICES_LIST_POINTER,
  PRICES_LIST_NUM
};

static GtkListStore 
*prices_list_table_model_new(){
	GtkListStore *store = gtk_list_store_new(PRICES_LIST_NUM, 
			G_TYPE_STRING,  // title
			G_TYPE_STRING,  // price
			G_TYPE_STRING,  // kod
			G_TYPE_STRING,  // about
			G_TYPE_STRING,  // category
			G_TYPE_POINTER, // NULL
			G_TYPE_POINTER  // ptr
	);

	return store;
}

static void 
prices_list_store_add(GtkListStore *store, struct price_t * price){

	GtkTreeIter iter;
	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 
			PRICES_LIST_COLUMN_TITLE,    price->title,
			PRICES_LIST_COLUMN_PRICE,    price->price,
			PRICES_LIST_COLUMN_KOD,      price->kod,
			PRICES_LIST_COLUMN_ABOUT,    price->about,
			PRICES_LIST_COLUMN_CATEGORY, price->category,
			PRICES_LIST_N_COLUMNS,       NULL,
			PRICES_LIST_POINTER,         price,
	-1);
}

static int 
prices_list_fill_table(void *userdata, struct price_t * price){

	GObject *delegate = userdata;
	GtkListStore *store = g_object_get_data(delegate, "pricesListStore");	
		
	prices_list_store_add(store, price);

	return 0;
}

static gboolean 
prices_list_table_model_free(GtkTreeModel* model, GtkTreePath* path, 
		GtkTreeIter* iter, gpointer data) 
{
	struct price_t * price;
	gtk_tree_model_get(model, iter, PRICES_LIST_POINTER, &price, -1);	
	prozubi_prices_free(price);
	return FALSE;
}

static void 
prices_list_update(GObject * delegate, prozubi_t *p){
	g_print("Update pricesList\n");
	GtkListStore *store = g_object_get_data(delegate, "pricesListStore");	

	/* set remove button insensitive */
	//gtk_widget_set_sensitive(GTK_WIDGET(g_object_get_data(delegate, "petientRemoveButton")), FALSE);		

	/* clear store */
	gtk_tree_model_foreach (GTK_TREE_MODEL(store), 
			prices_list_table_model_free, NULL);
	gtk_list_store_clear(store);
	
	/* get list of pricess */
	prozubi_price_foreach(p, delegate, prices_list_fill_table);
}

static void 
prices_list_table_cell_edited_callback (
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

	struct price_t * c;
	gtk_tree_model_get(treeModel, &iter_rawModel, PRICES_LIST_POINTER, &c, -1); 			
	prozubi_t *p = g_object_get_data(G_OBJECT(cell), "prozubi");

	if (!c || !p)
		return;
	
	switch (column_number) {
		case PRICES_LIST_COLUMN_TITLE:
			{
				if (!prozubi_prices_set_PRICETITLE(p, c, new_text, true))
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);				
				break;
			}			
		case PRICES_LIST_COLUMN_PRICE:
			{
				if (!prozubi_prices_set_PRICEPRICE(p, c, new_text, true))
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);				
				break;
			}			
		case PRICES_LIST_COLUMN_KOD:
			{
				if (!prozubi_prices_set_PRICEKOD(p, c, new_text, true))
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);				
				break;
			}			
		case PRICES_LIST_COLUMN_ABOUT:
			{
				if (!prozubi_prices_set_PRICEABOUT(p, c, new_text, true))
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);				
				break;
			}			
		case PRICES_LIST_COLUMN_CATEGORY:
			{
				if (!prozubi_prices_set_PRICECATEGORY(p, c, new_text, true))
					gtk_list_store_set(GTK_LIST_STORE(model), &iter_rawModel, column_number, new_text, -1);				
				break;
			}			
		default: break;
	}
}

static void 
prices_list_ask_to_remove_responce(
		GtkDialog *dialog, 
		gint responce, 
		gpointer userdata
		)
{
	if (responce == GTK_RESPONSE_DELETE_EVENT) {
		g_print("Remove commited\n");

		GObject *delegate = userdata;
		struct passport_t * prices =  g_object_get_data(delegate, "pricesToRemove"); 
		prozubi_t * p =  g_object_get_data(delegate, "prozubi"); 
		
		if (!prices){
			g_print("prices is NULL\n");
			/*gtk_window_destroy(GTK_WINDOW(dialog));*/
			gtk_widget_destroy(GTK_WIDGET(dialog));
			return;
		}		

		/* remove prices and update table */
		prozubi_passport_remove(p, prices);
		prices_list_update(delegate, p);
	}
	/*gtk_window_destroy(GTK_WINDOW(dialog));*/
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

static void 
prices_list_ask_to_remove(GObject *delegate, struct passport_t * prices) {
	if (!prices){
		g_print("prices is NULL\n");
		return;
	}
	
	g_object_set_data(delegate, "pricesToRemove", prices);
	
	char title[BUFSIZ] = "";
	sprintf(title, 
			"Удалить из базы пациенты: %s?", 
			prices->familiya
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
			G_CALLBACK (prices_list_ask_to_remove_responce), delegate);

	gtk_widget_show_all(dialog);
}

static void
prices_list_remove_clicked(gpointer user_data){
	
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
		gtk_tree_model_get(model, &iter, PRICES_LIST_POINTER, &c, -1); 			
		
		if (c){
			prices_list_ask_to_remove(delegate, c);
		}
	}
}

static void
prices_list_resize_column (GtkTreeViewColumn       *column,
                            gpointer                 w,
                            gpointer                 user_data)
{
	gint width = gtk_tree_view_column_get_width(column);
	gint col = GPOINTER_TO_INT(user_data);
	save_colummn_state(width - 4, col, "pricesList");
}

static gboolean
prices_list_SearchEqualFunc(
		GtkTreeModel* model,
		int column,
		const char* key,
		GtkTreeIter* iter,
		gpointer search_data
		)
{
	struct price_t * price;
	gtk_tree_model_get(model, iter, PRICES_LIST_POINTER, &price, -1); 			
	if (!price)
		return 1;
	
	char *needle = g_utf8_strup(key, BUFSIZ); 

#define PRICES_COLUMN_TEXT(member, number, title)\
	{\
		char *haystack = g_utf8_strup(price->member, BUFSIZ);\
		if (g_strrstr(haystack, needle) != NULL){\
			free(needle);\
			free(haystack);\
			return 0;\
		}\
	}	
	PRICES_COLUMNS
#undef PRICES_COLUMN_TEXT

	free(needle);
	return 1;
}


static GtkWidget *
prices_list_new(GtkWidget *mainWindow, prozubi_t *p){
	/* set delegate */
	GObject *delegate = G_OBJECT(mainWindow);
	
	g_object_set_data(delegate, "selectedItem", "prices");

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
	GtkListStore *store = prices_list_table_model_new();
	g_object_set_data(delegate, "pricesListStore", store);

	prices_list_update(delegate, p);

	/* set tree model for view */
	//GtkWidget *view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), GTK_TREE_MODEL(store));
	
	GtkWidget *search = g_object_get_data(delegate, "searchEntry");
	gtk_tree_view_set_search_entry(GTK_TREE_VIEW(treeView), GTK_ENTRY(search));
	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeView), true);	
	gtk_tree_view_set_search_equal_func(GTK_TREE_VIEW(treeView), prices_list_SearchEqualFunc, NULL, NULL);

	const char *column_titles[] = {
		"Наименование", 
		"Цена", 
		"Код", 
		"Дополнительно", 
		"Категория", 
	};
	
	/* fill tableView */
	int i;
	for (i = 0; i < PRICES_LIST_N_COLUMNS; ++i) {

		/* get width */
		gint width = get_colummn_state(i, "pricesList"); 
		if (width < 40)
			width = 40;

		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", TRUE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", width, NULL);	
		g_object_set(renderer, "width", width, NULL);	
		
		g_object_set_data(G_OBJECT(renderer), "prozubi", p);
		
		g_signal_connect(renderer, "edited", 
				(GCallback) prices_list_table_cell_edited_callback, treeView);

		g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
		g_object_set_data(G_OBJECT(renderer), "delegate", delegate);
		
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
				column_titles[i], renderer, "text", i,  NULL);

		switch (i) {
			case PRICES_LIST_COLUMN_TITLE:
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
										G_CALLBACK (prices_list_resize_column),
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

#endif /* ifndef PRICES_LIST_H */
