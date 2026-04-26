#!/bin/bash

# FindFriend 完整应用测试
# 用法: ./scripts/run_app.sh

cd /home/haiyan/汇总

echo "========================================"
echo "  FindFriend 完整应用测试"
echo "========================================"
echo ""

if [ -f "./platforms/linux/redhat/FindFriendApp_RedHat" ]; then
    echo "运行 FindFriendApp_RedHat..."
    echo ""
    timeout 5 ./platforms/linux/redhat/FindFriendApp_RedHat
    echo ""
    echo "应用已退出"
else
    echo "错误: 找不到可执行文件"
    echo "可能需要先编译项目"
fi
