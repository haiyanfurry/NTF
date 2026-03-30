#ifndef CHAT_VIEW_H
#define CHAT_VIEW_H

#include <gtk/gtk.h>
#include <stdint.h>

GtkWidget* chat_view_create(void);
void chat_view_show(uint32_t user_id);
void chat_view_hide(void);
void chat_view_add_message(uint32_t from_id, const char* message, int is_self);

#endif
