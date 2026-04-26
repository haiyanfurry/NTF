# FindFriend P2P 完整指南

## 概述

FindFriend 的 P2P 网络模块实现了完全去中心化的社交网络功能，包括节点发现、连接管理、NAT 穿透、消息传递、位置共享等功能。

## 架构设计

### P2P 网络拓扑

```
┌─────────────────────────────────────────────────────────────┐
│                        P2P Network                          │
│  ┌─────┐    ┌─────┐    ┌─────┐    ┌─────┐    ┌─────┐     │
│  │Node1│───│Node2│───│Node3│───│Node4│───│Node5│     │
│  └──┬──┘    └──┬──┘    └──┬──┘    └──┬──┘    └──┬──┘     │
│     │          │          │          │          │          │
│  ┌──┴──┐    ┌──┴──┐    ┌──┴──┐    ┌──┴──┐    ┌──┴──┐     │
│  │User │    │User │    │User │    │User │    │User │     │
│  │  A  │    │  B  │    │  C  │    │  D  │    │  E  │     │
│  └─────┘    └─────┘    └─────┘    └─────┘    └─────┘     │
└─────────────────────────────────────────────────────────────┘
         │                    │                    │
    Location              Message              Game
    Sharing               Chat                 Play
```

### 模块结构

```
common_core/p2p/
├─ node_discovery.c      # 节点发现
├─ connection.c           # 连接管理
├─ message_routing.c      # 消息路由
├─ nat_traversal.c        # NAT 穿透
├─ stun_client.c          # STUN 客户端
├─ turn_client.c          # TURN 中继
├─ hole_punch.c           # UDP 打洞
├─ friend.c               # 好友管理
└─ location.c             # 位置共享
```

## 核心概念

### 节点 (Node)

每个 FindFriend 用户都是一个 P2P 节点：

```c
typedef struct P2PNode {
    uint8_t node_id[32];        // 唯一标识符 (SHA-256)
    char public_ip[46];         // 公网 IP (支持 IPv6)
    uint16_t public_port;        // 公网端口
    uint32_t uid;               // 用户 ID
    uint8_t node_type;          // 0=full, 1=light
    uint64_t last_seen;         // 最后活跃时间
    int32_t reputation_score;   // 信誉评分
    uint8_t status;             // 0=offline, 1=online, 2=busy
} P2PNode;
```

### 连接状态

```c
typedef enum P2PConnectionState {
    P2P_STATE_DISCONNECTED = 0,  // 未连接
    P2P_STATE_CONNECTING = 1,     // 连接中
    P2P_STATE_CONNECTED = 2,     // 已连接
    P2P_STATE_ERROR = 3          // 错误
} P2PConnectionState;
```

### NAT 类型

```c
typedef enum P2PNATType {
    P2P_NAT_OPEN = 0,              // 开放 NAT (无 NAT)
    P2P_NAT_FULL_CONE = 1,         // 全锥型 NAT
    P2P_NAT_RESTRICTED = 2,       // 受限锥型 NAT
    P2P_NAT_PORT_RESTRICTED = 3,  // 端口受限锥型 NAT
    P2P_NAT_SYMMETRIC = 4          // 对称型 NAT
} P2PNATType;
```

## NAT 穿透

### STUN 协议

STUN (Session Traversal Utilities for NAT) 用于获取公网地址：

```c
// 初始化 STUN 客户端
ff_stun_init();

// 检测 NAT 类型
P2PNATType nat_type;
ff_stun_detect_nat_type("stun.l.google.com", 19302, &nat_type);

// 获取公网地址
char external_ip[16];
uint16_t external_port;
ff_stun_get_external_address("stun.l.google.com", 19302,
                             external_ip, &external_port);
```

### TURN 中继

TURN (Traversal Using Relays around NAT) 用于对称型 NAT：

```c
// 初始化 TURN 客户端
ff_turn_init("turn.example.com", 3478, "username", "password");

// 分配中继地址
char relayed_ip[16];
uint16_t relayed_port;
ff_turn_allocate(sockfd, relayed_ip, &relayed_port, NULL, NULL);

// 发送数据
ff_turn_send_data(sockfd, data, data_len, peer_ip, peer_port);
```

### UDP Hole Punching

UDP 打洞是最常用的 NAT 穿透方法：

```c
// 执行打洞
ff_hole_punch_perform(peer_id, peer_ip, peer_port,
                      "stun.l.google.com", 19302,
                      callback, user_data);

// 检查打洞结果
bool success = ff_hole_punch_check_success(peer_id);
```

### 穿透流程

```
1. 客户端 A 和 B 同时向 STUN 服务器发送请求
   A -> STUN: 请求公网地址
   B -> STUN: 请求公网地址

2. STUN 服务器返回公网地址
   STUN -> A: 1.2.3.4:5000
   STUN -> B: 5.6.7.8:6000

3. 服务器通知双方对方的地址
   Server -> A: B 的地址是 5.6.7.8:6000
   Server -> B: A 的地址是 1.2.3.4:5000

4. 双方同时向对方发送 UDP 数据包
   A -> B: UDP packet (建立映射)
   B -> A: UDP packet (建立映射)

5. NAT 为双方创建临时映射，允许直接通信
```

## 好友管理

### 数据结构

```c
typedef struct {
    uint8_t uid[32];           // 好友 UID
    char nickname[128];        // 昵称
    uint64_t added_time;       // 添加时间
    uint8_t status;            // 状态 (在线/离线/忙碌)
    P2PLocation last_location; // 最后位置
    bool location_shared;      // 是否共享位置
} FriendInfo;
```

### 好友请求

```c
// 发送好友请求
ff_friend_send_request(target_uid, "Hello, let's be friends!");

// 接受好友请求
ff_friend_accept_request(requester_uid);

// 拒绝好友请求
ff_friend_reject_request(requester_uid);

// 获取待处理请求
FriendRequest* requests;
size_t count;
ff_friend_get_pending_requests(&requests, &count);
```

### 好友列表

```c
// 获取所有好友
FriendInfo* friends;
size_t count;
ff_friend_get_all(&friends, &count);

// 检查是否为好友
bool is_friend;
ff_friend_is_friend(uid, &is_friend);

// 移除好友
ff_friend_remove(friend_uid);
```

## 位置共享

### 位置隐私

- **精确位置**：只有主动分享时才发送给好友
- **模糊位置**：可以共享大致位置（1km 半径）
- **距离计算**：可以计算与好友的距离而不获取精确位置

### 位置数据结构

```c
typedef struct P2PLocation {
    double latitude;           // 纬度 (-90 ~ 90)
    double longitude;          // 经度 (-180 ~ 180)
    uint64_t timestamp;        // 时间戳
    bool is_approximate;       // 是否为模糊位置
} P2PLocation;
```

### 更新位置

```c
// 更新自己的位置
ff_location_update(37.7749, -122.4194, false);  // 精确位置
ff_location_update(37.7749, -122.4194, true);   // 模糊位置

// 共享位置给好友
ff_location_share_with_friend(friend_uid, callback, user_data);

// 共享模糊位置
ff_location_share_blurred(friend_uid, 1.0, callback, user_data);  // 1km 半径
```

### 获取好友位置

```c
// 获取好友位置
P2PLocation location;
bool is_approximate;
ff_location_get_friend_location(friend_uid, &location, &is_approximate);

// 计算与好友的距离
double distance;
ff_location_calculate_distance(&my_location, &friend_location, &distance);

// 格式化距离显示
char* distance_str = ff_location_format_distance(distance);
char* approx_str = ff_location_format_approximate_distance(distance);
```

### 距离计算算法

使用 Haversine 公式计算两点间的球面距离：

```c
double ff_location_calculate_distance(const P2PLocation* loc1, const P2PLocation* loc2) {
    const double R = 6371.0;  // 地球半径 (km)

    double lat1_rad = loc1->latitude * M_PI / 180.0;
    double lat2_rad = loc2->latitude * M_PI / 180.0;
    double delta_lat = (loc2->latitude - loc1->latitude) * M_PI / 180.0;
    double delta_lon = (loc2->longitude - loc1->longitude) * M_PI / 180.0;

    double a = sin(delta_lat / 2.0) * sin(delta_lat / 2.0) +
               cos(lat1_rad) * cos(lat2_rad) *
               sin(delta_lon / 2.0) * sin(delta_lon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return R * c * 1000.0;  // 返回米
}
```

## 消息传递

### 消息类型

```c
typedef enum P2PMessageType {
    P2P_MSG_DISCOVERY = 0,      // 节点发现
    P2P_MSG_CONNECT = 1,        // 连接请求
    P2P_MSG_DISCONNECT = 2,     // 断开连接
    P2P_MSG_DATA = 3,           // 通用数据
    P2P_MSG_LOCATION = 4,       // 位置消息
    P2P_MSG_MESSAGE = 5,         // 文本消息
    P2P_MSG_GAME = 6,            // 游戏数据
    P2P_MSG_MEDIA = 7,           // 媒体数据
    P2P_MSG_FRIEND_REQUEST = 8,  // 好友请求
    P2P_MSG_FRIEND_ACCEPT = 9,  // 好友接受
    P2P_MSG_FRIEND_REJECT = 10  // 好友拒绝
} P2PMessageType;
```

### 发送消息

```c
// 发送数据到好友
ff_p2p_send_to_friend(uid, data, data_len, callback, user_data);

// 广播数据
ff_p2p_broadcast(data, data_len, callback, user_data);
```

### 接收消息

```c
// 接收来自好友的消息
void buffer[1024];
size_t received;
ff_p2p_receive_from_friend(uid, buffer, sizeof(buffer), &received, callback, user_data);
```

## API 参考

### 初始化

```c
FFResult ff_p2p_init(FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_shutdown(void);
bool ff_p2p_is_running(void);
```

### 节点管理

```c
FFResult ff_p2p_start_node(FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_stop_node(void);
FFResult ff_p2p_get_node_info(P2PNode** node_info);
```

### 连接管理

```c
FFResult ff_p2p_connect(const uint8_t* peer_id, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_disconnect(const uint8_t* peer_id);
FFResult ff_p2p_get_connection_state(const uint8_t* peer_id, P2PConnectionState* state);
```

### NAT 穿透

```c
FFResult ff_p2p_detect_nat_type(P2PNATType* nat_type, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_hole_punch(const uint8_t* target_peer_id, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_set_stun_server(const char* stun_host, uint16_t stun_port);
FFResult ff_p2p_get_external_address(char* ip, uint16_t* port);
```

### 好友 P2P 连接

```c
FFResult ff_p2p_connect_friend(uint32_t uid, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_send_to_friend(uint32_t uid, const void* data, size_t data_len,
                                FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_receive_from_friend(uint32_t uid, void* buffer, size_t buffer_size,
                                     size_t* received, FFAsyncCallback callback, void* user_data);
```

### 位置共享

```c
FFResult ff_p2p_share_location(const uint8_t* friend_id, const P2PLocation* location,
                                FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_get_friend_location(const uint8_t* friend_id, P2PLocation* location,
                                    FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_calculate_distance(const P2PLocation* loc1, const P2PLocation* loc2,
                                   double* distance);
```

## 使用示例

### 完整初始化流程

```c
// 1. 初始化 P2P 模块
ff_p2p_init(NULL, NULL);

// 2. 启动节点
ff_p2p_start_node(NULL, NULL);

// 3. 设置 STUN 服务器
ff_p2p_set_stun_server("stun.l.google.com", 19302);

// 4. 检测 NAT 类型
P2PNATType nat_type;
ff_p2p_detect_nat_type(&nat_type, NULL, NULL);
printf("NAT Type: %d\n", nat_type);

// 5. 开始节点发现
ff_p2p_start_discovery(NULL, NULL);

// 6. 搜索好友
P2PNode* nodes;
size_t count;
ff_p2p_search_nodes(criteria, criteria_len, &nodes, &count, NULL, NULL);

// 7. 连接到好友
ff_p2p_connect_friend(friend_uid, NULL, NULL);

// 8. 发送消息
const char* message = "Hello!";
ff_p2p_send_to_friend(friend_uid, message, strlen(message), NULL, NULL);

// 9. 共享位置
P2PLocation my_location = {
    .latitude = 37.7749,
    .longitude = -122.4194,
    .timestamp = time(NULL),
    .is_approximate = false
};
ff_p2p_share_location(friend_uid, &my_location, NULL, NULL);

// 10. 清理
ff_p2p_shutdown();
```

### 位置共享完整示例

```c
// 1. 初始化位置模块
ff_location_init();

// 2. 更新自己的位置
ff_location_update(37.7749, -122.4194, true);  // 模糊位置

// 3. 共享给好友
uint8_t friend_uid[32] = { /* ... */ };
ff_location_share_with_friend(friend_uid, NULL, NULL);

// 4. 获取好友位置
P2PLocation friend_location;
bool is_approx;
ff_location_get_friend_location(friend_uid, &friend_location, &is_approx);
printf("Friend location: %.4f, %.4f (approximate: %s)\n",
       friend_location.latitude, friend_location.longitude,
       is_approx ? "yes" : "no");

// 5. 计算距离
double distance_m;
P2PLocation my_location;
ff_location_get_mine(&my_location);
ff_location_calculate_distance(&my_location, &friend_location, &distance_m);
printf("Distance: %s\n", ff_location_format_distance(distance_m));

// 6. 获取模糊距离描述
printf("Approximate distance: %s\n",
       ff_location_format_approximate_distance(distance_m));

// 7. 清理
ff_location_cleanup();
```

## 安全性

### 好友验证

- 所有位置共享仅限好友
- 使用 UID 而非 IP 地址保护隐私
- 消息加密传输

### 防伪造

- 位置数据签名验证
- 时间戳检查防止重放攻击
- 速度检测防止虚假 GPS

### 隐私保护

- 位置模糊处理
- 距离计算不暴露精确位置
- 可随时关闭位置共享

## 性能优化

### 连接池

```c
// 维护活跃连接池
#define MAX_CONNECTIONS 32
P2PConnection g_connection_pool[MAX_CONNECTIONS];
```

### 消息队列

```c
// 异步消息处理
typedef struct {
    void* data;
    size_t len;
    uint8_t priority;
} MessageQueue;
```

### NAT 映射缓存

```c
// 缓存 NAT 映射减少打洞次数
typedef struct {
    uint8_t peer_id[32];
    char mapped_ip[16];
    uint16_t mapped_port;
    uint64_t expire_time;
} NATMappingCache;
```

## 故障排除

### 连接失败

1. **检查 NAT 类型**：
   ```bash
   ff_cli --detect-nat-type
   ```

2. **测试 STUN 连接**：
   ```bash
   nc -u stun.l.google.com 19302
   ```

3. **检查防火墙**：
   ```bash
   # 确保 UDP 端口开放
   sudo pfctl -s all
   ```

### 位置共享失败

1. **确认是好友**：
   ```bash
   ff_cli --get-friends
   ```

2. **检查位置权限**：
   ```bash
   ff-cli --platform-info
   ```

### 消息发送失败

1. **检查连接状态**：
   ```bash
   ff-cli --p2p-get-connections
   ```

2. **重新连接**：
   ```bash
   ff-cli --p2p-disconnect <peer_id>
   ff-cli --p2p-connect <peer_id>
   ```

## 最佳实践

### 开发环境

1. 使用本地 STUN 服务器进行测试
2. 使用 Debug 模式查看详细日志
3. 模拟不同 NAT 类型环境

### 生产环境

1. 部署可靠的 STUN/TURN 服务器
2. 启用日志和监控
3. 定期更新安全证书
4. 实现连接健康检查

### 移动端

1. 处理网络切换（WiFi/蜂窝）
2. 实现低功耗模式
3. 优化后台连接保活
