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
#include <math.h>

#define MAX_USERS 1000
#define USER_TIMEOUT 300

static UserLocation users[MAX_USERS];
static int user_count = 0;
static pthread_mutex_t users_mutex = PTHREAD_MUTEX_INITIALIZER;
static int server_socket = -1;
static int running = 0;

static double distance_km(double lat1, double lon1, double lat2, double lon2) {
    double R = 6371;
    double dlat = (lat2 - lat1) * M_PI / 180;
    double dlon = (lon2 - lon1) * M_PI / 180;
    double a = sin(dlat/2) * sin(dlat/2) +
               cos(lat1 * M_PI / 180) * cos(lat2 * M_PI / 180) *
               sin(dlon/2) * sin(dlon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return R * c;
}

static void cleanup_timeout_users(void) {
    time_t now = time(NULL);
    pthread_mutex_lock(&users_mutex);
    for (int i = 0; i < user_count; i++) {
        if (now - users[i].last_seen > USER_TIMEOUT) {
            for (int j = i; j < user_count - 1; j++) {
                users[j] = users[j+1];
            }
            user_count--;
            i--;
        }
    }
    pthread_mutex_unlock(&users_mutex);
}

static void handle_client(int client_fd) {
    uint8_t buffer[4096];
    int n = recv(client_fd, buffer, sizeof(buffer), 0);
    if (n <= 0) {
        close(client_fd);
        return;
    }
    
    uint8_t cmd = buffer[0];
    
    if (cmd == 0x01) {
        UserLocation *loc = (UserLocation*)(buffer + 1);
        loc->last_seen = time(NULL);
        
        pthread_mutex_lock(&users_mutex);
        int found = -1;
        for (int i = 0; i < user_count; i++) {
            if (users[i].user_id == loc->user_id) {
                found = i;
                break;
            }
        }
        
        if (found >= 0) {
            users[found] = *loc;
        } else if (user_count < MAX_USERS) {
            users[user_count++] = *loc;
        }
        pthread_mutex_unlock(&users_mutex);
        
        uint8_t resp = 0x01;
        send(client_fd, &resp, 1, 0);
        
    } else if (cmd == 0x02) {
        double lat, lon, radius;
        memcpy(&lat, buffer + 1, 8);
        memcpy(&lon, buffer + 9, 8);
        memcpy(&radius, buffer + 17, 8);
        
        NearbyResponse resp;
        resp.count = 0;
        
        pthread_mutex_lock(&users_mutex);
        cleanup_timeout_users();
        
        for (int i = 0; i < user_count; i++) {
            double dist = distance_km(lat, lon, users[i].latitude, users[i].longitude);
            if (dist <= radius && users[i].user_id != 0) {
                resp.users[resp.count++] = users[i];
                if (resp.count >= 50) break;
            }
        }
        pthread_mutex_unlock(&users_mutex);
        
        send(client_fd, &resp, sizeof(resp), 0);
    }
    
    close(client_fd);
}

static void* server_thread(void *arg) {
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

static pthread_t server_tid;

int start_good_samaritan(int port) {
    pthread_create(&server_tid, NULL, server_thread, &port);
    return 0;
}

void stop_good_samaritan(void) {
    running = 0;
    pthread_join(server_tid, NULL);
}

int report_location(UserLocation *loc) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }
    
    uint8_t buffer[sizeof(UserLocation) + 1];
    buffer[0] = 0x01;
    memcpy(buffer + 1, loc, sizeof(UserLocation));
    send(sock, buffer, sizeof(buffer), 0);
    
    uint8_t resp;
    recv(sock, &resp, 1, 0);
    close(sock);
    return resp == 0x01 ? 0 : -1;
}

int get_nearby_users(double lat, double lon, double radius, NearbyResponse *resp) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }
    
    uint8_t buffer[25];
    buffer[0] = 0x02;
    memcpy(buffer + 1, &lat, 8);
    memcpy(buffer + 9, &lon, 8);
    memcpy(buffer + 17, &radius, 8);
    send(sock, buffer, 25, 0);
    
    recv(sock, resp, sizeof(NearbyResponse), 0);
    close(sock);
    return resp->count;
}
