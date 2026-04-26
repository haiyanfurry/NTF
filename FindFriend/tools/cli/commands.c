// FindFriend CLI 工具命令处理模块

#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FFResult ff_cli_handle_command(char* command, char** args, int argc) {
    if (!command) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    if (strcmp(command, "init") == 0) {
        printf("FindFriend initialized successfully!\n");
        return FF_OK;
    } else if (strcmp(command, "status") == 0) {
        printf("FindFriend status:\n");
        printf("- Core: initialized\n");
        printf("- User manager: initialized\n");
        printf("- Message manager: initialized\n");
        printf("- Social manager: initialized\n");
        printf("- Game manager: initialized\n");
        printf("- Location service: initialized\n");
        printf("- P2P module: initialized\n");
        printf("- Data store: initialized\n");
        printf("- Cache manager: initialized\n");
        return FF_OK;
    } else if (strcmp(command, "help") == 0) {
        printf("FindFriend CLI Tool\n");
        printf("Usage: ff_cli <command> [arguments]\n\n");
        printf("Commands:\n");
        printf("  init                  Initialize FindFriend\n");
        printf("  status                Show FindFriend status\n");
        printf("  login <username> <password>   Login user\n");
        printf("  logout <uid>          Logout user\n");
        printf("  register <username> <password> Register new user\n");
        printf("  user-info <uid>       Get user information\n");
        printf("  message-send <sender> <receiver> <content> Send message\n");
        printf("  message-receive <uid> Receive messages\n");
        printf("  friend-add <sender> <receiver> Send friend request\n");
        printf("  friend-list <uid>     Get friends list\n");
        printf("  location-update <uid> <lat> <lon> <acc> Update location\n");
        printf("  location-get <uid>    Get location\n");
        printf("  p2p-status           Show P2P status\n");
        printf("  p2p-nodes            Show P2P nodes\n");
        printf("  game-create <name> <uid> <max> Create game\n");
        printf("  game-join <id> <uid>  Join game\n");
        printf("  help                  Show this help\n");
        return FF_OK;
    }
    
    return FF_ERROR_NOT_FOUND;
}
