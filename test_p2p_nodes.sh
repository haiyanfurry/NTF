#!/bin/bash

# P2P 多节点测试脚本
# 启动多个 P2P 节点，测试节点发现和通信

echo "=== P2P 多节点测试 ==="
echo "注意: 每个节点使用相同的广播端口(5000)，但在 localhost 环境下"
echo "      UDP 广播可能无法被其他节点接收，因为所有节点共享同一个 loopback 接口"
echo ""

# 清理之前的进程
pkill -f ff_p2p_test 2>/dev/null
sleep 1

# 检查防火墙状态
echo "[0] 检查防火墙状态..."
if command -v ufw &> /dev/null; then
    echo "    UFW 状态: $(ufw status | head -1)"
fi
echo "    UDP 端口 5000 (BROADCAST_PORT) 检查:"
if command -v netstat &> /dev/null; then
    netstat -ulnp 2>/dev/null | grep :5000 || echo "    端口 5000 未被占用"
elif command -v ss &> /dev/null; then
    ss -ulnp 2>/dev/null | grep :5000 || echo "    端口 5000 未被占用"
fi
echo ""

# 启动节点 1
echo "[1] 启动节点 1..."
cd /home/haiyan/汇总/FindFriend/build/bin
./ff_p2p_test > node1.log 2>&1 &
NODE1_PID=$!
echo "    节点 1 PID: $NODE1_PID"
sleep 3

# 启动节点 2
echo "[2] 启动节点 2..."
./ff_p2p_test > node2.log 2>&1 &
NODE2_PID=$!
echo "    节点 2 PID: $NODE2_PID"
sleep 3

# 等待发现过程
echo "[3] 等待节点发现..."
sleep 5

echo ""
echo "=== 节点 1 日志 ==="
cat node1.log

echo ""
echo "=== 节点 2 日志 ==="
cat node2.log

echo ""
echo "=== 分析 ==="

# 提取关键信息
echo "[4] 节点 1 广播状态:"
grep -E "broadcast|broadcasting" node1.log | tail -5 || echo "    未发现广播日志"

echo ""
echo "[5] 节点 2 广播状态:"
grep -E "broadcast|broadcasting" node2.log | tail -5 || echo "    未发现广播日志"

echo ""
echo "[6] 节点发现统计:"
echo "    节点 1 发现数量: $(grep -c "known nodes count" node1.log 2>/dev/null || echo 0)"
echo "    节点 2 发现数量: $(grep -c "known nodes count" node2.log 2>/dev/null || echo 0)"

echo ""
echo "=== 测试结论 ==="

# 检查是否发现了节点
NODE1_DISCOVERY=$(grep -o "Known nodes: [0-9]*" node1.log 2>/dev/null | head -1 | grep -o "[0-9]*" || echo "0")
NODE2_DISCOVERY=$(grep -o "Known nodes: [0-9]*" node2.log 2>/dev/null | head -1 | grep -o "[0-9]*" || echo "0")

if [ "$NODE1_DISCOVERY" -gt "0" ] || [ "$NODE2_DISCOVERY" -gt "0" ]; then
    echo "✅ 节点发现成功!"
    echo "   节点 1 发现了 $NODE1_DISCOVERY 个节点"
    echo "   节点 2 发现了 $NODE2_DISCOVERY 个节点"
else
    echo "⚠️  节点未发现对方，这在内网环境下可能是正常现象"
    echo ""
    echo "可能原因:"
    echo "  1. localhost 环境下 UDP 广播可能无法正常工作"
    echo "  2. 所有节点共享同一个 loopback 接口 (127.0.0.1)"
    echo "  3. 建议: 在真实局域网环境中测试，节点应位于不同机器上"
    echo ""
    echo "调试建议:"
    echo "  - 使用 tcpdump 或 Wireshark 抓包检查 UDP 广播是否发出"
    echo "  - 检查防火墙规则: sudo iptables -L -n | grep 5000"
    echo "  - 尝试在不同物理机器上运行节点"
fi

# 停止节点
echo ""
echo "[7] 停止所有节点..."
kill $NODE1_PID $NODE2_PID 2>/dev/null
wait $NODE1_PID 2>/dev/null
wait $NODE2_PID 2>/dev/null

echo "[8] 测试完成!"

# 清理日志文件（可选，注释掉以保留日志）
# rm node1.log node2.log 2>/dev/null