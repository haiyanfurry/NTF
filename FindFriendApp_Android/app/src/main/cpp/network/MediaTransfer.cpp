#include "MediaTransfer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <android/log.h>

#define TAG "MediaTransfer"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

MediaTransfer::MediaTransfer() : tcpSocket(-1) {
}

MediaTransfer::~MediaTransfer() {
    if (tcpSocket != -1) {
        close(tcpSocket);
    }
}

bool MediaTransfer::initTransfer() {
    // 创建TCP socket
    tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSocket == -1) {
        LOGE("Failed to create TCP socket");
        return false;
    }

    LOGI("Transfer initialized successfully");
    return true;
}

bool MediaTransfer::sendFile(const std::string& filePath, const std::string& targetIP) {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = inet_addr(targetIP.c_str());

    // 连接到目标设备
    if (connect(tcpSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        LOGE("Failed to connect to target device");
        return false;
    }

    // 打开文件
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        LOGE("Failed to open file: %s", filePath.c_str());
        close(tcpSocket);
        tcpSocket = -1;
        return false;
    }

    // 获取文件名
    size_t pos = filePath.find_last_of("/");
    std::string fileName = (pos != std::string::npos) ? filePath.substr(pos + 1) : filePath;

    // 发送文件名长度和文件名
    int fileNameLen = fileName.size();
    send(tcpSocket, &fileNameLen, sizeof(fileNameLen), 0);
    send(tcpSocket, fileName.c_str(), fileNameLen, 0);

    // 发送文件内容
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        send(tcpSocket, buffer, file.gcount(), 0);
    }
    // 发送剩余数据
    if (file.gcount() > 0) {
        send(tcpSocket, buffer, file.gcount(), 0);
    }

    file.close();
    close(tcpSocket);
    tcpSocket = -1;

    LOGI("File sent successfully: %s", filePath.c_str());
    return true;
}

bool MediaTransfer::receiveFile(const std::string& savePath) {
    // 这里可以实现文件接收逻辑
    LOGI("Receiving file to: %s", savePath.c_str());
    return true;
}

bool MediaTransfer::sendMultipleFiles(const std::vector<std::string>& filePaths, const std::string& targetIP) {
    for (const auto& filePath : filePaths) {
        if (!sendFile(filePath, targetIP)) {
            LOGE("Failed to send file: %s", filePath.c_str());
            return false;
        }
    }

    LOGI("All files sent successfully");
    return true;
}

bool MediaTransfer::startReceiveService(int port) {
    // 这里可以实现接收服务逻辑
    LOGI("Starting receive service on port: %d", port);
    return true;
}

void MediaTransfer::stopReceiveService() {
    LOGI("Stopping receive service");
}