# FindFriend 编译指南

## 1. 环境要求

### 1.1 必需工具

| 工具 | 版本 | 用途 |
|------|------|------|
| GCC / Clang | 10+ / 12+ | C/C++ 编译器 |
| CMake | 3.20+ | 构建系统 |
| Make | 4.0+ | 构建工具 |
| pthread | - | 线程支持 |
| OpenSSL | 1.1.0+ | TLS/加密 |

### 1.2 可选工具

| 工具 | 版本 | 用途 |
|------|------|------|
| Rust | 1.60+ | 安全模块编译 |
| Cargo | 1.60+ | Rust 包管理 |
| GDB | 10+ | 调试器 |
| Valgrind | 3.17+ | 内存检查 |

---

## 2. 平台特定要求

### 2.1 Linux

**Ubuntu/Debian**:
```bash
sudo apt update
sudo apt install build-essential cmake libssl-dev libpthread-stubs0-dev
```

**Fedora/RHEL**:
```bash
sudo dnf install gcc gcc-c++ cmake openssl-devel
```

**Arch Linux**:
```bash
sudo pacman -S base-devel cmake openssl
```

### 2.2 Windows

**使用 MSYS2**:
```bash
pacman -S mingw-w64-gcc cmake make openssl-devel
```

**使用 Visual Studio**:
- 需要 Visual Studio 2019 或更高版本
- 使用 "C++ Desktop Development" 工作负载

### 2.3 macOS

```bash
xcode-select --install
brew install cmake openssl
```

### 2.4 Android

**使用 NDK**:
```bash
export ANDROID_NDK_HOME=/path/to/ndk
export PATH=$PATH:$ANDROID_NDK_HOME
```

### 2.5 BSD

**FreeBSD**:
```bash
sudo pkg install cmake openssl
```

**OpenBSD/NetBSD**:
```bash
sudo pkg_add cmake openssl
```

---

## 3. 获取源码

### 3.1 Git 克隆

```bash
git clone https://github.com/your-org/findfriend.git
cd findfriend
```

### 3.2 目录结构

```
findfriend/
├── core/                    # 核心业务逻辑
├── security_rust/            # Rust 安全模块
├── platform/                 # 平台适配
├── ui/                      # UI 层
├── tools/                   # 工具
├── docs/                    # 文档
├── CMakeLists.txt           # 主构建文件
└── README.md
```

---

## 4. 构建步骤

### 4.1 标准构建流程

```bash
# 1. 创建构建目录
mkdir build && cd build

# 2. 配置 CMake
cmake ..

# 3. 编译
make -j$(nproc)

# 4. 安装 (可选)
sudo make install
```

### 4.2 CMake 选项

| 选项 | 默认 | 描述 |
|------|------|------|
| `FF_USE_RUST` | ON | 启用 Rust 安全模块 |
| `FF_BUILD_TESTS` | ON | 构建测试 |
| `FF_BUILD_CLI` | ON | 构建 CLI 工具 |
| `FF_ENABLE_LOGGING` | ON | 启用日志 |
| `CMAKE_BUILD_TYPE` | Release | 构建类型 |

**示例**:
```bash
# Debug 构建
cmake .. -DCMAKE_BUILD_TYPE=Debug

# 不使用 Rust (仅 C)
cmake .. -DFF_USE_RUST=OFF

# Release 构建 + 自定义选项
cmake .. -DCMAKE_BUILD_TYPE=Release -DFF_ENABLE_LOGGING=ON
```

---

## 5. Rust 模块构建

### 5.1 安装 Rust

```bash
# Linux/macOS
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

# Windows
# 下载并运行 rustup-init.exe
```

### 5.2 编译 Rust 模块

```bash
# 进入 Rust 模块目录
cd security_rust

# 编译 (debug)
cargo build

# 编译 (release)
cargo build --release

# 运行测试
cargo test
```

### 5.3 手动集成

如果 CMake 自动检测失败，可以手动编译：

```bash
cd security_rust
cargo build --release

# 复制静态库
cp target/release/libff_security.a ../lib/
```

---

## 6. 交叉编译

### 6.1 Android (Linux)

```bash
export ANDROID_NDK_HOME=/opt/android-ndk
export AARCH64_GCC=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android21-clang

cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-21

make
```

### 6.2 iOS (macOS)

```bash
cmake .. \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
    -DCMAKE_OSX_ARCHITECTURES=arm64;x86_64 \
    -DIOS=ON

make
```

### 6.3 Windows (Linux 交叉编译)

```bash
# 安装 MinGW
sudo apt install mingw-w64

# 配置交叉编译
cmake .. \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_C_COMPILER=i686-w64-mingw32-gcc \
    -DCMAKE_RC_COMPILER=i686-w64-mingw32-windres

make
```

---

## 7. 测试

### 7.1 运行测试

```bash
# 进入构建目录
cd build

# 运行所有测试
ctest --output-on-failure

# 运行特定测试
ctest -R unit_user -V

# 运行集成测试
./bin/findfriend_integration_test
```

### 7.2 代码覆盖率

```bash
# 重新编译，开启覆盖率
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON
make

# 运行测试
ctest

# 生成报告
make coverage
```

---

## 8. 调试

### 8.1 GDB 调试

```bash
# 编译 Debug 版本
cmake .. -DCMAKE_BUILD_TYPE=Debug
make

# 启动调试
gdb ./bin/findfriend

# GDB 命令
(gdb) break main
(gdb) run
(gdb) bt          # 打印栈追踪
(gdb) print var    # 打印变量
(gdb) next         # 单步执行
```

### 8.2 Valgrind 内存检查

```bash
# 检查内存泄漏
valgrind --leak-check=full ./bin/findfriend

# 检查未初始化内存
valgrind --track-origins=yes ./bin/findfriend

# 检查并发问题
valgrind --tool=helgrind ./bin/findfriend
```

---

## 9. 性能分析

### 9.1 perf (Linux)

```bash
# 记录性能数据
perf record -g ./bin/findfriend

# 查看报告
perf report
perf report --symbol-filter=user_
```

### 9.2 gprof

```bash
# 编译时启用性能分析
cmake .. -DCMAKE_BUILD_TYPE=Debug -DPROFILING=ON
make

# 运行
./bin/findfriend

# 查看报告
gprof ./bin/findfriend gmon.out > profile.txt
```

---

## 10. 常见问题

### 10.1 编译错误

**问题**: `Could not find CMake`
**解决**: `sudo apt install cmake`

**问题**: `openssl/ssl.h not found`
**解决**: `sudo apt install libssl-dev`

**问题**: `Rust not found`
**解决**: 安装 Rust 或使用 `-DFF_USE_RUST=OFF`

### 10.2 运行时错误

**问题**: `Segmentation fault`
**解决**: 使用 GDB 或 Valgrind 调试

**问题**: `Failed to load security module`
**解决**: 检查 Rust 库是否正确编译和链接

---

## 11. 发布构建

### 11.1 Release 构建

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DFF_USE_RUST=ON
make -j$(nproc)
```

### 11.2 打包

```bash
# 创建分发包
make package

# 或手动打包
tar czvf findfriend-5.0-linux-x86_64.tar.gz \
    -C build/bin findfriend \
    -C build/lib libfindfriend*.a \
    -C docs .
```

---

**文档版本**: 1.0
**更新日期**: 2026-04-26
