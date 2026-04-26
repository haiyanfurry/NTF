#!/bin/bash

# FindFriend C <-> Rust 混合架构构建脚本
# 在 Linux 上运行，自动尝试构建（即使没有 Rust 也能工作）

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR/.."
cd "$PROJECT_ROOT"

echo "========================================="
echo "  FindFriend Hybrid Build System"
echo "  (C + Rust)"
echo "========================================="
echo ""

# 检查是否有 Rust 和 Cargo
RUST_AVAILABLE=0
if command -v cargo >/dev/null 2>&1; then
    RUST_AVAILABLE=1
    RUST_VERSION=$(cargo --version)
    echo "✓ Cargo found: $RUST_VERSION"
else
    echo "⚠ Cargo not found. Rust modules will NOT be compiled."
    echo "  Build will proceed with C-only components."
fi

echo ""

# 创建构建目录
BUILD_DIR="build_output"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# 配置 CMake
echo "Configuring CMake..."
if [ "$RUST_AVAILABLE" -eq 1 ]; then
    cmake .. -DFF_USE_RUST_MODULES=ON
else
    cmake .. -DFF_USE_RUST_MODULES=OFF
fi

echo ""
echo "Building FindFriend..."
make -j$(nproc)

echo ""
echo "========================================="
echo "  Build Complete!"
echo "========================================="
echo ""
echo "Output files:"
if [ -f "bin/FindFriendCLI" ]; then
    echo "  ✓ bin/FindFriendCLI - CLI Debug Tool"
fi
if [ -f "lib/libFindFriendCore.a" ]; then
    echo "  ✓ lib/libFindFriendCore.a - Core Library"
fi

echo ""
echo "Run quick test: $BUILD_DIR/bin/FindFriendCLI"
echo ""
