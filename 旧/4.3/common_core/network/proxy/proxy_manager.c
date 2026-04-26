#include "proxy_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

/* 全局代理状态 */
static ProxyConfig g_proxy_config = {0};
static ProxyStatus g_proxy_status = {0};
static pthread_mutex_t g_proxy_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool g_initialized = false;

/* 初始化代理模块 */
bool proxy_manager_init(void) {
    pthread_mutex_lock(&g_proxy_mutex);
    
    if (g_initialized) {
        pthread_mutex_unlock(&g_proxy_mutex);
        return true;
    }
    
    memset(&g_proxy_config, 0, sizeof(g_proxy_config));
    memset(&g_proxy_status, 0, sizeof(g_proxy_status));
    g_proxy_config.type = PROXY_TYPE_NONE;
    g_proxy_config.enabled = false;
    g_proxy_config.timeout_ms = 30000; /* 默认30秒超时 */
    
    g_initialized = true;
    
    pthread_mutex_unlock(&g_proxy_mutex);
    
    printf("[Proxy] Manager initialized\n");
    return true;
}

/* 清理代理模块 */
void proxy_manager_cleanup(void) {
    pthread_mutex_lock(&g_proxy_mutex);
    
    if (!g_initialized) {
        pthread_mutex_unlock(&g_proxy_mutex);
        return;
    }
    
    /* 安全清除配置 */
    memset(&g_proxy_config, 0, sizeof(g_proxy_config));
    memset(&g_proxy_status, 0, sizeof(g_proxy_status));
    g_initialized = false;
    
    pthread_mutex_unlock(&g_proxy_mutex);
    
    printf("[Proxy] Manager cleaned up\n");
}

/* 设置代理配置 */
bool proxy_set_config(const ProxyConfig* config) {
    if (!config || !g_initialized) return false;
    
    /* 验证配置 */
    if (!proxy_validate_config(config)) {
        return false;
    }
    
    pthread_mutex_lock(&g_proxy_mutex);
    memcpy(&g_proxy_config, config, sizeof(ProxyConfig));
    pthread_mutex_unlock(&g_proxy_mutex);
    
    printf("[Proxy] Config updated: %s:%d (type=%d)\n", 
           config->host, config->port, config->type);
    return true;
}

/* 获取代理配置 */
bool proxy_get_config(ProxyConfig* config) {
    if (!config || !g_initialized) return false;
    
    pthread_mutex_lock(&g_proxy_mutex);
    memcpy(config, &g_proxy_config, sizeof(ProxyConfig));
    pthread_mutex_unlock(&g_proxy_mutex);
    
    return true;
}

/* 启用代理 */
bool proxy_enable(void) {
    if (!g_initialized) return false;
    
    pthread_mutex_lock(&g_proxy_mutex);
    
    if (g_proxy_config.type == PROXY_TYPE_NONE || 
        strlen(g_proxy_config.host) == 0 ||
        g_proxy_config.port == 0) {
        pthread_mutex_unlock(&g_proxy_mutex);
        return false;
    }
    
    g_proxy_config.enabled = true;
    
    pthread_mutex_unlock(&g_proxy_mutex);
    
    printf("[Proxy] Enabled\n");
    return true;
}

/* 禁用代理 */
bool proxy_disable(void) {
    if (!g_initialized) return false;
    
    pthread_mutex_lock(&g_proxy_mutex);
    g_proxy_config.enabled = false;
    pthread_mutex_unlock(&g_proxy_mutex);
    
    printf("[Proxy] Disabled\n");
    return true;
}

/* 检查代理是否启用 */
bool proxy_is_enabled(void) {
    if (!g_initialized) return false;
    
    pthread_mutex_lock(&g_proxy_mutex);
    bool enabled = g_proxy_config.enabled;
    pthread_mutex_unlock(&g_proxy_mutex);
    
    return enabled;
}

/* 创建到代理服务器的TCP连接 */
static int create_proxy_socket(const ProxyConfig* config) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return -1;
    }
    
    /* 设置超时 */
    struct timeval tv;
    tv.tv_sec = config->timeout_ms / 1000;
    tv.tv_usec = (config->timeout_ms % 1000) * 1000;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    
    /* 解析代理服务器地址 */
    struct hostent* server = gethostbyname(config->host);
    if (!server) {
        close(sockfd);
        return -1;
    }
    
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(config->port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    
    /* 连接代理服务器 */
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        close(sockfd);
        return -1;
    }
    
    return sockfd;
}

/* 测试代理连接 */
bool proxy_test_connection(const ProxyConfig* config) {
    if (!config) return false;
    
    int sockfd = create_proxy_socket(config);
    if (sockfd < 0) {
        return false;
    }
    
    close(sockfd);
    return true;
}

/* HTTP代理CONNECT方法 */
bool proxy_http_connect(int sockfd, const char* target_host, uint16_t target_port) {
    char request[512];
    snprintf(request, sizeof(request),
             "CONNECT %s:%d HTTP/1.1\r\n"
             "Host: %s:%d\r\n"
             "Proxy-Connection: Keep-Alive\r\n"
             "\r\n",
             target_host, target_port, target_host, target_port);
    
    if (send(sockfd, request, strlen(request), 0) < 0) {
        return false;
    }
    
    char response[1024];
    int received = recv(sockfd, response, sizeof(response) - 1, 0);
    if (received < 0) {
        return false;
    }
    response[received] = '\0';
    
    /* 检查HTTP 200响应 */
    if (strstr(response, "200") == NULL) {
        return false;
    }
    
    return true;
}

/* SOCKS5代理认证和连接 */
bool proxy_socks5_auth(int sockfd, const char* username, const char* password) {
    /* 发送认证方法选择 */
    unsigned char auth_request[3] = {0x05, 0x01, 0x02}; /* VER, NMETHODS, METHOD (用户名密码) */
    if (send(sockfd, auth_request, 3, 0) != 3) {
        return false;
    }
    
    unsigned char auth_response[2];
    if (recv(sockfd, auth_response, 2, 0) != 2) {
        return false;
    }
    
    if (auth_response[0] != 0x05 || auth_response[1] != 0x02) {
        return false;
    }
    
    /* 发送用户名密码 */
    size_t ulen = strlen(username);
    size_t plen = strlen(password);
    
    unsigned char* auth_data = malloc(3 + ulen + plen);
    auth_data[0] = 0x01; /* VER */
    auth_data[1] = ulen;
    memcpy(auth_data + 2, username, ulen);
    auth_data[2 + ulen] = plen;
    memcpy(auth_data + 3 + ulen, password, plen);
    
    if (send(sockfd, auth_data, 3 + ulen + plen, 0) != (ssize_t)(3 + ulen + plen)) {
        free(auth_data);
        return false;
    }
    free(auth_data);
    
    unsigned char auth_result[2];
    if (recv(sockfd, auth_result, 2, 0) != 2) {
        return false;
    }
    
    return auth_result[0] == 0x01 && auth_result[1] == 0x00;
}

/* SOCKS5代理连接 */
bool proxy_socks5_connect(int sockfd, const char* target_host, uint16_t target_port) {
    /* 发送认证方法选择 (无认证) */
    unsigned char init_request[3] = {0x05, 0x01, 0x00};
    if (send(sockfd, init_request, 3, 0) != 3) {
        return false;
    }
    
    unsigned char init_response[2];
    if (recv(sockfd, init_response, 2, 0) != 2) {
        return false;
    }
    
    if (init_response[0] != 0x05) {
        return false;
    }
    
    /* 发送连接请求 */
    unsigned char conn_request[256];
    conn_request[0] = 0x05; /* VER */
    conn_request[1] = 0x01; /* CMD (CONNECT) */
    conn_request[2] = 0x00; /* RSV */
    conn_request[3] = 0x03; /* ATYP (域名) */
    
    size_t host_len = strlen(target_host);
    conn_request[4] = host_len;
    memcpy(conn_request + 5, target_host, host_len);
    
    conn_request[5 + host_len] = (target_port >> 8) & 0xFF;
    conn_request[6 + host_len] = target_port & 0xFF;
    
    if (send(sockfd, conn_request, 7 + host_len, 0) != (ssize_t)(7 + host_len)) {
        return false;
    }
    
    /* 接收响应 */
    unsigned char conn_response[256];
    if (recv(sockfd, conn_response, 10, 0) < 10) {
        return false;
    }
    
    return conn_response[0] == 0x05 && conn_response[1] == 0x00;
}

/* 通过代理建立TCP连接 */
int proxy_connect(const char* target_host, uint16_t target_port) {
    if (!g_initialized || !target_host) return -1;
    
    pthread_mutex_lock(&g_proxy_mutex);
    ProxyConfig config = g_proxy_config;
    pthread_mutex_unlock(&g_proxy_mutex);
    
    if (!config.enabled || config.type == PROXY_TYPE_NONE) {
        /* 直接连接 */
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) return -1;
        
        struct hostent* server = gethostbyname(target_host);
        if (!server) {
            close(sockfd);
            return -1;
        }
        
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(target_port);
        memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
        
        if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            close(sockfd);
            return -1;
        }
        
        return sockfd;
    }
    
    /* 通过代理连接 */
    int proxy_sock = create_proxy_socket(&config);
    if (proxy_sock < 0) {
        return -1;
    }
    
    bool connected = false;
    
    switch (config.type) {
        case PROXY_TYPE_HTTP:
        case PROXY_TYPE_HTTPS:
            connected = proxy_http_connect(proxy_sock, target_host, target_port);
            break;
            
        case PROXY_TYPE_SOCKS5:
            if (config.auth_required) {
                connected = proxy_socks5_auth(proxy_sock, config.username, config.password);
                if (!connected) {
                    close(proxy_sock);
                    return -1;
                }
            }
            connected = proxy_socks5_connect(proxy_sock, target_host, target_port);
            break;
            
        default:
            close(proxy_sock);
            return -1;
    }
    
    if (!connected) {
        close(proxy_sock);
        return -1;
    }
    
    /* 更新状态 */
    pthread_mutex_lock(&g_proxy_mutex);
    g_proxy_status.is_connected = true;
    g_proxy_status.connected_since = time(NULL);
    pthread_mutex_unlock(&g_proxy_mutex);
    
    return proxy_sock;
}

/* 通过代理发送数据 */
ssize_t proxy_send(int sockfd, const void* buf, size_t len) {
    ssize_t sent = send(sockfd, buf, len, 0);
    
    if (sent > 0) {
        pthread_mutex_lock(&g_proxy_mutex);
        g_proxy_status.bytes_sent += sent;
        pthread_mutex_unlock(&g_proxy_mutex);
    }
    
    return sent;
}

/* 通过代理接收数据 */
ssize_t proxy_recv(int sockfd, void* buf, size_t len) {
    ssize_t received = recv(sockfd, buf, len, 0);
    
    if (received > 0) {
        pthread_mutex_lock(&g_proxy_mutex);
        g_proxy_status.bytes_received += received;
        pthread_mutex_unlock(&g_proxy_mutex);
    }
    
    return received;
}

/* 关闭代理连接 */
bool proxy_close(int sockfd) {
    if (sockfd < 0) return false;
    
    close(sockfd);
    
    pthread_mutex_lock(&g_proxy_mutex);
    g_proxy_status.is_connected = false;
    pthread_mutex_unlock(&g_proxy_mutex);
    
    return true;
}

/* 获取代理状态 */
bool proxy_get_status(ProxyStatus* status) {
    if (!status || !g_initialized) return false;
    
    pthread_mutex_lock(&g_proxy_mutex);
    memcpy(status, &g_proxy_status, sizeof(ProxyStatus));
    pthread_mutex_unlock(&g_proxy_mutex);
    
    return true;
}

/* 重置代理统计 */
void proxy_reset_stats(void) {
    pthread_mutex_lock(&g_proxy_mutex);
    g_proxy_status.bytes_sent = 0;
    g_proxy_status.bytes_received = 0;
    g_proxy_status.error_count = 0;
    pthread_mutex_unlock(&g_proxy_mutex);
}

/* 获取代理错误信息 */
const char* proxy_get_error_string(int error_code) {
    switch (error_code) {
        case 0: return "Success";
        case -1: return "Connection failed";
        case -2: return "Authentication failed";
        case -3: return "Timeout";
        case -4: return "Invalid response";
        default: return "Unknown error";
    }
}

/* 代理URL解析 */
bool proxy_parse_url(const char* url, ProxyConfig* config) {
    if (!url || !config) return false;
    
    memset(config, 0, sizeof(ProxyConfig));
    
    /* 解析协议 */
    if (strncmp(url, "http://", 7) == 0) {
        config->type = PROXY_TYPE_HTTP;
        url += 7;
    } else if (strncmp(url, "https://", 8) == 0) {
        config->type = PROXY_TYPE_HTTPS;
        url += 8;
    } else if (strncmp(url, "socks5://", 9) == 0) {
        config->type = PROXY_TYPE_SOCKS5;
        url += 9;
    } else {
        return false;
    }
    
    /* 解析认证信息 */
    const char* at_sign = strchr(url, '@');
    if (at_sign) {
        char auth[256];
        size_t auth_len = at_sign - url;
        if (auth_len >= sizeof(auth)) return false;
        
        strncpy(auth, url, auth_len);
        auth[auth_len] = '\0';
        
        char* colon = strchr(auth, ':');
        if (colon) {
            *colon = '\0';
            strncpy(config->username, auth, sizeof(config->username) - 1);
            strncpy(config->password, colon + 1, sizeof(config->password) - 1);
            config->auth_required = true;
        }
        
        url = at_sign + 1;
    }
    
    /* 解析主机和端口 */
    const char* colon = strchr(url, ':');
    if (colon) {
        size_t host_len = colon - url;
        if (host_len >= sizeof(config->host)) return false;
        
        strncpy(config->host, url, host_len);
        config->host[host_len] = '\0';
        config->port = atoi(colon + 1);
    } else {
        strncpy(config->host, url, sizeof(config->host) - 1);
        config->port = (config->type == PROXY_TYPE_SOCKS5) ? 1080 : 8080;
    }
    
    config->timeout_ms = 30000;
    
    return strlen(config->host) > 0 && config->port > 0;
}

/* 验证代理配置 */
bool proxy_validate_config(const ProxyConfig* config) {
    if (!config) return false;
    
    if (config->type == PROXY_TYPE_NONE) {
        return true; /* 无代理是有效的 */
    }
    
    if (strlen(config->host) == 0 || config->port == 0) {
        return false;
    }
    
    if (config->auth_required) {
        if (strlen(config->username) == 0) {
            return false;
        }
    }
    
    return true;
}
