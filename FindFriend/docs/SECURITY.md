# FindFriend 安全策略文档

## 1. 安全架构

### 1.1 纵深防御

```
┌─────────────────────────────────────────────┐
│           Layer 7: Application Security      │
│  - Input Validation                         │
│  - Business Logic Protection                 │
└─────────────────────────────────────────────┘
                     │
┌─────────────────────────────────────────────┐
│           Layer 6: Data Security            │
│  - Encryption                               │
│  - Integrity Checks                         │
│  - Secure Storage                           │
└─────────────────────────────────────────────┘
                     │
┌─────────────────────────────────────────────┐
│           Layer 5: Communication Security    │
│  - TLS/SSL                                  │
│  - Certificate Validation                    │
│  - MITM Prevention                          │
└─────────────────────────────────────────────┘
                     │
┌─────────────────────────────────────────────┐
│           Layer 4: Network Security         │
│  - P2P Encryption                           │
│  - NAT Traversal                            │
│  - DoS Protection                           │
└─────────────────────────────────────────────┘
                     │
┌─────────────────────────────────────────────┐
│           Layer 3: Platform Security        │
│  - Secure Memory                            │
│  - Device Fingerprint                       │
│  - Root/Jailbreak Detection                 │
└─────────────────────────────────────────────┘
```

### 1.2 Rust 安全模块

使用 Rust 实现关键安全功能，利用其内存安全特性：

| 模块 | 功能 | 安全保障 |
|------|------|----------|
| memory | 内存加密 | 防止内存Dump |
| device | 设备指纹 | 防伪造设备 |
| mitm | MITM防护 | 证书验证 |
| integrity | 完整性校验 | CRC64+HMAC |
| location | 位置模糊 | 隐私保护 |

---

## 2. 反黑产策略

### 2.1 防批量注册

```c
// 速率限制
typedef struct RateLimit {
    uint32_t window_seconds;    // 时间窗口（秒）
    uint32_t max_requests;      // 最大请求数
    uint32_t current_count;     // 当前计数
    uint64_t window_start;      // 窗口开始时间
} RateLimit;

// 限制规则
- 同一 IP: 10注册/小时
- 同一设备: 3注册/天
- 同一手机: 5注册/周
```

**检测机制**:
1. IP 频率检测
2. 设备指纹重复检测
3. 短信/邮箱验证
4. 图形验证码（CAPTCHA）

### 2.2 防虚假定位

```c
FFResult ff_security_detect_fake_location(
    double lat,        //上报纬度
    double lon,        //上报经度
    uint32_t uid,      //用户ID
    bool* is_fake      //输出：是否虚假
);
```

**检测方法**:
1. **速度检查** - 如果位置变化速度 > 500km/h，标记为可疑
2. **IP-GPS 一致性** - 比较 IP 地理位置和 GPS 位置
3. **历史轨迹** - 检查位置变化是否合理
4. **网络延迟** - 消息时间戳与位置时间戳一致性

### 2.3 防爬虫扫描

```c
typedef struct CrawlerDetection {
    uint32_t uid;
    uint32_t query_count;        // 查询次数
    uint64_t first_query_time;   // 首次查询时间
    uint32_t same_user_queries;  // 针对同一用户的查询
    bool is_crawler;             // 是否为爬虫
} CrawlerDetection;
```

**特征检测**:
- 短时间内大量查询
- 顺序扫描用户列表
- 异常时间活跃（凌晨批量查询）
- 无正常交互行为

### 2.4 防 DoS 攻击

```c
typedef struct DoSProtection {
    uint32_t max_connections_per_ip;
    uint32_t max_requests_per_second;
    uint32_t max_bandwidth_per_node;
    uint32_t block_duration_seconds;
} DoSProtection;
```

**防护措施**:
- 连接数限制：每IP最多100个连接
- 请求限流：每秒最多1000请求
- 带宽限制：每节点最多10Mbps
- 自动封禁：检测到攻击后封禁30分钟

---

## 3. 输入安全

### 3.1 SQL 注入防护

```c
FFResult ff_security_check_sql_injection(const char* input);
```

**检测模式**:
```
危险关键字:
- UNION, SELECT, INSERT, UPDATE, DELETE, DROP
- --, /*, */
- EXEC, EXECUTE
- CHAR, ASCII
- SLEEP, WAITFOR
```

**防护措施**:
1. 参数化查询
2. 输入白名单验证
3. 特殊字符转义

### 3.2 XSS 防护

```c
FFResult ff_security_check_xss_injection(const char* input);
```

**检测模式**:
```html
危险标签:
- <script>
- <iframe>
- <img onerror>
- javascript:
- onload=, onerror=, onclick=
```

**防护措施**:
1. HTML 实体转义
2. Content-Type 头设置
3. CSP (Content Security Policy)

### 3.3 JSON/XML 注入防护

```c
FFResult ff_security_check_json(const char* json_str);
FFResult ff_security_check_xml(const char* xml_str);
```

**检测内容**:
- JSON: 特殊字符、递归结构、字段长度
- XML: ENTITY 注入、CDATA 注入、Doctype 注入

---

## 4. 通信安全

### 4.1 TLS/SSL

```c
typedef struct TLSConfig {
    const char* ca_file;         // CA 证书
    const char* cert_file;       // 服务器证书
    const char* key_file;        // 私钥
    uint16_t min_tls_version;    // 最低 TLS 版本
    const char* cipher_suites;   // 允许的加密套件
} TLSConfig;
```

**配置要求**:
- TLS 1.2 或更高
- 支持前向保密 (PFS)
- 强加密套件 (AES-256-GCM, ChaCha20)

### 4.2 P2P 通信加密

```
┌─────────────────────────────────────────┐
│       P2P Message Encryption            │
│                                          │
│  Sender                                 │
│    │                                     │
│    │  1. Generate ephemeral key          │
│    │  2. ECDH key exchange              │
│    │  3. Derive session key             │
│    │  4. Encrypt with AES-256-GCM       │
│    │                                     │
│    └──────► Encrypted Message ──────────►│
│                                          │
│  Receiver                               │
│    │                                     │
│    │  1. ECDH key agreement              │
│    │  2. Derive session key              │
│    │  3. Decrypt and verify              │
│    │  4. Check integrity tag             │
│    │                                     │
└─────────────────────────────────────────┘
```

### 4.3 MITM 防护

```c
FFResult ff_security_verify_certificate(
    const char* hostname,
    const uint8_t* cert_data,
    size_t cert_len,
    bool* is_valid
);
```

**防护措施**:
1. 证书固定 (Certificate Pinning)
2. 证书链验证
3. 主机名验证
4. 证书透明度日志检查

---

## 5. 数据安全

### 5.1 存储加密

```c
// 敏感数据加密存储
typedef struct SecureStorage {
    uint8_t master_key[32];       // 主密钥
    uint8_t iv[16];              // 初始向量
    bool is_encrypted;            // 是否加密
} SecureStorage;
```

**加密数据**:
- 用户密码（bcrypt 哈希）
- 认证令牌
- 位置数据
- 消息内容（可选）

### 5.2 完整性校验

```c
FFResult ff_security_compute_integrity_check(
    const uint8_t* data,
    size_t data_len,
    uint8_t* checksum  // 输出: 64-bit CRC
);
```

**校验流程**:
1. 计算 CRC64
2. 计算 HMAC-SHA256
3. 存储校验值
4. 读取时验证

### 5.3 安全删除

```c
// 安全擦除数据
void ff_security_secure_delete(void* ptr, size_t size);
```

**实现**:
1. 用随机数据覆写3次
2. 用0覆写1次
3. 释放内存

---

## 6. 隐私保护

### 6.1 位置隐私

```c
typedef enum FFPrivacyLevel {
    FF_PRIVACY_EXACT = 0,      // 精确（仅好友）
    FF_PRIVACY_FRIEND_FAR = 1, // 模糊1km（好友）
    FF_PRIVACY_NEARBY = 2,     // 模糊5km（附近）
    FF_PRIVACY_HIDDEN = 3      // 完全隐藏
} FFPrivacyLevel;
```

**位置模糊化** (Rust 实现):
```rust
pub fn obfuscate(lat: f64, lon: f64, radius_meters: f64) -> (f64, f64) {
    // 使用高斯随机偏移
    let angle = random_angle();
    let distance = random_distance(radius_meters);

    let offset_lat = (distance * cos(angle)) / 111320.0;
    let offset_lon = (distance * sin(angle)) / (111320.0 * cos(lat * PI / 180.0));

    (lat + offset_lat, lon + offset_lon)
}
```

### 6.2 数据最小化

- 只收集必要数据
- 数据保留期限限制
- 匿名化处理
- 用户数据导出/删除权

---

## 7. 审计日志

### 7.1 日志类型

```c
typedef struct SecurityLog {
    FFSecurityEventType type;
    uint32_t uid;
    char description[512];
    uint64_t timestamp;
    uint8_t severity;  // 0=info, 1=warning, 2=critical
} SecurityLog;
```

**记录事件**:
- 登录尝试（成功/失败）
- 权限变更
- 安全策略变更
- 可疑行为检测
- 管理员操作

### 7.2 告警机制

```c
FFResult ff_security_trigger_alert(const char* message);
```

**告警级别**:
- **低**: 常规日志
- **中**: 需要关注
- **高**: 需要立即处理
- **紧急**: 需要人工介入

---

## 8. 安全配置

### 8.1 配置项

```c
typedef struct SecurityConfig {
    // 密码策略
    uint32_t min_password_length;
    bool require_uppercase;
    bool require_lowercase;
    bool require_digit;
    bool require_special;

    // 会话策略
    uint32_t session_timeout_minutes;
    uint32_t max_sessions_per_user;
    bool require_reauth_for_sensitive;

    // 限流策略
    uint32_t login_max_attempts;
    uint32_t login_lockout_minutes;
    uint32_t api_rate_limit;
} SecurityConfig;
```

### 8.2 默认配置

```json
{
  "security": {
    "password": {
      "min_length": 8,
      "require_uppercase": true,
      "require_lowercase": true,
      "require_digit": true,
      "require_special": true,
      "max_age_days": 90
    },
    "session": {
      "timeout_minutes": 30,
      "max_sessions": 5,
      "require_reauth_sensitive": true
    },
    "rate_limit": {
      "login_attempts": 5,
      "lockout_minutes": 15,
      "api_requests_per_minute": 100
    }
  }
}
```

---

## 9. 合规性

### 9.1 GDPR 合规

- 用户知情同意
- 数据访问权
- 数据删除权 ("被遗忘权")
- 数据可携带权
- 数据保护默认

### 9.2 CCPA 合规

- 知情权
- 选择退出权
- 删除权
- 不受歧视权

---

## 10. 应急响应

### 10.1 事件分级

| 级别 | 描述 | 响应时间 |
|------|------|----------|
| P1 | 数据泄露 | 立即 |
| P2 | 服务中断 | 1小时 |
| P3 | 可疑活动 | 4小时 |
| P4 | 一般事件 | 24小时 |

### 10.2 响应流程

```
事件检测
    │
    ▼
初步评估 ───► 判断级别
    │
    ▼
containment ───► 隔离影响
    │
    ▼
根因分析 ───► 找出漏洞
    │
    ▼
修复部署 ───► 修复问题
    │
    ▼
恢复验证 ───► 确认安全
    │
    ▼
事后复盘 ───► 改进措施
```

---

**文档版本**: 1.0
**更新日期**: 2026-04-26
