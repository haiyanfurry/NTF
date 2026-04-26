# 安全模块说明

## 概述

FindFriend 的安全模块使用 Rust 编写，提供了全面的安全防护功能，保护应用免受各种安全威胁。

## 核心功能

### 1. 启动检查与文件完整性

- **Boot check**：应用启动时验证自身完整性
- **文件完整性**：检查关键文件是否被篡改
- **签名验证**：验证应用签名是否有效

### 2. Root/Jailbreak 检测

- **Root 检测**：检测 Android 设备是否被 Root
- **Jailbreak 检测**：检测 iOS 设备是否被 Jailbreak
- **注入过滤**：防止代码注入攻击

### 3. MITM/SSL/TLS 防护

- **证书验证**：验证 TLS 证书的有效性
- **证书锁定**：防止中间人攻击
- **SSL 固定**：固定服务器证书指纹

### 4. DoS 防护

- **请求限速**：限制单个 IP 的请求频率
- **流量分析**：检测异常流量模式
- **IP 封锁**：临时封锁恶意 IP

### 5. XML/数据包安全检测

- **输入验证**：验证用户输入的合法性
- **SQL 注入检测**：防止 SQL 注入攻击
- **XSS 检测**：防止跨站脚本攻击
- **XML 注入检测**：防止 XML 注入攻击

### 6. 反爬虫

- **请求频率检测**：检测异常的请求频率
- **行为分析**：分析用户行为模式
- **爬虫识别**：识别常见爬虫特征

### 7. 反虚假 GPS

- **速度检测**：检测不 realistic 的移动速度
- **位置突变**：检测位置的突然变化
- **坐标验证**：验证坐标的有效性

### 8. 日志和审计

- **安全日志**：记录安全事件
- **审计日志**：记录关键操作
- **异常检测**：检测异常行为

## 模块结构

```
security_rust/
├─ src/
│  ├─ lib.rs          # 主库文件
│  ├─ memory.rs       # 内存加密
│  ├─ device.rs       # 设备指纹
│  ├─ mitm.rs         # MITM 防护
│  ├─ input.rs        # 输入验证
│  ├─ anti_crawler.rs # 反爬虫
│  ├─ anti_dos.rs     # 反 DoS
│  ├─ anti_fake_gps.rs # 反虚假 GPS
│  └─ root_detect.rs  # Root 检测
└─ ffi/               # C 接口
```

## API 使用

### 初始化与销毁

```c
// 初始化安全模块
FFResult result = ff_security_init();
if (result != FF_OK) {
    printf("Security init failed: %d\n", result);
    return;
}

// 关闭安全模块
ff_security_cleanup();
```

### 启动检查

```c
// 执行启动检查
bool is_valid = ff_security_boot_check();
if (!is_valid) {
    printf("Boot check failed\n");
    return;
}
```

### Root/Jailbreak 检测

```c
// 检测 Root
bool is_rooted = ff_security_is_rooted();
if (is_rooted) {
    printf("Device is rooted\n");
}

// 检测 Jailbreak (iOS)
bool is_jailbroken = ff_security_is_jailbroken();
if (is_jailbroken) {
    printf("Device is jailbroken\n");
}
```

### MITM 防护

```c
// 验证证书
bool is_valid = ff_security_verify_certificate(cert_data, cert_len);
if (!is_valid) {
    printf("Certificate verification failed\n");
}

// 检查 MITM 攻击
bool is_mitm = ff_security_check_mitm();
if (is_mitm) {
    printf("MITM attack detected\n");
}
```

### DoS 防护

```c
// 检测 DoS 攻击
bool is_dos = ff_security_detect_dos(ip_address, request_count, time_window);
if (is_dos) {
    printf("DoS attack detected from %s\n", ip_address);
}

// 检查 IP 是否被封锁
bool is_blocked = ff_security_is_ip_blocked(ip_address);
if (is_blocked) {
    printf("IP %s is blocked\n", ip_address);
}
```

### 输入验证

```c
// 验证邮箱
bool is_valid = ff_security_validate_email(email);
if (!is_valid) {
    printf("Invalid email\n");
}

// 验证用户名
is_valid = ff_security_validate_username(username);
if (!is_valid) {
    printf("Invalid username\n");
}

// 检测 SQL 注入
bool is_sql_injection = ff_security_detect_sql_injection(input);
if (is_sql_injection) {
    printf("SQL injection detected\n");
}

// 检测 XSS
bool is_xss = ff_security_detect_xss(input);
if (is_xss) {
    printf("XSS detected\n");
}
```

### 反爬虫

```c
// 检测爬虫
bool is_crawler = ff_security_detect_crawler(uid, query_count, time_window);
if (is_crawler) {
    printf("Crawler detected for user %u\n", uid);
}

// 添加查询记录
ff_security_add_query(uid);
```

### 反虚假 GPS

```c
// 检测虚假位置
bool is_fake = ff_security_detect_fake_location(
    lat, lon, speed, last_lat, last_lon, time_diff
);
if (is_fake) {
    printf("Fake GPS detected\n");
}

// 添加位置记录
is_fake = ff_security_add_location(uid, lat, lon, speed);
if (is_fake) {
    printf("Fake GPS detected when adding location\n");
}
```

### 内存加密

```c
// 加密内存
void* encrypted_data = ff_security_encrypt_memory(data, data_len, key, key_len);
if (!encrypted_data) {
    printf("Memory encryption failed\n");
}

// 解密内存
void* decrypted_data = ff_security_decrypt_memory(encrypted_data, data_len, key, key_len);
if (!decrypted_data) {
    printf("Memory decryption failed\n");
}
```

### 设备指纹

```c
// 获取设备指纹
uint8_t fingerprint[32];
size_t fingerprint_len = sizeof(fingerprint);
FFResult result = ff_security_get_device_fingerprint(fingerprint, &fingerprint_len);
if (result == FF_OK) {
    printf("Device fingerprint: ");
    for (size_t i = 0; i < fingerprint_len; i++) {
        printf("%02x", fingerprint[i]);
    }
    printf("\n");
}
```

### 日志和审计

```c
// 记录安全事件
ff_security_log_event(FF_SECURITY_EVENT_INFO, "Application started");

// 记录安全警告
ff_security_log_event(FF_SECURITY_EVENT_WARNING, "Suspicious activity detected");

// 记录安全错误
ff_security_log_event(FF_SECURITY_EVENT_ERROR, "Security breach detected");

// 查看安全状态
FFSecurityStatus status;
result = ff_security_get_status(&status);
if (result == FF_OK) {
    printf("Security status: %d\n", status.overall);
    printf("Root detected: %s\n", status.root_detected ? "yes" : "no");
    printf("MITM detected: %s\n", status.mitm_detected ? "yes" : "no");
}
```

## 安全级别

| 级别 | 描述 | 处理方式 |
|------|------|---------|
| `FF_SECURITY_LEVEL_LOW` | 低风险 | 记录警告 |
| `FF_SECURITY_LEVEL_MEDIUM` | 中风险 | 记录警告并采取预防措施 |
| `FF_SECURITY_LEVEL_HIGH` | 高风险 | 记录错误并采取强制措施 |
| `FF_SECURITY_LEVEL_CRITICAL` | 严重风险 | 记录错误并终止操作 |

## 安全事件类型

| 类型 | 描述 | 级别 |
|------|------|------|
| `FF_SECURITY_EVENT_INFO` | 信息性事件 | 低 |
| `FF_SECURITY_EVENT_WARNING` | 警告事件 | 中 |
| `FF_SECURITY_EVENT_ERROR` | 错误事件 | 高 |
| `FF_SECURITY_EVENT_CRITICAL` | 严重事件 | 严重 |

## 最佳实践

### 1. 启动时检查

- 在应用启动时执行完整的安全检查
- 验证应用完整性
- 检测 Root/Jailbreak

### 2. 运行时保护

- 对所有用户输入进行验证
- 监控异常行为
- 定期检查安全状态

### 3. 网络安全

- 验证所有 TLS 证书
- 使用证书锁定
- 监控 MITM 攻击

### 4. 数据保护

- 加密敏感数据
- 使用安全的存储方式
- 定期清理敏感信息

### 5. 日志和审计

- 记录所有安全事件
- 定期分析安全日志
- 及时响应安全警报

## 故障排除

### 安全模块初始化失败

1. **检查 Rust 环境**：确保 Rust 版本 >= 1.60
2. **检查依赖**：确保所有依赖已安装
3. **检查权限**：确保应用有足够的权限

### 误报问题

1. **Root 检测误报**：某些定制 ROM 可能被误判为 Root
2. **虚假 GPS 误报**：快速移动的交通工具可能触发误报
3. **DoS 检测误报**：合法的高频率操作可能被误判

### 性能问题

1. **内存使用**：安全模块可能增加内存使用
2. **CPU 使用率**：某些安全检查可能消耗较多 CPU
3. **启动时间**：启动检查可能增加启动时间

## 性能优化

1. **按需检查**：只在必要时执行安全检查
2. **缓存结果**：缓存重复的安全检查结果
3. **异步执行**：将耗时的安全检查放在后台执行
4. **配置调整**：根据应用场景调整安全级别

## 示例代码

### 完整安全检查

```c
// 初始化安全模块
if (ff_security_init() != FF_OK) {
    printf("Security initialization failed\n");
    return 1;
}

// 执行启动检查
if (!ff_security_boot_check()) {
    printf("Boot check failed\n");
    ff_security_cleanup();
    return 1;
}

// 检测 Root/Jailbreak
if (ff_security_is_rooted() || ff_security_is_jailbroken()) {
    printf("Device is rooted/jailbroken\n");
    // 可以选择退出应用或采取其他措施
}

// 检查 MITM 攻击
if (ff_security_check_mitm()) {
    printf("MITM attack detected\n");
    // 可以选择拒绝网络连接
}

// 获取安全状态
FFSecurityStatus status;
if (ff_security_get_status(&status) == FF_OK) {
    printf("Security status: %d\n", status.overall);
    printf("Root detected: %s\n", status.root_detected ? "yes" : "no");
    printf("MITM detected: %s\n", status.mitm_detected ? "yes" : "no");
}

// 记录安全事件
ff_security_log_event(FF_SECURITY_EVENT_INFO, "Security check completed");

// 关闭安全模块
ff_security_cleanup();
```

### 输入验证

```c
// 验证用户输入
char* email = "user@example.com";
char* username = "user123";
char* password = "Password123!";

if (!ff_security_validate_email(email)) {
    printf("Invalid email\n");
    return;
}

if (!ff_security_validate_username(username)) {
    printf("Invalid username\n");
    return;
}

// 检测 SQL 注入
char* user_input = "' OR 1=1 --";
if (ff_security_detect_sql_injection(user_input)) {
    printf("SQL injection detected\n");
    return;
}

// 检测 XSS
user_input = "<script>alert('XSS')</script>";
if (ff_security_detect_xss(user_input)) {
    printf("XSS detected\n");
    return;
}

printf("All inputs are valid\n");
```

## 结论

FindFriend 的安全模块提供了全面的安全防护功能，使用 Rust 编写确保了内存安全和性能。通过合理使用安全 API，可以构建更加安全、可靠的应用。
