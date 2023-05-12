/**
 * File              : interface.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 12.05.2023
 * Last Modified Date: 12.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */
#include <gtk/gtk.h>

#if GTK_CHECK_VERSION(3, 0, 0)
GtkWidget* create_mainWindow (GtkApplication *app);
#else
GtkWidget* create_mainWindow (void);
#endif
GtkWidget* create_casesWindow (void);
GtkWidget* create_patientEditWindow (void);
