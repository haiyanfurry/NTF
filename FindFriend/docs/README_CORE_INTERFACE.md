# FindFriend 核心接口说明

## 概述

本文档详细说明 FindFriend 项目的核心接口设计，包括模块划分、接口定义、数据结构等。

## 模块架构

### 核心模块 (common_core/)

核心模块使用 C99 编写，提供以下子模块：

```
common_core/
├─ interface/      # 接口定义
├─ p2p/            # P2P 网络模块
├─ business/        # 业务逻辑
├─ data/           # 数据管理
└─ utils/          # 工具函数
```

### 接口层级

1. **核心接口** (`ff_core_interface.h`)
   - 基础类型定义
   - 错误码定义
   - 异步回调定义

2. **P2P 接口** (`ff_p2p_interface.h`)
   - 节点管理
   - 连接管理
   - 消息路由

3. **安全接口** (`ff_security_interface.h`)
   - 安全检查
   - 加密解密
   - 完整性验证

4. **平台接口** (`ff_platform_interface.h`)
   - 平台初始化
   - 系统信息
   - 网络接口

## 核心接口详细说明

### 基础类型

```c
// 结果类型
typedef enum {
    FF_OK = 0,
    FF_ERROR = 1,
    FF_ERROR_INVALID_PARAM = 2,
    FF_ERROR_NOT_INITIALIZED = 3,
    FF_ERROR_NOT_FOUND = 4,
    FF_ERROR_ALREADY_INITIALIZED = 5,
    FF_ERROR_NO_MEMORY = 6,
    FF_ERROR_TIMEOUT = 7,
    FF_ERROR_NETWORK = 8,
    FF_ERROR_PERMISSION = 9
} FFResult;

// 异步回调
typedef void (*FFAsyncCallback)(FFResult result, void* user_data);
```

### P2P 节点

```c
typedef struct P2PNode {
    uint8_t node_id[32];        // 节点唯一标识
    char public_ip[46];         // IPv6 支持
    uint16_t public_port;
    uint32_t uid;               // 关联的用户ID
    uint8_t node_type;          // 0=full, 1=light
    uint64_t last_seen;
    int32_t reputation_score;   // 信誉评分
    uint8_t status;             // 0=offline, 1=online, 2=busy
} P2PNode;
```

### P2P 连接

```c
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
```

### NAT 类型

```c
typedef enum P2PNATType {
    P2P_NAT_OPEN = 0,
    P2P_NAT_FULL_CONE = 1,
    P2P_NAT_RESTRICTED = 2,
    P2P_NAT_PORT_RESTRICTED = 3,
    P2P_NAT_SYMMETRIC = 4
} P2PNATType;
```

### 位置信息

```c
typedef struct P2PLocation {
    double latitude;
    double longitude;
    uint64_t timestamp;
    bool is_approximate;
} P2PLocation;
```

### 消息类型

```c
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
```

## 接口函数列表

### 初始化与销毁

```c
// 核心初始化
FFResult ff_core_init(void);
void ff_core_destroy(void);

// P2P 初始化
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

### 节点发现

```c
FFResult ff_p2p_start_discovery(FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_stop_discovery(void);
FFResult ff_p2p_search_nodes(const uint8_t* criteria, size_t criteria_len,
                             P2PNode** nodes, size_t* count,
                             FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_get_known_nodes(P2PNode** nodes, size_t* count);
```

### 连接管理

```c
FFResult ff_p2p_connect(const uint8_t* peer_id, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_disconnect(const uint8_t* peer_id);
FFResult ff_p2p_get_connection_state(const uint8_t* peer_id, P2PConnectionState* state);
FFResult ff_p2p_get_active_connections(P2PConnection** connections, size_t* count);
```

### 数据传输

```c
FFResult ff_p2p_send(const uint8_t* peer_id, const void* data, size_t data_len,
                     FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_broadcast(const void* data, size_t data_len,
                          FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_receive(const uint8_t* peer_id, void* buffer, size_t buffer_size,
                        size_t* received, FFAsyncCallback callback, void* user_data);
```

### NAT 穿透

```c
FFResult ff_p2p_detect_nat_type(P2PNATType* nat_type, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_hole_punch(const uint8_t* target_peer_id, FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_set_stun_server(const char* stun_host, uint16_t stun_port);
FFResult ff_p2p_get_external_address(char* ip, uint16_t* port);
```

### 好友管理

```c
FFResult ff_p2p_send_friend_request(const uint8_t* target_id, const char* message,
                                    FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_accept_friend_request(const uint8_t* requester_id,
                                      FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_reject_friend_request(const uint8_t* requester_id,
                                      FFAsyncCallback callback, void* user_data);
FFResult ff_p2p_get_friends(uint8_t*** friend_ids, size_t* count);
FFResult ff_p2p_remove_friend(const uint8_t* friend_id, FFAsyncCallback callback, void* user_data);
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

## 错误码说明

| 错误码 | 说明 |
|--------|------|
| FF_OK | 成功 |
| FF_ERROR | 通用错误 |
| FF_ERROR_INVALID_PARAM | 无效参数 |
| FF_ERROR_NOT_INITIALIZED | 未初始化 |
| FF_ERROR_NOT_FOUND | 未找到 |
| FF_ERROR_ALREADY_INITIALIZED | 已初始化 |
| FF_ERROR_NO_MEMORY | 内存不足 |
| FF_ERROR_TIMEOUT | 超时 |
| FF_ERROR_NETWORK | 网络错误 |
| FF_ERROR_PERMISSION | 权限错误 |

## 使用示例

### 基本初始化

```c
// 初始化核心
if (ff_core_init() != FF_OK) {
    fprintf(stderr, "Core init failed\n");
    return 1;
}

// 初始化 P2P
if (ff_p2p_init(NULL, NULL) != FF_OK) {
    fprintf(stderr, "P2P init failed\n");
    ff_core_destroy();
    return 1;
}

// 启动节点
if (ff_p2p_start_node(NULL, NULL) != FF_OK) {
    fprintf(stderr, "Start node failed\n");
    ff_p2p_shutdown();
    ff_core_destroy();
    return 1;
}
```

### 发送消息

```c
uint8_t peer_id[32] = { /* ... */ };
const char* message = "Hello, world!";
FFResult result = ff_p2p_send(peer_id, message, strlen(message), NULL, NULL);
if (result != FF_OK) {
    fprintf(stderr, "Send failed: %d\n", result);
}
```

### 位置共享

```c
uint8_t friend_id[32] = { /* ... */ };
P2PLocation my_location = {
    .latitude = 37.7749,
    .longitude = -122.4194,
    .timestamp = time(NULL),
    .is_approximate = false
};

FFResult result = ff_p2p_share_location(friend_id, &my_location, NULL, NULL);
if (result == FF_OK) {
    printf("Location shared successfully\n");
}
```

## 跨平台接口

所有平台接口必须实现以下功能：

1. **平台初始化**：`platform_init()`
2. **平台清理**：`platform_cleanup()`
3. **事件轮询**：`platform_poll_events()`
4. **系统信息**：`platform_get_system_info()`
5. **网络接口**：`platform_get_network_interfaces()`

## 线程安全

- 所有接口函数均为线程安全
- 使用互斥锁保护共享数据
- 异步操作使用回调机制

## 内存管理

- 调用者负责分配和释放内存
- 接口返回的数据需要调用者释放
- 使用 `_free` 后缀函数释放内存

## 版本历史

| 版本 | 日期 | 说明 |
|------|------|------|
| 1.0 | 2024-01-01 | 初始版本 |
| 2.0 | 2024-04-01 | 添加 NAT 穿透 |
| 3.0 | 2024-07-01 | 添加好友管理 |
| 4.0 | 2024-10-01 | 添加位置共享 |
| 5.0 | 2025-01-01 | 商业级架构 |
