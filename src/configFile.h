/**
 * File              : configFile.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.05.2023
 * Last Modified Date: 12.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#define CONFIG_FILE "prozubi.ini"

#include <gtk/gtk.h>
#include "prozubilib/cases.h"
#include "casesEdit.h"

#include "prozubilib/prozubilib.h"

static void 
widget_save_state_to_config(
		const char * widget_name, 
		int x, int y, 
		int width, int height
		)
{
	GKeyFile *key_file = g_key_file_new ();
	g_key_file_load_from_file(key_file, CONFIG_FILE, 0, NULL);
	g_key_file_set_integer(key_file, widget_name, "x",  x);
	g_key_file_set_integer(key_file, widget_name, "y",  y);
	g_key_file_set_integer(key_file, widget_name, "width",  width);
	g_key_file_set_integer(key_file, widget_name, "height", height);
	g_key_file_save_to_file(key_file, CONFIG_FILE, NULL);
}


static void 
save_widget_state (
		GtkWidget *widget, 
		GtkAllocation *allocation, 
		gpointer userdata
		)
{
	const char * window_name = userdata;
	widget_save_state_to_config(window_name, allocation->x, allocation->y, allocation->width, allocation->height);
}

static void 
widget_restore_state_from_config(
		GtkWidget *widget, 
		const char * widget_name, 
		int default_width, 
		int default_height
		)
{
	//int x = -1, y = -1;
	int width = default_width, height = default_height;
	GKeyFile *key_file = g_key_file_new ();
	if (g_key_file_load_from_file(key_file, CONFIG_FILE, 0, NULL)){
		width  = g_key_file_get_integer(key_file, widget_name, "width", NULL); 
		height = g_key_file_get_integer(key_file, widget_name, "height", NULL);
		//x = g_key_file_get_integer(key_file, window_name, "x", NULL);
		//y = g_key_file_get_integer(key_file, window_name, "y", NULL);

		if (width < 100 || height < 100){
			width = default_width;	
			height = default_height;
		}
			
	}	
	if (GTK_IS_WINDOW(widget))
		gtk_window_set_default_size(GTK_WINDOW(widget), width, height);
	else
		gtk_widget_set_size_request(widget, width, height);
	//if (x > -1 && y > -1)
		//gtk_window_move(window, x, y);
}

static void 
save_colummn_state (
		gint width, 
		gint col,
		gchar *name
		)
{
	char col_n[10];
	sprintf(col_n, "%d", col);
	GKeyFile *key_file = g_key_file_new ();
	g_key_file_load_from_file(key_file, CONFIG_FILE, 0, NULL);
	g_key_file_set_integer(key_file, name, col_n,  width);
	g_key_file_save_to_file(key_file, CONFIG_FILE, NULL);	
}

static gint 
get_colummn_state (
		gint col,
		gchar *name
		)
{
	char col_n[10];
	sprintf(col_n, "%d", col);
	GKeyFile *key_file = g_key_file_new ();
	if (g_key_file_load_from_file(key_file, CONFIG_FILE, 0, NULL)){
		return g_key_file_get_integer(key_file, name, col_n, NULL); 
	}	
	return -1;
}

static void 
save_token_to_config(const char * token, time_t expires, const char * reftoken){
	GKeyFile *key_file = g_key_file_new ();
	g_key_file_load_from_file(key_file, CONFIG_FILE, 0, NULL);
	g_key_file_set_value(key_file, "YandexDisk", "token", token);
	g_key_file_set_int64(key_file, "YandexDisk", "expires", expires);
	g_key_file_set_value(key_file, "YandexDisk", "reftoken", reftoken);
	g_key_file_save_to_file(key_file, CONFIG_FILE, NULL);	
}

static char * 
token_from_config(){
	GKeyFile *key_file = g_key_file_new ();
	if (g_key_file_load_from_file(key_file, CONFIG_FILE, 0, NULL)){
		gchar * token = g_key_file_get_value(key_file, "YandexDisk", "token", NULL);
		if (token){
			char * ret = malloc(64);
			if (!ret)
				return strdup("");
			strncpy(ret, token, 63);
			ret[63] = 0;
			return ret;
		}
	}	
	return strdup("");
}

#endif /* ifndef CONFIG_FILE_H */
