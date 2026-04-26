# FindFriend BSD/Unix 平台支持

## 📋 平台支持概览

FindFriend 现已支持以下 BSD/Unix 平台：
- ✅ **FreeBSD**
- ✅ **OpenBSD**
- ✅ **NetBSD**

---

## 📁 目录结构

```
platforms/unix/
├── freebsd/
│   ├── CMakeLists.txt
│   ├── main.c
│   ├── platform_specific.c
│   └── platform_specific.h
├── openbsd/
│   ├── CMakeLists.txt
│   ├── main.c
│   ├── platform_specific.c
│   └── platform_specific.h
└── netbsd/
    ├── CMakeLists.txt
    ├── main.c
    ├── platform_specific.c
    └── platform_specific.h
```

---

## 🔧 核心适配

### 平台特定接口 (`platform_specific.h/c`)

每个 BSD 平台都包含：
- **系统信息获取** - CPU 数量、内存、系统运行时间
- **网络接口适配** - BSD socket API，IPv4/IPv6 支持
- **线程同步** - pthread  mutex 和 thread 支持
- **权限管理** - 用户/组操作、权限降级
- **文件系统** - 路径解析、文件权限检查

### 主要 API

```c
// 系统信息
int platform_get_cpu_count(void);
int64_t platform_get_memory_size(void);
int64_t platform_get_free_memory(void);
const char* platform_get_hostname(void);
int platform_get_system_uptime(void);

// 线程和同步
int platform_mutex_init(platform_mutex_t* mutex);
int platform_thread_create(platform_thread_t* thread,
                          platform_thread_func_t func, void* arg);

// 网络
int platform_init_network(void);
int platform_get_network_interfaces(char* buffer, size_t len);

// 安全
int platform_is_root(void);
int platform_drop_privileges(const char* user);
int platform_check_file_permissions(const char* path);
```

---

## 🏗️ 构建说明

### CMake 构建

在 BSD 系统上：

```bash
# FreeBSD
cd platforms/unix/freebsd
mkdir build && cd build
cmake ..
make

# OpenBSD
cd platforms/unix/openbsd
mkdir build && cd build
cmake ..
make

# NetBSD
cd platforms/unix/netbsd
mkdir build && cd build
cmake ..
make
```

### 使用构建脚本

```bash
# 在 Linux 环境中模拟 BSD 构建
./scripts/build_bsd.sh freebsd
./scripts/build_bsd.sh openbsd
./scripts/build_bsd.sh netbsd
```

---

## 📦 构建产物

编译后会生成：

| 产物 | 说明 |
|------|------|
| `FindFriendBSD_FreeBSD` | FreeBSD 完整应用 |
| `FindFriendBSD_OpenBSD` | OpenBSD 完整应用 |
| `FindFriendBSD_NetBSD` | NetBSD 完整应用 |
| `FindFriendCLI_BSD` | BSD 平台 CLI 调试工具 |

---

## 🔐 安全模块适配

所有 BSD 平台都支持完整的安全模块：

### 反黑产/反灰产
- 内容检测（消息、媒体、游戏行为）
- 异常行为防护（刷量、速率限制）
- 可疑用户检测

### 隐私保护
- 位置权限管理
- 精确位置不暴露
- 好友距离模糊化

### 审计日志
- 安全事件记录
- 日志导出功能

### Boot Check
```c
#define BOOT_CHECK_DEBUG_MODE 1  // 调试模式已启用
```

---

## 🌐 网络模块

### BSD Socket API

每个平台使用标准的 BSD socket 接口：

```c
// socket 创建
int sock = socket(AF_INET, SOCK_STREAM, 0);

// 地址绑定
bind(sock, (struct sockaddr*)&addr, sizeof(addr));

// 监听
listen(sock, 10);

// 连接
connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

// 发送/接收
send(sock, buffer, len, 0);
recv(sock, buffer, len, 0);
```

### IPv6 支持

所有平台都支持 IPv6：

```c
int sock = socket(AF_INET6, SOCK_STREAM, 0);
```

---

## 📝 平台特性对比

| 特性 | FreeBSD | OpenBSD | NetBSD |
|------|---------|---------|--------|
| sysctl CPU | hw.ncpu | hw.ncpu | hw.ncpu |
| sysctl Memory | hw.physmem | hw.physmem | hw.physmem |
| 内存信息 | vm.stats.vm.v_free_count | uvmexp2 | vmmeter |
| 随机数 | /dev/urandom | /dev/urandom | /dev/urandom |
|privilege drop | setuid + setgid | setuid + setgid | setuid + setgid |

---

## 🧪 测试

### 在 Linux 上模拟测试

由于我们在 Linux 环境中，可以使用 FreeBSD 的 platform_specific 代码进行模拟测试：

```bash
# 编译 FreeBSD 版本（在 Linux 上）
cd platforms/unix/freebsd
mkdir -p build && cd build
cmake ../..
make
```

### 验证系统信息

```c
#include "platform_specific.h"

void test_platform_info(void) {
    printf("Platform: %s\n", platform_get_platform_name());
    printf("Hostname: %s\n", platform_get_hostname());
    printf("Username: %s\n", platform_get_username());
    printf("CPU Count: %d\n", platform_get_cpu_count());
    printf("Memory: %ld MB\n", platform_get_memory_size() / 1024 / 1024);
    printf("Is Root: %s\n", platform_is_root() ? "YES" : "NO");
}
```

---

## 🔄 与其他平台的统一

### 统一的接口

所有平台（Linux、Windows、macOS、Android、iOS、BSD）都使用相同的：
- `common_core/` - 核心逻辑和接口
- `common_ui/` - 公共 UI 资源
- `core_interface.h` - 统一核心接口

### 平台适配层

每个平台的适配层只负责：
- 系统 API 调用（如 sysctl、pthread）
- 路径处理（符合各平台规范）
- 权限管理（各平台特定）
- 网络初始化

---

## 📚 相关文档

- [README.md](../README.md) - 主文档
- [CORE_INTERFACE.md](../docs/CORE_INTERFACE.md) - 核心接口
- [P2P_COMPLETE_GUIDE.md](../docs/P2P_COMPLETE_GUIDE.md) - P2P 系统
- [SECURITY.md](../docs/SECURITY.md) - 安全模块

---

## 🎯 下一步

1. 在真实的 BSD 系统上编译和测试
2. 根据实际系统调用调整 platform_specific 代码
3. 测试完整的 P2P 功能和安全性模块
4. 集成到各 BSD 发行版的包管理系统

---

**版本**: 4.0
**支持平台**: FreeBSD, OpenBSD, NetBSD
**最后更新**: 2026-04-25
