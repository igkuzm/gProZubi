/**
 * File              : TextEdit.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.06.2023
 * Last Modified Date: 01.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#include "TextEdit.h"

void 
cases_edit_text_edit_changed(
		GtkTextBuffer *buffer, 
		gpointer   data)
{
	struct case_t *c = g_object_get_data(G_OBJECT(buffer), "case");
	prozubi_t *p = g_object_get_data(G_OBJECT(buffer), "prozubi");
	CASES key = 
		GPOINTER_TO_INT(g_object_get_data(G_OBJECT(buffer), "key"));

	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	const char *text = 
			gtk_text_buffer_get_text(buffer, &start, &end, FALSE);	

	prozubi_case_set_text(key, p, c, text);
}

GtkWidget *
text_edit_new(
		GtkWidget *parent,
		prozubi_t *p,
		struct case_t *c,
		CASES key)
{
// vertical box
	GtkWidget *vbox;
#if GTK_CHECK_VERSION(3, 2, 0)
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
	vbox = gtk_vbox_new(FALSE, 0);
#endif
	gtk_container_add(GTK_CONTAINER(parent), vbox);
	gtk_widget_show(vbox);
	
	// toolbar
	GtkWidget *toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), GTK_ICON_SIZE_MENU);
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
	gtk_widget_show(toolbar);

	//toolbar buttons
	GtkToolItem *share = gtk_tool_button_new_from_stock(GTK_STOCK_GO_UP);
	gtk_container_add(GTK_CONTAINER(toolbar), GTK_WIDGET(share));
	gtk_widget_show(GTK_WIDGET(share));

	//scrolled vindow
	GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), 
			GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	gtk_box_pack_end(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
	gtk_widget_show(scrolled);

	// text edit
	GtkWidget *textEdit = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textEdit), GTK_WRAP_WORD);
#if GTK_CHECK_VERSION(3, 8, 0)
gtk_container_add(GTK_CONTAINER(scrolled), textEdit);
#else
gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),
			textEdit);
#endif
	GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textEdit));
	
	const char *value = prozubi_case_get(c, key);
	if (key == CASEDIAGNOZIS)
		if (!value || strlen(value) < 2){
			value = prozubi_diagnosis_get(p, c);	
		}
	if (value){
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buf), value, -1);
	}
	
	g_object_set_data(G_OBJECT(buf), "prozubi", p);
	g_object_set_data(G_OBJECT(buf), "case", c);
	g_object_set_data(G_OBJECT(buf), "key", GINT_TO_POINTER(key));
	
	g_signal_connect (buf, "changed", 
			G_CALLBACK (cases_edit_text_edit_changed), NULL);
	gtk_widget_show(textEdit);

	return textEdit;
}
