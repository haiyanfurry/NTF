// FindFriend P2P TURN 中继客户端接口

#ifndef FF_P2P_TURN_H
#define FF_P2P_TURN_H

#include "ff_core_interface.h"
#include <stdbool.h>

#define TURN_CHANNEL_DATA 0x4000
#define TURN_DATA 0x0003
#define TURN_ALLOCATE 0x0003
#define TURN_REFRESH 0x0004
#define TURN_CREATE_PERMISSION 0x0008
#define TURN_CHAN_BIND 0x0009

FFResult ff_turn_init(const char* turn_server, uint16_t turn_port,
                      const char* username, const char* password);

void ff_turn_cleanup(void);

FFResult ff_turn_allocate(int sockfd, char* relayed_ip, uint16_t* relayed_port,
                          char* mapped_ip, uint16_t* mapped_port);

FFResult ff_turn_send_data(int sockfd, const uint8_t* data, size_t data_len,
                           const char* peer_ip, uint16_t peer_port);

FFResult ff_turn_recv_data(int sockfd, uint8_t* data, size_t buffer_size,
                           size_t* received_len, uint32_t timeout_ms);

FFResult ff_turn_refresh(int sockfd);

FFResult ff_turn_create_permission(int sockfd, const char* peer_ip);

#endif // FF_P2P_TURN_H
