#ifndef PHOTO_TRANSFER_MANAGER_H
#define PHOTO_TRANSFER_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// 照片传输状态
#define PHOTO_STATUS_PENDING 0      // 待传输
#define PHOTO_STATUS_TRANSFERRING 1 // 传输中
#define PHOTO_STATUS_COMPLETED 2    // 已完成
#define PHOTO_STATUS_FAILED 3       // 传输失败

// 照片结构
typedef struct {
    char* photo_id;              // 照片ID
    char* file_path;             // 文件路径
    char* file_name;             // 文件名
    size_t file_size;            // 文件大小
    int status;                  // 传输状态
    float progress;              // 传输进度 (0-100)
    char* create_time;           // 创建时间
    char* transfer_time;         // 传输时间
    char* error_message;         // 错误信息
} Photo;

// 照片传输管理器
typedef struct {
    void* network_manager;      // 网络管理器
    Photo* photos;              // 照片列表
    size_t photo_count;          // 照片数量
    size_t max_photos;           // 最大照片数（默认250）
    bool is_transferring;        // 是否正在传输
    bool auto_save;              // 是否自动保存到相册
} PhotoTransferManager;

// 照片传输回调函数类型
typedef void (*PhotoTransferCallback)(void* user_data, int status, Photo* photos);

// 照片传输进度回调函数类型
typedef void (*PhotoProgressCallback)(void* user_data, const char* photo_id, float progress);

// 初始化照片传输管理器
PhotoTransferManager* photo_transfer_manager_init();

// 销毁照片传输管理器
void photo_transfer_manager_destroy(PhotoTransferManager* manager);

// 添加照片到传输列表
bool photo_transfer_manager_add_photo(PhotoTransferManager* manager, const char* file_path, const char* file_name);

// 批量添加照片到传输列表
bool photo_transfer_manager_add_photos(PhotoTransferManager* manager, const char** file_paths, const char** file_names, size_t count);

// 开始传输照片
bool photo_transfer_manager_start_transfer(PhotoTransferManager* manager, PhotoTransferCallback callback, void* user_data);

// 停止传输照片
bool photo_transfer_manager_stop_transfer(PhotoTransferManager* manager);

// 取消传输照片
bool photo_transfer_manager_cancel_transfer(PhotoTransferManager* manager, const char* photo_id);

// 获取传输状态
bool photo_transfer_manager_get_transfer_status(PhotoTransferManager* manager, PhotoTransferCallback callback, void* user_data);

// 清理传输列表
bool photo_transfer_manager_clear_photos(PhotoTransferManager* manager);

// 设置传输进度回调
void photo_transfer_manager_set_progress_callback(PhotoTransferManager* manager, PhotoProgressCallback callback, void* user_data);

// 设置自动保存到相册
void photo_transfer_manager_set_auto_save(PhotoTransferManager* manager, bool auto_save);

// 获取最大照片数
size_t photo_transfer_manager_get_max_photos(PhotoTransferManager* manager);

// 设置最大照片数
bool photo_transfer_manager_set_max_photos(PhotoTransferManager* manager, size_t max_photos);

#endif // PHOTO_TRANSFER_MANAGER_H
