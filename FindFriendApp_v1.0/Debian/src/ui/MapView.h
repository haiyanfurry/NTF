#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include <gtk/gtk.h>
#include <stdint.h>
#include "../models/User.h"

typedef void (*UserClickCallback)(uint32_t user_id);

GtkWidget* map_view_create(void);
void map_view_update_nearby_users(UserList *list);
void map_view_set_center(double lat, double lng, int zoom);
void map_view_set_on_user_click_callback(UserClickCallback cb);
void map_view_set_tags(uint16_t tags);

#endif
