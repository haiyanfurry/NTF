#include "Settings.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CONFIG_FILE ".findfriend_config"

void settings_init(AppSettings *s) {
    s->good_samaritan_mode = 0;
    s->server_port = 8888;
    s->map_zoom = 10.0;
    s->map_lat = 39.9042;
    s->map_lng = 116.4074;
    strcpy(s->nickname, "旅行者");
    strcpy(s->signature, "寻找志同道合的朋友~");
    s->tags = 1;
    s->animation_enabled = 1;
}

void settings_load(AppSettings *s) {
    FILE *f = fopen(CONFIG_FILE, "r");
    if (!f) {
        settings_init(s);
        return;
    }
    fread(s, sizeof(AppSettings), 1, f);
    fclose(f);
}

void settings_save(AppSettings *s) {
    FILE *f = fopen(CONFIG_FILE, "w");
    if (f) {
        fwrite(s, sizeof(AppSettings), 1, f);
        fclose(f);
    }
}
