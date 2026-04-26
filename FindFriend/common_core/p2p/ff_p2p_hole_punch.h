// FindFriend P2P UDP 打洞接口

#ifndef FF_P2P_HOLE_PUNCH_H
#define FF_P2P_HOLE_PUNCH_H

#include "ff_core_interface.h"

FFResult ff_hole_punch_init(void);

void ff_hole_punch_cleanup(void);

FFResult ff_hole_punch_perform(const uint8_t* peer_id, const char* peer_ip,
                               uint16_t peer_port, const char* stun_server,
                               uint16_t stun_port, FFAsyncCallback callback,
                               void* user_data);

bool ff_hole_punch_check_success(const uint8_t* peer_id);

FFResult ff_hole_punch_get_address(const uint8_t* peer_id, char* peer_ip,
                                   uint16_t* peer_port);

FFResult ff_hole_punch_send(int sockfd, const uint8_t* peer_id,
                           const void* data, size_t data_len);

FFResult ff_hole_punch_recv(int sockfd, uint8_t* peer_id,
                           void* buffer, size_t buffer_size,
                           size_t* received_len, uint32_t timeout_ms);

#endif // FF_P2P_HOLE_PUNCH_H
