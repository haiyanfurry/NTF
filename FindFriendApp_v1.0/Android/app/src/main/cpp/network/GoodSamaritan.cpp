#include "GoodSamaritan.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <cstring>
#include <android/log.h>

#define TAG "GoodSamaritan"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

GoodSamaritan::GoodSamaritan() : udpSocket(-1), discoveryRunning(false) {
    currentServerIP = "127.0.0.1"; // 默认本地IP
}

GoodSamaritan::~GoodSamaritan() {
    stopLocalDiscovery();
    if (udpSocket != -1) {
        close(udpSocket);
    }
}

bool GoodSamaritan::initNetwork() {
    // 创建UDP socket
    udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) {
        LOGE("Failed to create UDP socket");
        return false;
    }

    // 设置socket选项
    int opt = 1;
    if (setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) == -1) {
        LOGE("Failed to set broadcast option");
        close(udpSocket);
        udpSocket = -1;
        return false;
    }

    LOGI("Network initialized successfully");
    return true;
}

bool GoodSamaritan::startLocalDiscovery() {
    if (discoveryRunning) {
        LOGI("Discovery already running");
        return true;
    }

    if (udpSocket == -1 && !initNetwork()) {
        return false;
    }

    discoveryRunning = true;
    std::thread discoveryThread(&GoodSamaritan::discoveryThread, this);
    discoveryThread.detach();

    LOGI("Local discovery started");
    return true;
}

void GoodSamaritan::stopLocalDiscovery() {
    discoveryRunning = false;
    LOGI("Local discovery stopped");
}

bool GoodSamaritan::setServerIP(const std::string& ip) {
    // 简单的IP格式验证
    struct sockaddr_in sa;
    if (inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr)) == 1) {
        currentServerIP = ip;
        LOGI("Server IP set to: %s", ip.c_str());
        return true;
    } else {
        LOGE("Invalid IP address: %s", ip.c_str());
        return false;
    }
}

std::string GoodSamaritan::getServerIP() {
    return currentServerIP;
}

std::vector<std::string> GoodSamaritan::discoverLocalDevices() {
    std::vector<std::string> devices;
    // 这里可以实现具体的设备发现逻辑
    // 暂时返回一个空列表
    return devices;
}

void* GoodSamaritan::discoveryThread(void* arg) {
    GoodSamaritan* instance = static_cast<GoodSamaritan*>(arg);
    struct sockaddr_in broadcastAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[1024];

    // 设置广播地址
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(8888);
    broadcastAddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    while (instance->discoveryRunning) {
        // 发送广播消息
        const char* message = "FindFriendApp Discovery";
        sendto(instance->udpSocket, message, strlen(message), 0,
               (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

        // 接收响应
        int recvLen = recvfrom(instance->udpSocket, buffer, sizeof(buffer) - 1, 0,
                              (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (recvLen > 0) {
            buffer[recvLen] = '\0';
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
            LOGI("Found device: %s, message: %s", clientIP, buffer);
        }

        // 每2秒发送一次广播
        sleep(2);
    }

    return nullptr;
}