#!/bin/bash

# FindFriend CLI 交互式测试
# 用法: ./scripts/cli_test.sh

cd /home/haiyan/汇总

echo "========================================"
echo "  FindFriend CLI 交互式测试"
echo "========================================"
echo ""
echo "注意: CLI 工具需要先编译"
echo "  mkdir -p build/outputs/test"
echo "  cd build/outputs/test"
echo "  cmake ../../.."
echo "  make"
echo ""
echo "启动 CLI 工具..."
echo "输入 'help' 查看命令，输入 'quit' 退出"
echo ""

# 检查是否有预编译的 CLI
if [ -f "./build/outputs/test/bin/FindFriendCLI" ]; then
    ./build/outputs/test/bin/FindFriendCLI
else
    echo "错误: 未找到预编译的 CLI 工具"
    echo "请先运行 ./scripts/build.sh"
fi
