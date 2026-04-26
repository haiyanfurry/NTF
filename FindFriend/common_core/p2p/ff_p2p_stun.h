// FindFriend P2P STUN 客户端接口

#ifndef FF_P2P_STUN_H
#define FF_P2P_STUN_H

#include "ff_core_interface.h"
#include "ff_p2p_interface.h"

#define STUN_HEADER_SIZE 20
#define STUN_MAGIC_COOKIE 0x2112A442
#define STUN_BINDING_REQUEST 0x0001
#define STUN_BINDING_RESPONSE 0x0101
#define STUN_BINDING_ERROR 0x0111
#define STUN_ATTR_MAPPED_ADDRESS 0x0001
#define STUN_ATTR_XOR_MAPPED_ADDRESS 0x0020
#define STUN_ATTR_CHANGE_REQUEST 0x0003
#define STUN_ATTR_RESPONSE_ADDRESS 0x0002
#define STUN_ATTR_SOURCE_ADDRESS 0x0004
#define STUN_ATTR_CHANGED_ADDRESS 0x0005

FFResult ff_stun_create_binding_request(uint8_t* buffer, size_t buffer_size, size_t* out_len);

FFResult ff_stun_parse_response(const uint8_t* buffer, size_t buffer_len,
                                char* mapped_ip, uint16_t* mapped_port);

FFResult ff_stun_send_request(int sockfd, const char* stun_server, uint16_t stun_port);

FFResult ff_stun_recv_response(int sockfd, char* mapped_ip, uint16_t* mapped_port, uint32_t timeout_ms);

FFResult ff_stun_detect_nat_type(const char* stun_server, uint16_t stun_port, P2PNATType* nat_type);

FFResult ff_stun_get_external_address(const char* stun_server, uint16_t stun_port,
                                       char* external_ip, uint16_t* external_port);

#endif // FF_P2P_STUN_H
