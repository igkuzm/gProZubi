/**
 * File              : nomenklaturaLis.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 17.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef NOMENKLATURA_LIST_H
#define NOMENKLATURA_LIST_H

#include <gtk/gtk.h>
#include "prozubilib/nomenklatura.h"

#include "prozubilib/prozubilib.h"
#include "configFile.h"

enum {
  NOMENKLATURA_LIST_COLUMN_NAME,
  NOMENKLATURA_LIST_COLUMN_KOD,
  NOMENKLATURA_LIST_N_COLUMNS,
  NOMENKLATURA_LIST_ID,
  NOMENKLATURA_LIST_POINTER,
  NOMENKLATURA_LIST_NUM,
};

static GtkTreeStore *
nomenklatura_list_table_model_new(){
	GtkTreeStore *store = gtk_tree_store_new(NOMENKLATURA_LIST_NUM, 
			G_TYPE_STRING,  // name
			G_TYPE_STRING,  // kod
			G_TYPE_POINTER, // n_cols
			G_TYPE_STRING,  // id
			G_TYPE_POINTER  // ptr
	);

	return store;
}

static gboolean 
nomenklatura_list_table_model_free(GtkTreeModel* model, GtkTreePath* path, 
		GtkTreeIter* iter, gpointer data) 
{
	struct nomenklatura_t * c;
	gtk_tree_model_get(model, iter, NOMENKLATURA_LIST_POINTER, &c, -1);	
	free(c);
	return FALSE;
}

static void * 
nomenklatura_list_fill_table(
		void *user_data,
		void * parent,
		nomenklatura_t *p
		)
{
	GObject *delegate = user_data;
	GtkTreeStore *store = g_object_get_data(delegate, "nomenklaturaListStore");	

	GtkTreeIter * parent_iter = parent;
	GtkTreeIter iter;
	gtk_tree_store_append(store, &iter, parent);
	gtk_tree_store_set(store, &iter, 
			NOMENKLATURA_LIST_COLUMN_NAME,  p->name,
			NOMENKLATURA_LIST_COLUMN_KOD,   p->kod,
			NOMENKLATURA_LIST_N_COLUMNS,    NULL,
			NOMENKLATURA_LIST_ID,           p->iD,
			NOMENKLATURA_LIST_POINTER,      p,
	-1);

	return gtk_tree_iter_copy(&iter);
}

static void 
nomenklatura_list_update(GObject * delegate){
	g_print("Update nomenklaturaList\n");
	GtkTreeStore *store = g_object_get_data(delegate, "nomenklaturaListStore");	

	/* clear store */
	gtk_tree_model_foreach (GTK_TREE_MODEL(store), 
			nomenklatura_list_table_model_free, NULL);
	gtk_tree_store_clear(store);
	
	/* get list of nomenklaturas */
	prozubi_nomenklatura_foreach(delegate, nomenklatura_list_fill_table);
}

static void
nomenklatura_list_resize_column (
		GtkTreeViewColumn *column,
        gpointer w,
        gpointer user_data
		)
{
	gint width = gtk_tree_view_column_get_width(column);
	gint col = GPOINTER_TO_INT(user_data);
	save_colummn_state(width - 4, col, "nomenklaturaList");
}

static void 
nomenklatura_list_row_activated(
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

		char *name, *kod, *iD;
		gtk_tree_model_get(model, &iter, 
				NOMENKLATURA_LIST_COLUMN_NAME, &name,
				NOMENKLATURA_LIST_COLUMN_KOD,  &kod,
				NOMENKLATURA_LIST_ID,          &iD,
				-1); 

		/* add nomenklatura to price list */
		int (*callback)(const char*, const char*, const char*) =
				g_object_get_data(delegate, "nomenklaturaCallback");

		if (callback)
			callback(name, kod, iD);
	}
}

static GtkWidget *
nomenklatura_list_new(
		GtkWidget *parent,
		void *user_data,	
		int (*callback)(
			void *user_data,
			const char *name,
			const char *kod,
			const char *iD
			)
		)
{
	if (!parent) {
		/* create new window */
		GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_widget_show(window);	

		/* create scrolling window */
		parent = gtk_scrolled_window_new(NULL, NULL);
		gtk_container_add(GTK_CONTAINER(window), parent);
		gtk_widget_show(parent);	
	}

	/* set delegate */
	GObject *delegate = G_OBJECT(parent);
	g_object_set_data(delegate, "nomenklaturaCallback", callback);

	/* get treeView */
	GtkWidget * nomenklaturaListView = g_object_get_data(delegate, "nomenklaturaListView");
	if (!nomenklaturaListView){
		g_print("Error! Can't find nomenklaturaListView\n");
		return NULL;
	}
	GtkWidget * oldWidget = gtk_bin_get_child (GTK_BIN (nomenklaturaListView)); 
	if (oldWidget)
		gtk_container_remove (GTK_CONTAINER (nomenklaturaListView), oldWidget); 

	GtkWidget *treeView = gtk_tree_view_new();
	if (!treeView){
		g_print("Error! Can't create treeView\n");
		return NULL;
	}
	g_object_set_data(delegate, "treeView", treeView);

	/* create new model */
	GtkTreeStore *store = nomenklatura_list_table_model_new();
	g_object_set_data(delegate, "nomenklaturaListStore", store);

	nomenklatura_list_update(delegate);

	/* set tree model for view */
	gtk_tree_view_set_model(GTK_TREE_VIEW(treeView), GTK_TREE_MODEL(store));

	g_signal_connect(treeView, "row-activated", 
			(GCallback) nomenklatura_list_row_activated, delegate);
	
	const char *column_titles[] = {
		"Наименование", "Код"
	};
	
	/* fill tableView */
	int i;
	for (i = 0; i < NOMENKLATURA_LIST_N_COLUMNS; ++i) {

		/* get width */
		gint width = get_colummn_state(i, "nomenklaturaList"); 
		if (width < 40)
			width = 40;		
		
		GtkCellRenderer	*renderer = gtk_cell_renderer_text_new();
		g_object_set(renderer, "editable", FALSE, NULL);
		g_object_set(renderer, "wrap-mode", PANGO_WRAP_WORD, NULL);
		g_object_set(renderer, "wrap-width", 300, NULL);	
		
		g_object_set_data(G_OBJECT(renderer), "column_number", GUINT_TO_POINTER(i));
		g_object_set_data(G_OBJECT(renderer), "delegate", delegate);
		
		GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
				column_titles[i], renderer, "text", i,  NULL);

		switch (i) {
			case NOMENKLATURA_LIST_COLUMN_NAME:
				{
					gtk_cell_renderer_set_fixed_size(renderer, -1, 40);
					g_object_set(column, "expand", TRUE, NULL);	
					break;
				}			
			
			default: break;
		}
		g_object_set(column, "resizable", TRUE, NULL);	
		gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), column);	
		g_signal_connect ((gpointer) column, "notify::width",
										G_CALLBACK (nomenklatura_list_resize_column),		
										GINT_TO_POINTER(i));		
	}

	/* create scrolled window if no one */
	if (!GTK_IS_SCROLLED_WINDOW(parent)){
		GtkWidget *sw = gtk_scrolled_window_new(NULL, NULL);
		gtk_container_add(GTK_CONTAINER(parent), sw);
		gtk_widget_show(sw);	
		parent = sw;
	}

#if !GTK_CHECK_VERSION(3, 8, 0)
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(parent), treeView);	
#else
	gtk_container_add(GTK_CONTAINER(parent), treeView);
#endif

	gtk_widget_show(treeView);
	return treeView;	
}

#endif /* ifndef NOMENKLATURA_LIST_H */
