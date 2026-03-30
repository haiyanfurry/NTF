#include "User.h"
#include <string.h>

void user_init(User *u, uint32_t id, const char *name, const char *emoji, const char *sig) {
    u->id = id;
    strncpy(u->nickname, name, 63);
    strncpy(u->emoji, emoji, 7);
    strncpy(u->signature, sig, 127);
    u->online = 0;
    u->tags = 1;
    u->latitude = 0;
    u->longitude = 0;
    u->last_seen = 0;
}
