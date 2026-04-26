# P2P 使用说明

## 概述

FindFriend 的 P2P 网络模块提供了去中心化的通信功能，支持节点发现、连接建立、消息传递、位置共享等功能。

## 核心概念

### 节点 (Node)

- 每个客户端都是一个 P2P 节点
- 节点有唯一的 `node_id` (32 字节)
- 节点关联一个用户 `uid`
- 节点可以是 `full` 类型（完整功能）或 `light` 类型（轻量级）

### 连接 (Connection)

- 节点之间建立的 P2P 连接
- 支持 TCP/UDP 混合传输
- 连接状态：`DISCONNECTED`、`CONNECTING`、`CONNECTED`、`ERROR`

### NAT 类型

- `OPEN`：开放 NAT
- `FULL_CONE`：全锥型 NAT
- `RESTRICTED`：受限锥型 NAT
- `PORT_RESTRICTED`：端口受限锥型 NAT
- `SYMMETRIC`：对称型 NAT

## API 使用

### 初始化与销毁

```c
// 初始化 P2P 模块
FFResult result = ff_p2p_init(callback, user_data);

// 检查是否运行中
bool is_running = ff_p2p_is_running();

// 关闭 P2P 模块
result = ff_p2p_shutdown();
```

### 节点管理

```c
// 启动节点
result = ff_p2p_start_node(callback, user_data);

// 停止节点
result = ff_p2p_stop_node();

// 获取节点信息
P2PNode* node_info;
result = ff_p2p_get_node_info(&node_info);
```

### 节点发现

```c
// 开始节点发现
result = ff_p2p_start_discovery(callback, user_data);

// 停止节点发现
result = ff_p2p_stop_discovery();

// 搜索节点
P2PNode* nodes;
size_t count;
result = ff_p2p_search_nodes(criteria, criteria_len, &nodes, &count, callback, user_data);

// 获取已知节点
result = ff_p2p_get_known_nodes(&nodes, &count);
```

### 连接管理

```c
// 连接到节点
result = ff_p2p_connect(peer_id, callback, user_data);

// 断开连接
result = ff_p2p_disconnect(peer_id);

// 获取连接状态
P2PConnectionState state;
result = ff_p2p_get_connection_state(peer_id, &state);

// 获取活跃连接
P2PConnection* connections;
size_t count;
result = ff_p2p_get_active_connections(&connections, &count);
```

### 数据传输

```c
// 发送数据
result = ff_p2p_send(peer_id, data, data_len, callback, user_data);

// 广播数据
result = ff_p2p_broadcast(data, data_len, callback, user_data);

// 接收数据
result = ff_p2p_receive(peer_id, buffer, buffer_size, &received, callback, user_data);
```

### NAT 穿透

```c
// 检测 NAT 类型
P2PNATType nat_type;
result = ff_p2p_detect_nat_type(&nat_type, callback, user_data);

// UDP 打洞
result = ff_p2p_hole_punch(target_peer_id, callback, user_data);

// 设置 STUN 服务器
result = ff_p2p_set_stun_server("stun.l.google.com", 19302);

// 获取公网地址
char ip[46];
uint16_t port;
result = ff_p2p_get_external_address(ip, &port);
```

### 位置共享

```c
// 共享位置
P2PLocation location = {
    .latitude = 37.7749,
    .longitude = -122.4194,
    .timestamp = time(NULL),
    .is_approximate = true
};
result = ff_p2p_share_location(friend_id, &location, callback, user_data);

// 获取好友位置
P2PLocation friend_location;
result = ff_p2p_get_friend_location(friend_id, &friend_location, callback, user_data);

// 计算距离
double distance;
result = ff_p2p_calculate_distance(&my_location, &friend_location, &distance);
```

### 好友管理

```c
// 发送好友请求
result = ff_p2p_send_friend_request(target_id, "Hello, let's be friends!", callback, user_data);

// 接受好友请求
result = ff_p2p_accept_friend_request(requester_id, callback, user_data);

// 拒绝好友请求
result = ff_p2p_reject_friend_request(requester_id, callback, user_data);

// 获取好友列表
uint8_t** friend_ids;
size_t count;
result = ff_p2p_get_friends(&friend_ids, &count);

// 删除好友
result = ff_p2p_remove_friend(friend_id, callback, user_data);
```

## 消息类型

| 类型 | 描述 | 用途 |
|------|------|------|
| `P2P_MSG_DISCOVERY` | 节点发现消息 | 用于发现网络中的其他节点 |
| `P2P_MSG_CONNECT` | 连接请求消息 | 用于建立 P2P 连接 |
| `P2P_MSG_DISCONNECT` | 断开连接消息 | 用于关闭 P2P 连接 |
| `P2P_MSG_DATA` | 通用数据消息 | 用于传输任意数据 |
| `P2P_MSG_LOCATION` | 位置共享消息 | 用于共享位置信息 |
| `P2P_MSG_MESSAGE` | 文本消息 | 用于发送聊天消息 |
| `P2P_MSG_GAME` | 游戏数据消息 | 用于游戏相关数据 |
| `P2P_MSG_MEDIA` | 媒体数据消息 | 用于传输媒体文件 |
| `P2P_MSG_FRIEND_REQUEST` | 好友请求消息 | 用于发送好友请求 |
| `P2P_MSG_FRIEND_ACCEPT` | 好友接受消息 | 用于接受好友请求 |
| `P2P_MSG_FRIEND_REJECT` | 好友拒绝消息 | 用于拒绝好友请求 |

## 安全注意事项

1. **消息加密**：所有 P2P 消息均经过加密传输
2. **节点验证**：连接前验证节点身份
3. **位置保护**：位置信息默认使用模糊处理
4. **好友验证**：只有通过好友验证的节点才能共享位置
5. **流量控制**：防止 DoS 攻击

## 性能优化

1. **连接池**：复用已建立的连接
2. **消息队列**：异步处理消息
3. **NAT 穿透**：优先使用 UDP 打洞
4. **带宽限制**：避免占用过多网络资源
5. **缓存机制**：缓存常用节点信息

## 故障排除

### 连接失败

1. **检查网络**：确保网络连接正常
2. **防火墙**：检查防火墙是否允许 UDP 端口
3. **NAT 类型**：对称型 NAT 可能需要 TURN 服务器
4. **STUN 服务器**：尝试使用不同的 STUN 服务器

### 位置共享失败

1. **权限**：确保已获得位置权限
2. **好友关系**：只有好友才能共享位置
3. **网络**：确保 P2P 连接正常

### 消息发送失败

1. **连接状态**：检查连接是否活跃
2. **网络**：检查网络连接
3. **消息大小**：避免发送过大的消息

## 示例代码

### 基本用法

```c
// 初始化 P2P 模块
FFResult result = ff_p2p_init(NULL, NULL);
if (result != FF_OK) {
    printf("P2P init failed: %d\n", result);
    return;
}

// 启动节点
result = ff_p2p_start_node(NULL, NULL);
if (result != FF_OK) {
    printf("Start node failed: %d\n", result);
    ff_p2p_shutdown();
    return;
}

// 开始节点发现
result = ff_p2p_start_discovery(NULL, NULL);
if (result != FF_OK) {
    printf("Start discovery failed: %d\n", result);
    ff_p2p_stop_node();
    ff_p2p_shutdown();
    return;
}

// 搜索节点
P2PNode* nodes;
size_t count;
result = ff_p2p_search_nodes(NULL, 0, &nodes, &count, NULL, NULL);
if (result == FF_OK) {
    printf("Found %zu nodes\n", count);
    for (size_t i = 0; i < count; i++) {
        printf("Node %zu: %s:%d\n", i+1, nodes[i].public_ip, nodes[i].public_port);
    }
    ff_p2p_node_list_free(nodes, count);
}

// 关闭 P2P 模块
ff_p2p_stop_discovery();
ff_p2p_stop_node();
ff_p2p_shutdown();
```

### 位置共享示例

```c
// 共享位置给好友
P2PLocation location = {
    .latitude = 37.7749,
    .longitude = -122.4194,
    .timestamp = time(NULL),
    .is_approximate = true
};

uint8_t friend_id[32]; // 好友的 node_id
result = ff_p2p_share_location(friend_id, &location, NULL, NULL);
if (result == FF_OK) {
    printf("Location shared successfully\n");
}

// 获取好友位置
P2PLocation friend_location;
result = ff_p2p_get_friend_location(friend_id, &friend_location, NULL, NULL);
if (result == FF_OK) {
    printf("Friend location: %.6f, %.6f\n", 
           friend_location.latitude, friend_location.longitude);
    
    // 计算距离
    double distance;
    result = ff_p2p_calculate_distance(&location, &friend_location, &distance);
    if (result == FF_OK) {
        printf("Distance: %.2f meters\n", distance);
    }
}
```

## 高级功能

### 自定义消息处理

```c
// 注册消息处理器
void message_handler(const uint8_t* from_id, const void* data, size_t data_len) {
    // 处理接收到的消息
    printf("Received message from peer\n");
}

// 设置消息处理器
ff_p2p_set_message_handler(message_handler);
```

### 节点信誉管理

```c
// 更新节点信誉
result = ff_p2p_update_reputation(peer_id, 10); // 增加 10 分

// 获取节点信誉
int32_t score;
result = ff_p2p_get_reputation(peer_id, &score);
if (result == FF_OK) {
    printf("Peer reputation: %d\n", score);
}
```

## 结论

FindFriend 的 P2P 网络模块提供了强大的去中心化通信能力，支持多种平台和场景。通过合理使用 API，可以构建安全、高效的 P2P 应用。
