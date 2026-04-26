// FindFriend OpenBSD 网络接口实现

#include "platform_specific.h"
#include "../../../common_core/interface/ff_core_interface.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>

FFResult platform_get_network_interfaces(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) {
        return FF_ERROR_INVALID_PARAM;
    }
    
    struct ifaddrs *ifaddr, *ifa;
    char* pos = buffer;
    size_t remaining = buffer_size;
    
    if (getifaddrs(&ifaddr) == -1) {
        return FF_ERROR;
    }
    
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        
        int family = ifa->ifa_addr->sa_family;
        
        if (family == AF_INET) {
            struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
            int len = snprintf(pos, remaining, "%s: %s\n",
                              ifa->ifa_name,
                              inet_ntoa(addr->sin_addr));
            if (len > 0 && len < remaining) {
                pos += len;
                remaining -= len;
            }
        }
    }
    
    freeifaddrs(ifaddr);
    return FF_OK;
}

int platform_create_socket(int domain, int type, int protocol) {
    return socket(domain, type, protocol);
}

int platform_bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    return bind(sockfd, addr, addrlen);
}

int platform_listen(int sockfd, int backlog) {
    return listen(sockfd, backlog);
}

int platform_connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    return connect(sockfd, addr, addrlen);
}

ssize_t platform_send(int sockfd, const void* buf, size_t len, int flags) {
    return send(sockfd, buf, len, flags);
}

ssize_t platform_recv(int sockfd, void* buf, size_t len, int flags) {
    return recv(sockfd, buf, len, flags);
}

int platform_close(int fd) {
    return close(fd);
}

int platform_set_nonblocking(int fd, bool nonblocking) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }

    if (nonblocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    return fcntl(fd, F_SETFL, flags);
}

int platform_get_local_address(int fd, char* ip, uint16_t* port) {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    if (getsockname(fd, (struct sockaddr*)&addr, &addrlen) == -1) {
        return -1;
    }

    if (ip) {
        strncpy(ip, inet_ntoa(addr.sin_addr), 16);
    }

    if (port) {
        *port = ntohs(addr.sin_port);
    }

    return 0;
}

int platform_get_peer_address(int fd, char* ip, uint16_t* port) {
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);

    if (getpeername(fd, (struct sockaddr*)&addr, &addrlen) == -1) {
        return -1;
    }

    if (ip) {
        strncpy(ip, inet_ntoa(addr.sin_addr), 16);
    }

    if (port) {
        *port = ntohs(addr.sin_port);
    }

    return 0;
}
