/**
 * File              : Xray.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 31.05.2023
 * Last Modified Date: 31.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef XRAY_H
#define XRAY_H

#include "prozubilib/prozubilib.h"
#include <gtk/gtk.h>

GtkWidget *
xray_new(
		GtkWindow *window,
		GtkWidget *parent,
		prozubi_t *p, 
		struct case_t *c		
		);

#endif /* ifndef XRAY_H */

