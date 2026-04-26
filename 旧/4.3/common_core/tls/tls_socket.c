#include "tls_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// 初始化 OpenSSL 库
static bool openssl_initialized = false;

static void initialize_openssl(void) {
    if (!openssl_initialized) {
        SSL_load_error_strings();
        SSL_library_init();
        openssl_initialized = true;
    }
}

// 初始化 TLS 上下文
void* tls_init_context(void) {
    initialize_openssl();
    
    const SSL_METHOD* method = TLSv1_2_client_method();
    if (!method) {
        fprintf(stderr, "Error creating SSL method\n");
        return NULL;
    }
    
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (!ctx) {
        fprintf(stderr, "Error creating SSL context\n");
        return NULL;
    }
    
    // 设置验证模式
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_verify_depth(ctx, 10);
    
    // 加载默认证书
    if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
        fprintf(stderr, "Error setting default verify paths\n");
        SSL_CTX_free(ctx);
        return NULL;
    }
    
    return ctx;
}

// 创建 TLS 套接字
TLSSocket* tls_create_socket(const char* hostname, int port, void* ssl_ctx) {
    if (!ssl_ctx) {
        fprintf(stderr, "Invalid SSL context\n");
        return NULL;
    }
    
    // 创建 TCP 套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Error creating socket\n");
        return NULL;
    }
    
    // 解析服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // 尝试解析域名
    struct hostent* host = gethostbyname(hostname);
    if (!host) {
        fprintf(stderr, "Error resolving hostname\n");
        close(sockfd);
        return NULL;
    }
    
    memcpy(&server_addr.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    
    // 连接服务器
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Error connecting to server\n");
        close(sockfd);
        return NULL;
    }
    
    // 创建 SSL 连接
    SSL* ssl = SSL_new((SSL_CTX*)ssl_ctx);
    if (!ssl) {
        fprintf(stderr, "Error creating SSL\n");
        close(sockfd);
        return NULL;
    }
    
    // 绑定套接字
    SSL_set_fd(ssl, sockfd);
    
    // 进行 TLS 握手
    if (SSL_connect(ssl) != 1) {
        fprintf(stderr, "Error in SSL handshake\n");
        SSL_free(ssl);
        close(sockfd);
        return NULL;
    }
    
    // 创建 TLS 套接字结构体
    TLSSocket* socket = (TLSSocket*)malloc(sizeof(TLSSocket));
    if (!socket) {
        fprintf(stderr, "Error allocating memory\n");
        SSL_free(ssl);
        close(sockfd);
        return NULL;
    }
    
    socket->socket_fd = sockfd;
    socket->ssl_ctx = ssl_ctx;
    socket->ssl = ssl;
    
    return socket;
}

// 关闭 TLS 套接字
void tls_close_socket(TLSSocket* socket) {
    if (socket) {
        if (socket->ssl) {
            SSL_shutdown((SSL*)socket->ssl);
            SSL_free((SSL*)socket->ssl);
        }
        if (socket->socket_fd >= 0) {
            close(socket->socket_fd);
        }
        free(socket);
    }
}

// 发送数据
int tls_send(TLSSocket* socket, const uint8_t* data, size_t length) {
    if (!socket || !socket->ssl) {
        return -1;
    }
    
    int bytes_sent = SSL_write((SSL*)socket->ssl, data, length);
    if (bytes_sent <= 0) {
        int err = SSL_get_error((SSL*)socket->ssl, bytes_sent);
        fprintf(stderr, "SSL write error: %d\n", err);
        return -1;
    }
    
    return bytes_sent;
}

// 接收数据
int tls_recv(TLSSocket* socket, uint8_t* buffer, size_t length) {
    if (!socket || !socket->ssl) {
        return -1;
    }
    
    int bytes_received = SSL_read((SSL*)socket->ssl, buffer, length);
    if (bytes_received <= 0) {
        int err = SSL_get_error((SSL*)socket->ssl, bytes_received);
        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
            fprintf(stderr, "SSL read error: %d\n", err);
        }
        return -1;
    }
    
    return bytes_received;
}

// 检查 TLS 连接是否有效
bool tls_is_connected(TLSSocket* socket) {
    if (!socket || !socket->ssl) {
        return false;
    }
    
    // 检查套接字是否有效
    if (socket->socket_fd < 0) {
        return false;
    }
    
    // 检查 SSL 连接状态
    return SSL_get_verify_result((SSL*)socket->ssl) == X509_V_OK;
}