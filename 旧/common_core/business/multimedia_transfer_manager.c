// 必须在所有头文件之前定义_GNU_SOURCE以使用strdup
#define _GNU_SOURCE

#include "multimedia_transfer_manager.h"
#include "../network/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

// 多媒体传输线程参数
typedef struct {
    MultimediaTransferManager* manager;
    MultimediaTransferCallback callback;
    void* user_data;
} MultimediaTransferThreadArgs;

// 多媒体传输响应回调函数
static void multimedia_transfer_response_callback(void* user_data, int status, const uint8_t* data, size_t length) {
    if (!user_data) {
        return;
    }
    
    // 解析回调数据
    MultimediaTransferCallback callback = (MultimediaTransferCallback)((void**)user_data)[0];
    void* callback_user_data = ((void**)user_data)[1];
    MultimediaTransferManager* manager = (MultimediaTransferManager*)((void**)user_data)[2];
    
    // 模拟解析响应数据
    // 实际应用中应该解析 JSON 数据
    if (status == 200) {
        // 模拟传输成功
        for (size_t i = 0; i < manager->media_count; i++) {
            if (manager->media_files[i].status == MEDIA_STATUS_TRANSFERRING) {
                manager->media_files[i].status = MEDIA_STATUS_COMPLETED;
                manager->media_files[i].progress = 100.0;
                
                // 生成传输时间
                time_t now = time(NULL);
                struct tm* tm_info = localtime(&now);
                char time_str[20];
                strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
                if (manager->media_files[i].transfer_time) {
                    free(manager->media_files[i].transfer_time);
                    manager->media_files[i].transfer_time = NULL;
                }
                manager->media_files[i].transfer_time = strdup(time_str);
                if (!manager->media_files[i].transfer_time) {
                    fprintf(stderr, "Error allocating memory for transfer time\n");
                }
            }
        }
    } else {
        // 模拟传输失败
        for (size_t i = 0; i < manager->media_count; i++) {
            if (manager->media_files[i].status == MEDIA_STATUS_TRANSFERRING) {
                manager->media_files[i].status = MEDIA_STATUS_FAILED;
                if (manager->media_files[i].error_message) {
                    free(manager->media_files[i].error_message);
                    manager->media_files[i].error_message = NULL;
                }
                manager->media_files[i].error_message = strdup("Transfer failed");
                if (!manager->media_files[i].error_message) {
                    fprintf(stderr, "Error allocating memory for error message\n");
                }
            }
        }
    }
    
    // 调用回调函数
    if (callback) {
        callback(callback_user_data, status, manager->media_files);
    }
    
    // 清理资源
    free(user_data);
}

// 多媒体传输线程函数
static void* multimedia_transfer_thread(void* arg) {
    MultimediaTransferThreadArgs* args = (MultimediaTransferThreadArgs*)arg;
    MultimediaTransferManager* manager = args->manager;
    MultimediaTransferCallback callback = args->callback;
    void* user_data = args->user_data;
    
    // 模拟多媒体传输过程
    for (size_t i = 0; i < manager->media_count; i++) {
        if (!manager->is_transferring) {
            break;
        }
        
        // 更新媒体文件状态为传输中
        manager->media_files[i].status = MEDIA_STATUS_TRANSFERRING;
        
        // 模拟压缩过程
        if (manager->media_files[i].compression_mode != COMPRESSION_MODE_NONE) {
            // 模拟压缩进度
            for (int j = 0; j <= 50; j += 10) {
                if (!manager->is_transferring) {
                    break;
                }
                manager->media_files[i].progress = j / 2.0;
                usleep(50000); // 50ms
            }
            
            // 模拟压缩后的大小
            if (manager->is_transferring) {
                switch (manager->media_files[i].compression_mode) {
                    case COMPRESSION_MODE_LOW:
                        manager->media_files[i].compressed_size = manager->media_files[i].file_size * 0.8;
                        break;
                    case COMPRESSION_MODE_MEDIUM:
                        manager->media_files[i].compressed_size = manager->media_files[i].file_size * 0.5;
                        break;
                    case COMPRESSION_MODE_HIGH:
                        manager->media_files[i].compressed_size = manager->media_files[i].file_size * 0.3;
                        break;
                    default:
                        manager->media_files[i].compressed_size = manager->media_files[i].file_size;
                        break;
                }
            }
        }
        
        // 模拟传输进度
        int start_progress = (manager->media_files[i].compression_mode != COMPRESSION_MODE_NONE) ? 50 : 0;
        for (int j = start_progress; j <= 100; j += 10) {
            if (!manager->is_transferring) {
                break;
            }
            manager->media_files[i].progress = j;
            usleep(100000); // 100ms
        }
        
        // 模拟传输完成
        if (manager->is_transferring) {
            manager->media_files[i].status = MEDIA_STATUS_COMPLETED;
            manager->media_files[i].progress = 100.0;
            
            // 生成传输时间
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        char time_str[20];
        strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
        if (manager->media_files[i].transfer_time) {
            free(manager->media_files[i].transfer_time);
            manager->media_files[i].transfer_time = NULL;
        }
        manager->media_files[i].transfer_time = strdup(time_str);
        if (!manager->media_files[i].transfer_time) {
            fprintf(stderr, "Error allocating memory for transfer time\n");
        }
        }
    }
    
    // 传输完成
    manager->is_transferring = false;
    
    // 调用回调函数
    if (callback) {
        callback(user_data, 200, manager->media_files);
    }
    
    free(args);
    return NULL;
}

// 初始化多媒体传输管理器
MultimediaTransferManager* multimedia_transfer_manager_init() {
    MultimediaTransferManager* manager = (MultimediaTransferManager*)malloc(sizeof(MultimediaTransferManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    
    manager->network_manager = NULL;
    manager->media_files = NULL;
    manager->media_count = 0;
    manager->is_transferring = false;
    manager->auto_save = true;
    
    return manager;
}

// 销毁多媒体传输管理器
void multimedia_transfer_manager_destroy(MultimediaTransferManager* manager) {
    if (manager) {
        // 停止传输
        multimedia_transfer_manager_stop_transfer(manager);
        
        // 释放媒体文件列表
        if (manager->media_files) {
            for (size_t i = 0; i < manager->media_count; i++) {
                if (manager->media_files[i].media_id) free(manager->media_files[i].media_id);
                if (manager->media_files[i].file_path) free(manager->media_files[i].file_path);
                if (manager->media_files[i].file_name) free(manager->media_files[i].file_name);
                if (manager->media_files[i].create_time) free(manager->media_files[i].create_time);
                if (manager->media_files[i].transfer_time) free(manager->media_files[i].transfer_time);
                if (manager->media_files[i].error_message) free(manager->media_files[i].error_message);
            }
            free(manager->media_files);
        }
        free(manager);
    }
}

// 添加媒体文件到传输列表
bool multimedia_transfer_manager_add_media(MultimediaTransferManager* manager, const char* file_path, const char* file_name, int media_type, int compression_mode) {
    if (!manager || !file_path || !file_name) {
        return false;
    }
    
    // 检查媒体类型
    if (media_type < MEDIA_TYPE_PHOTO || media_type > MEDIA_TYPE_AUDIO) {
        fprintf(stderr, "Invalid media type\n");
        return false;
    }
    
    // 检查压缩模式
    if (compression_mode < COMPRESSION_MODE_NONE || compression_mode > COMPRESSION_MODE_HIGH) {
        fprintf(stderr, "Invalid compression mode\n");
        return false;
    }
    
    // 检查是否正在传输
    if (manager->is_transferring) {
        fprintf(stderr, "Transfer is in progress\n");
        return false;
    }
    
    // 重新分配媒体文件列表
    MediaFile* new_media_files = (MediaFile*)realloc(manager->media_files, sizeof(MediaFile) * (manager->media_count + 1));
    if (!new_media_files) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    manager->media_files = new_media_files;
    
    // 初始化新媒体文件
    MediaFile* media = &manager->media_files[manager->media_count];
    media->media_id = (char*)malloc(32);
    if (!media->media_id) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    snprintf(media->media_id, 32, "media_%d", (int)time(NULL));
    
    media->file_path = strdup(file_path);
    media->file_name = strdup(file_name);
    media->file_size = 0; // 实际应用中应该获取文件大小
    media->type = media_type;
    media->status = MEDIA_STATUS_PENDING;
    media->progress = 0.0;
    media->compression_mode = compression_mode;
    media->compressed_size = 0;
    
    // 生成创建时间
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_str[20];
    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    media->create_time = strdup(time_str);
    
    media->transfer_time = NULL;
    media->error_message = NULL;
    
    manager->media_count++;
    
    return true;
}

// 批量添加媒体文件到传输列表
bool multimedia_transfer_manager_add_media_files(MultimediaTransferManager* manager, const char** file_paths, const char** file_names, int* media_types, int* compression_modes, size_t count) {
    if (!manager || !file_paths || !file_names || !media_types || !compression_modes || count == 0) {
        return false;
    }
    
    // 检查是否正在传输
    if (manager->is_transferring) {
        fprintf(stderr, "Transfer is in progress\n");
        return false;
    }
    
    // 重新分配媒体文件列表
    MediaFile* new_media_files = (MediaFile*)realloc(manager->media_files, sizeof(MediaFile) * (manager->media_count + count));
    if (!new_media_files) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    manager->media_files = new_media_files;
    
    // 生成创建时间
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_str[20];
    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    
    // 添加媒体文件
    for (size_t i = 0; i < count; i++) {
        // 检查媒体类型
        if (media_types[i] < MEDIA_TYPE_PHOTO || media_types[i] > MEDIA_TYPE_AUDIO) {
            fprintf(stderr, "Invalid media type for file %zu\n", i);
            return false;
        }
        
        // 检查压缩模式
        if (compression_modes[i] < COMPRESSION_MODE_NONE || compression_modes[i] > COMPRESSION_MODE_HIGH) {
            fprintf(stderr, "Invalid compression mode for file %zu\n", i);
            return false;
        }
        
        MediaFile* media = &manager->media_files[manager->media_count + i];
        media->media_id = (char*)malloc(32);
        if (!media->media_id) {
            fprintf(stderr, "Error allocating memory\n");
            return false;
        }
        snprintf(media->media_id, 32, "media_%d_%zu", (int)time(NULL), i);
        
        media->file_path = strdup(file_paths[i]);
        media->file_name = strdup(file_names[i]);
        media->file_size = 0; // 实际应用中应该获取文件大小
        media->type = media_types[i];
        media->status = MEDIA_STATUS_PENDING;
        media->progress = 0.0;
        media->compression_mode = compression_modes[i];
        media->compressed_size = 0;
        media->create_time = strdup(time_str);
        media->transfer_time = NULL;
        media->error_message = NULL;
    }
    
    manager->media_count += count;
    
    return true;
}

// 开始传输媒体文件
bool multimedia_transfer_manager_start_transfer(MultimediaTransferManager* manager, MultimediaTransferCallback callback, void* user_data) {
    if (!manager || manager->is_transferring || manager->media_count == 0) {
        return false;
    }
    
    manager->is_transferring = true;
    
    // 创建传输线程
    MultimediaTransferThreadArgs* args = (MultimediaTransferThreadArgs*)malloc(sizeof(MultimediaTransferThreadArgs));
    if (!args) {
        fprintf(stderr, "Error allocating memory\n");
        manager->is_transferring = false;
        return false;
    }
    args->manager = manager;
    args->callback = callback;
    args->user_data = user_data;
    
    pthread_t thread;
    if (pthread_create(&thread, NULL, multimedia_transfer_thread, args) != 0) {
        fprintf(stderr, "Error creating transfer thread\n");
        free(args);
        manager->is_transferring = false;
        return false;
    }
    
    // 分离线程，让它自行清理
    pthread_detach(thread);
    
    return true;
}

// 停止传输媒体文件
bool multimedia_transfer_manager_stop_transfer(MultimediaTransferManager* manager) {
    if (!manager || !manager->is_transferring) {
        return false;
    }
    
    manager->is_transferring = false;
    
    // 更新未完成的媒体文件状态
        for (size_t i = 0; i < manager->media_count; i++) {
            if (manager->media_files[i].status == MEDIA_STATUS_TRANSFERRING) {
                manager->media_files[i].status = MEDIA_STATUS_FAILED;
                if (manager->media_files[i].error_message) {
                    free(manager->media_files[i].error_message);
                    manager->media_files[i].error_message = NULL;
                }
                manager->media_files[i].error_message = strdup("Transfer stopped");
                if (!manager->media_files[i].error_message) {
                    fprintf(stderr, "Error allocating memory for error message\n");
                }
            }
        }
    
    return true;
}

// 取消传输媒体文件
bool multimedia_transfer_manager_cancel_transfer(MultimediaTransferManager* manager, const char* media_id) {
    if (!manager || !media_id) {
        return false;
    }
    
    // 查找媒体文件
    for (size_t i = 0; i < manager->media_count; i++) {
        if (strcmp(manager->media_files[i].media_id, media_id) == 0) {
            // 更新媒体文件状态
            manager->media_files[i].status = MEDIA_STATUS_FAILED;
            if (manager->media_files[i].error_message) {
                free(manager->media_files[i].error_message);
                manager->media_files[i].error_message = NULL;
            }
            manager->media_files[i].error_message = strdup("Transfer cancelled");
            if (!manager->media_files[i].error_message) {
                fprintf(stderr, "Error allocating memory for error message\n");
            }
            return true;
        }
    }
    
    return false;
}

// 获取传输状态
bool multimedia_transfer_manager_get_transfer_status(MultimediaTransferManager* manager, MultimediaTransferCallback callback, void* user_data) {
    if (!manager) {
        return false;
    }
    
    // 准备回调数据
    void** callback_data = (void**)malloc(sizeof(void*) * 3);
    if (!callback_data) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    
    callback_data[0] = (void*)callback;
    callback_data[1] = user_data;
    callback_data[2] = (void*)manager;
    
    // 模拟获取传输状态
    multimedia_transfer_response_callback(callback_data, 200, NULL, 0);
    
    return true;
}

// 清理传输列表
bool multimedia_transfer_manager_clear_media(MultimediaTransferManager* manager) {
    if (!manager || manager->is_transferring) {
        return false;
    }
    
    // 释放媒体文件列表
    if (manager->media_files) {
        for (size_t i = 0; i < manager->media_count; i++) {
            if (manager->media_files[i].media_id) free(manager->media_files[i].media_id);
            if (manager->media_files[i].file_path) free(manager->media_files[i].file_path);
            if (manager->media_files[i].file_name) free(manager->media_files[i].file_name);
            if (manager->media_files[i].create_time) free(manager->media_files[i].create_time);
            if (manager->media_files[i].transfer_time) free(manager->media_files[i].transfer_time);
            if (manager->media_files[i].error_message) free(manager->media_files[i].error_message);
        }
        free(manager->media_files);
        manager->media_files = NULL;
    }
    
    manager->media_count = 0;
    
    return true;
}

// 设置传输进度回调
void multimedia_transfer_manager_set_progress_callback(MultimediaTransferManager* manager, MultimediaProgressCallback callback, void* user_data) {
    // 实际应用中应该保存回调函数和用户数据
    (void)manager;
    (void)callback;
    (void)user_data;
}

// 设置自动保存
void multimedia_transfer_manager_set_auto_save(MultimediaTransferManager* manager, bool auto_save) {
    if (manager) {
        manager->auto_save = auto_save;
    }
}

// 获取媒体文件数量
size_t multimedia_transfer_manager_get_media_count(MultimediaTransferManager* manager) {
    if (!manager) {
        return 0;
    }
    return manager->media_count;
}

// 获取媒体文件
MediaFile* multimedia_transfer_manager_get_media(MultimediaTransferManager* manager, size_t index) {
    if (!manager || index >= manager->media_count) {
        return NULL;
    }
    return &manager->media_files[index];
}
