#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

// Function to create the main browser window
GtkWidget* create_browser_window() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_title(GTK_WINDOW(window), "Meisei Internet Explorer");

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // URL bar
    GtkWidget *url_bar = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), url_bar, FALSE, FALSE, 0);

    // Navigation buttons
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);

    GtkWidget *back_button = gtk_button_new_with_label("Back");
    gtk_box_pack_start(GTK_BOX(toolbar), back_button, FALSE, FALSE, 0);

    GtkWidget *forward_button = gtk_button_new_with_label("Forward");
    gtk_box_pack_start(GTK_BOX(toolbar), forward_button, FALSE, FALSE, 0);

    GtkWidget *reload_button = gtk_button_new_with_label("Reload");
    gtk_box_pack_start(GTK_BOX(toolbar), reload_button, FALSE, FALSE, 0);

    // WebView
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(web_view), TRUE, TRUE, 0);

    // Connect signals
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(url_bar, "activate", G_CALLBACK(on_url_bar_activate), web_view);
    g_signal_connect(back_button, "clicked", G_CALLBACK(on_back_button_clicked), web_view);
    g_signal_connect(forward_button, "clicked", G_CALLBACK(on_forward_button_clicked), web_view);
    g_signal_connect(reload_button, "clicked", G_CALLBACK(on_reload_button_clicked), web_view);

    return window;
}

// Callback for URL bar activation
void on_url_bar_activate(GtkEntry *entry, WebKitWebView *web_view) {
    const gchar *url = gtk_entry_get_text(entry);
    webkit_web_view_load_uri(web_view, url);
}

// Callback for back button
void on_back_button_clicked(GtkButton *button, WebKitWebView *web_view) {
    webkit_web_view_go_back(web_view);
}

// Callback for forward button
void on_forward_button_clicked(GtkButton *button, WebKitWebView *web_view) {
    webkit_web_view_go_forward(web_view);
}

// Callback for reload button
void on_reload_button_clicked(GtkButton *button, WebKitWebView *web_view) {
    webkit_web_view_reload(web_view);
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

    GtkWidget *window = create_browser_window();
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}