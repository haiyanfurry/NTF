// ====================================================================
// FindFriend P2P 网络接口
// 去中心化网络模块：节点发现、连接、消息传递
// ====================================================================

#ifndef FF_P2P_INTERFACE_H
#define FF_P2P_INTERFACE_H

#include "ff_core_interface.h"
#include <stdint.h>
#include <stdbool.h>

// ====================================================================
// P2P 消息类型
// ====================================================================
typedef enum P2PMessageType {
    P2P_MSG_DISCOVERY = 0,
    P2P_MSG_CONNECT = 1,
    P2P_MSG_DISCONNECT = 2,
    P2P_MSG_DATA = 3,
    P2P_MSG_LOCATION = 4,
    P2P_MSG_MESSAGE = 5,
    P2P_MSG_GAME = 6,
    P2P_MSG_MEDIA = 7,
    P2P_MSG_FRIEND_REQUEST = 8,
    P2P_MSG_FRIEND_ACCEPT = 9,
    P2P_MSG_FRIEND_REJECT = 10
} P2PMessageType;

// ====================================================================
// P2P 节点信息
// ====================================================================
typedef struct P2PNode {
    uint8_t node_id[32];        // 节点唯一标识
    char public_ip[46];         // IPv6 支持
    uint16_t public_port;
    uint32_t uid;               // 关联的用户ID
    uint8_t node_type;          // 0=full, 1=light
    uint64_t last_seen;
    int32_t reputation_score;    // 信誉评分
    uint8_t status;             // 0=offline, 1=online, 2=busy
} P2PNode;

// ====================================================================
// P2P 连接状态
// ====================================================================
typedef enum P2PConnectionState {
    P2P_STATE_DISCONNECTED = 0,
    P2P_STATE_CONNECTING = 1,
    P2P_STATE_CONNECTED = 2,
    P2P_STATE_ERROR = 3
} P2PConnectionState;

typedef struct P2PConnection {
    uint8_t peer_id[32];
    P2PConnectionState state;
    uint64_t connected_time;
    uint64_t last_activity;
    uint64_t bytes_sent;
    uint64_t bytes_received;
} P2PConnection;

// ====================================================================
// NAT 类型
// ====================================================================
typedef enum P2PNATType {
    P2P_NAT_OPEN = 0,           // 开放 NAT
    P2P_NAT_FULL_CONE = 1,      // 全锥型 NAT
    P2P_NAT_RESTRICTED = 2,     // 受限锥型 NAT
    P2P_NAT_PORT_RESTRICTED = 3,// 端口受限锥型 NAT
    P2P_NAT_SYMMETRIC = 4       // 对称型 NAT
} P2PNATType;

// ====================================================================
// P2P 位置信息
// ====================================================================
typedef struct P2PLocation {
    double latitude;            // 纬度
    double longitude;           // 经度
    uint64_t timestamp;         // 时间戳
    bool is_approximate;        // 是否为大致位置
} P2PLocation;

// ====================================================================
// P2P 接口
// ====================================================================

// 初始化与销毁
FFResult ff_p2p_init(FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_shutdown(void);
bool ff_p2p_is_running(void);

// 节点管理
FFResult ff_p2p_start_node(FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_stop_node(void);
FFResult ff_p2p_get_node_info(P2PNode** node_info);

// 节点发现
FFResult ff_p2p_start_discovery(FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_stop_discovery(void);
FFResult ff_p2p_search_nodes(const uint8_t* criteria, size_t criteria_len, P2PNode** nodes, size_t* count, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_get_known_nodes(P2PNode** nodes, size_t* count);

// 连接管理
FFResult ff_p2p_connect(const uint8_t* peer_id, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_disconnect(const uint8_t* peer_id);
FFResult ff_p2p_get_connection_state(const uint8_t* peer_id, P2PConnectionState* state);
FFResult ff_p2p_get_active_connections(P2PConnection** connections, size_t* count);

// 数据传输
FFResult ff_p2p_send(const uint8_t* peer_id, const void* data, size_t data_len, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_broadcast(const void* data, size_t data_len, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_receive(const uint8_t* peer_id, void* buffer, size_t buffer_size, size_t* received, FFAsyncCallback callback, void* user_data);

// NAT 打洞
FFResult ff_p2p_detect_nat_type(P2PNATType* nat_type, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_hole_punch(const uint8_t* target_peer_id, FFAsyncCallback callback, void* user_data);

// STUN 服务器
FFResult ff_p2p_set_stun_server(const char* stun_host, uint16_t stun_port);
FFResult ff_p2p_get_external_address(char* ip, uint16_t* port);

// 好友 P2P 连接
FFResult ff_p2p_connect_friend(uint32_t uid, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_send_to_friend(uint32_t uid, const void* data, size_t data_len, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_receive_from_friend(uint32_t uid, void* buffer, size_t buffer_size, size_t* received, FFAsyncCallback callback, void* user_data);

// 位置共享
FFResult ff_p2p_share_location(const uint8_t* friend_id, const P2PLocation* location, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_get_friend_location(const uint8_t* friend_id, P2PLocation* location, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_calculate_distance(const P2PLocation* loc1, const P2PLocation* loc2, double* distance);

// 好友管理
FFResult ff_p2p_send_friend_request(const uint8_t* target_id, const char* message, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_accept_friend_request(const uint8_t* requester_id, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_reject_friend_request(const uint8_t* requester_id, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_get_friends(uint8_t*** friend_ids, size_t* count);
FFResult ff_p2p_remove_friend(const uint8_t* friend_id, FFAsyncCallback callback, void* user_data);

// 节点信誉
FFResult ff_p2p_update_reputation(const uint8_t* peer_id, int32_t score_delta);
FFResult ff_p2p_get_reputation(const uint8_t* peer_id, int32_t* score);

// 内存释放
void ff_p2p_node_free(P2PNode* node);
void ff_p2p_node_list_free(P2PNode* nodes, size_t count);
void ff_p2p_connection_free(P2PConnection* connections, size_t count);
void ff_p2p_friend_list_free(uint8_t** friend_ids, size_t count);

#endif // FF_P2P_INTERFACE_H
