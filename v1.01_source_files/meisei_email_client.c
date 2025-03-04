#include <gtk/gtk.h>

// Function declarations
void on_new_email_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_send_button_clicked(GtkButton *button, gpointer user_data);

// Function to create the main email client window
GtkWidget* create_main_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);
    gtk_window_set_title(GTK_WINDOW(window), "Meisei Email Client");

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Menu bar
    GtkWidget *menubar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget *file_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);

    GtkWidget *new_item = gtk_menu_item_new_with_label("New Email");
    g_signal_connect(new_item, "activate", G_CALLBACK(on_new_email_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_item);

    GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(gtk_main_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    // Toolbar
    GtkWidget *toolbar = gtk_toolbar_new();
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    GtkToolItem *new_email_button = gtk_tool_button_new(NULL, "New Email");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(new_email_button), "document-new");
    g_signal_connect(new_email_button, "clicked", G_CALLBACK(on_new_email_activate), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new_email_button, -1);

    GtkToolItem *reply_button = gtk_tool_button_new(NULL, "Reply");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(reply_button), "mail-reply-sender");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), reply_button, -1);

    GtkToolItem *forward_button = gtk_tool_button_new(NULL, "Forward");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(forward_button), "mail-forward");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), forward_button, -1);

    GtkToolItem *send_receive_button = gtk_tool_button_new(NULL, "Send/Receive");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(send_receive_button), "mail-send-receive");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), send_receive_button, -1);

    GtkToolItem *find_button = gtk_tool_button_new(NULL, "Find");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(find_button), "edit-find");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), find_button, -1);

    GtkToolItem *organize_button = gtk_tool_button_new(NULL, "Organize");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(organize_button), "folder");
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), organize_button, -1);

    GtkToolItem *quit_button = gtk_tool_button_new(NULL, "Quit");
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(quit_button), "application-exit");
    g_signal_connect(quit_button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), quit_button, -1);

    // Main layout
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);

    // Shortcuts panel
    GtkWidget *shortcuts_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(hbox), shortcuts_panel, FALSE, FALSE, 0);

    GtkWidget *shortcuts_label = gtk_label_new("Shortcuts");
    gtk_box_pack_start(GTK_BOX(shortcuts_panel), shortcuts_label, FALSE, FALSE, 0);

    GtkWidget *inbox_button = gtk_button_new_with_label("Inbox");
    gtk_box_pack_start(GTK_BOX(shortcuts_panel), inbox_button, FALSE, FALSE, 0);

    GtkWidget *calendar_button = gtk_button_new_with_label("Calendar");
    gtk_box_pack_start(GTK_BOX(shortcuts_panel), calendar_button, FALSE, FALSE, 0);

    GtkWidget *contacts_button = gtk_button_new_with_label("Contacts");
    gtk_box_pack_start(GTK_BOX(shortcuts_panel), contacts_button, FALSE, FALSE, 0);

    GtkWidget *tasks_button = gtk_button_new_with_label("Tasks");
    gtk_box_pack_start(GTK_BOX(shortcuts_panel), tasks_button, FALSE, FALSE, 0);

    GtkWidget *notes_button = gtk_button_new_with_label("Notes");
    gtk_box_pack_start(GTK_BOX(shortcuts_panel), notes_button, FALSE, FALSE, 0);

    GtkWidget *my_shortcuts_button = gtk_button_new_with_label("My Shortcuts");
    gtk_box_pack_start(GTK_BOX(shortcuts_panel), my_shortcuts_button, FALSE, FALSE, 0);

    // Email list and preview pane
    GtkWidget *email_pane = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX(hbox), email_pane, TRUE, TRUE, 0);

    // Email list
    GtkWidget *email_list = gtk_tree_view_new();
    GtkTreeViewColumn *email_column = gtk_tree_view_column_new_with_attributes("Emails", gtk_cell_renderer_text_new(), "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(email_list), email_column);
    gtk_paned_pack1(GTK_PANED(email_pane), email_list, TRUE, FALSE);

    // Email preview pane
    GtkWidget *preview_pane = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(preview_pane), FALSE);
    gtk_paned_pack2(GTK_PANED(email_pane), preview_pane, TRUE, FALSE);

    // Status bar
    GtkWidget *statusbar = gtk_statusbar_new();
    gtk_box_pack_end(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

    // Connect signals
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    return window;
}

// Function to create the compose email window
GtkWidget* create_compose_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    gtk_window_set_title(GTK_WINDOW(window), "Compose Email");

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Recipient entry
    GtkWidget *to_label = gtk_label_new("To:");
    gtk_box_pack_start(GTK_BOX(vbox), to_label, FALSE, FALSE, 0);
    GtkWidget *to_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), to_entry, FALSE, FALSE, 0);

    // Subject entry
    GtkWidget *subject_label = gtk_label_new("Subject:");
    gtk_box_pack_start(GTK_BOX(vbox), subject_label, FALSE, FALSE, 0);
    GtkWidget *subject_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), subject_entry, FALSE, FALSE, 0);

    // Email body
    GtkWidget *body_view = gtk_text_view_new();
    gtk_box_pack_start(GTK_BOX(vbox), body_view, TRUE, TRUE, 0);

    // Send button
    GtkWidget *send_button = gtk_button_new_with_label("Send");
    gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 0);

    // Connect signals
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_button_clicked), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), window);

    return window;
}

// Callback for "New Email" menu item
void on_new_email_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkWidget *compose_window = create_compose_window();
    gtk_widget_show_all(compose_window);
}

// Callback for "Send" button
void on_send_button_clicked(GtkButton *button, gpointer user_data) {
    // Implement email sending functionality here
    g_print("Email sent!\n");
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Load and apply the GTK theme
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(css_provider, "themes/windows2000.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    GtkWidget *main_window = create_main_window();
    gtk_widget_show_all(main_window);

    gtk_main();
    return 0;
}