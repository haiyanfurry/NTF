#include "AppCore.h"
#include "../Pages/HomePage.h"
#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = create_home_page();
    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_widget_show_all(window);
}

int app_core_init(int argc, char **argv) {
    GtkApplication *app;
    int status;
    
    app = gtk_application_new("com.findfriend.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
