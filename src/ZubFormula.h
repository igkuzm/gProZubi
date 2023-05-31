/**
 * File              : ZubFormula.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 31.05.2023
 * Last Modified Date: 31.05.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef ZUBFORMULA_H
#define ZUBFORMULA_H

#include "prozubilib/prozubilib.h"
#include <gtk/gtk.h>

GtkWidget *
zubformula_new(
		GtkWidget *parent,
		prozubi_t *p, 
		struct case_t *c		
		);

#endif /* ifndef ZUBFORMULA_H */
