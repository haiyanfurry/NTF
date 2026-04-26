# FindFriend - 跨平台 P2P 社交应用核心系统

## 📋 项目简介

FindFriend 是一款跨平台社交、定位、多媒体应用，核心使用 C99 开发，支持 Linux、Windows、macOS、Android、iOS、BSD 等平台。

### ✨ 核心特性

- **P2P 网络层** - 节点既是客户端又是服务端，支持局域网/互联网好友发现和通信
- **位置共享** - 仅好友可查看模糊距离，保护精确位置隐私
- **安全模块** - 反黑产/反灰产检测、内容审核、异常行为防护
- **多平台支持** - 统一的 core_interface.h 接口，各平台独立 UI
- **调试系统** - CLI 工具、Boot Check 调试模式、实时日志
- **Rust 安全模块** - 安全字符串、内存池管理、日志加密

---

## 🚀 快速开始

### 1. Linux CLI 调试工具（推荐）

```bash
# 进入项目目录
cd /home/haiyan/汇总

# 运行 CLI 调试工具
./build/bin/FindFriendCLI

# 在 CLI 中测试功能
FindFriend> help
FindFriend> init localhost 8080
FindFriend> status
FindFriend> quit
```

### 2. 完整应用程序

```bash
# 运行完整应用（Boot Check 调试模式已启用）
./platforms/linux/redhat/FindFriendApp_RedHat
```

### 3. 编译项目

```bash
# 创建构建目录
cd /home/haiyan/汇总
mkdir -p build
cd build

# 编译
cmake .. -DCMAKE_BUILD_TYPE=Release -DFF_BUILD_UI=OFF
make -j4

# 运行 CLI
./bin/FindFriendCLI
```

---

## 📁 项目结构

```
.
├── common_core/                # 核心逻辑和接口
│   ├── core_interface.h/c    # 统一核心接口
│   ├── auth/                 # 认证模块
│   ├── data/                 # 数据管理
│   ├── game/                 # 游戏逻辑
│   ├── network/              # 网络层（含 P2P）
│   ├── security/             # 安全模块
│   └── util/                 # 工具函数
├── core/                      # 核心模块
│   ├── business/             # 业务逻辑
│   ├── data/                 # 数据存储
│   ├── interface/            # 接口定义
│   ├── p2p/                  # P2P 模块
│   └── utils/                # 工具函数
├── p2p/                      # P2P 模块
├── security_rust/             # Rust 安全模块
├── tools/cli/                 # CLI 调试工具
├── platforms/                 # 跨平台占位
│   ├── android/             # Android 平台
│   ├── windows/             # Windows 平台
│   ├── macos/               # macOS 平台
│   ├── ios/                 # iOS 平台
│   ├── linux/               # Linux 各发行版
│   │   ├── redhat/          # RedHat Linux
│   │   ├── debian/          # Debian Linux
│   │   ├── arch/            # Arch Linux
│   │   └── gentoo/          # Gentoo Linux
│   └── unix/                # BSD 平台
│       ├── freebsd/          # FreeBSD
│       ├── netbsd/           # NetBSD
│       └── openbsd/          # OpenBSD
├── ui/                        # UI 占位
├── lib/                      # 静态库
├── resources/                # 图片、样式、HTML 模板
├── docs/                     # 完整文档
│   ├── README.md            # 本文档
│   ├── README_CORE_INTERFACE.md # 核心接口说明
│   ├── FINISHED_GUIDE.md   # 开发和调试指南
│   ├── P2P_COMPLETE_GUIDE.md # P2P 系统说明
│   └── SECURITY.md          # 安全模块说明
├── scripts/                 # 构建和测试脚本
├── build/                   # 构建输出
│   ├── bin/                 # 可执行文件
│   └── lib/                 # 库文件
└── dist/                   # 发布打包文件
```

---

## 📚 文档指南

| 文档 | 内容说明 |
|------|---------|
| [docs/README.md](docs/README.md) | 项目概述和快速开始 |
| [docs/README_CORE_INTERFACE.md](docs/README_CORE_INTERFACE.md) | 核心接口 API 详解 |
| [docs/FINISHED_GUIDE.md](docs/FINISHED_GUIDE.md) | 开发调试流程和示例 |
| [docs/P2P_COMPLETE_GUIDE.md](docs/P2P_COMPLETE_GUIDE.md) | P2P 架构和好友发现 |
| [docs/SECURITY.md](docs/SECURITY.md) | 反黑产/反灰产安全说明 |

---

## 🔧 核心模块

### P2P 网络层 (`common_core/network/p2p_network.h`)
```c
// P2P 初始化
p2p_network_init(&config);

// 启动服务器和发现服务
p2p_start_server(9876);
p2p_start_discovery();

// 好友管理
p2p_add_friend("user123");
p2p_remove_friend("user123");

// 位置共享（仅好友）
p2p_update_location(39.9042, 116.4074);
p2p_send_location_to_friend("friend_id", lat, lon);
```

### 安全模块 (`common_core/security/security_module.h`)
```c
// 初始化
security_module_init();

// 内容检测
check_message_content("hello");        // 返回 true/false
check_media_file("/path/to/file");    // 返回 true/false

// 异常行为
is_suspicious_user("user123");       // 检查可疑用户
check_rate_limit(uid, "message", 60, 10);  // 速率限制

// 隐私保护
can_view_exact_location(viewer, owner);    // 精确位置权限
get_friend_distance_fuzzy(distance);        // 模糊距离
```

### 核心接口 (`common_core/core_interface.h`)
```c
// 初始化
core_interface_init("localhost", 8080);

// 用户管理
core_login(username, password, callback, user_data);
core_logout();
core_get_current_user(callback, user_data);

// 消息管理
core_send_message(to_id, message, callback, user_data);
core_get_messages(count, callback, user_data);

// 位置管理
core_report_location(lat, lon, callback, user_data);
core_get_nearby_users(lat, lon, radius, callback, user_data);

// 调试
core_debug_command("status", callback, user_data);
core_debug_get_logs(100, callback, user_data);
```

---

## 🛠️ 开发工具

### CLI 调试命令

```
help                          # 显示帮助
init <host> <port>           # 初始化核心接口
destroy                        # 销毁核心接口
status                         # 显示状态

login <user> <pass>          # 用户登录
logout                         # 用户登出
current-user                   # 获取当前用户
user <id>                     # 获取用户信息

send <id> <msg>              # 发送消息
messages <count>              # 获取消息列表

report-loc <lat> <lon>       # 上报位置
nearby <lat> <lon> <radius>  # 获取附近用户

debug <cmd>                   # 调试命令
logs                           # 查看日志
clear-logs                    # 清空日志

quit                          # 退出
```

### Boot Check 调试模式

在 `common_core/security/boot_check/boot_check.c` 中：
```c
#define BOOT_CHECK_DEBUG_MODE 1  // 调试模式已启用
```

调试模式下跳过核心文件检查，正式发布时设为 0。

---

## 📱 多平台支持

### Android
- **JNI 桥接层** - `platforms/android/jni/`
- **Java 接口** - `platforms/android/java/com/findfriend/core/`
- **调试界面** - DebugActivity.java

### Windows
- **Win32 GUI** - `platforms/windows/win32_debug.cpp`
- 完整的可视化调试界面

### macOS / iOS
- **Objective-C 接口** - `platforms/macos/FindFriendCore.m`
- 统一的核心接口调用

### Linux 各发行版
- **RedHat** - `platforms/linux/redhat/`
- **Debian** - `platforms/linux/debian/`
- **Arch** - `platforms/linux/arch/`
- **Gentoo** - `platforms/linux/gentoo/`
- **LFS** - `platforms/linux/lfs/`

### BSD 平台
- **FreeBSD** - `platforms/unix/freebsd/`
- **NetBSD** - `platforms/unix/netbsd/`
- **OpenBSD** - `platforms/unix/openbsd/`

---

## 🔐 安全特性

- ✅ **内容检测** - 消息、媒体、游戏行为审核
- ✅ **异常行为防护** - 刷量、批量操作、频繁注册检测
- ✅ **隐私保护** - 位置权限、精确位置不暴露
- ✅ **审计日志** - 安全事件记录和导出
- ✅ **Rust 安全模块** - 安全字符串、内存池管理、日志加密

---

## 🔄 构建顺序

1. **核心模块** - 基础功能和接口
2. **P2P 模块** - 节点发现和通信
3. **Rust 安全模块** - 安全增强
4. **CLI 调试工具** - 命令行测试
5. **UI 模块** - 用户界面
6. **平台特定代码** - 各平台适配

---

## 📊 项目状态

| 模块 | 状态 | 说明 |
|------|------|------|
| P2P 网络层 | ✅ 完成 | 支持好友发现和 P2P 通信 |
| 安全模块 | ✅ 完成 | 反黑产/反灰产检测 |
| 核心接口 | ✅ 完成 | 统一跨平台接口 |
| CLI 调试 | ✅ 完成 | 完整的命令行工具 |
| Boot Check | ✅ 完成 | 调试模式已启用 |
| Android JNI | ✅ 完成 | Java 接口和调试界面 |
| Windows GUI | ✅ 完成 | Win32 调试界面 |
| macOS/iOS | ✅ 完成 | Objective-C 接口 |
| BSD 平台 | ✅ 完成 | FreeBSD、NetBSD、OpenBSD 支持 |

---

## 🎯 下一步

1. 阅读 [docs/FINISHED_GUIDE.md](docs/FINISHED_GUIDE.md) 了解完整开发流程
2. 阅读 [docs/P2P_COMPLETE_GUIDE.md](docs/P2P_COMPLETE_GUIDE.md) 了解 P2P 架构
3. 阅读 [docs/README_CORE_INTERFACE.md](docs/README_CORE_INTERFACE.md) 查看 API 文档
4. 使用 CLI 工具测试各个模块

---

## 📞 技术支持

- 查看 `docs/` 目录下的完整文档
- 使用 `./build/bin/FindFriendCLI help` 查看 CLI 命令
- 运行 `./platforms/linux/redhat/FindFriendApp_RedHat` 测试完整应用

---

**版本**: 5.0
**构建环境**: Fedora 43, GCC 15.2.1, CMake 3.31.11
**最后更新**: 2026-04-26

## 开发维护 / Developers & Maintainers
个人维护：haiyanfurry
协作团队：霓拓锋team / NTF-ZEYZ

## 漏洞反馈 / Security Vulnerability Report
如有安全漏洞或BUG，请邮件提交：
If you find security bugs, please contact via email:

[2752842448@qq.com]

## 说明

```
目前开发者现在就几乎一个人
因为我的另一个朋友他要备战中考
所以只有我一个人了
然后就是我现在因为本身编程能力并不强
所以用到了ChatGPT和Trae两个ai
所以可能会有bug这些比较多的地方
```
