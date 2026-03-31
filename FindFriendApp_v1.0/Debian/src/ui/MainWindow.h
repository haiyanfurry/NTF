#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gtk/gtk.h>
#include <stdint.h>

GtkWidget* create_main_window(void);
void show_login_dialog(void);
void show_exhibition_panel(void);
void show_ticket_panel(void);
void show_transaction_panel(void);
void show_photo_gallery(void);

#endif
