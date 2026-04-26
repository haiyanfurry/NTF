# FindFriend C <-> Rust 混合安全架构

## 📋 概述

FindFriend 现在是一个混合架构项目，使用 C 作为核心框架，Rust 提供高安全性的关键模块。

### 架构优势

- ✅ **安全性**: Rust 模块提供内存安全保证
- ✅ **兼容性**: 保持与所有现有平台兼容
- ✅ **模块化**: Rust 模块可独立测试和部署
- ✅ **渐进式**: 可选择性启用/禁用 Rust 功能

---

## 📁 目录结构

```
汇总/
├── common_core/
│   ├── ff_safe_utils.h/c         # C 代码质量优化（字符串、内存、日志）
│   ├── ff_rust_integration_test.c  # C <-> Rust 集成测试
│   ├── core_interface.h/c          # 统一核心接口
│   ├── core_cli.c                  # CLI 调试工具
│   ├── auth/                       # 认证模块
│   ├── data/                       # 数据管理
│   ├── business/                   # 业务逻辑
│   ├── network/                    # 网络（C 版 + 可被 Rust 替代）
│   ├── security/                   # 安全（C 版 + 可被 Rust 替代）
│   └── ...
├── rust_modules/
│   ├── Cargo.toml                  # Rust 工作空间
│   ├── c_bindings/
│   │   └── ff_rust_bindings.h     # FFI 绑定头文件
│   ├── rust_network/
│   │   ├── Cargo.toml
│   │   └── src/lib.rs             # 安全网络模块
│   ├── rust_crypto/
│   │   ├── Cargo.toml
│   │   └── src/lib.rs             # 加密模块
│   └── rust_security/
│       ├── Cargo.toml
│       └── src/lib.rs             # 安全检查模块
├── platforms/
│   ├── unix/                       # BSD/Unix 平台
│   │   ├── freebsd/
│   │   ├── openbsd/
│   │   └── netbsd/
│   ├── linux/
│   ├── windows/
│   ├── macos/
│   ├── ios/
│   └── android/
├── docs/
│   └── ... (所有文档)
├── scripts/
│   ├── build_hybrid.sh             # 混合构建脚本
│   └── ... (其他脚本)
└── CMakeLists.txt                  # 更新后的构建系统
```

---

## 🔧 构建

### 前置要求

**C 部分**:
- GCC / Clang 编译器
- CMake 3.10+
- pthread 库（Unix）

**Rust 部分（可选）**:
- Rust 1.60+
- Cargo 包管理器

### 快速构建

```bash
# 方式 1：使用构建脚本（推荐）
cd 汇总
chmod +x scripts/build_hybrid.sh
scripts/build_hybrid.sh

# 方式 2：手动 CMake 构建
mkdir -p build
cd build
cmake .. -DFF_USE_RUST_MODULES=ON  # 启用 Rust 模块
# 或者 cmake .. -DFF_USE_RUST_MODULES=OFF  # 仅 C 模式
make -j4
```

### 构建选项

| 选项 | 描述 |
|------|------|
| `FF_USE_RUST_MODULES` | 启用/禁用 Rust 模块 (默认: ON) |
| `CMAKE_BUILD_TYPE` | 构建类型 (Debug/Release/RelWithDebInfo) |

---

## 🌐 Rust 模块说明

### 1. rust_network - 安全网络通信

**功能**:
- TCP/UDP 安全套接字
- P2P 节点发现和连接
- TLS 加密通信
- 自动断线重连

**C 接口（FFI）**:
```c
// rust_network.h
FFNetErrorCode rust_network_init(const FFNetConfig* config);
FFNetErrorCode rust_network_destroy(void);
FFNetErrorCode rust_send_secure(uint64_t conn_id, const uint8_t* data, size_t data_len, size_t* sent);
FFNetErrorCode rust_receive_secure(uint64_t conn_id, uint8_t* buffer, size_t buffer_size, size_t* received);
FFNetErrorCode rust_connect_peer(const char* peer_id, const char* host, uint16_t port, uint64_t* conn_id);
FFNetErrorCode rust_disconnect_peer(uint64_t conn_id);
FFNetErrorCode rust_p2p_discover(void);
```

**C 调用示例**:
```c
FFNetConfig config;
config.host = "127.0.0.1";
config.port = 8080;
config.use_tls = true;

FFNetErrorCode result = rust_network_init(&config);
if (result == FF_NET_OK) {
    uint64_t conn_id;
    rust_connect_peer("peer1", "192.168.1.100", 8080, &conn_id);
    // ...
}
```

---

### 2. rust_crypto - 加密安全模块

**功能**:
- SHA-256/SHA-512 哈希
- AES-GCM 加密/解密
- HMAC 消息认证
- 安全随机数生成
- 内存安全清理

**C 接口（FFI）**:
```c
FFCryptoErrorCode rust_crypto_init(void);
FFCryptoErrorCode rust_sha256(const uint8_t* input, size_t input_len, uint8_t* output, size_t output_size);
FFCryptoErrorCode rust_sha512(const uint8_t* input, size_t input_len, uint8_t* output, size_t output_size);
FFCryptoErrorCode rust_encrypt_aes256_gcm(...)
FFCryptoErrorCode rust_decrypt_aes256_gcm(...)
FFCryptoErrorCode rust_random_bytes(uint8_t* buffer, size_t buffer_size);
FFCryptoErrorCode rust_secure_zero(uint8_t* buffer, size_t buffer_size);
```

---

### 3. rust_security - 安全验证模块

**功能**:
- 输入验证（邮箱、电话、密码）
- SQL 注入检测
- XSS 注入检测
- JSON/XML 安全验证
- 字符串清理（HTML 转义）

**C 接口（FFI）**:
```c
FFSecurityErrorCode rust_security_init(void);
FFSecurityErrorCode rust_validate_email(const char* email);
FFSecurityErrorCode rust_validate_password(const char* password);
FFSecurityErrorCode rust_check_sql_injection(const char* input);
FFSecurityErrorCode rust_check_xss_injection(const char* input);
FFSecurityErrorCode rust_sanitize_string(const char* input, char* output, size_t output_size);
FFSecurityErrorCode rust_validate_json(const char* json_str);
FFSecurityErrorCode rust_validate_xml(const char* xml_str);
```

---

## 🔨 C 代码质量优化

### 安全字符串操作

```c
#include "ff_safe_utils.h"

// 安全复制
char buffer[64];
ff_safe_strncpy(buffer, source_string, sizeof(buffer));

// 安全拼接
ff_safe_strncat(buffer, "suffix", sizeof(buffer));

// 安全格式化
ff_safe_snprintf(buffer, sizeof(buffer), "User: %s", username);
```

### 内存安全管理

```c
// 带调试信息的安全分配
void* buffer = FF_MALLOC(1024);

// 安全释放（自动归零）
FF_FREE(buffer);

// 泄漏检查
ff_check_memory_leaks();
```

### 统一日志系统

```c
FF_LOG_SET_LEVEL(FF_LOG_LEVEL_DEBUG);

FF_DEBUG("Debug message");
FF_INFO("Info message");
FF_WARN("Warning message");
FF_ERROR("Error message");
```

---

## 🧪 测试

### 运行集成测试

```bash
# 编译并运行测试
mkdir build && cd build
cmake ..
make

# 运行 CLI 工具
./bin/FindFriendCLI

# 运行完整应用
./platforms/linux/redhat/FindFriendApp_RedHat
```

### 从 C 调用 Rust 模块

查看 `common_core/ff_rust_integration_test.c` 获取完整示例。

---

## 🔀 平台支持

### 已支持的平台

| 平台 | 状态 | C 模块 | Rust 模块 |
|------|------|--------|-----------|
| Linux (所有发行版) | ✅ 完成 | ✅ | ✅ |
| FreeBSD | ✅ 完成 | ✅ | ✅ |
| OpenBSD | ✅ 完成 | ✅ | ✅ |
| NetBSD | ✅ 完成 | ✅ | ✅ |
| Windows | ✅ 完成 | ✅ | ✅ |
| macOS | ✅ 完成 | ✅ | ✅ |
| iOS | ✅ 完成 | ✅ | ✅ |
| Android | ✅ 完成 | ✅ | ✅ |

---

## 📊 安全改进对比

| 方面 | C-only 架构 | C + Rust 混合架构 |
|------|-------------|------------------|
| 内存安全 | 需要手动管理 | Rust 自动保证 |
| 数据竞争 | Mutex 容易出错 | Rust 类型安全 |
| 缓冲区溢出 | 需要仔细编码 | Rust 自动边界检查 |
| 加密模块 | OpenSSL 绑定 | RustCrypto 原生 |
| 输入验证 | 手动验证 | Rust regex + 黑名单 |
| 依赖链 | 较复杂 | 简化 |

---

## 🎯 迁移路径

### 渐进式采用策略

```
阶段 1 (当前): C 核心 + Rust 可选模块
阶段 2: 将高风险功能迁移到 Rust
阶段 3: 主要功能双模式（C + Rust）
阶段 4 (未来): 可选 Rust 核心
```

---

## 📝 开发指南

### 修改 Rust 模块

```bash
cd rust_modules/rust_security
# 编辑 src/lib.rs

# 测试构建
cargo build --release
```

### 添加新的 Rust 模块

1. 在 `rust_modules/` 创建新 crate
2. 更新 `Cargo.toml` 工作区
3. 添加 FFI 接口到 `c_bindings/ff_rust_bindings.h`
4. 更新 CMakeLists.txt
5. 更新文档

---

## 🔍 故障排查

### 常见问题

**Q: Cargo 找不到，如何禁用 Rust 模块？**

```bash
cmake .. -DFF_USE_RUST_MODULES=OFF
```

**Q: 链接错误找不到 Rust 库？**

检查 `Cargo.lock` 存在，确保 `cargo build` 成功。

**Q: 内存泄漏警告？**

使用 `ff_check_memory_leaks()` 在程序退出时检查。

---

## 📚 相关文档

- [README.md](../README.md) - 主项目文档
- [CORE_INTERFACE.md](../docs/CORE_INTERFACE.md) - 核心接口
- [BSD_PLATFORM.md](../docs/BSD_PLATFORM.md) - BSD 平台
- [FINISHED_GUIDE.md](../docs/FINISHED_GUIDE.md) - 完整开发指南
- [P2P_COMPLETE_GUIDE.md](../docs/P2P_COMPLETE_GUIDE.md) - P2P 架构
- [SECURITY.md](../docs/SECURITY.md) - 安全模块

---

## 📋 总结

FindFriend 现在拥有：

✅ **C 核心框架** - 稳定、成熟、高性能
✅ **Rust 安全模块** - 内存安全、高可靠性
✅ **混合构建系统** - 统一 CMake 管理
✅ **完整 FFI 绑定** - 透明的 C <-> Rust 互操作
✅ **代码质量工具** - 安全字符串、内存管理、统一日志
✅ **全平台兼容** - 所有平台均支持混合架构

---

**版本**: 4.0
**架构**: C + Rust 混合
**最后更新**: 2026-04-26
