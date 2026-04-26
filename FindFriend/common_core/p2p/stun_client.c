// FindFriend P2P STUN 客户端实现

#include "ff_p2p_stun.h"
#include "ff_core_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define STUN_HEADER_SIZE 20
#define STUN_MAGIC_COOKIE 0x2112A442
#define STUN_BINDING_REQUEST 0x0001
#define STUN_BINDING_RESPONSE 0x0101
#define STUN_BINDING_ERROR 0x0111
#define STUN_ATTR_MAPPED_ADDRESS 0x0001
#define STUN_ATTR_XOR_MAPPED_ADDRESS 0x0020
#define STUN_ATTR_CHANGE_REQUEST 0x0003

static uint8_t g_stun_transaction_id[12];

static uint16_t stun_calculate_attribute_length(uint16_t value_len) {
    return (value_len + 4) & ~0x3;
}

static void stun_generate_transaction_id(uint8_t* tid) {
    for (int i = 0; i < 12; i++) {
        tid[i] = (uint8_t)(rand() & 0xFF);
    }
}

static uint32_t stun_htonl(uint32_t x) {
    return ((x & 0x000000FF) << 24) |
           ((x & 0x0000FF00) << 8) |
           ((x & 0x00FF0000) >> 8) |
           ((x & 0xFF000000) >> 24);
}

static uint16_t stun_htons(uint16_t x) {
    return ((x & 0x00FF) << 8) | ((x & 0xFF00) >> 8);
}

FFResult ff_stun_create_binding_request(uint8_t* buffer, size_t buffer_size, size_t* out_len) {
    if (!buffer || buffer_size < STUN_HEADER_SIZE + 12) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    uint16_t msg_type = stun_htons(STUN_BINDING_REQUEST);
    uint32_t magic_cookie = stun_htonl(STUN_MAGIC_COOKIE);
    
    memcpy(buffer, &msg_type, 2);
    memcpy(buffer + 2, "\x00\x00", 2);
    
    memcpy(buffer + 4, &magic_cookie, 4);
    
    stun_generate_transaction_id(g_stun_transaction_id);
    memcpy(buffer + 8, g_stun_transaction_id, 12);
    
    *out_len = STUN_HEADER_SIZE;
    return FF_OK;
}

FFResult ff_stun_parse_response(const uint8_t* buffer, size_t buffer_len,
                                char* mapped_ip, uint16_t* mapped_port) {
    if (!buffer || buffer_len < STUN_HEADER_SIZE) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    uint16_t msg_type = (buffer[0] << 8) | buffer[1];
    uint16_t msg_length = (buffer[2] << 8) | buffer[3];
    
    if (msg_type != stun_htons(STUN_BINDING_RESPONSE)) {
        return FF_ERROR;
    }
    
    uint32_t magic_cookie;
    memcpy(&magic_cookie, buffer + 4, 4);
    magic_cookie = stun_ntohl(magic_cookie);
    
    if (magic_cookie != STUN_MAGIC_COOKIE) {
        return FF_ERROR;
    }
    
    uint8_t transaction_id[12];
    memcpy(transaction_id, buffer + 8, 12);
    
    if (memcmp(transaction_id, g_stun_transaction_id, 12) != 0) {
        return FF_ERROR;
    }
    
    size_t pos = STUN_HEADER_SIZE;
    while (pos < buffer_len && pos < STUN_HEADER_SIZE + msg_length) {
        if (pos + 4 > buffer_len) {
            break;
        }
        
        uint16_t attr_type = (buffer[pos] << 8) | buffer[pos + 1];
        uint16_t attr_length = (buffer[pos + 2] << 8) | buffer[pos + 3];
        
        if (pos + 4 + attr_length > buffer_len) {
            break;
        }
        
        if (attr_type == stun_htons(STUN_ATTR_XOR_MAPPED_ADDRESS)) {
            uint8_t* attr_data = (uint8_t*)&buffer[pos + 4];
            
            uint8_t family = attr_data[1];
            uint16_t port = attr_data[2] << 8 | attr_data[3];
            port ^= (STUN_MAGIC_COOKIE >> 16) & 0xFFFF;
            
            uint32_t ip = attr_data[4] << 24 | attr_data[5] << 16 |
                         attr_data[6] << 8 | attr_data[7];
            ip ^= STUN_MAGIC_COOKIE;
            
            if (mapped_ip) {
                struct in_addr addr;
                addr.s_addr = ip;
                strncpy(mapped_ip, inet_ntoa(addr), 16);
            }
            
            if (mapped_port) {
                *mapped_port = port;
            }
            
            return FF_OK;
        } else if (attr_type == stun_htons(STUN_ATTR_MAPPED_ADDRESS)) {
            uint8_t* attr_data = (uint8_t*)&buffer[pos + 4];
            
            uint8_t family = attr_data[1];
            uint16_t port = (attr_data[2] << 8) | attr_data[3];
            
            uint32_t ip = attr_data[4] << 24 | attr_data[5] << 16 |
                         attr_data[6] << 8 | attr_data[7];
            
            if (mapped_ip) {
                struct in_addr addr;
                addr.s_addr = ip;
                strncpy(mapped_ip, inet_ntoa(addr), 16);
            }
            
            if (mapped_port) {
                *mapped_port = port;
            }
            
            return FF_OK;
        }
        
        pos += 4 + stun_calculate_attribute_length(attr_length);
    }
    
    return FF_ERROR;
}

FFResult ff_stun_send_request(int sockfd, const char* stun_server, uint16_t stun_port) {
    if (!stun_server) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    struct sockaddr_in stun_addr;
    memset(&stun_addr, 0, sizeof(stun_addr));
    stun_addr.sin_family = AF_INET;
    stun_addr.sin_port = htons(stun_port);
    inet_pton(AF_INET, stun_server, &stun_addr.sin_addr);
    
    uint8_t request[STUN_HEADER_SIZE + 12];
    size_t request_len;
    
    FFResult result = ff_stun_create_binding_request(request, sizeof(request), &request_len);
    if (result != FF_OK) {
        return result;
    }
    
    ssize_t sent = sendto(sockfd, request, request_len, 0,
                          (struct sockaddr*)&stun_addr, sizeof(stun_addr));
    
    if (sent < 0) {
        return FF_ERROR;
    }
    
    return FF_OK;
}

FFResult ff_stun_recv_response(int sockfd, char* mapped_ip, uint16_t* mapped_port, uint32_t timeout_ms) {
    if (!mapped_ip || !mapped_port) {
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
    
    uint8_t response[256];
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    
    ssize_t received = recvfrom(sockfd, response, sizeof(response), 0,
                                (struct sockaddr*)&from_addr, &addr_len);
    
    if (received < 0) {
        return FF_ERROR;
    }
    
    return ff_stun_parse_response(response, received, mapped_ip, mapped_port);
}

FFResult ff_stun_detect_nat_type(const char* stun_server, uint16_t stun_port, P2PNATType* nat_type) {
    if (!stun_server || !nat_type) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return FF_ERROR;
    }
    
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    FFResult result = ff_stun_send_request(sockfd, stun_server, stun_port);
    if (result != FF_OK) {
        close(sockfd);
        return result;
    }
    
    char mapped_ip[16];
    uint16_t mapped_port;
    
    result = ff_stun_recv_response(sockfd, mapped_ip, &mapped_port, 5000);
    
    close(sockfd);
    
    if (result != FF_OK) {
        *nat_type = P2P_NAT_SYMMETRIC;
        return result;
    }
    
    *nat_type = P2P_NAT_FULL_CONE;
    
    return FF_OK;
}

FFResult ff_stun_get_external_address(const char* stun_server, uint16_t stun_port,
                                       char* external_ip, uint16_t* external_port) {
    if (!stun_server || !external_ip || !external_port) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return FF_ERROR;
    }
    
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    FFResult result = ff_stun_send_request(sockfd, stun_server, stun_port);
    if (result != FF_OK) {
        close(sockfd);
        return result;
    }
    
    result = ff_stun_recv_response(sockfd, external_ip, external_port, 5000);
    
    close(sockfd);
    
    return result;
}
