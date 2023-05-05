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

void
on_______________1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on___________________1_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on__________________________1_activate (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on___________1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_________________1_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_____________________1_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on______________________1_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_________________2_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_____________________2_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_______________2_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainWindowMenuCreate_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainWindowMenuOpen_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainWindowMenuSave_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainWindowMenuSaveAs_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainWindowMenuExit_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainWindowMenuCut_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainWindowMenuCopy_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainWindowMenuPaste_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainWindowMenuDelete_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_mainWindowMenuAbout_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_casesWindowMenuCut_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_casesWindowMenuPaste_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_casesWindowMenuDelete_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_casesWindowShare_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_casesWindowLoad_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_casesWindowDocs_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_casesWindowAdd_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_casesWindowDelete_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_casesWindowPrint_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_patientEditWindow_size_allocate     (GtkWidget       *widget,
                                        GdkRectangle    *allocation,
                                        gpointer         user_data);
