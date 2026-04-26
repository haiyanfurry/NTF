#ifndef P2P_NETWORK_H
#define P2P_NETWORK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define P2P_MAX_NODES 256
#define P2P_MAX_MESSAGE_SIZE 65536
#define P2P_PORT 9876
#define P2P_DISCOVERY_PORT 9877
#define P2P_MAX_FRIENDS 100

typedef enum {
    P2P_NODE_STATUS_OFFLINE = 0,
    P2P_NODE_STATUS_ONLINE,
    P2P_NODE_STATUS_CONNECTING,
    P2P_NODE_STATUS_CONNECTED
} P2PNodeStatus;

typedef enum {
    P2P_MESSAGE_TYPE_TEXT = 1,
    P2P_MESSAGE_TYPE_LOCATION = 2,
    P2P_MESSAGE_TYPE_GAME_ACTION = 3,
    P2P_MESSAGE_TYPE_MEDIA = 4,
    P2P_MESSAGE_TYPE_FRIEND_REQUEST = 5,
    P2P_MESSAGE_TYPE_FRIEND_RESPONSE = 6,
    P2P_MESSAGE_TYPE_HEARTBEAT = 7,
    P2P_MESSAGE_TYPE_DISCOVERY = 8,
    P2P_MESSAGE_TYPE_SYNC = 9
} P2PMessageType;

typedef struct {
    char uid[64];
    char nickname[128];
    uint32_t ip_addr;
    uint16_t port;
    P2PNodeStatus status;
    bool is_friend;
    double latitude;
    double longitude;
    uint64_t last_seen;
    uint64_t last_heartbeat;
} P2PNode;

typedef struct {
    P2PMessageType type;
    uint32_t from_ip;
    uint16_t from_port;
    char from_uid[64];
    char to_uid[64];
    uint32_t sequence;
    uint64_t timestamp;
    size_t payload_size;
    char payload[P2P_MAX_MESSAGE_SIZE];
} P2PMessage;

typedef void (*P2PMessageCallback)(P2PMessage* message, void* user_data);
typedef void (*P2PNodeCallback)(P2PNode* node, void* user_data);
typedef void (*P2PConnectionCallback)(const char* uid, bool connected, void* user_data);

typedef struct {
    char local_uid[64];
    char local_nickname[128];
    uint32_t local_ip;
    uint16_t listen_port;
    bool is_server_running;
    bool is_discovery_running;
    P2PNodeCallback node_discovered_callback;
    P2PMessageCallback message_received_callback;
    P2PConnectionCallback connection_changed_callback;
    void* callback_user_data;
} P2PConfig;

int p2p_network_init(P2PConfig* config);
void p2p_network_destroy(void);

bool p2p_start_server(uint16_t port);
void p2p_stop_server(void);

bool p2p_start_discovery(void);
void p2p_stop_discovery(void);

int p2p_connect_to_node(const char* uid, uint32_t ip, uint16_t port);
void p2p_disconnect_from_node(const char* uid);
void p2p_disconnect_all_nodes(void);

int p2p_send_message(const char* to_uid, P2PMessage* message);
int p2p_broadcast_message(P2PMessage* message);

P2PNode* p2p_get_node_by_uid(const char* uid);
P2PNode* p2p_get_friend_node(int index);
int p2p_get_online_friend_count(void);
int p2p_get_all_nodes_count(void);

bool p2p_add_friend(const char* uid);
bool p2p_remove_friend(const char* uid);
bool p2p_is_friend(const char* uid);

void p2p_update_location(double latitude, double longitude);
int p2p_send_location_to_friend(const char* friend_uid, double latitude, double longitude);

const char* p2p_get_local_uid(void);
uint32_t p2p_get_local_ip(void);
uint16_t p2p_get_listen_port(void);
bool p2p_is_connected_to(const char* uid);
bool p2p_is_server_running(void);
bool p2p_is_discovery_running(void);

void p2p_set_message_callback(P2PMessageCallback callback, void* user_data);
void p2p_set_node_callback(P2PNodeCallback callback, void* user_data);
void p2p_set_connection_callback(P2PConnectionCallback callback, void* user_data);

const char* p2p_get_node_status_string(P2PNodeStatus status);
void p2p_dump_nodes(void);
void p2p_dump_friends(void);

int p2p_heartbeat(void);
int p2p_cleanup_stale_connections(uint64_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif // P2P_NETWORK_H
