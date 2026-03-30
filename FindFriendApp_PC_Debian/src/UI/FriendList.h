#ifndef FRIEND_LIST_H
#define FRIEND_LIST_H

#include <gtk/gtk.h>
#include <stdint.h>

typedef void (*FriendSelectedCallback)(uint32_t user_id);

GtkWidget* friend_list_create(void);
void friend_list_add_user(uint32_t id, const char* name, const char* emoji, const char* signature, int online);
void friend_list_clear(void);
void friend_list_set_on_friend_selected_callback(FriendSelectedCallback cb);
void friend_list_update_user_info(const char* name, const char* signature, uint16_t tags);

#endif
