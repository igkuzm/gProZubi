/**
 * File              : ComboBox.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.06.2023
 * Last Modified Date: 07.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "ComboBox.h"

enum {
  COMBOBOX_TITLE,
  COMBOBOX_N
};

static GtkListStore *
combobox_model_new(){
	GtkListStore *store = gtk_list_store_new(COMBOBOX_N, 
			G_TYPE_STRING  // title
	);

	return store;
}

void 
cases_edit_combobox_changed (GtkComboBox *widget, gpointer user_data) {
	GtkComboBox *combo_box = widget;
	struct case_t *c = g_object_get_data(G_OBJECT(widget), "case");
	prozubi_t *p = g_object_get_data(G_OBJECT(widget), "prozubi");
	CASES key = 
		GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "key"));
	gint id = gtk_combo_box_get_active(combo_box);
	char **ptr = user_data;
	prozubi_case_set_text(key, p, c, ptr[id]);
}

GtkWidget *
combo_box_new(
		GtkWidget *parent,
		prozubi_t *p,
		struct case_t *c,
		CASES key,
		char **array
		)
{
	GtkWidget *bbox;
#if GTK_CHECK_VERSION(3, 2, 0)
	bbox = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
#else
	bbox = gtk_vbutton_box_new();
#endif
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_CENTER);
	gtk_container_add(GTK_CONTAINER(parent), bbox);
	gtk_widget_show(bbox);
	
	GtkListStore *model = combobox_model_new(); 
	GtkWidget *combobox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));
	
	const char *value = prozubi_case_get(c, key);
	char **ptr = array;
	int i = 0, selected = -1;
	while (ptr[i]){
		gtk_list_store_insert_with_values(model, NULL, -1, COMBOBOX_TITLE, ptr[i], -1);
		if (value)
			if (strcmp(value, ptr[i]) == 0)
				gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), i);
		i++;
	}
	g_object_unref(model);

	GtkCellRenderer *column = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox), column, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combobox), column,
                                   "text", COMBOBOX_TITLE,
									NULL);
	
	g_object_set_data(G_OBJECT(combobox), "prozubi", p);
	g_object_set_data(G_OBJECT(combobox), "case", c);
	g_object_set_data(G_OBJECT(combobox), "key", GINT_TO_POINTER(key));
	g_signal_connect (combobox, "changed", 
			G_CALLBACK (cases_edit_combobox_changed), array);				
	
	gtk_container_add(GTK_CONTAINER (bbox), combobox);	
	gtk_widget_show(combobox);

	return combobox;
}
