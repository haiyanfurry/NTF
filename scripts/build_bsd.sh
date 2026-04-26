#!/bin/bash

# FindFriend BSD/Unix 平台构建脚本
# 用法: ./scripts/build_bsd.sh [freebsd|openbsd|netbsd]

set -e

PLATFORM="${1:-freebsd}"

echo "========================================"
echo "  FindFriend BSD/Unix 构建脚本"
echo "  平台: $PLATFORM"
echo "========================================"
echo ""

PROJECT_ROOT="/home/haiyan/汇总"
cd "$PROJECT_ROOT"

case "$PLATFORM" in
    freebsd)
        BUILD_DIR="build/outputs/freebsd"
        PLATFORM_DIR="platforms/unix/freebsd"
        ;;
    openbsd)
        BUILD_DIR="build/outputs/openbsd"
        PLATFORM_DIR="platforms/unix/openbsd"
        ;;
    netbsd)
        BUILD_DIR="build/outputs/netbsd"
        PLATFORM_DIR="platforms/unix/netbsd"
        ;;
    *)
        echo "错误: 不支持的平台 $PLATFORM"
        echo "支持的平台: freebsd, openbsd, netbsd"
        exit 1
        ;;
esac

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "配置 CMake for $PLATFORM..."
cmake "../../../${PLATFORM_DIR}" > /dev/null 2>&1 && echo "✓ CMake 配置成功" || { echo "✗ CMake 配置失败"; exit 1; }

echo "编译项目..."
make -j4 && echo "✓ 编译成功" || { echo "✗ 编译失败"; exit 1; }

echo ""
echo "========================================"
echo "  构建完成！"
echo "========================================"
echo ""
echo "输出文件:"
ls -lh bin/ 2>/dev/null || echo "  bin/ 目录为空"
ls -lh lib/ 2>/dev/null || echo "  lib/ 目录为空"
echo ""
echo "下一步:"
echo "  cd $BUILD_DIR"
echo "  ./bin/FindFriendCLI_BSD"
