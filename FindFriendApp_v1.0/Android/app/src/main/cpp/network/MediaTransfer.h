#ifndef FINDFRIENDAPP_MEDIATRANSFER_H
#define FINDFRIENDAPP_MEDIATRANSFER_H

#include <string>
#include <vector>

class MediaTransfer {
private:
    int tcpSocket;

public:
    MediaTransfer();
    ~MediaTransfer();

    // 初始化传输
    bool initTransfer();

    // 发送文件
    bool sendFile(const std::string& filePath, const std::string& targetIP);

    // 接收文件
    bool receiveFile(const std::string& savePath);

    // 发送多个文件
    bool sendMultipleFiles(const std::vector<std::string>& filePaths, const std::string& targetIP);

    // 开始接收文件服务
    bool startReceiveService(int port);

    // 停止接收文件服务
    void stopReceiveService();
};

#endif // FINDFRIENDAPP_MEDIATRANSFER_H