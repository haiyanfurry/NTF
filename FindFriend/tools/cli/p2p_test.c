// FindFriend P2P 测试工具

#include "ff_core_interface.h"
#include "ff_p2p_interface.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void print_node_info(P2PNode* node) {
    if (!node) {
        printf("  (null)\n");
        return;
    }
    printf("  Node ID: ");
    for (int i = 0; i < 16; i++) printf("%02x", node->node_id[i]);
    printf("\n");
    printf("  Public IP: %s:%u\n", node->public_ip, node->public_port);
    printf("  UID: %u\n", node->uid);
    printf("  Status: %s\n", node->status == 1 ? "Online" : "Offline");
    printf("  Last seen: %lu\n", (unsigned long)node->last_seen);
}

int main(int argc, char* argv[]) {
    FFResult res;

    printf("=== FindFriend P2P Test ===\n\n");

    // 初始化核心
    res = ff_core_init();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize core: %d\n", res);
        return 1;
    }
    printf("[1] Core initialized\n");

    // 初始化 P2P
    res = ff_p2p_init(NULL, NULL);
    if (res != FF_OK) {
        fprintf(stderr, "Failed to initialize P2P: %d\n", res);
        return 1;
    }
    printf("[2] P2P initialized\n");

    // 启动节点
    res = ff_p2p_start_node(NULL, NULL);
    if (res != FF_OK) {
        fprintf(stderr, "Failed to start node: %d\n", res);
        return 1;
    }
    printf("[3] Node started\n");

    // 获取本节点信息
    P2PNode* my_node = NULL;
    res = ff_p2p_get_node_info(&my_node);
    if (res == FF_OK && my_node) {
        printf("[4] My node info:\n");
        print_node_info(my_node);
        ff_p2p_node_free(my_node);
    } else {
        printf("[4] Failed to get node info: %d\n", res);
    }

    // 启动发现
    res = ff_p2p_start_discovery(NULL, NULL);
    if (res != FF_OK) {
        fprintf(stderr, "Failed to start discovery: %d\n", res);
    } else {
        printf("[5] Discovery started\n");
    }

    // 等待一下让发现完成
    sleep(2);

    // 获取已知节点
    P2PNode* nodes = NULL;
    size_t count = 0;
    res = ff_p2p_get_known_nodes(&nodes, &count);
    if (res == FF_OK) {
        printf("[6] Known nodes: %zu\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("  Node %zu:\n", i + 1);
            print_node_info(&nodes[i]);
        }
        ff_p2p_node_list_free(nodes, count);
    } else {
        printf("[6] Failed to get known nodes: %d\n", res);
    }

    // 检查 P2P 运行状态
    printf("[7] P2P running: %s\n", ff_p2p_is_running() ? "Yes" : "No");

    // 停止节点
    res = ff_p2p_stop_node();
    if (res != FF_OK) {
        fprintf(stderr, "Failed to stop node: %d\n", res);
    } else {
        printf("[8] Node stopped\n");
    }

    printf("\n=== P2P Test Complete ===\n");
    return 0;
}