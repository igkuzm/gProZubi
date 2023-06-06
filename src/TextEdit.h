/**
 * File              : TextEdit.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.06.2023
 * Last Modified Date: 01.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef TEXT_EDIT_H
#define TEXT_EDIT_H

#include <gtk/gtk.h>
#include "prozubilib/prozubilib.h"

GtkWidget *
text_edit_new(
		GtkWidget *parent,
		prozubi_t *p,
		struct case_t *c,
		CASES key
		);
#endif /* ifndef TEXT_EDIT_H */
