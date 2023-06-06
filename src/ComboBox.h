/**
 * File              : ComboBox.h
 * Author            : Igor V. Sementsov <ig.kuzm@gmail.com>
 * Date              : 01.06.2023
 * Last Modified Date: 01.06.2023
 * Last Modified By  : Igor V. Sementsov <ig.kuzm@gmail.com>
 */

#ifndef COMBO_BOX_H
#define COMBO_BOX_H

#include <gtk/gtk.h>
#include "prozubilib/prozubilib.h"

GtkWidget *
combo_box_new(
		GtkWidget *parent,
		prozubi_t *p,
		struct case_t *c,
		CASES key,
		char **array
		);
#endif /* ifndef COMBO_BOX_H */
