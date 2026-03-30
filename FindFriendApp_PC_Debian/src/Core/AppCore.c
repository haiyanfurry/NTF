#include <gtk/gtk.h>
#include "AppCore.h"
#include "../Pages/HomePage.h"

static void activate(GtkApplication* app, gpointer data) {
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 420, 600);
    gtk_window_set_title(GTK_WINDOW(window), "福瑞扩列计划 - Debian");

    GtkWidget* main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_append(GTK_BOX(main_box), home_page_create());
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    gtk_widget_show(window);
}

int app_core_start(int argc, char **argv) {
    GtkApplication* app = gtk_application_new("com.findfriend.app", 0);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int res = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return res;
}
