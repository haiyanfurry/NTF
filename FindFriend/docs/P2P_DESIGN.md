# FindFriend P2P 架构设计

## 1. P2P 概述

### 1.1 设计目标

- **完全去中心化** - 无需中心服务器
- **高可用** - 节点可以随时加入/离开
- **隐私保护** - 通信内容加密
- **NAT 穿透** - 支持各种网络环境

### 1.2 节点类型

| 类型 | 描述 | 存储需求 | 带宽需求 |
|------|------|----------|----------|
| Full Node | 完整节点，存储所有数据 | 高 | 高 |
| Light Node | 轻量节点，只存储必要数据 | 低 | 低 |
| Relay Node | 中继节点，转发消息 | 中 | 中 |

---

## 2. 节点发现

### 2.1 发现机制

```
┌─────────────────────────────────────────────────────────┐
│                  Node Discovery Flow                     │
│                                                          │
│  Node A ──mDNS Broadcast──► Local Network               │
│                                                          │
│  Node B ◄───────────────────────────── Response          │
│  Node C ◄───────────────────────────── Response          │
│                                                          │
│  Then ─── DHT Bootstrap ───► Known Nodes                │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### 2.2 发现协议

| 阶段 | 协议 | 用途 |
|------|------|------|
| 局域网发现 | mDNS/Bonjour | 同一网络内节点发现 |
| 初始引导 | DNS Seed / Hardcoded | 首次连接引导节点 |
| 节点同步 | DHT (Kademlia) | 广域网节点发现 |

### 2.3 节点信息交换

```c
typedef struct P2PNode {
    uint8_t node_id[32];        // 节点唯一标识
    char public_ip[46];          // 公网IP (IPv6)
    uint16_t public_port;        // 公网端口
    uint32_t uid;               // 关联用户ID
    uint8_t node_type;         // 节点类型
    uint64_t last_seen;         // 最后活跃时间
    int32_t reputation_score;   // 信誉评分
} P2PNode;
```

---

## 3. 连接管理

### 3.1 连接流程

```
Node A                              Node B
   │                                   │
   │  1. NAT Type Detection             │
   │───────────────────────────────────►│
   │                                   │
   │  2. Hole Punching (if needed)     │
   │───────────────────────────────────►│
   │                                   │
   │  3. TCP/UDP Connection Established │
   │═════════════════════════════════════│
   │                                   │
   │  4. Exchange Node Info            │
   │───────────────────────────────────►│
   │                                   │
   │  5. Start P2P Communication       │
   │═════════════════════════════════════│
```

### 3.2 NAT 类型检测

```c
typedef enum P2PNATType {
    P2P_NAT_OPEN = 0,           // 开放 NAT
    P2P_NAT_FULL_CONE = 1,      // 全锥型 NAT
    P2P_NAT_RESTRICTED = 2,     // 受限锥型 NAT
    P2P_NAT_PORT_RESTRICTED = 3,// 端口受限锥型 NAT
    P2P_NAT_SYMMETRIC = 4       // 对称型 NAT
} P2PNATType;
```

### 3.3 NAT 打洞

```
Scenario: Symmetric NAT

Step 1: Both nodes send UDP to STUN server
  Node A ──────────► STUN ──────────► Node A's seen address: 1.2.3.4:1234
  Node B ──────────► STUN ──────────► Node B's seen address: 5.6.7.8:5678

Step 2: Hole punching attempts
  Node A ──UDP to 5.6.7.8:5678──► Node B
  Node B ──UDP to 1.2.3.4:1234───► Node A

Step 3: Success (if timing aligns)
  Both nodes have "holes" punched in their NATs
```

---

## 4. 消息路由

### 4.1 路由算法

使用 Kademlia DHT 进行路由

```
Kademlia XOR Distance:
  distance(A, B) = A XOR B

Example:
  Node A: 0b1011 (11)
  Node B: 0b0110 (6)
  Distance: 0b1101 (13)
```

### 4.2 路由表

每个节点维护 K-Bucket:

```c
typedef struct KBucket {
    uint8_t prefix[16];          // K-Bucket 前缀
    P2PNode* nodes[20];           // 最多20个节点
    uint32_t count;              // 当前节点数
    uint64_t last_updated;       // 最后更新时间
} KBucket;
```

### 4.3 消息转发

```
Source ──► Intermediate Node ──► Intermediate Node ──► Destination
   │              │                      │              │
   │  Find route  │                      │              │
   │────────────► │                      │              │
   │              │  Forward message     │              │
   │              │────────────────────►│              │
   │              │                      │  Forward     │
   │              │                      │────────────►│
```

---

## 5. 数据同步

### 5.1 同步类型

| 类型 | 描述 | 触发条件 |
|------|------|----------|
| 全量同步 | 首次连接，全量下载数据 | 新节点加入 |
| 增量同步 | 只同步变更数据 | 定期/事件触发 |
| 主动推送 | 有变更时主动推送 | 好友状态变化 |

### 5.2 冲突解决

使用向量时钟 (Vector Clock) 解决冲突

```c
typedef struct VectorClock {
    uint32_t node_id;
    uint64_t counter;
} VectorClock;

typedef struct ClockEntry {
    VectorClock clocks[16];  // 最多16个节点
    uint32_t count;
} ClockEntry;
```

**冲突解决策略**:
1. 比较向量时钟
2. 如果一方包含另一方所有时钟，取较大的
3. 如果互相独立，保留两个版本（应用层决定）

---

## 6. 位置共享 P2P

### 6.1 位置更新流程

```
User A                           User B (Friend)
   │                                  │
   │  1. Update location to P2P      │
   │─────────────────────────────────►│
   │                                  │
   │  2. Encrypt with session key    │
   │─────────────────────────────────►│
   │                                  │
   │  3. Verify friendship + permission│
   │                                  │  [Check if A is friend and sharing enabled]
   │                                  │
   │  4. Return exact/fuzzy location  │
   │◄─────────────────────────────────│
   │                                  │
```

### 6.2 位置隐私

- **非好友**: 只显示"附近有人"（不暴露位置）
- **好友**: 显示模糊距离（不显示精确坐标）
- **好友+授权**: 显示精确坐标

```c
FFResult ff_location_obfuscate(
    double exact_lat,      // 精确纬度
    double exact_lon,       // 精确经度
    double radius_meters,   // 模糊半径（默认1000米）
    double* out_lat,        // 输出模糊纬度
    double* out_lon         // 输出模糊经度
);
```

---

## 7. 好友发现 P2P

### 7.1 发现机制

```
┌─────────────────────────────────────────┐
│           Friend Discovery                │
│                                          │
│  User A wants to find friends nearby    │
│                                          │
│  1. Broadcast discovery request         │
│     - Current location (fuzzy)          │
│     - UID hash                          │
│                                          │
│  2. Neighbors receive and check:        │
│     - Is this a friend of theirs?       │
│     - Is this person in my friend list? │
│                                          │
│  3. If match found, notify both users   │
│                                          │
└─────────────────────────────────────────┘
```

### 7.2 匿名发现

为了防止隐私泄露：
- 位置使用模糊坐标
- UID 使用一次性哈希
- 发现结果不持久化

---

## 8. 信誉系统

### 8.1 评分机制

```c
typedef struct ReputationScore {
    uint32_t total_transactions;     // 总交易数
    int32_t positive_feedback;       // 正面反馈
    int32_t negative_feedback;       // 负面反馈
    uint64_t last_activity;          // 最后活跃
    uint8_t trust_level;            // 信任级别 (0-10)
} ReputationScore;
```

### 8.2 评分规则

| 行为 | 分数变化 |
|------|----------|
| 正常消息传递 | +1 |
| 帮助新节点连接 | +5 |
| 提供有效数据 | +3 |
| 恶意行为举报确认 | -10 |
| 多次连接失败 | -2 |
| 发送垃圾消息 | -5 |

---

## 9. 安全考虑

### 9.1 通信加密

- 所有 P2P 通信使用 AES-256-GCM 加密
- 密钥通过 ECDH 交换
- 每个会话生成新的会话密钥

### 9.2 节点验证

- 使用设备指纹验证节点身份
- 定期重新验证防止节点被劫持
- 可信节点白名单

### 9.3 防攻击

- Sybil 攻击：IP 限制 + 信誉评分
- Eclipse 攻击：多个引导节点
- DoS 攻击：限流 + 节点信誉

---

## 10. 性能指标

| 指标 | 目标 |
|------|------|
| 节点发现延迟 | < 500ms (局域网) |
| 消息传递延迟 | < 100ms (局域网) |
| NAT 打洞成功率 | > 80% |
| 节点存储占用 | < 10MB (Light Node) |
| 网络带宽占用 | < 1MB/s (空闲) |

---

**文档版本**: 1.0
**更新日期**: 2026-04-26
