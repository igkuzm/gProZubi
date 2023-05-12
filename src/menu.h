/**
 * File              : menu.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 12.05.2023
 * Last Modified Date: 12.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#ifndef MANU_H
#define MANU_H

#include <gtk/gtk.h>
#include "callbacks.h"
#include "callbacks.h"
#include "patientList.h"


#if GTK_CHECK_VERSION(3, 10, 0)
#define CALLBACK_MENU_ITEM GSimpleAction *action, GVariant *parameter, gpointer user_data
#else
#define CALLBACK_MENU_ITEM GtkMenuItem *menuitem, gpointer user_data
#endif

#define MAIN_MENU_ITEMS \
	MAIN_MENU_ROOT_ITEM(file, "Файл", 1)\
	MAIN_MENU_CHILD_ITEM(file, new, "Новый", 1, "gtk-new", "app.new", on_addButton_clicked(NULL, user_data))\
	MAIN_MENU_CHILD_ITEM(file, open, "Открыть", 1, "gtk-open", "app.open", NULL)\
	MAIN_MENU_CHILD_ITEM(file, dele, "Удалить", 1, "gtk-delete", "app.delete", patient_list_remove_clicked(user_data))\
	MAIN_MENU_CHILD_ITEM(file, quit, "Завершить «GProZubi»", 0, "gtk-quit", "app.quit", gprozubi_quit(user_data))\
	MAIN_MENU_ROOT_ITEM(edit, "Правка", 1)\
	MAIN_MENU_CHILD_ITEM(edit, cut, "Вырезать", 1, "gtk-cut", "app.cut", NULL)\
	MAIN_MENU_CHILD_ITEM(edit, copy, "Копировать", 1, "gtk-copy", "app.copy", NULL)\
	MAIN_MENU_CHILD_ITEM(edit, paste, "Вставить", 1, "gtk-paste", "app.paste", NULL)\
	MAIN_MENU_ROOT_ITEM(config, "Настройки", 0)\
	MAIN_MENU_CHILD_ITEM(config, preferences, "Настройки", 0, "gtk-preferences", "app.preferences", NULL)\
	MAIN_MENU_ROOT_ITEM(help, "Справка", 1)\
	MAIN_MENU_CHILD_ITEM(help, about, "О программе...", 0, "gtk-about", "app.about", NULL)\
	MAIN_MENU_ROOT_APPLE_ITEM(a_about, "О программе...", "gtk-about", "app.about", NULL)\
	MAIN_MENU_ROOT_APPLE_ITEM(a_quit, "Завершить «GProZubi»", "gtk-quit", "app.quit", NULL)\
	MAIN_MENU_ROOT_APPLE_ITEM(a_preferences, "Настройки", "gtk-config", "app.preferences", NULL)

#define MAIN_MENU_ROOT_ITEM(name, title, apple)
#define MAIN_MENU_CHILD_ITEM(root, name, title, apple, mnemonic, action, cb)\
	static void callback_##name(CALLBACK_MENU_ITEM){\
		cb;\
	};
#define MAIN_MENU_ROOT_APPLE_ITEM(name, title, mnemonic, action, cb)\
	static void callback_##name(CALLBACK_MENU_ITEM){\
		cb;\
	};
	MAIN_MENU_ITEMS
#undef MAIN_MENU_ROOT_ITEM
#undef MAIN_MENU_CHILD_ITEM
#undef MAIN_MENU_ROOT_APPLE_ITEM

#if GTK_CHECK_VERSION(3, 10, 0)
const GActionEntry app_actions[] = {
#define MAIN_MENU_ROOT_ITEM(name, title, apple)
#define MAIN_MENU_CHILD_ITEM(root, name, title, apple, mnemonic, action, cb)\
	{ #name, callback_##name },
#define MAIN_MENU_ROOT_APPLE_ITEM(name, title, mnemonic, action, cb)\
	{ #name, callback_##name },
	MAIN_MENU_ITEMS
#undef MAIN_MENU_ROOT_ITEM
#undef MAIN_MENU_CHILD_ITEM
#undef MAIN_MENU_ROOT_APPLE_ITEM
};
#endif

static GtkWidget *  
#if GTK_CHECK_VERSION(3, 10, 0)
create_main_menu( GtkApplication *app, GtkWidget *mainWindow)
{
	g_action_map_add_action_entries (
			G_ACTION_MAP (app), 
			app_actions, 
			G_N_ELEMENTS (app_actions), 
			mainWindow); //activate menu actions

/* Apple application menu */	
int isApple = 0;
#ifdef __APPLE__ 
	isApple = 1;
	GMenu *appmenu = g_menu_new(); //main application menu
#define MAIN_MENU_ROOT_ITEM(name, title, apple)
#define MAIN_MENU_CHILD_ITEM(root, name, title, apple, mnemonic, action, cb)
#define MAIN_MENU_ROOT_APPLE_ITEM(name, title, mnemonic, action, cb)\
	g_menu_append (appmenu, title, action);
	MAIN_MENU_ITEMS
#undef MAIN_MENU_ROOT_ITEM
#undef MAIN_MENU_CHILD_ITEM
#undef MAIN_MENU_ROOT_APPLE_ITEM
	
	gtk_application_set_app_menu(GTK_APPLICATION (app), G_MENU_MODEL(appmenu));
#endif

/* main menu bar */	
GMenu *menuBar = g_menu_new (); //main menu bar
#define MAIN_MENU_ROOT_ITEM(name, title, apple)\
	GMenu *menu_##name = g_menu_new();\
	if (!isApple || (isApple && apple)) {\
		g_menu_append_submenu(menuBar, title, G_MENU_MODEL(menu_##name));\
	}
#define MAIN_MENU_CHILD_ITEM(root, name, title, apple, mnemonic, action, cb)\
	if (!isApple || (isApple && apple)) {\
		g_menu_append (menu_##root, title, action);\
	}
#define MAIN_MENU_ROOT_APPLE_ITEM(name, title, mnemonic, action, cb)
MAIN_MENU_ITEMS
#undef MAIN_MENU_ROOT_ITEM
#undef MAIN_MENU_CHILD_ITEM
#undef MAIN_MENU_ROOT_APPLE_ITEM
	gtk_application_set_menubar (GTK_APPLICATION (app), G_MENU_MODEL (menuBar));
	gtk_application_window_set_show_menubar (GTK_APPLICATION_WINDOW (mainWindow), TRUE);

#else
create_main_menu(GtkWidget *mainWindow)
{

	int isApple = 0;
#ifdef __APPLE__
	isApple = 1;
#endif

  GtkAccelGroup *accel_group;
  accel_group = gtk_accel_group_new ();
  gtk_window_add_accel_group (GTK_WINDOW (mainWindow), accel_group);
	
  GtkWidget *mainWindowMenu = gtk_menu_bar_new ();
  gtk_widget_show (mainWindowMenu);

#define MAIN_MENU_ROOT_ITEM(name, title, apple)\
  GtkWidget *name = gtk_menu_item_new_with_mnemonic (title);\
	if (!isApple || (isApple && apple)) {\
		gtk_widget_show (name);\
	}\
		gtk_container_add (GTK_CONTAINER (mainWindowMenu), name);\
		GtkWidget * menu_##name = gtk_menu_new ();\
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (name), menu_##name);
#define MAIN_MENU_CHILD_ITEM(root, name, title, apple, mnemonic, action, cb)\
	if (!isApple || (isApple && apple)) {\
		GtkWidget * name = gtk_image_menu_item_new_from_stock (mnemonic, accel_group);\
		gtk_widget_show (name);\
		gtk_container_add (GTK_CONTAINER (menu_##root), name);\
		g_signal_connect ((gpointer) name, "activate",\
				G_CALLBACK (callback_##name), mainWindow);\
	}
#define MAIN_MENU_ROOT_APPLE_ITEM(name, title, mnemonic, action, cb)
MAIN_MENU_ITEMS
#undef MAIN_MENU_ROOT_ITEM
#undef MAIN_MENU_CHILD_ITEM
#undef MAIN_MENU_ROOT_APPLE_ITEM

	return  mainWindowMenu;

#endif //GTK_CHECK_VERSION
}

#endif /* ifndef MANU_H */
