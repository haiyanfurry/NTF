// FindFriend P2P TURN 中继客户端实现

#include "ff_p2p_turn.h"
#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define TURN_HEADER_SIZE 4
#define TURN_CHANNEL_DATA 0x4000
#define TURN_DATA (0x0003)
#define TURN_ALLOCATE (0x0003)
#define TURN_REFRESH (0x0004)
#define TURN_CREATE_PERMISSION (0x0008)
#define TURN_CHAN_BIND (0x0009)

static bool g_turn_initialized = false;
static char g_turn_server[256];
static uint16_t g_turn_port = 3478;
static char g_username[128];
static char g_password[128];

FFResult ff_turn_init(const char* turn_server, uint16_t turn_port,
                      const char* username, const char* password) {
    if (!turn_server || !username || !password) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    strncpy(g_turn_server, turn_server, sizeof(g_turn_server) - 1);
    g_turn_port = turn_port;
    strncpy(g_username, username, sizeof(g_username) - 1);
    strncpy(g_password, password, sizeof(g_password) - 1);
    
    g_turn_initialized = true;
    
    return FF_OK;
}

void ff_turn_cleanup(void) {
    g_turn_initialized = false;
    memset(g_turn_server, 0, sizeof(g_turn_server));
    memset(g_username, 0, sizeof(g_username));
    memset(g_password, 0, sizeof(g_password));
}

FFResult ff_turn_allocate(int sockfd, char* relayed_ip, uint16_t* relayed_port,
                          char* mapped_ip, uint16_t* mapped_port) {
    if (!g_turn_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    struct sockaddr_in turn_addr;
    memset(&turn_addr, 0, sizeof(turn_addr));
    turn_addr.sin_family = AF_INET;
    turn_addr.sin_port = htons(g_turn_port);
    inet_pton(AF_INET, g_turn_server, &turn_addr.sin_addr);
    
    uint8_t allocate_request[1024];
    memset(allocate_request, 0, sizeof(allocate_request));
    
    uint16_t msg_type = htons(0x0001);
    memcpy(allocate_request, &msg_type, 2);
    
    uint32_t magic_cookie = htonl(0x2112A442);
    memcpy(allocate_request + 4, &magic_cookie, 4);
    
    for (int i = 0; i < 12; i++) {
        allocate_request[8 + i] = (uint8_t)(rand() & 0xFF);
    }
    
    ssize_t sent = sendto(sockfd, allocate_request, 36, 0,
                          (struct sockaddr*)&turn_addr, sizeof(turn_addr));
    
    if (sent < 0) {
        return FF_ERROR;
    }
    
    uint8_t response[1024];
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    
    fd_set read_fds;
    struct timeval tv;
    
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    
    int ready = select(sockfd + 1, &read_fds, NULL, NULL, &tv);
    if (ready <= 0) {
        return FF_ERROR_TIMEOUT;
    }
    
    ssize_t received = recvfrom(sockfd, response, sizeof(response), 0,
                                (struct sockaddr*)&from_addr, &addr_len);
    
    if (received < 0) {
        return FF_ERROR;
    }
    
    if (relayed_ip && relayed_port) {
        strncpy(relayed_ip, "0.0.0.0", 16);
        *relayed_port = 0;
    }
    
    if (mapped_ip && mapped_port) {
        strncpy(mapped_ip, "0.0.0.0", 16);
        *mapped_port = 0;
    }
    
    return FF_OK;
}

FFResult ff_turn_send_data(int sockfd, const uint8_t* data, size_t data_len,
                           const char* peer_ip, uint16_t peer_port) {
    if (!g_turn_initialized || !data || !peer_ip) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    struct sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(peer_port);
    inet_pton(AF_INET, peer_ip, &peer_addr.sin_addr);
    
    uint8_t channel_data[65536];
    uint16_t channel_id = TURN_CHANNEL_DATA;
    
    channel_id = htons(channel_id);
    memcpy(channel_data, &channel_id, 2);
    
    uint16_t length = htons((uint16_t)data_len);
    memcpy(channel_data + 2, &length, 2);
    
    memcpy(channel_data + 4, data, data_len);
    
    ssize_t sent = sendto(sockfd, channel_data, 4 + data_len, 0,
                          (struct sockaddr*)&peer_addr, sizeof(peer_addr));
    
    if (sent < 0) {
        return FF_ERROR;
    }
    
    return FF_OK;
}

FFResult ff_turn_recv_data(int sockfd, uint8_t* data, size_t buffer_size,
                           size_t* received_len, uint32_t timeout_ms) {
    if (!g_turn_initialized || !data || !received_len) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    fd_set read_fds;
    struct timeval tv;
    
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    
    int ready = select(sockfd + 1, &read_fds, NULL, NULL, &tv);
    if (ready < 0) {
        return FF_ERROR;
    }
    
    if (ready == 0) {
        return FF_ERROR_TIMEOUT;
    }
    
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    
    ssize_t received = recvfrom(sockfd, data, buffer_size, 0,
                                (struct sockaddr*)&from_addr, &addr_len);
    
    if (received < 0) {
        return FF_ERROR;
    }
    
    *received_len = received;
    return FF_OK;
}

FFResult ff_turn_refresh(int sockfd) {
    if (!g_turn_initialized) {
        return FF_ERROR_NOT_INITIALIZED;
    }
    
    struct sockaddr_in turn_addr;
    memset(&turn_addr, 0, sizeof(turn_addr));
    turn_addr.sin_family = AF_INET;
    turn_addr.sin_port = htons(g_turn_port);
    inet_pton(AF_INET, g_turn_server, &turn_addr.sin_addr);
    
    uint8_t refresh_request[36];
    memset(refresh_request, 0, sizeof(refresh_request));
    
    uint16_t msg_type = htons(0x0004);
    memcpy(refresh_request, &msg_type, 2);
    
    uint32_t magic_cookie = htonl(0x2112A442);
    memcpy(refresh_request + 4, &magic_cookie, 4);
    
    for (int i = 0; i < 12; i++) {
        refresh_request[8 + i] = (uint8_t)(rand() & 0xFF);
    }
    
    ssize_t sent = sendto(sockfd, refresh_request, 36, 0,
                          (struct sockaddr*)&turn_addr, sizeof(turn_addr));
    
    if (sent < 0) {
        return FF_ERROR;
    }
    
    return FF_OK;
}

FFResult ff_turn_create_permission(int sockfd, const char* peer_ip) {
    if (!g_turn_initialized || !peer_ip) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    struct sockaddr_in turn_addr;
    memset(&turn_addr, 0, sizeof(turn_addr));
    turn_addr.sin_family = AF_INET;
    turn_addr.sin_port = htons(g_turn_port);
    inet_pton(AF_INET, g_turn_server, &turn_addr.sin_addr);
    
    uint8_t permission_request[1024];
    memset(permission_request, 0, sizeof(permission_request));
    
    uint16_t msg_type = htons(0x0008);
    memcpy(permission_request, &msg_type, 2);
    
    uint32_t magic_cookie = htonl(0x2112A442);
    memcpy(permission_request + 4, &magic_cookie, 4);
    
    for (int i = 0; i < 12; i++) {
        permission_request[8 + i] = (uint8_t)(rand() & 0xFF);
    }
    
    ssize_t sent = sendto(sockfd, permission_request, 36, 0,
                          (struct sockaddr*)&turn_addr, sizeof(turn_addr));
    
    if (sent < 0) {
        return FF_ERROR;
    }
    
    return FF_OK;
}
