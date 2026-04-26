#ifndef TLS_SOCKET_H
#define TLS_SOCKET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// TLS 套接字结构体
typedef struct {
    int socket_fd;
    void* ssl_ctx;
    void* ssl;
} TLSSocket;

// 初始化 TLS 上下文
void* tls_init_context(void);

// 创建 TLS 套接字
TLSSocket* tls_create_socket(const char* hostname, int port, void* ssl_ctx);

// 关闭 TLS 套接字
void tls_close_socket(TLSSocket* socket);

// 发送数据
int tls_send(TLSSocket* socket, const uint8_t* data, size_t length);

// 接收数据
int tls_recv(TLSSocket* socket, uint8_t* buffer, size_t length);

// 检查 TLS 连接是否有效
bool tls_is_connected(TLSSocket* socket);

#ifdef __cplusplus
}
#endif

#endif // TLS_SOCKET_H