#ifndef PROXY_MANAGER_H
#define PROXY_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 代理类型 */
typedef enum {
    PROXY_TYPE_NONE = 0,
    PROXY_TYPE_HTTP,
    PROXY_TYPE_HTTPS,
    PROXY_TYPE_SOCKS4,
    PROXY_TYPE_SOCKS5
} ProxyType;

/* 代理配置 */
typedef struct {
    ProxyType type;
    char host[256];
    uint16_t port;
    char username[128];
    char password[128];
    bool auth_required;
    int timeout_ms;
    bool enabled;
} ProxyConfig;

/* 代理状态 */
typedef struct {
    bool is_connected;
    bool is_authenticated;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    time_t connected_since;
    int error_count;
} ProxyStatus;

/* 初始化代理模块 */
bool proxy_manager_init(void);

/* 清理代理模块 */
void proxy_manager_cleanup(void);

/* 设置代理配置 */
bool proxy_set_config(const ProxyConfig* config);

/* 获取代理配置 */
bool proxy_get_config(ProxyConfig* config);

/* 启用代理 */
bool proxy_enable(void);

/* 禁用代理 */
bool proxy_disable(void);

/* 检查代理是否启用 */
bool proxy_is_enabled(void);

/* 测试代理连接 */
bool proxy_test_connection(const ProxyConfig* config);

/* 通过代理建立TCP连接 */
int proxy_connect(const char* target_host, uint16_t target_port);

/* 通过代理发送数据 */
ssize_t proxy_send(int sockfd, const void* buf, size_t len);

/* 通过代理接收数据 */
ssize_t proxy_recv(int sockfd, void* buf, size_t len);

/* 关闭代理连接 */
bool proxy_close(int sockfd);

/* 获取代理状态 */
bool proxy_get_status(ProxyStatus* status);

/* 重置代理统计 */
void proxy_reset_stats(void);

/* HTTP代理CONNECT方法 */
bool proxy_http_connect(int sockfd, const char* target_host, uint16_t target_port);

/* SOCKS5代理认证和连接 */
bool proxy_socks5_connect(int sockfd, const char* target_host, uint16_t target_port);

/* SOCKS5认证 */
bool proxy_socks5_auth(int sockfd, const char* username, const char* password);

/* 获取代理错误信息 */
const char* proxy_get_error_string(int error_code);

/* 代理URL解析 */
bool proxy_parse_url(const char* url, ProxyConfig* config);

/* 验证代理配置 */
bool proxy_validate_config(const ProxyConfig* config);

#ifdef __cplusplus
}
#endif

#endif /* PROXY_MANAGER_H */
