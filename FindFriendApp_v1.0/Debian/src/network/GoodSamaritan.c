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
#define MAX_SERVER_IPS 20
#define BROADCAST_PORT 8888
#define BROADCAST_INTERVAL 5

static int running = 0;
static int server_socket = -1;
static pthread_t server_tid;

// 服务器IP管理
static char *server_ips[MAX_SERVER_IPS];
static char *server_names[MAX_SERVER_IPS];
static int server_ip_count = 0;
static char *current_server_ip = NULL;

// 同城用户发现
static int broadcast_socket = -1;
static int discovery_socket = -1;
static pthread_t broadcast_tid;
static pthread_t discovery_tid;

// 发现的设备
#define MAX_DEVICES 50

static DeviceInfo devices[MAX_DEVICES];
static int device_count = 0;

// 函数声明
static void* broadcast_thread_func(void *arg);
static void* discovery_thread_func(void *arg);
static void process_discovery_message(const char *message, const char *ip);
static void update_device(const char *ip, const char *name, const char *type);
static void cleanup_devices(void);

typedef struct {
    uint32_t user_id;
    time_t cooldown_until;
} CooldownEntry;

static CooldownEntry cooldowns[100];
static int cooldown_count = 0;

int check_cooldown(uint32_t target_id) {
    time_t now = time(NULL);
    for (int i = 0; i < cooldown_count; i++) {
        if (cooldowns[i].user_id == target_id) {
            if (now < cooldowns[i].cooldown_until) {
                return 1;
            }
        }
    }
    return 0;
}

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

int send_friend_request(uint32_t target_id) {
    if (check_cooldown(target_id)) {
        return -1;
    }
    add_cooldown(target_id);
    printf("发送好友请求给用户 %u\n", target_id);
    return 0;
}

static void handle_client(int client_fd) {
    uint8_t buffer[4096];
    int n = recv(client_fd, buffer, sizeof(buffer), 0);
    if (n <= 0) {
        close(client_fd);
        return;
    }
    send(client_fd, buffer, n, 0);
    close(client_fd);
}

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
    return 0;
}

int get_nearby_users(double lat, double lon, double radius, void *list) {
    return 0;
}

// IP快速切换功能实现
int set_server_ip(const char *ip) {
    if (ip == NULL) {
        return -1;
    }
    
    if (current_server_ip) {
        free(current_server_ip);
    }
    current_server_ip = strdup(ip);
    printf("服务器IP已切换至: %s\n", current_server_ip);
    return 0;
}

const char* get_current_server_ip(void) {
    return current_server_ip ? current_server_ip : "未设置";
}

int add_server_ip(const char *ip, const char *name) {
    if (ip == NULL || name == NULL) {
        return -1;
    }
    
    if (server_ip_count >= MAX_SERVER_IPS) {
        return -2;
    }
    
    server_ips[server_ip_count] = strdup(ip);
    server_names[server_ip_count] = strdup(name);
    server_ip_count++;
    printf("添加服务器: %s (%s)\n", name, ip);
    return 0;
}

int remove_server_ip(int index) {
    if (index < 0 || index >= server_ip_count) {
        return -1;
    }
    
    free(server_ips[index]);
    free(server_names[index]);
    
    for (int i = index; i < server_ip_count - 1; i++) {
        server_ips[i] = server_ips[i + 1];
        server_names[i] = server_names[i + 1];
    }
    
    server_ip_count--;
    printf("移除服务器索引: %d\n", index);
    return 0;
}

int get_server_ip_list(char ***ips, char ***names, int *count) {
    if (ips == NULL || names == NULL || count == NULL) {
        return -1;
    }
    
    *ips = server_ips;
    *names = server_names;
    *count = server_ip_count;
    return 0;
}

// 同城用户发现功能

// 启动同城用户发现
int start_local_discovery(void) {
    // 启动广播线程
    pthread_create(&broadcast_tid, NULL, broadcast_thread_func, NULL);
    // 启动发现线程
    pthread_create(&discovery_tid, NULL, discovery_thread_func, NULL);
    return 0;
}

// 停止同城用户发现
void stop_local_discovery(void) {
    if (broadcast_socket > 0) {
        close(broadcast_socket);
    }
    if (discovery_socket > 0) {
        close(discovery_socket);
    }
    pthread_cancel(broadcast_tid);
    pthread_cancel(discovery_tid);
    pthread_join(broadcast_tid, NULL);
    pthread_join(discovery_tid, NULL);
}

// 广播线程函数
static void* broadcast_thread_func(void *arg) {
    // 创建广播套接字
    broadcast_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (broadcast_socket < 0) {
        perror("socket");
        return NULL;
    }
    
    // 设置广播选项
    int opt = 1;
    setsockopt(broadcast_socket, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROADCAST_PORT);
    addr.sin_addr.s_addr = INADDR_BROADCAST;
    
    // 广播消息
    char message[256];
    sprintf(message, "FINDFRIEND_DISCOVERY:name=FindFriendApp&type=user&version=1.0");
    
    while (running) {
        sendto(broadcast_socket, message, strlen(message), 0, 
               (struct sockaddr*)&addr, sizeof(addr));
        sleep(BROADCAST_INTERVAL);
    }
    
    close(broadcast_socket);
    return NULL;
}

// 发现线程函数
static void* discovery_thread_func(void *arg) {
    // 创建发现套接字
    discovery_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (discovery_socket < 0) {
        perror("socket");
        return NULL;
    }
    
    // 设置套接字选项
    int opt = 1;
    setsockopt(discovery_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(BROADCAST_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(discovery_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(discovery_socket);
        return NULL;
    }
    
    // 接收广播消息
    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    
    while (running) {
        int n = recvfrom(discovery_socket, buffer, sizeof(buffer), 0, 
                         (struct sockaddr*)&client_addr, &len);
        if (n > 0) {
            buffer[n] = '\0';
            process_discovery_message(buffer, inet_ntoa(client_addr.sin_addr));
        }
    }
    
    close(discovery_socket);
    return NULL;
}

// 处理发现消息
static void process_discovery_message(const char *message, const char *ip) {
    // 解析消息
    char name[64] = "";
    char type[32] = "";
    
    char *token = strtok((char*)message, "&");
    while (token) {
        if (strstr(token, "name=") == token) {
            strcpy(name, token + 5);
        } else if (strstr(token, "type=") == token) {
            strcpy(type, token + 5);
        }
        token = strtok(NULL, "&");
    }
    
    // 更新设备列表
    update_device(ip, name, type);
}

// 更新设备信息
static void update_device(const char *ip, const char *name, const char *type) {
    time_t now = time(NULL);
    
    // 检查是否已存在
    for (int i = 0; i < device_count; i++) {
        if (strcmp(devices[i].ip, ip) == 0) {
            // 更新现有设备
            strcpy(devices[i].name, name);
            strcpy(devices[i].type, type);
            devices[i].last_seen = now;
            return;
        }
    }
    
    // 添加新设备
    if (device_count < MAX_DEVICES) {
        strcpy(devices[device_count].ip, ip);
        strcpy(devices[device_count].name, name);
        strcpy(devices[device_count].type, type);
        devices[device_count].last_seen = now;
        device_count++;
        printf("发现新设备: %s (%s) - %s\n", name, type, ip);
    }
    
    // 清理超时设备
    cleanup_devices();
}

// 清理超时设备
static void cleanup_devices(void) {
    time_t now = time(NULL);
    int i = 0;
    while (i < device_count) {
        if (now - devices[i].last_seen > 30) { // 30秒超时
            // 移除超时设备
            for (int j = i; j < device_count - 1; j++) {
                devices[j] = devices[j + 1];
            }
            device_count--;
        } else {
            i++;
        }
    }
}

// 获取设备列表
int get_device_list(DeviceInfo **device_list, int *count) {
    if (device_list == NULL || count == NULL) {
        return -1;
    }
    
    *device_list = devices;
    *count = device_count;
    return 0;
}

// 连接到设备
int connect_to_device(const char *ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }
    
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }
    
    printf("成功连接到设备: %s:%d\n", ip, port);
    return sock;
}
