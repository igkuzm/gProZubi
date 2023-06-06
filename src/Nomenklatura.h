/**
 * File              : Nomenklatura.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.04.2023
 * Last Modified Date: 06.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef NOMENKLATURA_LIST_H
#define NOMENKLATURA_LIST_H

#include <gtk/gtk.h>
#include "prozubilib/prozubilib.h"

GtkWidget *
nomenklatura_list_new(
		GtkWidget *parent,
		prozubi_t *p,
		void *user_data,	
		int (*callback)(
			void *user_data,
			const char *name,
			const char *kod,
			const char *iD
			)
		);


#endif /* ifndef NOMENKLATURA_LIST_H */
