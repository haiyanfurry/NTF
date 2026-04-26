# BSD 平台编译和运行指南

## 支持的 BSD 平台

- **FreeBSD**：12.0+ 版本
- **NetBSD**：9.0+ 版本
- **OpenBSD**：7.0+ 版本

## 系统要求

### 通用要求

- **CMake** (>= 3.16)
- **C99 兼容编译器** (clang 或 gcc)
- **Rust** (>= 1.60) - 用于安全模块
- **OpenSSL** - 用于加密功能

### FreeBSD 特定要求

```bash
pkg install cmake rust openssl
```

### NetBSD 特定要求

```bash
pkgin install cmake rust openssl
```

### OpenBSD 特定要求

```bash
pkg_add cmake rust openssl
```

## 编译步骤

### 1. 克隆代码

```bash
git clone https://github.com/findfriend-project/findfriend.git
cd findfriend
```

### 2. 配置构建

#### FreeBSD

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DFREEBSD=ON
```

#### NetBSD

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNETBSD=ON
```

#### OpenBSD

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DOPENBSD=ON
```

### 3. 编译

```bash
cmake --build .
```

### 4. 运行测试

```bash
cmake --build . --target test
```

## 运行应用

### 命令行工具

```bash
./build_test/ff_cli --help
```

### 后台服务

```bash
./common_core/p2p/ff_p2p_node --daemon
```

## 平台特定注意事项

### FreeBSD

- 使用 `pkg` 包管理器安装依赖
- 确保 `pf` 防火墙允许 P2P 端口（默认 3478-3479 UDP）
- 建议使用 `jail` 隔离运行

### NetBSD

- 使用 `pkgin` 包管理器安装依赖
- 确保 `ipfilter` 防火墙允许 P2P 端口
- 建议使用 `chroot` 隔离运行

### OpenBSD

- 使用 `pkg_add` 包管理器安装依赖
- 确保 `pf` 防火墙允许 P2P 端口
- 建议使用 `chroot` 和 `pledge` 限制权限

## 网络配置

### NAT 穿透设置

1. **STUN 服务器配置**：
   ```bash
   ./ff_cli --set-stun stun.l.google.com:19302
   ```

2. **端口转发**（可选）：
   - 在路由器上转发 UDP 端口 3478-3479 到本地机器

### 防火墙规则

#### FreeBSD (pf)

```bash
# /etc/pf.conf
pass in proto udp from any to any port 3478:3479 keep state
```

#### OpenBSD (pf)

```bash
# /etc/pf.conf
pass in proto udp from any to any port 3478:3479
```

#### NetBSD (ipfilter)

```bash
# /etc/ipf.conf
pass in quick proto udp from any to any port = 3478
pass in quick proto udp from any to any port = 3479
```

## 性能优化

### 系统调优

#### FreeBSD

```bash
# 增加网络缓冲区
sysctl net.inet.udp.recvspace=65536
sysctl net.inet.udp.sendspace=65536

# 增加文件描述符限制
sysctl kern.maxfiles=65536
```

#### NetBSD

```bash
# 增加网络缓冲区
sysctl net.inet.udp.recvspace=65536
sysctl net.inet.udp.sendspace=65536

# 增加文件描述符限制
sysctl kern.maxfiles=65536
```

#### OpenBSD

```bash
# 增加文件描述符限制
ulimit -n 65536
```

## 故障排除

### 常见问题

1. **编译失败**：
   - 检查依赖是否安装完整
   - 确保 CMake 版本 >= 3.16
   - 检查 Rust 版本 >= 1.60

2. **P2P 连接失败**：
   - 检查防火墙是否允许 UDP 端口
   - 尝试使用不同的 STUN 服务器
   - 检查 NAT 类型是否支持穿透

3. **安全模块初始化失败**：
   - 检查 Rust 环境是否正确配置
   - 确保 OpenSSL 库可用

### 日志查看

```bash
# 查看应用日志
./ff_cli --logs

# 查看安全日志
./ff_cli --security-logs
```

## 最佳实践

1. **生产环境**：
   - 使用 `jail`/`chroot` 隔离运行
   - 配置适当的防火墙规则
   - 定期更新依赖和安全补丁

2. **开发环境**：
   - 使用 `Debug` 构建类型进行调试
   - 启用详细日志
   - 使用 `ff_cli` 工具进行功能测试

## 联系我们

如有问题或建议，请通过以下方式联系：

- **GitHub Issues**：https://github.com/findfriend-project/findfriend/issues
- **邮件**：support@findfriend.com
- **社区**：https://discord.gg/findfriend
