#include "User.h"
#include <string.h>
#include <stdio.h>

void user_init(User *u) {
    memset(u, 0, sizeof(User));
    u->identity = IDENTITY_NORMAL;
    u->location_enabled = 1;
    strcpy(u->emoji, "🐾");
}

void user_set_identity(User *u, UserIdentity identity) {
    u->identity = identity;
    switch(identity) {
        case IDENTITY_COSPLAYER: strcpy(u->emoji, "🎭"); break;
        case IDENTITY_BIANHE: strcpy(u->emoji, "🌾"); break;
        case IDENTITY_OFFICIAL: strcpy(u->emoji, "⭐"); break;
        case IDENTITY_EXHIBITOR: strcpy(u->emoji, "🏢"); break;
        default: strcpy(u->emoji, "👤");
    }
}

const char* user_get_identity_icon(UserIdentity identity) {
    switch(identity) {
        case IDENTITY_COSPLAYER: return "🎭";
        case IDENTITY_BIANHE: return "🌾";
        case IDENTITY_OFFICIAL: return "⭐";
        case IDENTITY_EXHIBITOR: return "🏢";
        default: return "👤";
    }
}

const char* user_get_identity_color(UserIdentity identity) {
    switch(identity) {
        case IDENTITY_COSPLAYER: return "#e91e63";
        case IDENTITY_BIANHE: return "#4caf50";
        case IDENTITY_OFFICIAL: return "#ff9800";
        case IDENTITY_EXHIBITOR: return "#2196f3";
        default: return "#9e9e9e";
    }
}
