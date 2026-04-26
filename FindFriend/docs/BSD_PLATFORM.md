# BSD 平台支持指南

## 概述

FindFriend 项目全面支持 BSD 系列操作系统，包括 FreeBSD、NetBSD 和 OpenBSD。

## 支持的平台

| 平台 | 版本 | 状态 |
|------|------|------|
| FreeBSD | 12.0+ | ✅ 完全支持 |
| NetBSD | 9.0+ | ✅ 完全支持 |
| OpenBSD | 7.0+ | ✅ 完全支持 |

## 平台特性

### FreeBSD

- **包管理器**: pkg
- **防火墙**: pf
- **文件系统**: UFS/ZFS
- **特性**: execinfo 库支持

### NetBSD

- **包管理器**: pkgin
- **防火墙**: ipfilter
- **文件系统**: FFS
- **特性**: 高度可移植

### OpenBSD

- **包管理器**: pkg_add
- **防火墙**: pf
- **文件系统**: FFS
- **特性**: 安全强化

## 依赖安装

### FreeBSD

```bash
pkg update && pkg upgrade
pkg install cmake rust openssl
```

### NetBSD

```bash
pkgin update && pkgin upgrade
pkgin install cmake rust openssl
```

### OpenBSD

```bash
pkg_add cmake rust openssl
```

## 构建

### CMake 配置

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### 平台检测

CMake 会自动检测平台：

```cmake
if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    set(BSD_PLATFORM "freebsd")
elseif(CMAKE_SYSTEM_NAME MATCHES "NetBSD")
    set(BSD_PLATFORM "netbsd")
elseif(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
    set(BSD_PLATFORM "openbsd")
endif()
```

## 平台实现

### 目录结构

```
platform/bsd/
├─ CMakeLists.txt           # BSD 构建配置
├─ freebsd/
│  ├─ main.c              # 主程序
│  ├─ platform_specific.h # 接口定义
│  ├─ platform_specific.c # 通用实现
│  ├─ network.c           # 网络接口
│  ├─ security.c          # 安全接口
│  └─ system.c            # 系统接口
├─ netbsd/
│  └─ ...
└─ openbsd/
   └─ ...
```

### 核心接口

```c
// 平台初始化
FFResult platform_init(void);

// 平台清理
void platform_cleanup(void);

// 事件轮询
FFResult platform_poll_events(void);

// 系统信息
FFResult platform_get_system_info(char* buffer, size_t buffer_size);

// 网络接口
FFResult platform_get_network_interfaces(char* buffer, size_t buffer_size);

// 进程管理
FFResult platform_get_process_info(char* buffer, size_t buffer_size);

// 内存信息
FFResult platform_get_memory_info(char* buffer, size_t buffer_size);

// 系统负载
FFResult platform_get_system_load(double* load1, double* load5, double* load15);
```

### BSD 特性接口

```c
// 获取 CPU 信息
FFResult platform_get_cpu_info(char* buffer, size_t buffer_size);

// 获取运行时间
FFResult platform_get_uptime(uint64_t* uptime);

// 获取电池信息
FFResult platform_get_battery_info(int* percentage, bool* is_charging);
```

## 网络功能

### Socket 操作

```c
int platform_create_socket(int domain, int type, int protocol);
int platform_bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
int platform_listen(int sockfd, int backlog);
int platform_connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
ssize_t platform_send(int sockfd, const void* buf, size_t len, int flags);
ssize_t platform_recv(int sockfd, void* buf, size_t len, int flags);
int platform_close(int fd);
int platform_set_nonblocking(int fd, bool nonblocking);
```

### 地址操作

```c
int platform_get_local_address(int fd, char* ip, uint16_t* port);
int platform_get_peer_address(int fd, char* ip, uint16_t* port);
```

## 安全功能

### 安全初始化

```c
FFResult platform_security_init(void);
void platform_security_cleanup(void);
```

### 安全检查

```c
bool platform_is_rooted(void);
bool platform_check_integrity(void);
bool platform_detect_injection(void);
bool platform_check_mitm(void);
```

### 数据加密

```c
FFResult platform_encrypt_data(const uint8_t* input, size_t input_len,
                               uint8_t* output, size_t* output_len,
                               const uint8_t* key, size_t key_len);

FFResult platform_decrypt_data(const uint8_t* input, size_t input_len,
                               uint8_t* output, size_t* output_len,
                               const uint8_t* key, size_t key_len);
```

### 设备指纹

```c
FFResult platform_get_device_fingerprint(uint8_t* fingerprint, size_t* fingerprint_len);
```

## 系统信息

### FreeBSD 特定信息

```c
// 使用 sysctl 获取 FreeBSD 特定信息
sysctlbyname("kern.ostype", ...);      // OS 类型
sysctlbyname("kern.osrelease", ...);   // OS 版本
sysctlbyname("hw.physmem", ...);       // 物理内存
sysctlbyname("hw.model", ...);         // CPU 型号
sysctlbyname("hw.ncpu", ...);          // CPU 核心数
sysctlbyname("vm.loadavg", ...);       // 系统负载
```

### NetBSD 特定信息

```c
// 使用 sysctl 获取 NetBSD 特定信息
sysctlbyname("kern.ostype", ...);
sysctlbyname("kern.osrelease", ...);
sysctlbyname("hw.physmem", ...);
sysctlbyname("hw.model", ...);
sysctlbyname("hw.ncpu", ...);
sysctlbyname("vm.loadavg", ...);
```

### OpenBSD 特定信息

```c
// 使用 sysctl 获取 OpenBSD 特定信息
sysctlbyname("kern.ostype", ...);
sysctlbyname("kern.osrelease", ...);
sysctlbyname("hw.physmem", ...);
sysctlbyname("hw.model", ...);
sysctlbyname("hw.ncpu", ...);
sysctlbyname("vm.loadavg", ...);
```

## 防火墙配置

### FreeBSD (pf)

编辑 `/etc/pf.conf`：

```bash
# 允许 P2P 端口
pass in proto udp from any to any port 3478:3479
pass out proto udp from any to any port 3478:3479

# 允许 STUN
pass in proto udp from any to any port 19302
pass out proto udp from any to any port 19302
```

启用 pf：

```bash
pfctl -e /etc/pf.conf
```

### OpenBSD (pf)

编辑 `/etc/pf.conf`：

```bash
# 允许 P2P 端口
pass in proto udp from any to any port 3478:3479
pass out proto udp from any to any port 3478:3479
```

启用 pf：

```bash
pfctl -e
```

### NetBSD (ipfilter)

编辑 `/etc/ipf.conf`：

```bash
# 允许 P2P 端口
pass in quick proto udp from any to any port = 3478
pass in quick proto udp from any to any port = 3479
pass out quick proto udp from any to any port = 3478
pass out quick proto udp from any to any port = 3479
```

启用 ipfilter：

```bash
ipf -E
ipf -f /etc/ipf.conf
```

## 安全加固

### FreeBSD

1. **使用 Jail 隔离**：
   ```bash
   jail -c name=findfriend path=/var/empty persist
   ```

2. **设置资源限制**：
   ```bash
   vi /etc/login.conf
   # 添加 findfriend 类
   ```

### OpenBSD

1. **使用 chroot**：
   ```bash
   chroot /var/empty /path/to/findfriend
   ```

2. **使用 pledge**：
   ```c
   pledge("stdio inet", NULL);
   ```

### NetBSD

1. **使用 chroot**：
   ```bash
   chroot /var/empty /path/to/findfriend
   ```

## 性能调优

### FreeBSD

```bash
# 网络缓冲区
sysctl net.inet.udp.recvspace=65536
sysctl net.inet.udp.sendspace=65536

# 文件描述符
sysctl kern.maxfiles=65536
sysctl kern.maxfilesperproc=32768
```

### NetBSD

```bash
# 网络缓冲区
sysctl net.inet.udp.recvspace=65536
sysctl net.inet.udp.sendspace=65536

# 文件描述符
sysctl kern.maxfiles=65536
```

### OpenBSD

```bash
# 文件描述符
ulimit -n 65536
```

## 故障排除

### 编译问题

1. **CMake 版本**：
   ```bash
   cmake --version  # 需要 >= 3.16
   ```

2. **编译器**：
   ```bash
   cc --version  # 需要支持 C99
   ```

### 运行时问题

1. **权限问题**：
   ```bash
   # 检查端口权限
   sudo ./findfriend
   ```

2. **库依赖**：
   ```bash
   # 检查库依赖
   ldd ./findfriend
   ```

3. **网络问题**：
   ```bash
   # 检查防火墙
   pfctl -s all
   ```

## 测试

### 本地测试

```bash
# 运行 CLI 工具
./build_test/ff_cli --help

# 初始化 P2P
./build_test/ff_cli --p2p-init

# 检查平台信息
./build_test/ff_cli --platform-info
```

### 网络测试

```bash
# 测试 STUN 连接
nc -u stun.l.google.com 19302

# 测试端口
nc -lu 3478
```

## 部署

### 目录结构

```
/opt/findfriend/
├─ bin/
│  └─ findfriend
├─ etc/
│  └─ findfriend.conf
├─ var/
│  ├─ log/
│  └─ data/
└─ lib/
   └─ libfindfriend.a
```

### 启动脚本

```bash
#!/bin/sh
/opt/findfriend/bin/findfriend --daemon --config /opt/findfriend/etc/findfriend.conf
```

## 维护

### 日志管理

```bash
# 轮转日志
newsyslog /etc/newsyslog.conf

# 查看日志
tail -f /var/log/findfriend.log
```

### 更新

```bash
# 停止服务
killall findfriend

# 更新代码
cd /path/to/findfriend
git pull

# 重新编译
mkdir build && cd build
cmake ..
cmake --build .

# 重启服务
/opt/findfriend/bin/findfriend --daemon
```

## 参考资源

- [FreeBSD 手册](https://www.freebsd.org/doc/en_US.ISO8859-1/books/handbook/)
- [NetBSD 指南](https://www.netbsd.org/docs/guide/)
- [OpenBSD 文档](https://www.openbsd.org/docs.html)
