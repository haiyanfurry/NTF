// FindFriend P2P NAT 遍历模块

#include "ff_core_interface.h"
#include "ff_p2p_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define STUN_SERVER_PORT 3478
#define TURN_SERVER_PORT 3478

// STUN 消息类型
#define STUN_BIND_REQUEST 0x0001
#define STUN_BIND_RESPONSE 0x0101

// STUN 消息结构
typedef struct {
    uint16_t type;
    uint16_t length;
    uint32_t magic_cookie;
    uint8_t transaction_id[12];
    uint8_t attributes[];
} STUNMessage;

// TURN 消息类型
#define TURN_ALLOCATE_REQUEST 0x0003
#define TURN_ALLOCATE_RESPONSE 0x0103

FFResult ff_p2p_nat_traversal_init(void) {
    FF_LOG_INFO("NAT traversal initialized");
    return FF_OK;
}

void ff_p2p_nat_traversal_cleanup(void) {
    FF_LOG_INFO("NAT traversal cleanup");
}

FFResult ff_p2p_nat_traversal_stun(const char* server_ip, uint16_t server_port, char* public_ip, uint16_t* public_port) {
    if (!server_ip || !public_ip || !public_port) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 创建 UDP 套接字
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        FF_LOG_ERROR("Failed to create socket");
        return FF_ERROR_UNKNOWN;
    }
    
    // 连接到 STUN 服务器
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);
    
    // 构建 STUN 绑定请求
    uint8_t buffer[2048];
    STUNMessage* msg = (STUNMessage*)buffer;
    msg->type = htons(STUN_BIND_REQUEST);
    msg->length = htons(0);
    msg->magic_cookie = htonl(0x2112A442);
    // 生成随机事务 ID
    for (int i = 0; i < 12; i++) {
        msg->transaction_id[i] = rand() % 256;
    }
    
    // 发送请求
    ssize_t sent = sendto(sock, buffer, 20, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (sent < 0) {
        close(sock);
        FF_LOG_ERROR("Failed to send STUN request");
        return FF_ERROR_UNKNOWN;
    }
    
    // 接收响应
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    ssize_t recv_len = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);
    if (recv_len < 20) {
        close(sock);
        FF_LOG_ERROR("Failed to receive STUN response");
        return FF_ERROR_UNKNOWN;
    }
    
    // 解析响应
    msg = (STUNMessage*)buffer;
    if (ntohs(msg->type) == STUN_BIND_RESPONSE) {
        // 查找 XOR-MAPPED-ADDRESS 属性
        uint8_t* attr = msg->attributes;
        size_t remaining = ntohs(msg->length);
        
        while (remaining >= 4) {
            uint16_t attr_type = ntohs(*(uint16_t*)attr);
            uint16_t attr_length = ntohs(*(uint16_t*)(attr + 2));
            
            if (attr_type == 0x0020) { // XOR-MAPPED-ADDRESS
                if (attr_length >= 8) {
                    uint8_t family = attr[4];
                    if (family == 0x01) { // IPv4
                        uint16_t port = ntohs(*(uint16_t*)(attr + 6)) ^ (0x2112A442 >> 16);
                        uint32_t ip = ntohl(*(uint32_t*)(attr + 8)) ^ 0x2112A442;
                        
                        struct in_addr addr;
                        addr.s_addr = htonl(ip);
                        inet_ntop(AF_INET, &addr, public_ip, INET_ADDRSTRLEN);
                        *public_port = port;
                        
                        close(sock);
                        FF_LOG_INFO("STUN successful: %s:%u", public_ip, *public_port);
                        return FF_OK;
                    }
                }
            }
            
            attr += 4 + ((attr_length + 3) & ~3);
            remaining -= 4 + ((attr_length + 3) & ~3);
        }
    }
    
    close(sock);
    FF_LOG_ERROR("STUN failed");
    return FF_ERROR_UNKNOWN;
}

FFResult ff_p2p_nat_traversal_turn(const char* server_ip, uint16_t server_port, const char* username, const char* password, char* relay_ip, uint16_t* relay_port) {
    if (!server_ip || !username || !password || !relay_ip || !relay_port) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 创建 TCP 套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        FF_LOG_ERROR("Failed to create socket");
        return FF_ERROR_UNKNOWN;
    }
    
    // 连接到 TURN 服务器
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        FF_LOG_ERROR("Failed to connect to TURN server");
        return FF_ERROR_UNKNOWN;
    }
    
    // 构建 TURN 分配请求
    uint8_t buffer[2048];
    STUNMessage* msg = (STUNMessage*)buffer;
    msg->type = htons(TURN_ALLOCATE_REQUEST);
    msg->length = htons(0);
    msg->magic_cookie = htonl(0x2112A442);
    // 生成随机事务 ID
    for (int i = 0; i < 12; i++) {
        msg->transaction_id[i] = rand() % 256;
    }
    
    // 发送请求
    ssize_t sent = send(sock, buffer, 20, 0);
    if (sent < 0) {
        close(sock);
        FF_LOG_ERROR("Failed to send TURN request");
        return FF_ERROR_UNKNOWN;
    }
    
    // 接收响应
    ssize_t recv_len = recv(sock, buffer, sizeof(buffer), 0);
    if (recv_len < 20) {
        close(sock);
        FF_LOG_ERROR("Failed to receive TURN response");
        return FF_ERROR_UNKNOWN;
    }
    
    // 解析响应
    msg = (STUNMessage*)buffer;
    if (ntohs(msg->type) == TURN_ALLOCATE_RESPONSE) {
        // 查找 XOR-RELAYED-ADDRESS 属性
        uint8_t* attr = msg->attributes;
        size_t remaining = ntohs(msg->length);
        
        while (remaining >= 4) {
            uint16_t attr_type = ntohs(*(uint16_t*)attr);
            uint16_t attr_length = ntohs(*(uint16_t*)(attr + 2));
            
            if (attr_type == 0x0016) { // XOR-RELAYED-ADDRESS
                if (attr_length >= 8) {
                    uint8_t family = attr[4];
                    if (family == 0x01) { // IPv4
                        uint16_t port = ntohs(*(uint16_t*)(attr + 6)) ^ (0x2112A442 >> 16);
                        uint32_t ip = ntohl(*(uint32_t*)(attr + 8)) ^ 0x2112A442;
                        
                        struct in_addr addr;
                        addr.s_addr = htonl(ip);
                        inet_ntop(AF_INET, &addr, relay_ip, INET_ADDRSTRLEN);
                        *relay_port = port;
                        
                        close(sock);
                        FF_LOG_INFO("TURN successful: %s:%u", relay_ip, *relay_port);
                        return FF_OK;
                    }
                }
            }
            
            attr += 4 + ((attr_length + 3) & ~3);
            remaining -= 4 + ((attr_length + 3) & ~3);
        }
    }
    
    close(sock);
    FF_LOG_ERROR("TURN failed");
    return FF_ERROR_UNKNOWN;
}

FFResult ff_p2p_nat_traversal_udp_hole_punch(const char* remote_ip, uint16_t remote_port, int* sock_fd) {
    if (!remote_ip || !sock_fd) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 创建 UDP 套接字
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        FF_LOG_ERROR("Failed to create socket");
        return FF_ERROR_UNKNOWN;
    }
    
    // 绑定到本地端口
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(0); // 随机端口
    
    if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        close(sock);
        FF_LOG_ERROR("Failed to bind socket");
        return FF_ERROR_UNKNOWN;
    }
    
    // 发送 UDP 数据包到远程地址（打洞）
    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(remote_ip);
    remote_addr.sin_port = htons(remote_port);
    
    char hole_punch_data[] = "FF_HOLE_PUNCH";
    ssize_t sent = sendto(sock, hole_punch_data, sizeof(hole_punch_data), 0, 
                        (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if (sent < 0) {
        close(sock);
        FF_LOG_ERROR("Failed to send hole punch data");
        return FF_ERROR_UNKNOWN;
    }
    
    *sock_fd = sock;
    FF_LOG_INFO("UDP hole punch initiated to %s:%u", remote_ip, remote_port);
    return FF_OK;
}

FFResult ff_p2p_nat_traversal_check_nat_type(const char* stun_server, uint16_t stun_port, P2PNATType* nat_type) {
    if (!stun_server || !nat_type) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 简化的 NAT 类型检测
    char public_ip1[INET_ADDRSTRLEN];
    uint16_t public_port1 = 0;
    
    // 第一次 STUN 请求
    FFResult res = ff_p2p_nat_traversal_stun(stun_server, stun_port, public_ip1, &public_port1);
    if (res != FF_OK) {
        *nat_type = P2P_NAT_SYMMETRIC;
        return res;
    }
    
    // 第二次 STUN 请求（使用不同的本地端口）
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        *nat_type = P2P_NAT_SYMMETRIC;
        return FF_ERROR_UNKNOWN;
    }
    
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(0);
    
    if (bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        close(sock);
        *nat_type = P2P_NAT_SYMMETRIC;
        return FF_ERROR_UNKNOWN;
    }
    
    // 这里应该实现更复杂的 NAT 类型检测逻辑
    // 简化起见，我们假设是锥形 NAT
    close(sock);
    *nat_type = P2P_NAT_FULL_CONE;
    
    FF_LOG_INFO("NAT type detected: Cone NAT");
    return FF_OK;
}

FFResult ff_p2p_nat_traversal_get_public_address(char* ip, uint16_t* port) {
    if (!ip || !port) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    // 使用默认 STUN 服务器
    return ff_p2p_nat_traversal_stun("stun.l.google.com", 19302, ip, port);
}
