#!/bin/bash

# FindFriend 构建脚本
# 用法: ./scripts/build.sh [platform]

set -e

PLATFORM="${1:-test}"
BUILD_DIR="build/outputs/${PLATFORM}"

echo "========================================"
echo "  FindFriend 构建脚本"
echo "  平台: $PLATFORM"
echo "========================================"
echo ""

# 项目根目录
PROJECT_ROOT="/home/haiyan/汇总"
cd "$PROJECT_ROOT"

# 创建构建目录
mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"

# 配置 CMake
echo "配置 CMake..."
cmake ../../.. > /dev/null 2>&1 && echo "✓ CMake 配置成功" || { echo "✗ CMake 配置失败"; exit 1; }

# 编译
echo "编译项目..."
make -j4 && echo "✓ 编译成功" || { echo "✗ 编译失败"; exit 1; }

# 复制库文件到 lib/
echo "复制库文件..."
cp lib/*.a ../../lib/ 2>/dev/null || true

echo ""
echo "========================================"
echo "  构建完成！"
echo "========================================"
echo ""
echo "输出文件:"
ls -lh ../../lib/ 2>/dev/null || echo "  lib/ 目录为空"
ls -lh bin/ 2>/dev/null || echo "  bin/ 目录为空"
echo ""
echo "下一步:"
echo "  cd $BUILD_DIR"
echo "  ./bin/FindFriendCLI"
