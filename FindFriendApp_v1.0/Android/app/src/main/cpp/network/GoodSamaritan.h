#ifndef FINDFRIENDAPP_GOODSAMARITAN_H
#define FINDFRIENDAPP_GOODSAMARITAN_H

#include <string>
#include <vector>

class GoodSamaritan {
private:
    int udpSocket;
    bool discoveryRunning;
    std::string currentServerIP;

public:
    GoodSamaritan();
    ~GoodSamaritan();

    // 初始化网络
    bool initNetwork();

    // 开始同城用户发现
    bool startLocalDiscovery();

    // 停止同城用户发现
    void stopLocalDiscovery();

    // 设置服务器IP
    bool setServerIP(const std::string& ip);

    // 获取当前服务器IP
    std::string getServerIP();

    // 发现本地设备
    std::vector<std::string> discoverLocalDevices();

private:
    // UDP广播线程函数
    static void* discoveryThread(void* arg);
};

#endif // FINDFRIENDAPP_GOODSAMARITAN_H