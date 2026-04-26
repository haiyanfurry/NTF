# FindFriend 核心接口与调试系统

## 概述

FindFriend 现在包含一个完整的跨平台核心接口抽象层和调试系统，支持 Linux、Windows、macOS、iOS、Android 和 BSD 平台。

## 项目结构

```
.
├── common_core/                 # 核心逻辑和接口
│   ├── core_interface.h      # 核心接口定义
│   ├── core_interface.c      # 核心接口实现
│   ├── core_cli.c            # CLI 调试工具
│   ├── auth/                 # 认证模块
│   ├── data/                 # 数据管理模块
│   ├── business/             # 业务逻辑模块
│   ├── game/                 # 游戏模块
│   ├── network/              # 网络模块（含 P2P）
│   ├── security/             # 安全模块（Boot Check）
│   └── util/                 # 工具函数
├── core/                       # 核心模块
│   ├── business/             # 业务逻辑
│   ├── data/                 # 数据存储
│   ├── interface/            # 接口定义
│   ├── p2p/                  # P2P 模块
│   └── utils/                # 工具函数
├── p2p/                       # P2P 模块
├── security_rust/              # Rust 安全模块
├── tools/cli/                  # CLI 调试工具
├── platforms/                  # 跨平台占位
│   ├── android/             # Android 平台
│   ├── windows/             # Windows 平台
│   ├── macos/               # macOS 平台
│   ├── ios/                 # iOS 平台
│   ├── linux/               # Linux 各发行版
│   │   ├── redhat/          # RedHat Linux
│   │   ├── debian/          # Debian Linux
│   │   ├── arch/            # Arch Linux
│   │   ├── gentoo/          # Gentoo Linux
│   │   └── lfs/             # Linux From Scratch
│   └── unix/                # BSD 平台
│       ├── freebsd/          # FreeBSD
│       ├── netbsd/           # NetBSD
│       └── openbsd/          # OpenBSD
├── ui/                         # UI 占位
├── lib/                        # 静态库
├── build/                      # 构建输出
│   ├── bin/                 # 可执行文件
│   └── lib/                 # 库文件
└── docs/                       # 文档
```

## 核心接口 (core_interface.h)

### 初始化与销毁

```c
// 初始化核心接口
bool core_interface_init(const char* server_host, int port);

// 销毁核心接口
void core_interface_destroy(void);

// 获取版本信息
const char* core_interface_get_version(void);

// 检查是否已初始化
bool core_interface_is_initialized(void);
```

### 用户管理

```c
void core_login(const char* username, const char* password, 
                UserCallback callback, void* user_data);

void core_register(const char* username, const char* password, 
                   const char* nickname, UserCallback callback, 
                   void* user_data);

void core_logout(void);

void core_get_current_user(UserCallback callback, void* user_data);
void core_get_user_by_id(uint32_t user_id, UserCallback callback, void* user_data);
void core_update_user(const char* user_json, UserCallback callback, void* user_data);
```

### 消息管理

```c
void core_send_message(uint32_t to_id, const char* message, 
                       MessageCallback callback, void* user_data);

void core_get_messages(int count, MessageCallback callback, void* user_data);

void core_mark_message_read(uint32_t message_id, MessageCallback callback, void* user_data);

void core_delete_message(uint32_t message_id, MessageCallback callback, void* user_data);
```

### 位置管理

```c
void core_report_location(double latitude, double longitude, 
                          CoreLocationCallback callback, void* user_data);

void core_get_nearby_users(double latitude, double longitude, 
                           double radius_meters, CoreLocationCallback callback, 
                           void* user_data);

void core_get_user_location(uint32_t user_id, CoreLocationCallback callback, void* user_data);

void core_set_location_sharing(bool enabled, CoreLocationCallback callback, void* user_data);
```

### 游戏逻辑

```c
void core_get_games(GameCallback callback, void* user_data);
void core_join_game(uint32_t game_id, GameCallback callback, void* user_data);
void core_leave_game(uint32_t game_id, GameCallback callback, void* user_data);
void core_game_action(uint32_t game_id, const char* action_json, 
                      GameCallback callback, void* user_data);
```

### 多媒体传输

```c
void core_upload_media(const char* file_path, const char* media_type, 
                       MediaCallback callback, void* user_data);

void core_download_media(const char* media_id, const char* save_path, 
                         MediaCallback callback, void* user_data);

void core_delete_media(const char* media_id, MediaCallback callback, void* user_data);

void core_get_media_info(const char* media_id, MediaCallback callback, void* user_data);
```

### 调试接口

```c
void core_debug_command(const char* command, CoreCallback callback, void* user_data);
void core_debug_get_logs(int last_lines, CoreCallback callback, void* user_data);
void core_debug_clear_logs(void);
const char* core_debug_get_module_status(void);
```

## 调试工具使用

### CLI 调试工具

在 Linux 上，已编译好的 CLI 工具位于 `./build/bin/FindFriendCLI`。

启动 CLI 交互模式：
```bash
cd /home/haiyan/汇总
./build/bin/FindFriendCLI
```

常用命令：
```
help                          # 显示帮助信息
init <host> <port>           # 初始化核心接口
destroy                       # 销毁核心接口
status                        # 显示当前状态
login <user> <pass>          # 登录用户
logout                        # 登出用户
send <user_id> <msg>         # 发送消息
report-loc <lat> <lon>       # 上报位置
nearby <lat> <lon> <radius>  # 获取附近用户
debug <cmd>                  # 执行调试命令
logs                         # 显示日志
quit                         # 退出
```

示例：
```bash
./build/bin/FindFriendCLI help
```

### Boot Check 调试模式

在 `common_core/security/boot_check/boot_check.c` 中，可通过设置以下宏启用调试模式：
```c
#define BOOT_CHECK_DEBUG_MODE 1
```

调试模式下：
- 跳过核心文件完整性检查
- 保留日志输出
- 程序正常启动和运行

正式发布时设置为 0 即可重新启用完整检查。

## 平台特定接口

### Android (JNI)

Java 接口：
```java
package com.findfriend.core;

public class CoreInterface {
    public boolean init(String host, int port);
    public void destroy();
    public String getVersion();
    public boolean isInitialized();
    public void login(String username, String password);
    public void logout();
    public void sendMessage(int toId, String message);
    public void reportLocation(double latitude, double longitude);
    public void getNearbyUsers(double latitude, double longitude, double radius);
    public String getModuleStatus();
    public void debugCommand(String command);
    
    // 回调监听
    public void setOnCoreListener(OnCoreListener listener);
}
```

### Windows (Win32)

在 Windows 平台上提供了完整的 GUI 调试界面，通过 `platforms/windows/win32_debug.cpp` 实现，包含：
- 用户登录/登出
- 消息发送
- 位置上报
- 调试日志查看
- 状态显示

### macOS / iOS

在 macOS 和 iOS 平台上提供了 Objective-C 接口，通过 `platforms/macos/FindFriendCore.m` 实现，支持：
- 核心接口调用
- 平台特定功能集成
- 调试和状态监控

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

## 编译说明

### Linux (RedHat)

```bash
cd /home/haiyan/汇总
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DFF_BUILD_UI=OFF
make -j4
```

### Windows

使用 CMake 生成 Visual Studio 解决方案：
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Android

使用 Android Studio 或 NDK 进行编译，包含 JNI 层和 Java 层。

### macOS / iOS

使用 Xcode 进行编译，包含 Objective-C 接口。

### BSD 平台

```bash
cd /home/haiyan/汇总
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DFF_BUILD_UI=OFF
make -j4
```

## 构建顺序

1. **核心模块** - 基础功能和接口
2. **P2P 模块** - 节点发现和通信
3. **Rust 安全模块** - 安全增强
4. **CLI 调试工具** - 命令行测试
5. **UI 模块** - 用户界面
6. **平台特定代码** - 各平台适配

## 静态库

编译后生成的静态库位于：
- `lib/libFindFriendCore.a` - 核心库
- `lib/libFindFriendUI.a` - UI 库

## 调试技巧

1. **启用 Boot Check 调试模式** - 在开发阶段关闭文件检查
2. **使用 CLI 工具** - 快速测试各个功能模块
3. **查看调试日志** - 通过 `logs` 命令查看核心操作日志
4. **监控模块状态** - 通过 `status` 和 `debug status` 命令查看各模块状态

## 下一步开发

1. 完善业务逻辑实现
2. 完善平台特定 UI
3. 实现完整的网络通信
4. 添加更多调试功能

## 注意事项

- Boot Check 在调试模式下可跳过，但正式发布时必须启用
- 核心接口在所有平台保持一致，仅需实现平台特定 UI
- 回调函数应避免长时间阻塞
- 所有字符串使用 UTF-8 编码
