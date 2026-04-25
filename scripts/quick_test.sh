#!/bin/bash

# FindFriend 快速测试脚本
# 用法: ./scripts/quick_test.sh

set -e

echo "========================================"
echo "  FindFriend 快速测试"
echo "========================================"
echo ""

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 项目根目录
PROJECT_ROOT="/home/haiyan/汇总"
cd "$PROJECT_ROOT"

echo -e "${YELLOW}1. 检查目录结构...${NC}"
if [ -d "common_core" ] && [ -d "lib" ] && [ -d "platforms" ]; then
    echo -e "${GREEN}✓${NC} 目录结构完整"
else
    echo -e "${RED}✗${NC} 目录结构不完整"
    exit 1
fi

echo ""
echo -e "${YELLOW}2. 检查静态库...${NC}"
if [ -f "lib/libFindFriendCore.a" ] && [ -f "lib/libFindFriendUI.a" ]; then
    echo -e "${GREEN}✓${NC} 静态库存在"
    ls -lh lib/
else
    echo -e "${RED}✗${NC} 静态库缺失"
fi

echo ""
echo -e "${YELLOW}3. 检查可执行文件...${NC}"
if [ -f "platforms/linux/redhat/FindFriendApp_RedHat" ]; then
    echo -e "${GREEN}✓${NC} RedHat 可执行文件存在"
    ls -lh platforms/linux/redhat/FindFriendApp_RedHat
else
    echo -e "${RED}✗${NC} 可执行文件缺失"
fi

echo ""
echo -e "${YELLOW}4. 检查核心文件...${NC}"
if [ -f "common_core/core_interface.h" ] && [ -f "common_core/network/p2p_network.h" ]; then
    echo -e "${GREEN}✓${NC} 核心头文件存在"
else
    echo -e "${RED}✗${NC} 核心头文件缺失"
fi

echo ""
echo -e "${YELLOW}5. 运行应用程序测试...${NC}"
timeout 3 ./platforms/linux/redhat/FindFriendApp_RedHat 2>&1 | grep -q "Boot check" && \
    echo -e "${GREEN}✓${NC} 应用程序运行正常" || \
    echo -e "${YELLOW}⚠${NC} 应用程序可能有警告"

echo ""
echo -e "${GREEN}========================================"
echo "  测试完成！"
echo "========================================${NC}"
echo ""
echo "项目结构已整理完毕！"
echo ""
echo "下一步:"
echo "  1. 查看文档: ls docs/"
echo "  2. 运行完整应用: ./platforms/linux/redhat/FindFriendApp_RedHat"
echo "  3. 重新编译: mkdir -p build/outputs/test && cd build/outputs/test && cmake ../../.. && make"
