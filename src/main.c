/**
 * File              : main.c
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 02.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
/*
 * Initial main.c file generated by Glade. Edit as required.
 * Glade will not overwrite this file.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "interface.h"
#include "getbundle.h"

#include "prozubilib/prozubilib.h"

#include "configFile.h"

#if GTK_CHECK_VERSION(3, 0, 0)
void application_on_activate (
		GtkApplication *app, 
		gpointer user_data
		) 
{
	GtkWidget *mainWindow = user_data;
	mainWindow = create_mainWindow(app);
}
#endif

int
main (int argc, char *argv[])
{
#if GTK_CHECK_VERSION(3, 0, 0)
	GtkApplication *app;
#endif
  GtkWidget *mainWindow;

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

#if !GTK_CHECK_VERSION(2,24,0)
  gtk_set_locale ();
#endif
  
	/*load files from bundle */
	//get bundle directory
	char *bundle = getbundle(argv);
	if (!bundle){
		g_error("can't get application bundle\n");
		return 1;
	}
	
	//crete directory in home dir
	char workdir[BUFSIZ];
	sprintf(workdir, "%s/%s", g_get_home_dir(), "gProZubi");
	g_mkdir_with_parents(workdir, 0755);
	
	//for MacOS set gtk_pixbuf paths
#ifdef __APPLE__
	char * loaders_cache = STR("%s/gdk-pixbuf-loaders.cache", workdir); 
	char * loaders_dir   = STR("%s/lib/gdk-pixbuf-2.0/2.10.0/loaders", bundle); 
	setenv("GDK_PIXBUF_MODULE_FILE", loaders_cache, true);
	setenv("GDK_PIXBUF_MODULEDIR",   loaders_dir,   true);
	setenv("GTK_PATH",        bundle, true);
	setenv("GTK_DATA_PREFIX", bundle, true);
	setenv("GTK_PATH",        bundle, true);
	//fix loaders cache
	//fpstrrep(
			//STR("%s/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache.in", bundle), 
			//loaders_cache, 
			//"$bundle", 
			//bundle
			//);
#endif

	//copy files from bundle
	///* TODO: copy image files */
	char *files[] = 
	{
		"MKBCodes.sqlite", 
		"nomenklatura.sqlite", 
		NULL
	};
	//for (int i = 0; files[i]; ++i) {
		//GFile *sfile = g_file_new_build_filename(bundle,  files[i], NULL);
		//GFile *dfile = g_file_new_build_filename(workdir, files[i], NULL);
		
		//GError *error = NULL;
		//g_file_copy(sfile, dfile, 0, NULL, NULL, NULL, &error);
		//if (error)
			//printf("g_file_copy error: %s\n", error->message);
	//}

	//free bundle var
	free(bundle);

	//chworkdir
	chdir(workdir);	

#if GTK_CHECK_VERSION(3, 0, 0)
	app = gtk_application_new ("kuzm.ig.gprozubi", 0);
	g_signal_connect (app, "activate", G_CALLBACK (application_on_activate), mainWindow); 
	return g_application_run (G_APPLICATION (app), argc, argv);
#else
	gtk_init (&argc, &argv);
	create_mainWindow ();
	gtk_main ();
	return 0;
#endif
}

