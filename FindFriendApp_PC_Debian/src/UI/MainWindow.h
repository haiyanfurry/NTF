#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>

GtkWidget* create_main_window(void);
void show_settings_dialog(GtkWidget *parent);
void show_personal_center(GtkWidget *parent);

#endif
