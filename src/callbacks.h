#include <gtk/gtk.h>


void
on_patientListButton_activate          (GtkButton       *button,
                                        gpointer         user_data);

void
on_doctorListButton_activate           (GtkButton       *button,
                                        gpointer         user_data);

void
on_priceListButton_activate            (GtkButton       *button,
                                        gpointer         user_data);

void
on_configButton_activate               (GtkButton       *button,
                                        gpointer         user_data);

void
on_patientListButton_activate2         (GtkButton       *button,
                                        gpointer         user_data);

void
on_patientListButton_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_doctorListButton_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_priceListButton_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_configButton_clicked                (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_casesListView_select_cursor_row     (GtkTreeView     *treeview,
                                        gboolean         start_editing,
                                        gpointer         user_data);

gboolean
on_casesListView_toggle_cursor_row     (GtkTreeView     *treeview,
                                        gpointer         user_data);

void
on_caseAddButton_clicked               (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

gboolean
on_casesPaned_accept_position          (GtkPaned        *paned,
                                        gpointer         user_data);

gboolean
on_casesPaned_move_handle              (GtkPaned        *paned,
                                        GtkScrollType    scroll,
                                        gpointer         user_data);

void
on_casesPaned_size_request             (GtkWidget       *widget,
                                        GtkRequisition  *requisition,
                                        gpointer         user_data);

void
on_casesPaned_size_allocate            (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data);

void
on_mainWindow_size_allocate            (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data);

void
on_casesWindow_size_allocate           (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data);

void
on_casesPaned_size_allocate            (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data);

void
on_mainWindowPaned_size_allocate       (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data);

void
on_mainWindowLeftBox_size_allocate     (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data);

void
on_casesWindowLeftBox_size_allocate    (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data);

void
on_removeButton_clicked                (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_familiyaEntry_changed               (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_imiaEntry_changed                   (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_otchestvoEntry_changed              (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_emailEntry_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_dateOfBirthEntry_changed            (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_telEntry_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_commentEntry_changed                (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_patientEditCancelButton_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_patientEditSaveButton_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_addButton_clicked                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_editButton_clicked                  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_telEntry_insert_text                (GtkEditable     *editable,
                                        gchar           *new_text,
                                        gint             new_text_length,
                                        gpointer         position,
                                        gpointer         user_data);

void
on_dateOfBirthEntry_insert_text        (GtkEditable     *editable,
                                        gchar           *new_text,
                                        gint             new_text_length,
                                        gpointer         position,
                                        gpointer         user_data);
