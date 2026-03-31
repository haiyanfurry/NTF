#include <jni.h>
#include <string>
#include <android/log.h>

#include "network/GoodSamaritan.h"
#include "network/MediaTransfer.h"
#include "models/User.h"
#include "models/Database.h"

#define TAG "FindFriendApp"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// 全局对象
GoodSamaritan* gGoodSamaritan = nullptr;
MediaTransfer* gMediaTransfer = nullptr;
Database* gDatabase = nullptr;

// 定义native方法
extern "C" JNIEXPORT jstring JNICALL
Java_com_findfriend_app_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    LOGI("Hello from C++");
    return env->NewStringUTF(hello.c_str());
}

// 初始化应用
extern "C" JNIEXPORT jint JNICALL
Java_com_findfriend_app_MainActivity_initApp(
        JNIEnv* env,
        jobject /* this */) {
    LOGI("Initializing FindFriendApp");
    
    // 初始化全局对象
    gGoodSamaritan = new GoodSamaritan();
    gMediaTransfer = new MediaTransfer();
    gDatabase = new Database();
    
    // 初始化网络
    if (!gGoodSamaritan->initNetwork()) {
        LOGE("Failed to initialize network");
        return -1;
    }
    
    // 初始化传输
    if (!gMediaTransfer->initTransfer()) {
        LOGE("Failed to initialize transfer");
        return -1;
    }
    
    // 初始化数据库
    if (!gDatabase->init()) {
        LOGE("Failed to initialize database");
        return -1;
    }
    
    LOGI("FindFriendApp initialized successfully");
    return 0;
}

// 登录功能
extern "C" JNIEXPORT jint JNICALL
Java_com_findfriend_app_MainActivity_login(
        JNIEnv* env,
        jobject /* this */,
        jstring username,
        jstring password) {
    const char* c_username = env->GetStringUTFChars(username, nullptr);
    const char* c_password = env->GetStringUTFChars(password, nullptr);
    
    LOGI("Login attempt: %s", c_username);
    
    // 登录验证
    User user;
    bool success = gDatabase->login(c_username, c_password, user);
    
    env->ReleaseStringUTFChars(username, c_username);
    env->ReleaseStringUTFChars(password, c_password);
    
    return success ? 0 : -1; // 0表示成功，-1表示失败
}

// 开始同城用户发现
extern "C" JNIEXPORT jint JNICALL
Java_com_findfriend_app_MainActivity_startLocalDiscovery(
        JNIEnv* env,
        jobject /* this */) {
    LOGI("Starting local discovery");
    
    if (gGoodSamaritan && gGoodSamaritan->startLocalDiscovery()) {
        LOGI("Local discovery started successfully");
        return 0;
    } else {
        LOGE("Failed to start local discovery");
        return -1;
    }
}

// 停止同城用户发现
extern "C" JNIEXPORT void JNICALL
Java_com_findfriend_app_MainActivity_stopLocalDiscovery(
        JNIEnv* env,
        jobject /* this */) {
    LOGI("Stopping local discovery");
    
    if (gGoodSamaritan) {
        gGoodSamaritan->stopLocalDiscovery();
        LOGI("Local discovery stopped successfully");
    }
}

// 设置服务器IP
extern "C" JNIEXPORT jint JNICALL
Java_com_findfriend_app_MainActivity_setServerIP(
        JNIEnv* env,
        jobject /* this */,
        jstring ip) {
    const char* c_ip = env->GetStringUTFChars(ip, nullptr);
    LOGI("Setting server IP: %s", c_ip);
    
    bool success = false;
    if (gGoodSamaritan) {
        success = gGoodSamaritan->setServerIP(c_ip);
    }
    
    env->ReleaseStringUTFChars(ip, c_ip);
    return success ? 0 : -1; // 0表示成功，-1表示失败
}
