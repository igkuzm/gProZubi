/**
 * File              : callbacks.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 06.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "prozubilib/alloc.h"
#include "prozubilib/log.h"

#include "patientList.h"
#include "patientEdit.h"
#include "doctorList.h"
#include "configFile.h"

void clear_columns(GtkWidget *mainWindow){
	
	GObject *delegate = G_OBJECT(mainWindow);
	GtkWidget * treeView = g_object_get_data(delegate, "mainTreeView");
	if (!treeView){
		g_print("Error! Can't get treeView\n");
		return;
	}
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
	GtkWidget *mainWindow = lookup_widget(GTK_WIDGET(button), "mainWindow");;
	prozubi_t *p = g_object_get_data(G_OBJECT(mainWindow), "prozubi");
	clear_columns(mainWindow);
	patient_list_new(mainWindow, p);
}


void
on_doctorListButton_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *mainWindow = lookup_widget(GTK_WIDGET(button), "mainWindow");;
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

void
on_caseAddButton_clicked               (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget *window = lookup_widget(GTK_WIDGET(toolbutton), "casesWindow");;
	GObject *delegate = G_OBJECT(window);
	prozubi_t *p = g_object_get_data(delegate, "prozubi");
	char *patientid = g_object_get_data(delegate, "patientid");
	prozubi_case_new_for_patient(p, patientid);
	cases_list_update(delegate, p, patientid);
}

void
on_mainWindow_size_allocate            (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data)
{
	save_widget_state(widget, allocation, "mainWindow");
}


void
on_casesWindow_size_allocate           (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data)
{
	save_widget_state(widget, allocation, "casesWindow");
}

void
on_mainWindowLeftBox_size_allocate     (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data)
{
	save_widget_state(widget, allocation, "mainWindowLeftBox");

}


void
on_casesWindowLeftBox_size_allocate    (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data)
{
	save_widget_state(widget, allocation, "casesWindowLeftBox");

}


void
on_removeButton_clicked                (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	patient_list_remove_clicked(toolbutton);
}


void
on_familiyaEntry_changed               (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


void
on_imiaEntry_changed                   (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


void
on_otchestvoEntry_changed              (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


void
on_emailEntry_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


void
on_dateOfBirthEntry_changed            (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


void
on_telEntry_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


void
on_commentEntry_changed                (GtkEditable     *editable,
                                        gpointer         user_data)
{

}


void
on_patientEditCancelButton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget * patientEditWindow = lookup_widget(
			GTK_WIDGET(button), "patientEditWindow");
	if (patientEditWindow)
		gtk_widget_destroy(patientEditWindow);
}


void
on_patientEditSaveButton_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget * patientEditWindow = lookup_widget(
			GTK_WIDGET(button), "patientEditWindow");
	
	if (patientEditWindow)
		patient_edit_save_button_pushed(patientEditWindow);
}


void
on_addButton_clicked                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget * mainWindow = 
			lookup_widget(GTK_WIDGET(toolbutton), "mainWindow");
	if (!mainWindow){
		g_print("Error! Can't find mainWindow\n");
		return;
	}	
	GObject *delegate = G_OBJECT(mainWindow);	
	
	prozubi_t *p = g_object_get_data(delegate, "prozubi");
	if (!p){
		g_print("prozubi_t pointer is null\n");
		return;
	}

	struct passport_t *c = prozubi_passport_new(
			p, 
			"Новый", 
			"пациент", 
			"", 
			"", 
			"", 
			"", 
			"", 
			"", 
			0, 
			NULL
			);

	if (c)
		patient_edit_new(mainWindow, p, c, TRUE);
}


void
on_editButton_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	GtkWidget * mainWindow = 
			lookup_widget(GTK_WIDGET(toolbutton), "mainWindow");
	if (!mainWindow){
		g_print("Error! Can't find mainWindow\n");
		return;
	}	
	GObject *delegate = G_OBJECT(mainWindow);	

	prozubi_t *p = g_object_get_data(delegate, "prozubi");

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
	
	GtkTreeModel *model = g_object_get_data(delegate, "patientListStore"); 
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
		
		if (c)
			patient_edit_new(mainWindow, p, c, FALSE);
	}	
}

void
on_telEntry_insert_text                (GtkEditable     *editable,
                                        gchar           *new_text,
                                        gint             new_text_length,
                                        gpointer         position,
                                        gpointer         user_data)
{
	/* format input like telephone number  +7999123456 */
	GtkEntry *entry = GTK_ENTRY(editable);
	const gchar * old_text = gtk_entry_get_text(entry);	

	char *text = MALLOC(BUFSIZ, ERR("%s", "can't allocate text"), return);

	int i = 0;
	text[i] = 0;

	char *ptr = new_text;
	char c = *ptr++;
	int *p = position;
	if (*p == 0){
		if (c == '+')
			text[i++] = '+';
		else if (c == '8'){
			text[i++] = '+';
			text[i++] = '7';
		}
		else {
			text[i++] = '+';
			if (c >= '0' && c<= '9')
				text[i++] = c;
		}		
	} else {
		if (c >= '0' && c<= '9')
			text[i++] = c;
	} 
	
	while(*ptr){
		c = *ptr++; 
		if (c >= '0' && c<= '9')
			text[i++] = c;
	}		
	text[i] = 0;

	/* stop entry */
	g_signal_handlers_block_by_func (editable, (gpointer) on_telEntry_insert_text, user_data);
  
	/* white text */
	/*gtk_entry_set_text(entry, text);*/
	gtk_editable_insert_text(editable, text, strlen(text), position);
	free(text);
  
	/* start entry */
	g_signal_handlers_unblock_by_func (editable, (gpointer) on_telEntry_insert_text, user_data);
	
	/* stop callback */
	g_signal_stop_emission_by_name (editable, "insert_text");
}


void
on_dateOfBirthEntry_insert_text        (GtkEditable     *editable,
                                        gchar           *new_text,
                                        gint             new_text_length,
                                        gpointer         position,
                                        gpointer         user_data)
{
	/* format input like date dd.mm.yyyy */
	GtkEntry *entry = GTK_ENTRY(editable);
	const gchar * old_text = gtk_entry_get_text(entry);	

	int *p = position;
	
	g_print("POSITION: %d\n", *p);

	if (*p > 9){
		g_signal_stop_emission_by_name (editable, "insert_text");
		return;
	} 

	if (*p == 0 && new_text[0] > '3'){
		g_signal_stop_emission_by_name (editable, "insert_text");
		return;
	}	

	if (*p == 4 && new_text[0] > '2' && old_text[3] == '1'){
		g_signal_stop_emission_by_name (editable, "insert_text");
		return;
	}	

	char *text = MALLOC(BUFSIZ, ERR("%s", "can't allocate text"), return);
	int i = 0;
	text[i] = 0;

	char *ptr = new_text;
	char c = *ptr++;
	
	if (*p == 1 || *p == 4){
		if (c >= '0' && c<= '9'){
			text[i++] = c;		
			text[i++] = '.';		
		}
	} else if (*p == 3){
		if (c < '2'){
			text[i++] = c;		
		} else {
			text[i++] = '0';		
			text[i++] = c;		
		}
	} else if (*p == 2 || *p == 5){
		if (c == '/' || c == ' ' || c == '.')
			text[i++] = '.';		

	} else {
		if (c >= '0' && c<= '9')
			text[i++] = c;		
	}

	while(*ptr){
		c = *ptr++; 
		if ((c >= '0' && c<= '9') || c == '.')
			text[i++] = c;
	}				
	
	text[i] = 0;

	/* stop entry */
	g_signal_handlers_block_by_func (editable, (gpointer) on_dateOfBirthEntry_insert_text, user_data);
  
	/* white text */
	/*gtk_entry_set_text(entry, text);*/
	gtk_editable_insert_text(editable, text, strlen(text), position);
	free(text);
  
	/* start entry */
	g_signal_handlers_unblock_by_func (editable, (gpointer) on_dateOfBirthEntry_insert_text, user_data);
	
	/* stop callback */
	g_signal_stop_emission_by_name (editable, "insert_text");
}

void
on_mainWindowMenuCreate_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	on_addButton_clicked(GTK_TOOL_BUTTON(menuitem), user_data);
}


void
on_mainWindowMenuOpen_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_mainWindowMenuSave_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_mainWindowMenuSaveAs_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_mainWindowMenuExit_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit();	
}


void
on_mainWindowMenuCut_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_mainWindowMenuCopy_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_mainWindowMenuPaste_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_mainWindowMenuDelete_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	patient_list_remove_clicked(menuitem);
}


void
on_mainWindowMenuAbout_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_casesWindowMenuCut_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_casesWindowMenuPaste_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_casesWindowMenuDelete_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_casesWindowShare_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_casesWindowLoad_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_casesWindowDocs_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_casesWindowAdd_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_casesWindowDelete_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
}


void
on_casesWindowPrint_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_patientEditWindow_size_allocate     (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data)
{
	save_widget_state(widget, allocation, "patientEditWindow");

}


void
on_caseRemoveButton_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	cases_list_remove_clicked(toolbutton);
}

