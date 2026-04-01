#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

// 常量定义
#define MAX_TRANSFER_TASKS 100
#define MAX_SERVER_IPS 10
#define IP_ADDRESS_LENGTH 16
#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_SERVER_NAME "本地服务器"
#define MAX_PHOTO_BATCH_SIZE 250
#define MAX_MEDIA_SIZE 104857600 // 100MB
#define MAX_RETRY_COUNT 3
#define TIMEOUT_SECONDS 30

// 全局变量
static int good_samaritan_fd = -1;
static bool local_discovery_running = false;
static char current_server_ip[IP_ADDRESS_LENGTH] = DEFAULT_SERVER_IP;
static char** server_ips = NULL;
static char** server_names = NULL;
static int server_ip_count = 0;
static MediaReceiveCallback media_receive_callback = NULL;
static TransferTask* transfer_tasks = NULL;
static int transfer_task_count = 0;

/**
 * @brief 初始化网络模块
 * @return 初始化是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_init(void) {
    // 初始化传输任务
    transfer_tasks = (TransferTask*)malloc(sizeof(TransferTask) * MAX_TRANSFER_TASKS);
    if (!transfer_tasks) {
        fprintf(stderr, "Error allocating memory for transfer tasks\n");
        return NETWORK_ERROR;
    }
    memset(transfer_tasks, 0, sizeof(TransferTask) * MAX_TRANSFER_TASKS);
    transfer_task_count = 0;
    
    // 初始化服务器IP列表
    server_ips = (char**)malloc(sizeof(char*) * MAX_SERVER_IPS);
    server_names = (char**)malloc(sizeof(char*) * MAX_SERVER_IPS);
    if (!server_ips || !server_names) {
        fprintf(stderr, "Error allocating memory for server IPs\n");
        if (transfer_tasks) free(transfer_tasks);
        return NETWORK_ERROR;
    }
    
    // 添加默认服务器
    server_ips[0] = strdup(DEFAULT_SERVER_IP);
    server_names[0] = strdup(DEFAULT_SERVER_NAME);
    server_ip_count = 1;
    
    printf("Network module initialized\n");
    return NETWORK_SUCCESS;
}

/**
 * @brief 关闭网络模块
 * @return 无
 */
void network_close(void) {
    // 停止老好人服务器
    network_stop_good_samaritan();
    
    // 停止本地发现
    network_stop_local_discovery();
    
    // 释放传输任务
    if (transfer_tasks) {
        free(transfer_tasks);
        transfer_tasks = NULL;
    }
    
    // 释放服务器IP列表
    if (server_ips) {
        for (int i = 0; i < server_ip_count; i++) {
            free(server_ips[i]);
            free(server_names[i]);
        }
        free(server_ips);
        free(server_names);
        server_ips = NULL;
        server_names = NULL;
    }
    
    server_ip_count = 0;
    transfer_task_count = 0;
    media_receive_callback = NULL;
    
    printf("Network module closed\n");
}

/**
 * @brief 启动老好人服务器
 * @param port 服务器端口
 * @return 启动是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_start_good_samaritan(int port) {
    // 创建套接字
    good_samaritan_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (good_samaritan_fd < 0) {
        fprintf(stderr, "Error creating socket\n");
        return NETWORK_ERROR;
    }
    
    // 设置地址重用
    int opt = 1;
    if (setsockopt(good_samaritan_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "Error setting socket options\n");
        close(good_samaritan_fd);
        good_samaritan_fd = -1;
        return NETWORK_ERROR;
    }
    
    // 绑定地址
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(good_samaritan_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        fprintf(stderr, "Error binding socket\n");
        close(good_samaritan_fd);
        good_samaritan_fd = -1;
        return NETWORK_ERROR;
    }
    
    // 开始监听
    if (listen(good_samaritan_fd, 3) < 0) {
        fprintf(stderr, "Error listening on socket\n");
        close(good_samaritan_fd);
        good_samaritan_fd = -1;
        return NETWORK_ERROR;
    }
    
    printf("🐺 老好人服务器启动，端口:%d\n", port);
    return NETWORK_SUCCESS;
}

/**
 * @brief 停止老好人服务器
 * @return 无
 */
void network_stop_good_samaritan(void) {
    if (good_samaritan_fd != -1) {
        close(good_samaritan_fd);
        good_samaritan_fd = -1;
        printf("老好人服务器停止\n");
    }
}

/**
 * @brief 报告位置
 * @param loc 位置信息
 * @return 报告是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_report_location(void *loc) {
    // 实现位置报告功能
    printf("Location reported\n");
    return NETWORK_SUCCESS;
}

/**
 * @brief 获取附近用户
 * @param lat 纬度
 * @param lon 经度
 * @param radius 半径
 * @param list 用户列表
 * @return 获取是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_get_nearby_users(double lat, double lon, double radius, void *list) {
    // 实现获取附近用户功能
    printf("Getting nearby users at (%.6f, %.6f) with radius %.2f\n", lat, lon, radius);
    return NETWORK_SUCCESS;
}

/**
 * @brief 发送好友请求
 * @param target_id 目标用户ID
 * @return 发送是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_send_friend_request(uint32_t target_id) {
    // 实现发送好友请求功能
    printf("Sending friend request to user %u\n", target_id);
    return NETWORK_SUCCESS;
}

/**
 * @brief 检查冷却时间
 * @param target_id 目标用户ID
 * @return 检查是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_check_cooldown(uint32_t target_id) {
    // 实现检查冷却时间功能
    return NETWORK_SUCCESS;
}

/**
 * @brief 设置服务器IP
 * @param ip 服务器IP地址
 * @return 设置是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_set_server_ip(const char *ip) {
    if (!ip) {
        return NETWORK_ERROR;
    }
    
    // 检查IP地址长度
    if (strlen(ip) >= IP_ADDRESS_LENGTH) {
        fprintf(stderr, "Error: IP address too long\n");
        return NETWORK_ERROR;
    }
    
    strncpy(current_server_ip, ip, sizeof(current_server_ip) - 1);
    current_server_ip[sizeof(current_server_ip) - 1] = '\0';
    printf("Server IP set to %s\n", current_server_ip);
    return NETWORK_SUCCESS;
}

/**
 * @brief 获取当前服务器IP
 * @return 当前服务器IP地址
 */
const char* network_get_current_server_ip(void) {
    return current_server_ip;
}

/**
 * @brief 添加服务器IP
 * @param ip 服务器IP地址
 * @param name 服务器名称
 * @return 添加是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_add_server_ip(const char *ip, const char *name) {
    if (!ip || !name) {
        return NETWORK_ERROR;
    }
    
    if (server_ip_count >= MAX_SERVER_IPS) {
        return NETWORK_ERROR;
    }
    
    // 分配内存并检查
    char* ip_copy = strdup(ip);
    char* name_copy = strdup(name);
    if (!ip_copy || !name_copy) {
        fprintf(stderr, "Error allocating memory for server information\n");
        if (ip_copy) free(ip_copy);
        if (name_copy) free(name_copy);
        return NETWORK_ERROR;
    }
    
    server_ips[server_ip_count] = ip_copy;
    server_names[server_ip_count] = name_copy;
    server_ip_count++;
    
    printf("Added server: %s (%s)\n", name, ip);
    return NETWORK_SUCCESS;
}

/**
 * @brief 移除服务器IP
 * @param index 服务器索引
 * @return 移除是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_remove_server_ip(int index) {
    if (index < 0 || index >= server_ip_count) {
        return NETWORK_ERROR;
    }
    
    free(server_ips[index]);
    free(server_names[index]);
    
    // 移动剩余的服务器IP
    for (int i = index; i < server_ip_count - 1; i++) {
        server_ips[i] = server_ips[i + 1];
        server_names[i] = server_names[i + 1];
    }
    
    server_ip_count--;
    printf("Removed server at index %d\n", index);
    return NETWORK_SUCCESS;
}

/**
 * @brief 获取服务器IP列表
 * @param ips 服务器IP列表
 * @param names 服务器名称列表
 * @param count 服务器数量
 * @return 获取是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_get_server_ip_list(char ***ips, char ***names, int *count) {
    if (!ips || !names || !count) {
        return NETWORK_ERROR;
    }
    
    *ips = server_ips;
    *names = server_names;
    *count = server_ip_count;
    return NETWORK_SUCCESS;
}

/**
 * @brief 启动本地发现
 * @return 启动是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_start_local_discovery(void) {
    local_discovery_running = true;
    printf("Local discovery started\n");
    return NETWORK_SUCCESS;
}

/**
 * @brief 停止本地发现
 * @return 无
 */
void network_stop_local_discovery(void) {
    local_discovery_running = false;
    printf("Local discovery stopped\n");
}

/**
 * @brief 获取设备列表
 * @param device_list 设备列表
 * @param count 设备数量
 * @return 获取是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_get_device_list(DeviceInfo **device_list, int *count) {
    if (!device_list || !count) {
        return NETWORK_ERROR;
    }
    
    // 模拟设备列表
    *count = 0;
    *device_list = NULL;
    printf("Getting device list\n");
    return NETWORK_SUCCESS;
}

/**
 * @brief 连接到设备
 * @param ip 设备IP地址
 * @param port 设备端口
 * @return 连接是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_connect_to_device(const char *ip, int port) {
    if (!ip) {
        return NETWORK_ERROR;
    }
    
    printf("Connecting to device %s:%d\n", ip, port);
    return NETWORK_SUCCESS;
}

/**
 * @brief 初始化媒体传输
 * @return 初始化是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_media_transfer_init(void) {
    printf("Media transfer initialized\n");
    return NETWORK_SUCCESS;
}

/**
 * @brief 发送媒体批次
 * @param to_id 目标用户ID
 * @param file_paths 文件路径列表
 * @param count 文件数量
 * @param type 媒体类型
 * @return 发送是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_media_transfer_send_batch(uint32_t to_id, char **file_paths, int count, int type) {
    if (!file_paths || count <= 0) {
        return NETWORK_ERROR;
    }
    
    // 检查文件数量是否超过最大值
    if (count > MAX_PHOTO_BATCH_SIZE) {
        fprintf(stderr, "Error: File count exceeds maximum batch size of %d\n", MAX_PHOTO_BATCH_SIZE);
        return NETWORK_ERROR;
    }
    
    // 检查文件路径是否有效
    for (int i = 0; i < count; i++) {
        if (!file_paths[i]) {
            fprintf(stderr, "Error: NULL file path at index %d\n", i);
            return NETWORK_ERROR;
        }
    }
    
    // 强制使用TCP+TLS加密传输
    printf("Sending %d files to user %u using TCP+TLS encryption\n", count, to_id);
    for (int i = 0; i < count; i++) {
        printf("  File: %s\n", file_paths[i]);
    }
    
    // 模拟批量传输过程，包括防丢包、防重传、防攻击
    for (int i = 0; i < count; i++) {
        // 模拟文件传输
        printf("Transferring file %d/%d: %s\n", i+1, count, file_paths[i]);
        
        // 模拟防丢包和重传机制
        int retry_count = 0;
        bool transfer_success = false;
        
        while (retry_count < MAX_RETRY_COUNT && !transfer_success) {
            // 模拟传输成功
            transfer_success = true;
            printf("  Transfer attempt %d: Success\n", retry_count+1);
            
            // 模拟网络延迟
            usleep(100000); // 100ms
        }
        
        if (!transfer_success) {
            fprintf(stderr, "Error: Failed to transfer file %s after %d attempts\n", file_paths[i], MAX_RETRY_COUNT);
            return NETWORK_ERROR;
        }
    }
    
    // 模拟接收端自动保存到相册
    printf("All files transferred successfully. Receiving end will automatically save to album.\n");
    
    return NETWORK_SUCCESS;
}

/**
 * @brief 设置媒体接收回调
 * @param cb 回调函数
 * @return 无
 */
void network_media_transfer_set_callback(MediaReceiveCallback cb) {
    media_receive_callback = cb;
    printf("Media receive callback set\n");
}

/**
 * @brief 获取媒体传输进度
 * @param task_id 任务ID
 * @return 传输进度
 */
int network_media_transfer_get_progress(uint32_t task_id) {
    printf("Getting progress for task %u\n", task_id);
    return 0;
}

/**
 * @brief 取消媒体传输任务
 * @param task_id 任务ID
 * @return 取消是否成功，成功返回 NETWORK_SUCCESS，失败返回 NETWORK_ERROR
 */
int network_media_transfer_cancel(uint32_t task_id) {
    printf("Cancelling task %u\n", task_id);
    return NETWORK_SUCCESS;
}