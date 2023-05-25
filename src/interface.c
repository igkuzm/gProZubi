/**
 * File              : interface.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 12.05.2023
 * Last Modified Date: 20.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"

#include "configFile.h"
#include "prozubilib/prozubilib.h"
#include "patientList.h"
#include "menu.h"

void application_on_deactivate (GtkWidget *window, gpointer userdata) {
	g_application_quit (userdata);
}

GtkWidget*
#if GTK_CHECK_VERSION(3, 0, 0)
create_mainWindow (GtkApplication *app)
#else
create_mainWindow (void)
#endif
{
  GtkWidget *mainWindow;
  AtkObject *atko;
  GtkWidget *mainBox;
  GtkWidget *mainWindowPaned;
  GtkWidget *mainWindowLeftBox;
  GtkWidget *patientListButton;
  GtkWidget *doctorListButton;
  GtkWidget *priceListButton;
  GtkWidget *configButton;
  GtkWidget *mainWindowRightBox;
  GtkWidget *toolbar1;
  GtkIconSize tmp_toolbar_icon_size;
  GtkWidget *mailToButton;
  GtkWidget *telegramToButton;
  GtkWidget *addToCalendarButton;
  GtkWidget *addButton;
  GtkWidget *removeButton;
  GtkWidget *editButton;
  GtkWidget *printButton;
  GtkWidget *searchItem;
  GtkWidget *searchEntry;
  GtkWidget *mainView;

	/* MAIN WINDOW */
#if GTK_CHECK_VERSION(3, 0, 0)
	mainWindow = gtk_application_window_new(app);
	gtk_application_window_set_show_menubar (
			GTK_APPLICATION_WINDOW (mainWindow), TRUE);
	//quit application on window destroy
	g_signal_connect(G_OBJECT(mainWindow), "destroy", 
			G_CALLBACK(application_on_deactivate), app); 
  g_object_set_data (G_OBJECT(mainWindow), "app", app);
#else
	mainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect ((gpointer) mainWindow, "destroy",
                    G_CALLBACK (gtk_main_quit),
                    NULL);
#endif
	GObject *delegate = G_OBJECT(mainWindow);
  gtk_widget_set_size_request (mainWindow, 640, 480);
  gtk_window_set_title (GTK_WINDOW (mainWindow), "ProZubi");
  g_signal_connect ((gpointer) mainWindow, "size_allocate",
                    G_CALLBACK (on_mainWindow_size_allocate),
                    NULL);

  atko = gtk_widget_get_accessible (mainWindow);
  atk_object_set_name (atko, "ProZubi");
  atk_object_set_description (atko, "ProZubi application main window");

#if GTK_CHECK_VERSION(3, 2, 0)
  mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
  mainBox = gtk_vbox_new (FALSE, 0);
#endif
  gtk_widget_show (mainBox);
  gtk_container_add (GTK_CONTAINER (mainWindow), mainBox);

	/* create main menu */

#if GTK_CHECK_VERSION(3, 2, 0)
	GtkWidget *mainWindowMenu =  create_main_menu(app, mainWindow);
#else
	GtkWidget *mainWindowMenu =  create_main_menu(mainWindow);
  gtk_box_pack_start (GTK_BOX (mainBox), mainWindowMenu, FALSE, FALSE, 0);
#endif

#if GTK_CHECK_VERSION(3, 2, 0)
  mainWindowPaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add (GTK_CONTAINER (mainBox), mainWindowPaned);
#else
  mainWindowPaned = gtk_hpaned_new ();
  gtk_container_add (GTK_CONTAINER (mainBox), mainWindowPaned);
  //gtk_box_pack_end (GTK_BOX (mainBox), mainWindowMenu, TRUE, TRUE, 0);
#endif
  gtk_widget_show (mainWindowPaned);

#if GTK_CHECK_VERSION(3, 2, 0)
  mainWindowLeftBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
  mainWindowLeftBox = gtk_vbox_new (FALSE, 0);
#endif
  g_signal_connect ((gpointer) mainWindowLeftBox, "size_allocate",
                    G_CALLBACK (on_mainWindowLeftBox_size_allocate),
                    NULL);
  gtk_widget_show (mainWindowLeftBox);
  gtk_paned_pack1 (GTK_PANED (mainWindowPaned), mainWindowLeftBox, FALSE, TRUE);

  patientListButton = gtk_button_new_with_mnemonic ("Список пациентов");
  gtk_widget_show (patientListButton);
  gtk_box_pack_start (GTK_BOX (mainWindowLeftBox), patientListButton, FALSE, FALSE, 0);
  gtk_button_set_relief (GTK_BUTTON (patientListButton), GTK_RELIEF_NONE);
  g_signal_connect ((gpointer) patientListButton, "clicked",
                            G_CALLBACK (on_patientListButton_clicked),
                            mainWindow);

  doctorListButton = gtk_button_new_with_mnemonic ("Список сотрудников");
  gtk_widget_show (doctorListButton);
  gtk_box_pack_start (GTK_BOX (mainWindowLeftBox), doctorListButton, FALSE, FALSE, 0);
  gtk_button_set_relief (GTK_BUTTON (doctorListButton), GTK_RELIEF_NONE);
  g_signal_connect ((gpointer) doctorListButton, "clicked",
                            G_CALLBACK (on_doctorListButton_clicked),
                            mainWindow);

  priceListButton = gtk_button_new_with_mnemonic ("Список услуг (прайс)");
  gtk_widget_show (priceListButton);
  gtk_box_pack_start (GTK_BOX (mainWindowLeftBox), priceListButton, FALSE, FALSE, 0);
  gtk_button_set_relief (GTK_BUTTON (priceListButton), GTK_RELIEF_NONE);
  g_signal_connect ((gpointer) priceListButton, "clicked",
                            G_CALLBACK (on_priceListButton_clicked),
                            mainWindow);

  configButton = gtk_button_new_with_mnemonic ("Настройки");
  gtk_widget_show (configButton);
  gtk_box_pack_start (GTK_BOX (mainWindowLeftBox), configButton, FALSE, FALSE, 20);
  gtk_button_set_relief (GTK_BUTTON (configButton), GTK_RELIEF_NONE);
  g_signal_connect ((gpointer) configButton, "clicked",
                            G_CALLBACK (on_configButton_clicked),
                            mainWindow);

#if GTK_CHECK_VERSION(3, 2, 0)
  mainWindowRightBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
  mainWindowRightBox = gtk_vbox_new (FALSE, 0);
#endif
  gtk_widget_show (mainWindowRightBox);
  gtk_paned_pack2 (GTK_PANED (mainWindowPaned), mainWindowRightBox, TRUE, TRUE);

  	/* MAIN TOOLBAR */
  toolbar1 = gtk_toolbar_new ();
  gtk_widget_show (toolbar1);
  gtk_box_pack_start (GTK_BOX (mainWindowRightBox), toolbar1, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH);
  tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar1));

  mailToButton = (GtkWidget*) gtk_tool_button_new (NULL, "mail");
  gtk_widget_show (mailToButton);
  gtk_container_add (GTK_CONTAINER (toolbar1), mailToButton);

  telegramToButton = (GtkWidget*) gtk_tool_button_new (NULL, "telegram");
  gtk_widget_show (telegramToButton);
  gtk_container_add (GTK_CONTAINER (toolbar1), telegramToButton);

  addToCalendarButton = (GtkWidget*) gtk_tool_button_new (NULL, "calendar");
  gtk_widget_show (addToCalendarButton);
  gtk_container_add (GTK_CONTAINER (toolbar1), addToCalendarButton);

  addButton = (GtkWidget*) gtk_tool_button_new (NULL, "add");
  gtk_widget_show (addButton);
  gtk_container_add (GTK_CONTAINER (toolbar1), addButton);
  g_signal_connect ((gpointer) addButton, "clicked",
                    G_CALLBACK (on_addButton_clicked),
                    mainWindow);

  removeButton = (GtkWidget*) gtk_tool_button_new (NULL, "remove");
  gtk_widget_show (removeButton);
  gtk_container_add (GTK_CONTAINER (toolbar1), removeButton);
  g_signal_connect ((gpointer) removeButton, "clicked",
                    G_CALLBACK (on_removeButton_clicked),
                    mainWindow);

  editButton = (GtkWidget*) gtk_tool_button_new (NULL, "edit");
  gtk_widget_show (editButton);
  gtk_container_add (GTK_CONTAINER (toolbar1), editButton);
  g_signal_connect ((gpointer) editButton, "clicked",
                    G_CALLBACK (on_editButton_clicked),
                    mainWindow);

  printButton = (GtkWidget*) gtk_tool_button_new (NULL, "print");
  gtk_widget_show (printButton);
  gtk_container_add (GTK_CONTAINER (toolbar1), printButton);

  searchItem = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (searchItem);
  gtk_container_add (GTK_CONTAINER (toolbar1), searchItem);

  searchEntry = gtk_entry_new ();
  gtk_widget_show (searchEntry);
  gtk_container_add (GTK_CONTAINER (searchItem), searchEntry);
  gtk_entry_set_invisible_char (GTK_ENTRY (searchEntry), 9679);
  g_object_set_data(delegate, "searchEntry", searchEntry);

  /* MAIN VIEW */
  mainView = gtk_scrolled_window_new (NULL, NULL);
  g_object_set_data (delegate, "mainView", mainView);
  gtk_widget_show (mainView);
  gtk_box_pack_start (GTK_BOX (mainWindowRightBox), mainView, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (mainView), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (mainView), GTK_SHADOW_IN);
	g_object_set_data(G_OBJECT(mainView), "mainWindow", mainWindow);

	/* restore window size */
	widget_restore_state_from_config(mainWindow, "mainWindow", 640, 480); 
	widget_restore_state_from_config(mainWindowLeftBox, "mainWindowLeftBox", 300, 480); 

	//get token
	char * token = token_from_config();
	g_print("init with token: %s\n", token);

  /* init database */
  prozubi_t *p = prozubi_init(
			"ProZubi.sqlite",
			token);

  g_object_set_data(delegate, "prozubi", p);

	/* patient list as default at start */
	patient_list_new(mainWindow, p);
	
	gtk_widget_show (mainWindow);

  return mainWindow;
}

GtkWidget*
create_casesWindow (void)
{
  GtkWidget *casesWindow;
  GtkWidget *casesPaned;
  GtkWidget *casesWindowLeftBox;
  GtkWidget *casesListToolbar;
  GtkIconSize tmp_toolbar_icon_size;
  GtkWidget *caseAddButton;
  GtkWidget *caseRemoveButton;
  GtkWidget *caseRefreshButton;
  GtkWidget *casesListView;
  GtkWidget *casesListInfo;
  GtkWidget *casesEditFrame;
  GtkWidget *alignment1;
  GtkWidget *label12;

	/* CASES WINDOW */
  casesWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	GObject *delegate = G_OBJECT(casesWindow);
  //gtk_widget_set_size_request (casesWindow, 640, 480);
	widget_restore_state_from_config(casesWindow, "casesWindow", 640, 480); 
  gtk_window_set_position (GTK_WINDOW (casesWindow), GTK_WIN_POS_CENTER_ALWAYS);
  gtk_window_set_modal (GTK_WINDOW (casesWindow), TRUE);
  g_signal_connect ((gpointer) casesWindow, "size_allocate",
					G_CALLBACK (on_casesWindow_size_allocate),
					NULL);

#if GTK_CHECK_VERSION(3, 2, 0)
  casesPaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
#else
  casesPaned = gtk_hpaned_new ();
#endif
  g_object_set_data (delegate, "casesPaned", casesPaned);
  gtk_widget_show (casesPaned);
  gtk_container_add (GTK_CONTAINER (casesWindow), casesPaned);

#if GTK_CHECK_VERSION(3, 2, 0)
  casesWindowLeftBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
  casesWindowLeftBox = gtk_vbox_new (FALSE, 0);
#endif
  g_object_set_data (delegate, "casesWindowLeftBox", casesWindowLeftBox);
  gtk_paned_pack1 (GTK_PANED (casesPaned), casesWindowLeftBox, FALSE, TRUE);
  widget_restore_state_from_config(casesWindowLeftBox, "casesWindowLeftBox", 300, 480);  
  gtk_widget_show (casesWindowLeftBox);
  g_signal_connect ((gpointer) casesWindowLeftBox, "size_allocate",
                    G_CALLBACK (on_casesWindowLeftBox_size_allocate),
                    NULL);
	/* CASES TOOLBAR */
  casesListToolbar = gtk_toolbar_new ();
  g_object_set_data (delegate, "casesListToolbar", casesListToolbar);
  gtk_box_pack_start (GTK_BOX (casesWindowLeftBox), casesListToolbar, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (casesListToolbar), GTK_TOOLBAR_ICONS);
  tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (casesListToolbar));
  gtk_widget_show (casesListToolbar);

#if GTK_CHECK_VERSION(3, 10, 0)
	caseAddButton = (GtkWidget*) gtk_tool_button_new(
			gtk_image_new_from_icon_name("gtk-add", GTK_ICON_SIZE_LARGE_TOOLBAR), "gtk-add");
#else
  caseAddButton = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-add");
#endif
  g_object_set_data (delegate, "caseAddButton", caseAddButton);
  gtk_container_add (GTK_CONTAINER (casesListToolbar), caseAddButton);
  gtk_widget_show (caseAddButton);
  g_signal_connect ((gpointer) caseAddButton, "clicked",
                    G_CALLBACK (on_caseAddButton_clicked),
                    casesWindow);

#if GTK_CHECK_VERSION(3, 10, 0)
	caseRemoveButton = (GtkWidget*) gtk_tool_button_new(
			gtk_image_new_from_icon_name("gtk-remove", GTK_ICON_SIZE_LARGE_TOOLBAR), "gtk-remove");
#else
  caseRemoveButton = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-remove");
#endif
  g_object_set_data (delegate, "caseRemoveButton", caseRemoveButton);
  gtk_container_add (GTK_CONTAINER (casesListToolbar), caseRemoveButton);
  gtk_widget_show (caseRemoveButton);
  g_signal_connect ((gpointer) caseRemoveButton, "clicked",
                    G_CALLBACK (on_caseRemoveButton_clicked),
                    casesWindow);

#if GTK_CHECK_VERSION(3, 10, 0)
	caseRefreshButton = (GtkWidget*) gtk_tool_button_new(
			gtk_image_new_from_icon_name("gtk-refresh", GTK_ICON_SIZE_LARGE_TOOLBAR), "gtk-refresh");
#else
  caseRefreshButton = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-refresh");
#endif
  g_object_set_data (delegate, "caseRefreshButton", caseRefreshButton);
  gtk_container_add (GTK_CONTAINER (casesListToolbar), caseRefreshButton);
  gtk_widget_show (caseRefreshButton);
  g_signal_connect ((gpointer) caseRefreshButton, "clicked",
                    G_CALLBACK (on_caseRefreshButton_clicked),
                    casesWindow);

	/* CASES LIST VIEW */
  casesListView = gtk_scrolled_window_new (NULL, NULL);
  g_object_set_data (delegate, "casesListView", casesListView);
  gtk_box_pack_start (GTK_BOX (casesWindowLeftBox), casesListView, TRUE, TRUE, 0);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (casesListView), GTK_SHADOW_IN);
  gtk_widget_show (casesListView);

  casesListInfo = gtk_label_new ("");
  g_object_set_data (delegate, "casesListInfo", casesListInfo);
  gtk_box_pack_start (GTK_BOX (casesWindowLeftBox), casesListInfo, FALSE, FALSE, 0);
  gtk_widget_show (casesListInfo);

	/* CASES EDIT FRAME */
  casesEditFrame = gtk_frame_new (NULL);
  g_object_set_data (delegate, "casesEditFrame", casesEditFrame);
  gtk_widget_show (casesEditFrame);
  gtk_paned_pack2 (GTK_PANED (casesPaned), casesEditFrame, TRUE, TRUE);
  gtk_frame_set_label_align (GTK_FRAME (casesEditFrame), 0, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (casesEditFrame), GTK_SHADOW_NONE);

#if GTK_CHECK_VERSION(3, 14, 0)
#else
  alignment1 = gtk_alignment_new (0, 0, 1, 1);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (casesEditFrame), alignment1);
#endif

  label12 = gtk_label_new ("");
  gtk_widget_show (label12);
  gtk_frame_set_label_widget (GTK_FRAME (casesEditFrame), label12);
  gtk_label_set_use_markup (GTK_LABEL (label12), TRUE);

  gtk_widget_show_all (casesWindow);
  return casesWindow;
}

GtkWidget*
create_patientEditWindow (void)
{
  GtkWidget *patientEditWindow;
  GtkWidget *vbox5;
  GtkWidget *label3;
  GtkWidget *familiyaEntry;
  GtkWidget *label4;
  GtkWidget *imiaEntry;
  GtkWidget *label5;
  GtkWidget *otchestvoEntry;
  GtkWidget *label6;
  GtkWidget *scrolledwindow3;
  GtkWidget *addressTextview;
  GtkWidget *label7;
  GtkWidget *emailEntry;
  GtkWidget *label8;
  GtkWidget *dateOfBirthEntry;
  GtkWidget *label9;
  GtkWidget *telEntry;
  GtkWidget *label10;
  GtkWidget *scrolledwindow4;
  GtkWidget *passportTextview;
  GtkWidget *label11;
  GtkWidget *commentEntry;
  GtkWidget *hbox3;
  GtkWidget *patientEditSaveButton;
  GtkWidget *patientEditCancelButton;

	/* PATIENT EDIT WINDOW */
  patientEditWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	GObject *delegate = (GObject *)patientEditWindow;
  gtk_window_set_position (GTK_WINDOW (patientEditWindow), GTK_WIN_POS_CENTER);
  gtk_window_set_modal (GTK_WINDOW (patientEditWindow), TRUE);
  gtk_window_set_type_hint (GTK_WINDOW (patientEditWindow), GDK_WINDOW_TYPE_HINT_DIALOG);
  g_signal_connect ((gpointer) patientEditWindow, "size_allocate",
                    G_CALLBACK (on_patientEditWindow_size_allocate),
                    NULL);

#if GTK_CHECK_VERSION(3, 2, 0)
  vbox5 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
#else
  vbox5 = gtk_vbox_new (FALSE, 0);
#endif
  gtk_widget_show (vbox5);
  gtk_container_add (GTK_CONTAINER (patientEditWindow), vbox5);

  label3 = gtk_label_new ("Фамилия:");
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (vbox5), label3, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label3), GTK_JUSTIFY_CENTER);
#if GTK_CHECK_VERSION(3, 14, 0)
	gtk_widget_set_halign(label3, 0.1);
	gtk_widget_set_valign(label3, 0.5);
#else
  gtk_misc_set_alignment (GTK_MISC (label3), 0.1, 0.5);
#endif

  familiyaEntry = gtk_entry_new ();
	g_object_set_data(delegate, "familiyaEntry", familiyaEntry);
  gtk_widget_show (familiyaEntry);
  gtk_box_pack_start (GTK_BOX (vbox5), familiyaEntry, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (familiyaEntry), 9679);

  label4 = gtk_label_new ("Имя:");
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (vbox5), label4, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(3, 14, 0)
	gtk_widget_set_halign(label4, 0.1);
	gtk_widget_set_valign(label4, 0.5);
#else
  gtk_misc_set_alignment (GTK_MISC (label4), 0.1, 0.5);
#endif

  imiaEntry = gtk_entry_new ();
	g_object_set_data(delegate, "imiaEntry", imiaEntry);
  gtk_widget_show (imiaEntry);
  gtk_box_pack_start (GTK_BOX (vbox5), imiaEntry, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (imiaEntry), 9679);

  label5 = gtk_label_new ("Отчество:");
  gtk_widget_show (label5);
  gtk_box_pack_start (GTK_BOX (vbox5), label5, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(3, 14, 0)
	gtk_widget_set_halign(label5, 0.1);
	gtk_widget_set_valign(label5, 0.5);
#else
  gtk_misc_set_alignment (GTK_MISC (label5), 0.1, 0.5);
#endif

  otchestvoEntry = gtk_entry_new ();
	g_object_set_data(delegate, "otchestvoEntry", otchestvoEntry);
  gtk_widget_show (otchestvoEntry);
  gtk_box_pack_start (GTK_BOX (vbox5), otchestvoEntry, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (otchestvoEntry), 9679);

  label6 = gtk_label_new ("Адрес проживания:");
  gtk_widget_show (label6);
  gtk_box_pack_start (GTK_BOX (vbox5), label6, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(3, 14, 0)
	gtk_widget_set_halign(label6, 0.1);
	gtk_widget_set_valign(label6, 0.5);
#else
  gtk_misc_set_alignment (GTK_MISC (label6), 0.1, 0.5);
#endif

  scrolledwindow3 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow3);
  gtk_box_pack_start (GTK_BOX (vbox5), scrolledwindow3, FALSE, FALSE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow3), GTK_SHADOW_IN);

  addressTextview = gtk_text_view_new ();
	g_object_set_data(delegate, "addressTextview", addressTextview);
  gtk_widget_show (addressTextview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow3), addressTextview);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (addressTextview), GTK_WRAP_WORD);

  label7 = gtk_label_new ("Email:");
  gtk_widget_show (label7);
  gtk_box_pack_start (GTK_BOX (vbox5), label7, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(3, 14, 0)
	gtk_widget_set_halign(label7, 0.1);
	gtk_widget_set_valign(label7, 0.5);
#else
  gtk_misc_set_alignment (GTK_MISC (label7), 0.1, 0.5);
#endif

  emailEntry = gtk_entry_new ();
	g_object_set_data(delegate, "emailEntry", emailEntry);
  gtk_widget_show (emailEntry);
  gtk_box_pack_start (GTK_BOX (vbox5), emailEntry, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (emailEntry), 9679);

  label8 = gtk_label_new ("Дата рождения:");
  gtk_widget_show (label8);
  gtk_box_pack_start (GTK_BOX (vbox5), label8, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(3, 14, 0)
	gtk_widget_set_halign(label8, 0.1);
	gtk_widget_set_valign(label8, 0.5);
#else
  gtk_misc_set_alignment (GTK_MISC (label8), 0.1, 0.5);
#endif

  dateOfBirthEntry = gtk_entry_new ();
	g_object_set_data(delegate, "dateOfBirthEntry", dateOfBirthEntry);
  gtk_widget_show (dateOfBirthEntry);
  gtk_box_pack_start (GTK_BOX (vbox5), dateOfBirthEntry, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (dateOfBirthEntry), 9679);
	g_signal_connect ((gpointer) dateOfBirthEntry, "insert_text",
                    G_CALLBACK (on_dateOfBirthEntry_insert_text),
                    NULL);

  label9 = gtk_label_new ("Номер телефона:");
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (vbox5), label9, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(3, 14, 0)
	gtk_widget_set_halign(label9, 0.1);
	gtk_widget_set_valign(label9, 0.5);
#else
  gtk_misc_set_alignment (GTK_MISC (label9), 0.1, 0.5);
#endif

  telEntry = gtk_entry_new ();
	g_object_set_data(delegate, "telEntry", telEntry);
  gtk_widget_show (telEntry);
  gtk_box_pack_start (GTK_BOX (vbox5), telEntry, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (telEntry), 9679);
  g_signal_connect ((gpointer) telEntry, "insert_text",
                    G_CALLBACK (on_telEntry_insert_text),
                    NULL);

  label10 = gtk_label_new ("Данные паспорта:");
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (vbox5), label10, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(3, 14, 0)
	gtk_widget_set_halign(label10, 0.1);
	gtk_widget_set_valign(label10, 0.5);
#else
  gtk_misc_set_alignment (GTK_MISC (label10), 0.1, 0.5);
#endif

  scrolledwindow4 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow4);
  gtk_box_pack_start (GTK_BOX (vbox5), scrolledwindow4, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow4), GTK_SHADOW_IN);

  passportTextview = gtk_text_view_new ();
	g_object_set_data(delegate, "passportTextview", passportTextview);
  gtk_widget_show (passportTextview);
  gtk_container_add (GTK_CONTAINER (scrolledwindow4), passportTextview);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (passportTextview), GTK_WRAP_WORD);

  label11 = gtk_label_new ("Коментарий:");
  gtk_widget_show (label11);
  gtk_box_pack_start (GTK_BOX (vbox5), label11, FALSE, FALSE, 0);
#if GTK_CHECK_VERSION(3, 14, 0)
	gtk_widget_set_halign(label11, 0.1);
	gtk_widget_set_valign(label11, 0.5);
#else
  gtk_misc_set_alignment (GTK_MISC (label11), 0.1, 0.5);
#endif

  commentEntry = gtk_entry_new ();
	g_object_set_data(delegate, "commentEntry", commentEntry);
  gtk_widget_show (commentEntry);
  gtk_box_pack_start (GTK_BOX (vbox5), commentEntry, FALSE, FALSE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (commentEntry), 9679);

#if GTK_CHECK_VERSION(3, 2, 0)
  hbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
#else
  hbox3 = gtk_hbox_new (FALSE, 0);
#endif
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox3, TRUE, TRUE, 0);

  patientEditSaveButton = gtk_button_new_with_mnemonic ("Сохранить");
  gtk_widget_show (patientEditSaveButton);
  gtk_box_pack_start (GTK_BOX (hbox3), patientEditSaveButton, FALSE, FALSE, 0);
  gtk_button_set_relief (GTK_BUTTON (patientEditSaveButton), GTK_RELIEF_NONE);
  g_signal_connect ((gpointer) patientEditSaveButton, "clicked",
                    G_CALLBACK (on_patientEditSaveButton_clicked),
                    patientEditWindow);

  patientEditCancelButton = gtk_button_new_with_mnemonic ("Отмена");
  g_signal_connect ((gpointer) patientEditCancelButton, "clicked",
                    G_CALLBACK (on_patientEditCancelButton_clicked),
                    patientEditWindow);
  gtk_widget_show (patientEditCancelButton);
  gtk_box_pack_start (GTK_BOX (hbox3), patientEditCancelButton, FALSE, FALSE, 0);
  gtk_button_set_relief (GTK_BUTTON (patientEditCancelButton), GTK_RELIEF_NONE);

	return patientEditWindow;
}

