// FindFriend CLI 工具
// 用于开发和调试

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "../common_core/interface/ff_core_interface.h"
#include "../common_core/interface/ff_p2p_interface.h"
#include "../security_rust/ffi/ff_security_ffi.h"

static bool g_running = false;

// 显示帮助信息
static void show_help(void) {
    printf("FindFriend CLI Tool\n");
    printf("Usage: ff_cli [options]\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -v, --version           Show version information\n");
    printf("\n");
    printf("P2P Commands:\n");
    printf("  --p2p-init              Initialize P2P module\n");
    printf("  --p2p-shutdown          Shutdown P2P module\n");
    printf("  --p2p-start-node        Start P2P node\n");
    printf("  --p2p-stop-node         Stop P2P node\n");
    printf("  --p2p-start-discovery   Start node discovery\n");
    printf("  --p2p-stop-discovery    Stop node discovery\n");
    printf("  --p2p-search-nodes      Search for nodes\n");
    printf("  --p2p-connect PEER_ID   Connect to peer\n");
    printf("  --p2p-disconnect PEER_ID Disconnect from peer\n");
    printf("  --p2p-send PEER_ID DATA Send data to peer\n");
    printf("  --p2p-broadcast DATA    Broadcast data\n");
    printf("  --p2p-get-connections   Get active connections\n");
    printf("  --p2p-get-nodes         Get known nodes\n");
    printf("\n");
    printf("Security Commands:\n");
    printf("  --security-init         Initialize security module\n");
    printf("  --security-cleanup      Cleanup security module\n");
    printf("  --security-boot-check   Perform boot check\n");
    printf("  --security-root-check   Check for root/jailbreak\n");
    printf("  --security-mitm-check   Check for MITM attack\n");
    printf("  --security-validate-email EMAIL Validate email\n");
    printf("  --security-validate-username USERNAME Validate username\n");
    printf("  --security-detect-sql SQL Detect SQL injection\n");
    printf("  --security-detect-xss XSS Detect XSS\n");
    printf("  --security-get-status   Get security status\n");
    printf("\n");
    printf("Platform Commands:\n");
    printf("  --platform-info         Show platform information\n");
    printf("  --platform-network      Show network interfaces\n");
    printf("  --platform-system       Show system information\n");
    printf("\n");
    printf("NAT Commands:\n");
    printf("  --set-stun HOST:PORT    Set STUN server\n");
    printf("  --get-external-addr     Get external address\n");
    printf("  --detect-nat-type       Detect NAT type\n");
    printf("  --hole-punch PEER_ID    Perform hole punching\n");
    printf("\n");
    printf("Location Commands:\n");
    printf("  --share-location FRIEND_ID LAT LON Share location\n");
    printf("  --get-friend-location FRIEND_ID Get friend location\n");
    printf("  --calculate-distance LAT1 LON1 LAT2 LON2 Calculate distance\n");
    printf("\n");
    printf("Friend Commands:\n");
    printf("  --send-friend-request TARGET_ID MESSAGE Send friend request\n");
    printf("  --accept-friend-request REQUESTER_ID Accept friend request\n");
    printf("  --reject-friend-request REQUESTER_ID Reject friend request\n");
    printf("  --get-friends           Get friends list\n");
    printf("  --remove-friend FRIEND_ID Remove friend\n");
    printf("\n");
    printf("Debug Commands:\n");
    printf("  --logs                  Show application logs\n");
    printf("  --security-logs         Show security logs\n");
    printf("  --clear-logs            Clear all logs\n");
    printf("  --test-performance      Test performance\n");
    printf("  --test-security         Test security module\n");
    printf("\n");
}

// 显示版本信息
static void show_version(void) {
    printf("FindFriend CLI Tool v1.0.0\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);
}

// 回调函数
static void async_callback(FFResult result, void* user_data) {
    printf("Callback received: result=%d\n", result);
}

// 解析节点 ID
static bool parse_node_id(const char* str, uint8_t* node_id) {
    if (strlen(str) != 64) {
        return false;
    }
    
    for (int i = 0; i < 32; i++) {
        char hex[3] = {str[i*2], str[i*2+1], '\0'};
        node_id[i] = (uint8_t)strtol(hex, NULL, 16);
    }
    
    return true;
}

// 显示节点信息
static void show_node_info(const P2PNode* node) {
    printf("Node ID: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", node->node_id[i]);
    }
    printf("\n");
    printf("Public IP: %s\n", node->public_ip);
    printf("Public Port: %d\n", node->public_port);
    printf("UID: %u\n", node->uid);
    printf("Node Type: %s\n", node->node_type == 0 ? "full" : "light");
    printf("Last Seen: %llu\n", node->last_seen);
    printf("Reputation: %d\n", node->reputation_score);
    printf("Status: %s\n", node->status == 0 ? "offline" : node->status == 1 ? "online" : "busy");
    printf("\n");
}

// 显示连接信息
static void show_connection_info(const P2PConnection* connection) {
    printf("Peer ID: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", connection->peer_id[i]);
    }
    printf("\n");
    printf("State: %s\n", 
           connection->state == P2P_STATE_DISCONNECTED ? "disconnected" :
           connection->state == P2P_STATE_CONNECTING ? "connecting" :
           connection->state == P2P_STATE_CONNECTED ? "connected" : "error");
    printf("Connected Time: %llu\n", connection->connected_time);
    printf("Last Activity: %llu\n", connection->last_activity);
    printf("Bytes Sent: %llu\n", connection->bytes_sent);
    printf("Bytes Received: %llu\n", connection->bytes_received);
    printf("\n");
}

// 显示安全状态
static void show_security_status(const FFSecurityStatus* status) {
    printf("Overall Status: %d\n", status->overall);
    printf("Root Detected: %s\n", status->root_detected ? "yes" : "no");
    printf("MITM Detected: %s\n", status->mitm_detected ? "yes" : "no");
    printf("DoS Attack Detected: %s\n", status->dos_attack ? "yes" : "no");
    printf("Fake GPS Detected: %s\n", status->fake_gps ? "yes" : "no");
    printf("Crawler Detected: %s\n", status->crawler_detected ? "yes" : "no");
    printf("\n");
}

int main(int argc, char* argv[]) {
    int opt;
    int option_index = 0;
    
    static struct option long_options[] = {
        // Help and version
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        
        // P2P commands
        {"p2p-init", no_argument, 0, 0},
        {"p2p-shutdown", no_argument, 0, 0},
        {"p2p-start-node", no_argument, 0, 0},
        {"p2p-stop-node", no_argument, 0, 0},
        {"p2p-start-discovery", no_argument, 0, 0},
        {"p2p-stop-discovery", no_argument, 0, 0},
        {"p2p-search-nodes", no_argument, 0, 0},
        {"p2p-connect", required_argument, 0, 0},
        {"p2p-disconnect", required_argument, 0, 0},
        {"p2p-send", required_argument, 0, 0},
        {"p2p-broadcast", required_argument, 0, 0},
        {"p2p-get-connections", no_argument, 0, 0},
        {"p2p-get-nodes", no_argument, 0, 0},
        
        // Security commands
        {"security-init", no_argument, 0, 0},
        {"security-cleanup", no_argument, 0, 0},
        {"security-boot-check", no_argument, 0, 0},
        {"security-root-check", no_argument, 0, 0},
        {"security-mitm-check", no_argument, 0, 0},
        {"security-validate-email", required_argument, 0, 0},
        {"security-validate-username", required_argument, 0, 0},
        {"security-detect-sql", required_argument, 0, 0},
        {"security-detect-xss", required_argument, 0, 0},
        {"security-get-status", no_argument, 0, 0},
        
        // Platform commands
        {"platform-info", no_argument, 0, 0},
        {"platform-network", no_argument, 0, 0},
        {"platform-system", no_argument, 0, 0},
        
        // NAT commands
        {"set-stun", required_argument, 0, 0},
        {"get-external-addr", no_argument, 0, 0},
        {"detect-nat-type", no_argument, 0, 0},
        {"hole-punch", required_argument, 0, 0},
        
        // Location commands
        {"share-location", required_argument, 0, 0},
        {"get-friend-location", required_argument, 0, 0},
        {"calculate-distance", required_argument, 0, 0},
        
        // Friend commands
        {"send-friend-request", required_argument, 0, 0},
        {"accept-friend-request", required_argument, 0, 0},
        {"reject-friend-request", required_argument, 0, 0},
        {"get-friends", no_argument, 0, 0},
        {"remove-friend", required_argument, 0, 0},
        
        // Debug commands
        {"logs", no_argument, 0, 0},
        {"security-logs", no_argument, 0, 0},
        {"clear-logs", no_argument, 0, 0},
        {"test-performance", no_argument, 0, 0},
        {"test-security", no_argument, 0, 0},
        
        {0, 0, 0, 0}
    };
    
    while ((opt = getopt_long(argc, argv, "hv", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                show_help();
                return 0;
            case 'v':
                show_version();
                return 0;
            case 0:
                // 长选项处理
                if (strcmp(long_options[option_index].name, "p2p-init") == 0) {
                    printf("Initializing P2P module...\n");
                    FFResult result = ff_p2p_init(async_callback, NULL);
                    printf("P2P init result: %d\n", result);
                } else if (strcmp(long_options[option_index].name, "p2p-shutdown") == 0) {
                    printf("Shutting down P2P module...\n");
                    FFResult result = ff_p2p_shutdown();
                    printf("P2P shutdown result: %d\n", result);
                } else if (strcmp(long_options[option_index].name, "p2p-start-node") == 0) {
                    printf("Starting P2P node...\n");
                    FFResult result = ff_p2p_start_node(async_callback, NULL);
                    printf("Start node result: %d\n", result);
                } else if (strcmp(long_options[option_index].name, "p2p-stop-node") == 0) {
                    printf("Stopping P2P node...\n");
                    FFResult result = ff_p2p_stop_node();
                    printf("Stop node result: %d\n", result);
                } else if (strcmp(long_options[option_index].name, "p2p-start-discovery") == 0) {
                    printf("Starting node discovery...\n");
                    FFResult result = ff_p2p_start_discovery(async_callback, NULL);
                    printf("Start discovery result: %d\n", result);
                } else if (strcmp(long_options[option_index].name, "p2p-stop-discovery") == 0) {
                    printf("Stopping node discovery...\n");
                    FFResult result = ff_p2p_stop_discovery();
                    printf("Stop discovery result: %d\n", result);
                } else if (strcmp(long_options[option_index].name, "p2p-search-nodes") == 0) {
                    printf("Searching for nodes...\n");
                    P2PNode* nodes;
                    size_t count;
                    FFResult result = ff_p2p_search_nodes(NULL, 0, &nodes, &count, async_callback, NULL);
                    if (result == FF_OK) {
                        printf("Found %zu nodes\n", count);
                        for (size_t i = 0; i < count; i++) {
                            show_node_info(&nodes[i]);
                        }
                        ff_p2p_node_list_free(nodes, count);
                    } else {
                        printf("Search nodes result: %d\n", result);
                    }
                } else if (strcmp(long_options[option_index].name, "p2p-connect") == 0) {
                    uint8_t peer_id[32];
                    if (parse_node_id(optarg, peer_id)) {
                        printf("Connecting to peer...\n");
                        FFResult result = ff_p2p_connect(peer_id, async_callback, NULL);
                        printf("Connect result: %d\n", result);
                    } else {
                        printf("Invalid peer ID format\n");
                    }
                } else if (strcmp(long_options[option_index].name, "p2p-disconnect") == 0) {
                    uint8_t peer_id[32];
                    if (parse_node_id(optarg, peer_id)) {
                        printf("Disconnecting from peer...\n");
                        FFResult result = ff_p2p_disconnect(peer_id);
                        printf("Disconnect result: %d\n", result);
                    } else {
                        printf("Invalid peer ID format\n");
                    }
                } else if (strcmp(long_options[option_index].name, "p2p-send") == 0) {
                    // 解析参数：PEER_ID DATA
                    char* peer_id_str = strtok(optarg, " ");
                    char* data = strtok(NULL, "");
                    if (peer_id_str && data) {
                        uint8_t peer_id[32];
                        if (parse_node_id(peer_id_str, peer_id)) {
                            printf("Sending data to peer...\n");
                            FFResult result = ff_p2p_send(peer_id, data, strlen(data), async_callback, NULL);
                            printf("Send result: %d\n", result);
                        } else {
                            printf("Invalid peer ID format\n");
                        }
                    } else {
                        printf("Invalid arguments\n");
                    }
                } else if (strcmp(long_options[option_index].name, "p2p-broadcast") == 0) {
                    printf("Broadcasting data...\n");
                    FFResult result = ff_p2p_broadcast(optarg, strlen(optarg), async_callback, NULL);
                    printf("Broadcast result: %d\n", result);
                } else if (strcmp(long_options[option_index].name, "p2p-get-connections") == 0) {
                    printf("Getting active connections...\n");
                    P2PConnection* connections;
                    size_t count;
                    FFResult result = ff_p2p_get_active_connections(&connections, &count);
                    if (result == FF_OK) {
                        printf("Active connections: %zu\n", count);
                        for (size_t i = 0; i < count; i++) {
                            show_connection_info(&connections[i]);
                        }
                        ff_p2p_connection_free(connections, count);
                    } else {
                        printf("Get connections result: %d\n", result);
                    }
                } else if (strcmp(long_options[option_index].name, "p2p-get-nodes") == 0) {
                    printf("Getting known nodes...\n");
                    P2PNode* nodes;
                    size_t count;
                    FFResult result = ff_p2p_get_known_nodes(&nodes, &count);
                    if (result == FF_OK) {
                        printf("Known nodes: %zu\n", count);
                        for (size_t i = 0; i < count; i++) {
                            show_node_info(&nodes[i]);
                        }
                        ff_p2p_node_list_free(nodes, count);
                    } else {
                        printf("Get nodes result: %d\n", result);
                    }
                } else if (strcmp(long_options[option_index].name, "security-init") == 0) {
                    printf("Initializing security module...\n");
                    FFResult result = ff_security_init();
                    printf("Security init result: %d\n", result);
                } else if (strcmp(long_options[option_index].name, "security-cleanup") == 0) {
                    printf("Cleaning up security module...\n");
                    ff_security_cleanup();
                    printf("Security cleanup done\n");
                } else if (strcmp(long_options[option_index].name, "security-boot-check") == 0) {
                    printf("Performing boot check...\n");
                    bool result = ff_security_boot_check();
                    printf("Boot check result: %s\n", result ? "passed" : "failed");
                } else if (strcmp(long_options[option_index].name, "security-root-check") == 0) {
                    printf("Checking for root/jailbreak...\n");
                    bool is_rooted = ff_security_is_rooted();
                    bool is_jailbroken = ff_security_is_jailbroken();
                    printf("Root detected: %s\n", is_rooted ? "yes" : "no");
                    printf("Jailbreak detected: %s\n", is_jailbroken ? "yes" : "no");
                } else if (strcmp(long_options[option_index].name, "security-mitm-check") == 0) {
                    printf("Checking for MITM attack...\n");
                    bool is_mitm = ff_security_check_mitm();
                    printf("MITM attack detected: %s\n", is_mitm ? "yes" : "no");
                } else if (strcmp(long_options[option_index].name, "security-validate-email") == 0) {
                    printf("Validating email: %s\n", optarg);
                    bool is_valid = ff_security_validate_email(optarg);
                    printf("Email validation result: %s\n", is_valid ? "valid" : "invalid");
                } else if (strcmp(long_options[option_index].name, "security-validate-username") == 0) {
                    printf("Validating username: %s\n", optarg);
                    bool is_valid = ff_security_validate_username(optarg);
                    printf("Username validation result: %s\n", is_valid ? "valid" : "invalid");
                } else if (strcmp(long_options[option_index].name, "security-detect-sql") == 0) {
                    printf("Detecting SQL injection: %s\n", optarg);
                    bool is_sql = ff_security_detect_sql_injection(optarg);
                    printf("SQL injection detected: %s\n", is_sql ? "yes" : "no");
                } else if (strcmp(long_options[option_index].name, "security-detect-xss") == 0) {
                    printf("Detecting XSS: %s\n", optarg);
                    bool is_xss = ff_security_detect_xss(optarg);
                    printf("XSS detected: %s\n", is_xss ? "yes" : "no");
                } else if (strcmp(long_options[option_index].name, "security-get-status") == 0) {
                    printf("Getting security status...\n");
                    FFSecurityStatus status;
                    FFResult result = ff_security_get_status(&status);
                    if (result == FF_OK) {
                        show_security_status(&status);
                    } else {
                        printf("Get security status result: %d\n", result);
                    }
                } else if (strcmp(long_options[option_index].name, "platform-info") == 0) {
                    printf("Platform information:\n");
                    char platform[256];
                    char version[256];
                    if (ff_platform_get_info(platform, sizeof(platform), version, sizeof(version)) == FF_OK) {
                        printf("Platform: %s\n", platform);
                        printf("Version: %s\n", version);
                    } else {
                        printf("Failed to get platform information\n");
                    }
                } else if (strcmp(long_options[option_index].name, "platform-network") == 0) {
                    printf("Network interfaces:\n");
                    // 实现网络接口显示
                    printf("TODO: Network interfaces\n");
                } else if (strcmp(long_options[option_index].name, "platform-system") == 0) {
                    printf("System information:\n");
                    // 实现系统信息显示
                    printf("TODO: System information\n");
                } else if (strcmp(long_options[option_index].name, "set-stun") == 0) {
                    // 解析 STUN 服务器地址
                    char* host = strtok(optarg, ":");
                    char* port_str = strtok(NULL, ":");
                    if (host && port_str) {
                        uint16_t port = atoi(port_str);
                        printf("Setting STUN server: %s:%d\n", host, port);
                        FFResult result = ff_p2p_set_stun_server(host, port);
                        printf("Set STUN server result: %d\n", result);
                    } else {
                        printf("Invalid STUN server format\n");
                    }
                } else if (strcmp(long_options[option_index].name, "get-external-addr") == 0) {
                    printf("Getting external address...\n");
                    char ip[46];
                    uint16_t port;
                    FFResult result = ff_p2p_get_external_address(ip, &port);
                    if (result == FF_OK) {
                        printf("External address: %s:%d\n", ip, port);
                    } else {
                        printf("Get external address result: %d\n", result);
                    }
                } else if (strcmp(long_options[option_index].name, "detect-nat-type") == 0) {
                    printf("Detecting NAT type...\n");
                    P2PNATType nat_type;
                    FFResult result = ff_p2p_detect_nat_type(&nat_type, async_callback, NULL);
                    if (result == FF_OK) {
                        const char* nat_type_str[] = {
                            "OPEN", "FULL_CONE", "RESTRICTED", "PORT_RESTRICTED", "SYMMETRIC"
                        };
                        printf("NAT type: %s\n", nat_type_str[nat_type]);
                    } else {
                        printf("Detect NAT type result: %d\n", result);
                    }
                } else if (strcmp(long_options[option_index].name, "hole-punch") == 0) {
                    uint8_t peer_id[32];
                    if (parse_node_id(optarg, peer_id)) {
                        printf("Performing hole punching...\n");
                        FFResult result = ff_p2p_hole_punch(peer_id, async_callback, NULL);
                        printf("Hole punch result: %d\n", result);
                    } else {
                        printf("Invalid peer ID format\n");
                    }
                } else if (strcmp(long_options[option_index].name, "share-location") == 0) {
                    // 解析参数：FRIEND_ID LAT LON
                    char* friend_id_str = strtok(optarg, " ");
                    char* lat_str = strtok(NULL, " ");
                    char* lon_str = strtok(NULL, " ");
                    if (friend_id_str && lat_str && lon_str) {
                        uint8_t friend_id[32];
                        if (parse_node_id(friend_id_str, friend_id)) {
                            double lat = atof(lat_str);
                            double lon = atof(lon_str);
                            P2PLocation location = {
                                .latitude = lat,
                                .longitude = lon,
                                .timestamp = time(NULL),
                                .is_approximate = true
                            };
                            printf("Sharing location: %.6f, %.6f\n", lat, lon);
                            FFResult result = ff_p2p_share_location(friend_id, &location, async_callback, NULL);
                            printf("Share location result: %d\n", result);
                        } else {
                            printf("Invalid friend ID format\n");
                        }
                    } else {
                        printf("Invalid arguments\n");
                    }
                } else if (strcmp(long_options[option_index].name, "get-friend-location") == 0) {
                    uint8_t friend_id[32];
                    if (parse_node_id(optarg, friend_id)) {
                        printf("Getting friend location...\n");
                        P2PLocation location;
                        FFResult result = ff_p2p_get_friend_location(friend_id, &location, async_callback, NULL);
                        if (result == FF_OK) {
                            printf("Friend location: %.6f, %.6f\n", location.latitude, location.longitude);
                            printf("Timestamp: %llu\n", location.timestamp);
                            printf("Approximate: %s\n", location.is_approximate ? "yes" : "no");
                        } else {
                            printf("Get friend location result: %d\n", result);
                        }
                    } else {
                        printf("Invalid friend ID format\n");
                    }
                } else if (strcmp(long_options[option_index].name, "calculate-distance") == 0) {
                    // 解析参数：LAT1 LON1 LAT2 LON2
                    char* lat1_str = strtok(optarg, " ");
                    char* lon1_str = strtok(NULL, " ");
                    char* lat2_str = strtok(NULL, " ");
                    char* lon2_str = strtok(NULL, " ");
                    if (lat1_str && lon1_str && lat2_str && lon2_str) {
                        double lat1 = atof(lat1_str);
                        double lon1 = atof(lon1_str);
                        double lat2 = atof(lat2_str);
                        double lon2 = atof(lon2_str);
                        P2PLocation loc1 = {.latitude = lat1, .longitude = lon1};
                        P2PLocation loc2 = {.latitude = lat2, .longitude = lon2};
                        double distance;
                        FFResult result = ff_p2p_calculate_distance(&loc1, &loc2, &distance);
                        if (result == FF_OK) {
                            printf("Distance: %.2f meters\n", distance);
                        } else {
                            printf("Calculate distance result: %d\n", result);
                        }
                    } else {
                        printf("Invalid arguments\n");
                    }
                } else if (strcmp(long_options[option_index].name, "send-friend-request") == 0) {
                    // 解析参数：TARGET_ID MESSAGE
                    char* target_id_str = strtok(optarg, " ");
                    char* message = strtok(NULL, "");
                    if (target_id_str && message) {
                        uint8_t target_id[32];
                        if (parse_node_id(target_id_str, target_id)) {
                            printf("Sending friend request...\n");
                            FFResult result = ff_p2p_send_friend_request(target_id, message, async_callback, NULL);
                            printf("Send friend request result: %d\n", result);
                        } else {
                            printf("Invalid target ID format\n");
                        }
                    } else {
                        printf("Invalid arguments\n");
                    }
                } else if (strcmp(long_options[option_index].name, "accept-friend-request") == 0) {
                    uint8_t requester_id[32];
                    if (parse_node_id(optarg, requester_id)) {
                        printf("Accepting friend request...\n");
                        FFResult result = ff_p2p_accept_friend_request(requester_id, async_callback, NULL);
                        printf("Accept friend request result: %d\n", result);
                    } else {
                        printf("Invalid requester ID format\n");
                    }
                } else if (strcmp(long_options[option_index].name, "reject-friend-request") == 0) {
                    uint8_t requester_id[32];
                    if (parse_node_id(optarg, requester_id)) {
                        printf("Rejecting friend request...\n");
                        FFResult result = ff_p2p_reject_friend_request(requester_id, async_callback, NULL);
                        printf("Reject friend request result: %d\n", result);
                    } else {
                        printf("Invalid requester ID format\n");
                    }
                } else if (strcmp(long_options[option_index].name, "get-friends") == 0) {
                    printf("Getting friends list...\n");
                    uint8_t** friend_ids;
                    size_t count;
                    FFResult result = ff_p2p_get_friends(&friend_ids, &count);
                    if (result == FF_OK) {
                        printf("Friends: %zu\n", count);
                        for (size_t i = 0; i < count; i++) {
                            printf("Friend %zu: ", i+1);
                            for (int j = 0; j < 32; j++) {
                                printf("%02x", friend_ids[i][j]);
                            }
                            printf("\n");
                        }
                        ff_p2p_friend_list_free(friend_ids, count);
                    } else {
                        printf("Get friends result: %d\n", result);
                    }
                } else if (strcmp(long_options[option_index].name, "remove-friend") == 0) {
                    uint8_t friend_id[32];
                    if (parse_node_id(optarg, friend_id)) {
                        printf("Removing friend...\n");
                        FFResult result = ff_p2p_remove_friend(friend_id, async_callback, NULL);
                        printf("Remove friend result: %d\n", result);
                    } else {
                        printf("Invalid friend ID format\n");
                    }
                } else if (strcmp(long_options[option_index].name, "logs") == 0) {
                    printf("Showing application logs...\n");
                    // 实现日志显示
                    printf("TODO: Application logs\n");
                } else if (strcmp(long_options[option_index].name, "security-logs") == 0) {
                    printf("Showing security logs...\n");
                    // 实现安全日志显示
                    printf("TODO: Security logs\n");
                } else if (strcmp(long_options[option_index].name, "clear-logs") == 0) {
                    printf("Clearing all logs...\n");
                    // 实现日志清理
                    printf("TODO: Clear logs\n");
                } else if (strcmp(long_options[option_index].name, "test-performance") == 0) {
                    printf("Testing performance...\n");
                    // 实现性能测试
                    printf("TODO: Performance test\n");
                } else if (strcmp(long_options[option_index].name, "test-security") == 0) {
                    printf("Testing security module...\n");
                    // 实现安全测试
                    printf("TODO: Security test\n");
                }
                break;
            default:
                show_help();
                return 1;
        }
    }
    
    if (optind == 1) {
        show_help();
        return 1;
    }
    
    return 0;
}
