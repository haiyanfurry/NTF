#include "GoodSamaritan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_USERS 1000
#define USER_TIMEOUT 300
#define COOLDOWN_SECONDS 60

static int running = 0;
static int server_socket = -1;
static pthread_t server_tid;

// 简单的冷却记录
typedef struct {
    uint32_t user_id;
    time_t cooldown_until;
} CooldownEntry;

static CooldownEntry cooldowns[100];
static int cooldown_count = 0;

// 检查冷却
int check_cooldown(uint32_t target_id) {
    time_t now = time(NULL);
    for (int i = 0; i < cooldown_count; i++) {
        if (cooldowns[i].user_id == target_id) {
            if (now < cooldowns[i].cooldown_until) {
                return 1; // 还在冷却中
            }
        }
    }
    return 0;
}

// 添加冷却
static void add_cooldown(uint32_t target_id) {
    time_t now = time(NULL);
    for (int i = 0; i < cooldown_count; i++) {
        if (cooldowns[i].user_id == target_id) {
            cooldowns[i].cooldown_until = now + COOLDOWN_SECONDS;
            return;
        }
    }
    if (cooldown_count < 100) {
        cooldowns[cooldown_count].user_id = target_id;
        cooldowns[cooldown_count].cooldown_until = now + COOLDOWN_SECONDS;
        cooldown_count++;
    }
}

// 发送好友请求
int send_friend_request(uint32_t target_id) {
    if (check_cooldown(target_id)) {
        return -1;
    }
    add_cooldown(target_id);
    // TODO: 实际发送网络请求
    printf("发送好友请求给用户 %u\n", target_id);
    return 0;
}

// 处理客户端
static void handle_client(int client_fd) {
    uint8_t buffer[4096];
    int n = recv(client_fd, buffer, sizeof(buffer), 0);
    if (n <= 0) {
        close(client_fd);
        return;
    }
    // 简单回显
    send(client_fd, buffer, n, 0);
    close(client_fd);
}

// 服务器线程
static void* server_thread_func(void *arg) {
    int port = *(int*)arg;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(server_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return NULL;
    }
    
    listen(server_socket, 10);
    running = 1;
    
    printf("🐺 老好人服务器已启动，端口: %d\n", port);
    printf("📡 等待用户连接...\n");
    
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(server_socket, (struct sockaddr*)&client_addr, &len);
        
        if (client_fd > 0) {
            handle_client(client_fd);
        }
    }
    
    close(server_socket);
    return NULL;
}

int start_good_samaritan(int port) {
    pthread_create(&server_tid, NULL, server_thread_func, &port);
    return 0;
}

void stop_good_samaritan(void) {
    running = 0;
    if (server_socket > 0) {
        close(server_socket);
    }
    pthread_join(server_tid, NULL);
}

int report_location(void *loc) {
    // 占位实现
    return 0;
}

int get_nearby_users(double lat, double lon, double radius, void *list) {
    // 占位实现，返回空列表
    return 0;
}
