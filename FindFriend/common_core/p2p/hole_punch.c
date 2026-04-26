// FindFriend P2P UDP 打洞实现

#include "ff_p2p_hole_punch.h"
#include "ff_core_interface.h"
#include "ff_p2p_stun.h"
#include "ff_p2p_turn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define HOLE_PUNCH_TIMEOUT 10000
#define HOLE_PUNCH_RETRY_COUNT 5
#define HOLE_PUNCH_INTERVAL 1000

typedef struct {
    char peer_ip[16];
    uint16_t peer_port;
    uint8_t peer_id[32];
    int sockfd;
    bool success;
    pthread_mutex_t mutex;
} HolePunchContext;

static HolePunchContext g_hole_punch_contexts[16];
static size_t g_context_count = 0;
static pthread_mutex_t g_context_mutex = PTHREAD_MUTEX_INITIALIZER;

static HolePunchContext* find_context(const uint8_t* peer_id) {
    for (size_t i = 0; i < g_context_count; i++) {
        if (memcmp(g_hole_punch_contexts[i].peer_id, peer_id, 32) == 0) {
            return &g_hole_punch_contexts[i];
        }
    }
    return NULL;
}

static HolePunchContext* allocate_context(void) {
    if (g_context_count >= 16) {
        return NULL;
    }
    return &g_hole_punch_contexts[g_context_count++];
}

FFResult ff_hole_punch_init(void) {
    pthread_mutex_lock(&g_context_mutex);
    g_context_count = 0;
    pthread_mutex_unlock(&g_context_mutex);
    return FF_OK;
}

void ff_hole_punch_cleanup(void) {
    pthread_mutex_lock(&g_context_mutex);
    for (size_t i = 0; i < g_context_count; i++) {
        if (g_hole_punch_contexts[i].sockfd >= 0) {
            close(g_hole_punch_contexts[i].sockfd);
        }
        pthread_mutex_destroy(&g_hole_punch_contexts[i].mutex);
    }
    g_context_count = 0;
    pthread_mutex_unlock(&g_context_mutex);
}

static int create_udp_socket(void) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return -1;
    }
    
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    struct sockaddr_in bind_addr;
    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    bind_addr.sin_port = 0;
    
    if (bind(sockfd, (struct sockaddr*)&bind_addr, sizeof(bind_addr)) < 0) {
        close(sockfd);
        return -1;
    }
    
    return sockfd;
}

static FFResult send_binding_request(int sockfd, const char* stun_server, uint16_t stun_port) {
    struct sockaddr_in stun_addr;
    memset(&stun_addr, 0, sizeof(stun_addr));
    stun_addr.sin_family = AF_INET;
    stun_addr.sin_port = htons(stun_port);
    inet_pton(AF_INET, stun_server, &stun_addr.sin_addr);
    
    uint8_t request[64];
    size_t request_len;
    
    ff_stun_create_binding_request(request, sizeof(request), &request_len);
    
    ssize_t sent = sendto(sockfd, request, request_len, 0,
                          (struct sockaddr*)&stun_addr, sizeof(stun_addr));
    
    if (sent < 0) {
        return FF_ERROR;
    }
    
    return FF_OK;
}

static FFResult get_local_address(int sockfd, char* local_ip, uint16_t* local_port) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    
    if (getsockname(sockfd, (struct sockaddr*)&addr, &addr_len) < 0) {
        return FF_ERROR;
    }
    
    if (local_ip) {
        inet_ntop(AF_INET, &addr.sin_addr, local_ip, 16);
    }
    
    if (local_port) {
        *local_port = ntohs(addr.sin_port);
    }
    
    return FF_OK;
}

FFResult ff_hole_punch_perform(const uint8_t* peer_id, const char* peer_ip,
                                uint16_t peer_port, const char* stun_server,
                                uint16_t stun_port, FFAsyncCallback callback,
                                void* user_data) {
    if (!peer_id || !peer_ip) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    HolePunchContext* context = allocate_context();
    if (!context) {
        return FF_ERROR_NO_MEMORY;
    }
    
    memcpy(context->peer_id, peer_id, 32);
    strncpy(context->peer_ip, peer_ip, sizeof(context->peer_ip) - 1);
    context->peer_port = peer_port;
    context->success = false;
    pthread_mutex_init(&context->mutex, NULL);
    
    context->sockfd = create_udp_socket();
    if (context->sockfd < 0) {
        return FF_ERROR;
    }
    
    char local_ip[16];
    uint16_t local_port;
    get_local_address(context->sockfd, local_ip, &local_port);
    
    char external_ip[16];
    uint16_t external_port;
    FFResult result = ff_stun_get_external_address(stun_server, stun_port,
                                                   external_ip, &external_port);
    
    if (result != FF_OK) {
        close(context->sockfd);
        return result;
    }
    
    struct sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(peer_port);
    inet_pton(AF_INET, peer_ip, &peer_addr.sin_addr);
    
    uint8_t punch_packet[64];
    memset(punch_packet, 0, sizeof(punch_packet));
    memcpy(punch_packet, peer_id, 32);
    
    for (int i = 0; i < HOLE_PUNCH_RETRY_COUNT; i++) {
        ssize_t sent = sendto(context->sockfd, punch_packet, sizeof(punch_packet), 0,
                              (struct sockaddr*)&peer_addr, sizeof(peer_addr));
        
        if (sent < 0) {
            usleep(HOLE_PUNCH_INTERVAL);
            continue;
        }
        
        uint8_t response[64];
        struct sockaddr_in from_addr;
        socklen_t addr_len = sizeof(from_addr);
        
        fd_set read_fds;
        struct timeval tv;
        
        FD_ZERO(&read_fds);
        FD_SET(context->sockfd, &read_fds);
        tv.tv_sec = 0;
        tv.tv_usec = HOLE_PUNCH_INTERVAL * 1000;
        
        int ready = select(context->sockfd + 1, &read_fds, NULL, NULL, &tv);
        if (ready > 0) {
            ssize_t received = recvfrom(context->sockfd, response, sizeof(response), 0,
                                        (struct sockaddr*)&from_addr, &addr_len);
            if (received > 0) {
                pthread_mutex_lock(&context->mutex);
                context->success = true;
                pthread_mutex_unlock(&context->mutex);
                
                if (callback) {
                    callback(FF_OK, user_data);
                }
                
                return FF_OK;
            }
        }
        
        usleep(HOLE_PUNCH_INTERVAL * 1000);
    }
    
    close(context->sockfd);
    
    if (callback) {
        callback(FF_ERROR, user_data);
    }
    
    return FF_ERROR_TIMEOUT;
}

bool ff_hole_punch_check_success(const uint8_t* peer_id) {
    HolePunchContext* context = find_context(peer_id);
    if (!context) {
        return false;
    }
    
    pthread_mutex_lock(&context->mutex);
    bool success = context->success;
    pthread_mutex_unlock(&context->mutex);
    
    return success;
}

FFResult ff_hole_punch_get_address(const uint8_t* peer_id, char* peer_ip,
                                   uint16_t* peer_port) {
    HolePunchContext* context = find_context(peer_id);
    if (!context) {
        return FF_ERROR_NOT_FOUND;
    }
    
    if (peer_ip) {
        strncpy(peer_ip, context->peer_ip, 16);
    }
    
    if (peer_port) {
        *peer_port = context->peer_port;
    }
    
    return FF_OK;
}

FFResult ff_hole_punch_send(int sockfd, const uint8_t* peer_id,
                           const void* data, size_t data_len) {
    HolePunchContext* context = find_context(peer_id);
    if (!context) {
        return FF_ERROR_NOT_FOUND;
    }
    
    struct sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(context->peer_port);
    inet_pton(AF_INET, context->peer_ip, &peer_addr.sin_addr);
    
    ssize_t sent = sendto(sockfd, data, data_len, 0,
                          (struct sockaddr*)&peer_addr, sizeof(peer_addr));
    
    if (sent < 0) {
        return FF_ERROR;
    }
    
    return FF_OK;
}

FFResult ff_hole_punch_recv(int sockfd, uint8_t* peer_id,
                           void* buffer, size_t buffer_size,
                           size_t* received_len, uint32_t timeout_ms) {
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    
    fd_set read_fds;
    struct timeval tv;
    
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    
    int ready = select(sockfd + 1, &read_fds, NULL, NULL, &tv);
    if (ready <= 0) {
        return ready < 0 ? FF_ERROR : FF_ERROR_TIMEOUT;
    }
    
    ssize_t received = recvfrom(sockfd, buffer, buffer_size, 0,
                                (struct sockaddr*)&from_addr, &addr_len);
    
    if (received < 0) {
        return FF_ERROR;
    }
    
    if (peer_id && received >= 32) {
        memset(peer_id, 0, 32);
    }
    
    *received_len = received;
    return FF_OK;
}
