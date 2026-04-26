# FindFriend 项目

## 项目简介

FindFriend 是一个跨平台去中心化社交应用，采用 C + Rust 混合架构，旨在提供安全、高效、可扩展的社交服务。

### 核心特性

- 🔐 **C + Rust 混合架构** - C 提供高性能核心，Rust 提供内存安全保障
- 🌐 **去中心化 P2P** - 无中心服务器，节点既作为客户端又作为服务端
- 📍 **隐私位置保护** - 精确位置仅好友可见，支持位置模糊化
- 🛡️ **企业级安全** - 反黑产、防注入、数据完整性校验
- 📱 **全平台支持** - Linux、Windows、macOS、iOS、Android、BSD

---

## 快速开始

### 环境要求

- **编译工具**: GCC 10+ / Clang 12+
- **构建系统**: CMake 3.20+
- **Rust**: 1.60+ (可选，用于安全模块)
- **平台**: Linux, Windows, macOS, iOS, Android, BSD

### 编译

```bash
# 克隆项目
git clone https://github.com/your-org/findfriend.git
cd findfriend

# 创建构建目录
mkdir build && cd build

# 配置 (启用 Rust 安全模块)
cmake .. -DFF_USE_RUST=ON

# 编译
make -j$(nproc)

# 运行 CLI 工具
./bin/ff_cli
```

### 运行

```bash
# 运行应用
./bin/findfriend

# 运行测试
./bin/findfriend_test
```

---

## 架构概览

```
FindFriend/
├── core/                    # 核心业务逻辑
│   ├── interface/          # 模块接口定义
│   ├── business/            # 业务模块
│   ├── p2p/               # P2P 网络
│   └── data/              # 数据管理
│
├── security_rust/           # Rust 安全模块
│   ├── src/               # Rust 源代码
│   └── ffi/               # FFI 桥接
│
├── platform/               # 平台适配层
│   ├── linux/
│   ├── windows/
│   ├── android/
│   ├── macos/
│   ├── ios/
│   └── bsd/
│
├── ui/                     # UI 层
│
├── tools/                  # 工具
│   └── cli/               # CLI 调试工具
│
├── docs/                   # 文档
│   ├── ARCHITECTURE.md
│   ├── P2P_DESIGN.md
│   ├── SECURITY.md
│   └── BUILD.md
│
└── CMakeLists.txt
```

---

## 核心模块

### 业务模块 (core/business/)

| 模块 | 功能 | 状态 |
|------|------|------|
| user_manager | 用户注册、登录、信息管理 | ✅ |
| message_manager | 消息发送、接收、已读 | ✅ |
| social_manager | 好友关系管理 | ✅ |
| location_service | 位置上报、附近用户 | ✅ |
| game_manager | 游戏逻辑、积分 | ✅ |

### P2P 模块 (core/p2p/)

| 模块 | 功能 | 状态 |
|------|------|------|
| node_discovery | 节点发现、广播 | ✅ |
| connection | 连接管理、状态维护 | ✅ |
| message_routing | 消息路由、转发 | ✅ |
| nat_traversal | NAT 打洞、STUN | ✅ |

### 安全模块 (security_rust/)

| 模块 | 功能 | 状态 |
|------|------|------|
| memory | 内存加密 | ✅ |
| device | 设备指纹 | ✅ |
| mitm | MITM 防护 | ✅ |
| integrity | 数据完整性校验 | ✅ |
| location | 位置模糊化 | ✅ |

---

## 接口文档

### 核心接口

所有模块通过 `core/interface/` 下的头文件进行交互：

- `ff_core_interface.h` - 核心初始化、事件系统
- `ff_user_interface.h` - 用户管理
- `ff_message_interface.h` - 消息系统
- `ff_location_interface.h` - 位置服务
- `ff_p2p_interface.h` - P2P 网络
- `ff_game_interface.h` - 游戏逻辑
- `ff_security_interface.h` - 安全策略

### 详细文档

- [架构文档](docs/ARCHITECTURE.md)
- [P2P 设计](docs/P2P_DESIGN.md)
- [安全策略](docs/SECURITY.md)
- [编译指南](docs/BUILD.md)

---

## 安全特性

### 抗攻击能力

- ✅ SQL/XSS/JSON/XML 注入防护
- ✅ 虚假 GPS 检测
- ✅ 爬虫行为识别
- ✅ DoS 防护
- ✅ MITM 攻击防护
- ✅ 数据完整性校验

### 隐私保护

- ✅ 位置模糊化
- ✅ 精确位置仅好友可见
- ✅ 设备指纹验证
- ✅ 内存加密

---

## 测试

### 运行测试

```bash
# 进入构建目录
cd build

# 运行单元测试
ctest --output-on-failure

# 运行集成测试
./bin/findfriend_integration_test
```

### 测试覆盖率

目标: 核心模块 80%+ 覆盖率

---

## 开发指南

### 代码规范

- C 代码遵循 C99 标准
- 使用 `ff_` 前缀命名公共函数
- 所有公共函数必须有文档注释
- 使用统一的错误码系统

### 提交规范

```
<type>(<scope>): <subject>

< body>

<footer>
```

类型: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

---

## 性能目标

| 指标 | 目标 |
|------|------|
| 启动时间 | < 2s |
| 消息延迟 | < 100ms (局域网) |
| 内存占用 | < 50MB (空闲) |
| CPU 使用 | < 5% (空闲) |

---

## 许可证

MIT License - 详见 [LICENSE](../LICENSE)

---

## 联系方式

- GitHub Issues: https://github.com/your-org/findfriend/issues
- Email: dev@findfriend.example.com

---

**版本**: 5.0
**更新日期**: 2026-04-26
