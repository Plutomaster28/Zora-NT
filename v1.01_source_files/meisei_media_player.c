#include <gtk/gtk.h>
#include <gst/gst.h>

// Function declarations
void on_open_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_play_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_pause_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_stop_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_quit_activate(GtkMenuItem *menuitem, gpointer user_data);
void on_volume_changed(GtkScaleButton *scale, gdouble value, gpointer user_data);

// Global variables
GtkWidget *window;
GtkWidget *video_area;
GtkWidget *statusbar;
guint context_id;
GstElement *pipeline;
GstElement *video_sink;
gboolean is_playing = FALSE;

void on_open_activate(GtkMenuItem *menuitem, gpointer user_data) {
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new("Open File",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        gst_element_set_state(pipeline, GST_STATE_NULL);
        g_object_set(pipeline, "uri", g_strdup_printf("file://%s", filename), NULL);
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        is_playing = TRUE;

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

void on_play_activate(GtkMenuItem *menuitem, gpointer user_data) {
    if (!is_playing) {
        gst_element_set_state(pipeline, GST_STATE_PLAYING);
        is_playing = TRUE;
    }
}

void on_pause_activate(GtkMenuItem *menuitem, gpointer user_data) {
    if (is_playing) {
        gst_element_set_state(pipeline, GST_STATE_PAUSED);
        is_playing = FALSE;
    }
}

void on_stop_activate(GtkMenuItem *menuitem, gpointer user_data) {
    gst_element_set_state(pipeline, GST_STATE_NULL);
    is_playing = FALSE;
}

void on_quit_activate(GtkMenuItem *menuitem, gpointer user_data) {
    gtk_main_quit();
}

void on_volume_changed(GtkScaleButton *scale, gdouble value, gpointer user_data) {
    g_object_set(pipeline, "volume", value, NULL);
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {
    GMainLoop *loop = (GMainLoop *)data;

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_EOS:
            g_print("End of stream\n");
            gst_element_set_state(pipeline, GST_STATE_NULL);
            is_playing = FALSE;
            break;
        case GST_MESSAGE_ERROR: {
            gchar *debug;
            GError *error;

            gst_message_parse_error(msg, &error, &debug);
            g_free(debug);

            g_printerr("Error: %s\n", error->message);
            g_error_free(error);

            gst_element_set_state(pipeline, GST_STATE_NULL);
            is_playing = FALSE;
            break;
        }
        default:
            break;
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_title(GTK_WINDOW(window), "Meisei Media Player");

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the menu bar, video area, and status bar
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the menu bar
    GtkWidget *menubar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget *file_item = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);

    GtkWidget *open_item = gtk_menu_item_new_with_label("Open");
    g_signal_connect(open_item, "activate", G_CALLBACK(on_open_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);

    GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");
    g_signal_connect(quit_item, "activate", G_CALLBACK(on_quit_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    GtkWidget *control_menu = gtk_menu_new();
    GtkWidget *control_item = gtk_menu_item_new_with_label("Control");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(control_item), control_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), control_item);

    GtkWidget *play_item = gtk_menu_item_new_with_label("Play");
    g_signal_connect(play_item, "activate", G_CALLBACK(on_play_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(control_menu), play_item);

    GtkWidget *pause_item = gtk_menu_item_new_with_label("Pause");
    g_signal_connect(pause_item, "activate", G_CALLBACK(on_pause_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(control_menu), pause_item);

    GtkWidget *stop_item = gtk_menu_item_new_with_label("Stop");
    g_signal_connect(stop_item, "activate", G_CALLBACK(on_stop_activate), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(control_menu), stop_item);

    // Create the video area
    video_area = gtk_drawing_area_new();
    gtk_widget_set_size_request(video_area, 800, 450);
    gtk_box_pack_start(GTK_BOX(vbox), video_area, TRUE, TRUE, 0);

    // Create the status bar
    statusbar = gtk_statusbar_new();
    context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "Statusbar");
    gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, FALSE, 0);

    // Create the volume control
    GtkWidget *volume_control = gtk_scale_button_new(GTK_ICON_SIZE_SMALL_TOOLBAR, 0.0, 1.0, 0.1, NULL);
    gtk_scale_button_set_value(GTK_SCALE_BUTTON(volume_control), 0.5);
    g_signal_connect(volume_control, "value-changed", G_CALLBACK(on_volume_changed), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), volume_control, FALSE, FALSE, 0);

    // Initialize GStreamer
    pipeline = gst_element_factory_make("playbin", "player");
    video_sink = gst_element_factory_make("gtksink", "video_sink");
    g_object_set(pipeline, "video-sink", video_sink, NULL);

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, bus_call, NULL);
    gst_object_unref(bus);

    gtk_widget_show_all(window);

    gtk_main();

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));

    return 0;
}