# FindFriend

## 项目概述

FindFriend 是一个跨平台的去中心化社交发现应用，基于 P2P 网络实现，支持多平台部署。

### 核心特性

- **跨平台支持**：Linux、Windows、macOS、iOS、Android、BSD 系列（FreeBSD、NetBSD、OpenBSD）
- **去中心化 P2P 网络**：客户端即服务端模式，支持公网 NAT 穿透
- **隐私保护**：仅好友可共享位置，显示大致距离而非精确位置
- **安全防护**：Rust 编写的安全模块，提供防黑产、反灰产、完整性验证等功能
- **多功能支持**：消息、游戏、媒体上传/下载在 P2P 网络运行
- **UID 加好友机制**：保证隐私和安全

## 目录结构

```
FindFriend/
├─ common_core/       # 核心模块（C99）
│  ├─ interface/      # 核心接口定义
│  ├─ p2p/            # P2P 网络模块
│  ├─ business/       # 业务逻辑
│  ├─ data/           # 数据管理
│  └─ utils/          # 工具函数
├─ common_ui/         # 跨平台 UI 组件
├─ security_rust/     # 安全模块（Rust）
│  ├─ src/            # Rust 源代码
│  └─ ffi/            # C 接口
├─ platform/          # 平台相关代码
│  ├─ linux/          # Linux 平台
│  ├─ windows/        # Windows 平台
│  ├─ macos/          # macOS 平台
│  ├─ ios/            # iOS 平台
│  ├─ android/        # Android 平台
│  └─ bsd/            # BSD 系列平台
│     ├─ freebsd/     # FreeBSD 实现
│     ├─ netbsd/      # NetBSD 实现
│     └─ openbsd/     # OpenBSD 实现
├─ build_test/        # CLI & 测试工具
├─ lib/               # 静态库
├─ resources/         # 资源文件
├─ docs/              # 文档
├─ README.md          # 项目说明
└─ README_CORE_INTERFACE.md  # 核心接口说明
```

## 平台支持

| 平台 | 支持状态 | 构建系统 | 特殊说明 |
|------|---------|---------|---------|
| Linux | ✅ | CMake | 支持多种发行版 |
| Windows | ✅ | CMake | 支持 Win32/WinUI |
| macOS | ✅ | CMake | 支持 Objective-C/Swift |
| iOS | ✅ | CMake | 支持 Swift |
| Android | ✅ | CMake | 支持 Java/Kotlin + JNI |
| FreeBSD | ✅ | CMake | 原生支持 |
| NetBSD | ✅ | CMake | 原生支持 |
| OpenBSD | ✅ | CMake | 原生支持 |

## 构建指南

### 通用构建步骤

1. **安装依赖**：
   - CMake (>= 3.16)
   - C99 兼容编译器
   - Rust (>= 1.60) - 用于安全模块

2. **配置构建**：
   ```bash
   mkdir build && cd build
   cmake ..
   ```

3. **编译**：
   ```bash
   cmake --build .
   ```

4. **运行测试**：
   ```bash
   cmake --build . --target test
   ```

### 平台特定构建

#### Linux
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

#### Windows
```bash
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

#### macOS
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

#### iOS
```bash
cmake .. -G Xcode -DCMAKE_SYSTEM_NAME=iOS
cmake --build . --config Release
```

#### Android
```bash
cmake .. -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake
cmake --build .
```

#### BSD 系列

##### FreeBSD
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

##### NetBSD
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

##### OpenBSD
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## 安全模块

安全模块使用 Rust 编写，提供以下功能：

- **Boot check & 文件完整性**：验证应用启动时的完整性
- **Root detect & inject filter**：检测设备是否被 Root 或 Jailbreak
- **MITM/SSL/TLS 防护**：防止中间人攻击
- **DoS 防护**：防止拒绝服务攻击
- **XML / 数据包安全检测**：防止注入攻击
- **日志和审计安全模块**：记录安全事件

## P2P 网络

### 核心功能

- **客户端即服务端模式**：每个客户端同时作为服务端
- **NAT 穿透**：支持 UDP Hole Punch、STUN/TURN
- **位置共享**：仅好友可共享位置，显示大致距离
- **消息传递**：加密消息传输
- **游戏功能**：P2P 网络上的游戏
- **媒体传输**：P2P 网络上的媒体上传/下载

### 隐私保护

- **位置模糊**：显示距离范围或模糊坐标
- **好友验证**：UID 加好友机制
- **加密通道**：所有 P2P 通信均加密

## 开发指南

### 模块拆分

项目采用模块化设计，各模块职责清晰：

- **common_core**：核心逻辑，使用 C99
- **security_rust**：安全模块，使用 Rust
- **platform**：平台特定实现
- **common_ui**：跨平台 UI 组件
- **build_test**：CLI 工具和测试

### 快速调试

使用 CLI 工具进行快速功能验证：
```bash
./build_test/ff_cli --help
```

### 安全测试

使用安全模块进行安全测试：
```bash
./build_test/ff_security_test
```

## 文档

- **README.md**：项目说明（本文档）
- **README_CORE_INTERFACE.md**：核心接口说明
- **docs/BSD_GUIDE.md**：BSD 平台编译和运行指南
- **docs/P2P_USAGE.md**：P2P 使用说明
- **docs/SECURITY_MODULE.md**：安全模块说明

## 贡献

欢迎贡献代码，提交 Pull Request 前请确保：

1. 代码风格符合项目规范
2. 所有测试通过
3. 提供详细的提交信息

## 许可证

MIT License
