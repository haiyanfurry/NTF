#ifndef MULTIMEDIA_TRANSFER_MANAGER_H
#define MULTIMEDIA_TRANSFER_MANAGER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// 媒体类型
#define MEDIA_TYPE_PHOTO 0     // 照片
#define MEDIA_TYPE_VIDEO 1     // 视频
#define MEDIA_TYPE_AUDIO 2     // 音频

// 压缩模式
#define COMPRESSION_MODE_NONE 0    // 无压缩（无损）
#define COMPRESSION_MODE_LOW 1     // 低压缩
#define COMPRESSION_MODE_MEDIUM 2  // 中等压缩
#define COMPRESSION_MODE_HIGH 3    // 高压缩

// 媒体传输状态
#define MEDIA_STATUS_PENDING 0      // 待传输
#define MEDIA_STATUS_TRANSFERRING 1 // 传输中
#define MEDIA_STATUS_COMPLETED 2    // 已完成
#define MEDIA_STATUS_FAILED 3       // 传输失败

// 媒体文件结构
typedef struct {
    char* media_id;              // 媒体ID
    char* file_path;             // 文件路径
    char* file_name;             // 文件名
    size_t file_size;            // 文件大小
    int type;                    // 媒体类型
    int status;                  // 传输状态
    float progress;              // 传输进度 (0-100)
    int compression_mode;        // 压缩模式
    size_t compressed_size;      // 压缩后大小
    char* create_time;           // 创建时间
    char* transfer_time;         // 传输时间
    char* error_message;         // 错误信息
} MediaFile;

// 多媒体传输管理器
typedef struct {
    void* network_manager;      // 网络管理器
    MediaFile* media_files;     // 媒体文件列表
    size_t media_count;          // 媒体文件数量
    bool is_transferring;        // 是否正在传输
    bool auto_save;              // 是否自动保存
} MultimediaTransferManager;

// 多媒体传输回调函数类型
typedef void (*MultimediaTransferCallback)(void* user_data, int status, MediaFile* media_files);

// 多媒体传输进度回调函数类型
typedef void (*MultimediaProgressCallback)(void* user_data, const char* media_id, float progress);

// 初始化多媒体传输管理器
MultimediaTransferManager* multimedia_transfer_manager_init();

// 销毁多媒体传输管理器
void multimedia_transfer_manager_destroy(MultimediaTransferManager* manager);

// 添加媒体文件到传输列表
bool multimedia_transfer_manager_add_media(MultimediaTransferManager* manager, const char* file_path, const char* file_name, int media_type, int compression_mode);

// 批量添加媒体文件到传输列表
bool multimedia_transfer_manager_add_media_files(MultimediaTransferManager* manager, const char** file_paths, const char** file_names, int* media_types, int* compression_modes, size_t count);

// 开始传输媒体文件
bool multimedia_transfer_manager_start_transfer(MultimediaTransferManager* manager, MultimediaTransferCallback callback, void* user_data);

// 停止传输媒体文件
bool multimedia_transfer_manager_stop_transfer(MultimediaTransferManager* manager);

// 取消传输媒体文件
bool multimedia_transfer_manager_cancel_transfer(MultimediaTransferManager* manager, const char* media_id);

// 获取传输状态
bool multimedia_transfer_manager_get_transfer_status(MultimediaTransferManager* manager, MultimediaTransferCallback callback, void* user_data);

// 清理传输列表
bool multimedia_transfer_manager_clear_media(MultimediaTransferManager* manager);

// 设置传输进度回调
void multimedia_transfer_manager_set_progress_callback(MultimediaTransferManager* manager, MultimediaProgressCallback callback, void* user_data);

// 设置自动保存
void multimedia_transfer_manager_set_auto_save(MultimediaTransferManager* manager, bool auto_save);

// 获取媒体文件数量
size_t multimedia_transfer_manager_get_media_count(MultimediaTransferManager* manager);

// 获取媒体文件
MediaFile* multimedia_transfer_manager_get_media(MultimediaTransferManager* manager, size_t index);

#endif // MULTIMEDIA_TRANSFER_MANAGER_H
