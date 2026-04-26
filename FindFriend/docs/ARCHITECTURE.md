# FindFriend 架构文档

## 1. 系统架构

### 1.1 分层架构

```
┌─────────────────────────────────────────────────────┐
│                    UI Layer (ui/)                   │
│         Platform-specific UI implementations        │
└─────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────┐
│                Platform Layer (platform/)           │
│    Linux │ Windows │ macOS │ iOS │ Android │ BSD   │
└─────────────────────────────────────────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────┐
│              Interface Layer (core/interface/)       │
│      统一的模块接口，跨模块交互的唯一入口            │
└─────────────────────────────────────────────────────┘
                          │
        ┌─────────────────┼─────────────────┐
        ▼                 ▼                 ▼
┌───────────────┐ ┌───────────────┐ ┌───────────────┐
│ Business Layer│ │  P2P Layer   │ │ Security Layer│
│  core/       │ │   core/p2p/  │ │security_rust/ │
│  business/   │ │              │ │    (Rust)     │
└───────────────┘ └───────────────┘ └───────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────┐
│                  Data Layer (core/data/)            │
│              数据存储、缓存、持久化                   │
└─────────────────────────────────────────────────────┘
```

### 1.2 模块依赖关系

```
ff_core_interface.h
       │
       ├── ff_user_interface.h
       │       │
       │       └── ff_security_interface.h
       │
       ├── ff_message_interface.h
       │       │
       │       ├── ff_p2p_interface.h
       │       │
       │       └── ff_security_interface.h
       │
       ├── ff_location_interface.h
       │       │
       │       ├── ff_p2p_interface.h
       │       │
       │       └── ff_security_interface.h
       │
       ├── ff_game_interface.h
       │
       └── ff_p2p_interface.h
               │
               └── ff_security_interface.h
```

---

## 2. 核心模块设计

### 2.1 接口层 (core/interface/)

**设计原则**:
- 所有模块通过接口层交互
- 禁止跨模块直接调用内部函数
- 统一错误码和回调机制
- 支持同步/异步两种调用模式

**主要接口**:

```c
// 初始化
FFResult ff_module_init(void);
FFResult ff_module_destroy(void);

// 异步调用
FFResult ff_async_call(FFAsyncCallback callback, void* user_data);

// 事件订阅
FFResult ff_event_subscribe(FFEventType type, FFEventCallback callback);
```

### 2.2 业务层 (core/business/)

**user_manager** - 用户管理
- 注册/登录/登出
- 用户信息 CRUD
- 密码安全管理

**message_manager** - 消息系统
- P2P 消息发送
- 消息状态同步
- 离线消息缓存

**social_manager** - 社交关系
- 好友请求/接受/拒绝
- 黑名单管理
- 好友在线状态

**location_service** - 位置服务
- 位置上报
- 附近用户发现
- 位置隐私控制

**game_manager** - 游戏模块
- 游戏创建/加入/离开
- 积分系统
- 排行榜

### 2.3 P2P 层 (core/p2p/)

**设计目标**: 完全去中心化，无需中心服务器

```
┌─────────┐     ┌─────────┐     ┌─────────┐
│ Node A  │◄──►│ Node B  │◄──►│ Node C  │
└─────────┘     └─────────┘     └─────────┘
     │               │               │
     └───────────────┼───────────────┘
                     │
              Peer Discovery
              (mDNS / DHT)
```

**子模块**:

| 模块 | 功能 |
|------|------|
| node_discovery | 节点发现、广播 |
| connection | TCP/UDP 连接管理 |
| message_routing | 消息路由、转发 |
| nat_traversal | NAT 类型检测、打洞 |

### 2.4 安全层 (security_rust/)

Rust 实现，提供最高级别的安全保障

| 模块 | 功能 |
|------|------|
| memory | 内存加密，防止内存Dump |
| device | 设备指纹，防伪造 |
| mitm | MITM 攻击防护 |
| integrity | CRC64 + HMAC 数据完整性 |
| location | 位置模糊化，保护隐私 |

---

## 3. 数据流设计

### 3.1 用户登录流程

```
User App                    Core                    P2P Network
   │                         │                         │
   │  ff_user_login()        │                         │
   │────────────────────────►│                         │
   │                         │                         │
   │                         │  Verify credentials      │
   │                         │─────────────────────────►│
   │                         │                         │
   │                         │  Token + Session key     │
   │                         │◄─────────────────────────│
   │                         │                         │
   │  Login success          │                         │
   │◄────────────────────────│                         │
   │                         │                         │
```

### 3.2 消息发送流程

```
Sender                    P2P Network               Receiver
   │                         │                         │
   │  ff_message_send()      │                         │
   │────────────────────────►│                         │
   │                         │                         │
   │                         │  Route to peer          │
   │                         │────────────────────────►│
   │                         │                         │
   │  Message sent           │                         │
   │◄────────────────────────│                         │
   │                         │                         │
```

### 3.3 位置共享流程

```
User A                   Location Service            User B
   │                            │                       │
   │  ff_location_update()      │                       │
   │───────────────────────────►│                       │
   │                            │                       │
   │                            │  Check friendship     │
   │                            │──────────────────────►│
   │                            │                       │
   │                            │  If friend:            │
   │                            │  Return exact coords  │
   │                            │◄──────────────────────│
   │                            │                       │
   │                            │  If not friend:       │
   │                            │  Return fuzzy coords  │
   │                            │◄──────────────────────│
```

---

## 4. 内存管理

### 4.1 内存布局

```
┌─────────────────────────────────────────┐
│           Code Segment (.text)          │
├─────────────────────────────────────────┤
│         Read-Only Data (.rodata)        │
├─────────────────────────────────────────┤
│              Data Segment               │
│  ┌─────────────────────────────────┐   │
│  │      Static / Global Variables   │   │
│  └─────────────────────────────────┘   │
├─────────────────────────────────────────┤
│              Heap (malloc/free)         │
│  ┌──────────┐  ┌──────────┐            │
│  │ Module A │  │ Module B │            │
│  └──────────┘  └──────────┘            │
├─────────────────────────────────────────┤
│              Stack (local vars)         │
│  ┌─────────────────────────────────┐   │
│  │  Function calls, local buffers  │   │
│  └─────────────────────────────────┘   │
└─────────────────────────────────────────┘
```

### 4.2 内存安全策略

- 使用内存池减少碎片
- 敏感数据使用后立即清零
- 禁止使用未初始化变量
- 数组访问边界检查

---

## 5. 并发模型

### 5.1 线程架构

```
┌─────────────────────────────────────────┐
│              Main Thread                │
│  - UI event loop                       │
│  - User input handling                 │
│  - Display updates                     │
└─────────────────────────────────────────┘
                    │
        ┌───────────┼───────────┐
        ▼           ▼           ▼
┌───────────┐ ┌───────────┐ ┌───────────┐
│  P2P      │ │  Worker  │ │  Timer    │
│  Thread    │ │  Thread  │ │  Thread   │
│  - Network │ │  - Crypto│ │  - Retry  │
│  - Events  │ │  - Hash  │ │  - Keep   │
│            │ │  - Parse │ │    alive  │
└───────────┘ └───────────┘ └───────────┘
```

### 5.2 同步机制

| 场景 | 机制 |
|------|------|
| 共享数据访问 | pthread_mutex |
| 读多写少 | pthread_rwlock |
| 事件通知 | pthread_cond |
| 超时等待 | sem_timedwait |

---

## 6. 错误处理

### 6.1 统一错误码

```c
typedef enum FFResult {
    FF_OK = 0,
    FF_ERROR_INVALID_PARAM = -1,
    FF_ERROR_NO_MEMORY = -2,
    FF_ERROR_NOT_FOUND = -3,
    FF_ERROR_PERMISSION = -4,
    FF_ERROR_NETWORK = -6,
    FF_ERROR_SECURITY = -7,
    // ...
} FFResult;
```

### 6.2 错误传播

```
Layer 1          Layer 2          Layer 3
   │                │                │
   │  error         │                │
   │───────►        │                │
   │                │                │
   │         Error  │                │
   │         wrapper│                │
   │         ◄──────│                │
   │                │                │
   │                │  Error + context
   │                │────────────────►│
   │                │                │
```

---

## 7. 扩展性设计

### 7.1 模块热插拔

```c
FFResult ff_module_register(const FFModuleDescriptor* module);
FFResult ff_module_unregister(const char* module_name);
```

### 7.2 插件系统

未来支持动态加载业务插件

---

**文档版本**: 1.0
**更新日期**: 2026-04-26
