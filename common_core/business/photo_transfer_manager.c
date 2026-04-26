// 必须在所有头文件之前定义_GNU_SOURCE以使用strdup
#define _GNU_SOURCE

#include "photo_transfer_manager.h"
#include "../network/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

// 默认最大照片数
#define DEFAULT_MAX_PHOTOS 250

// 照片传输线程参数
typedef struct {
    PhotoTransferManager* manager;
    PhotoTransferCallback callback;
    void* user_data;
} TransferThreadArgs;

// 照片传输响应回调函数
static void photo_transfer_response_callback(void* user_data, int status, const uint8_t* data, size_t length) {
    if (!user_data) {
        return;
    }
    
    // 解析回调数据
    PhotoTransferCallback callback = (PhotoTransferCallback)((void**)user_data)[0];
    void* callback_user_data = ((void**)user_data)[1];
    PhotoTransferManager* manager = (PhotoTransferManager*)((void**)user_data)[2];
    
    // 模拟解析响应数据
    // 实际应用中应该解析 JSON 数据
    if (status == 200) {
        // 模拟传输成功
        for (size_t i = 0; i < manager->photo_count; i++) {
            if (manager->photos[i].status == PHOTO_STATUS_TRANSFERRING) {
                manager->photos[i].status = PHOTO_STATUS_COMPLETED;
                manager->photos[i].progress = 100.0;
                
                // 生成传输时间
                time_t now = time(NULL);
                struct tm* tm_info = localtime(&now);
                char time_str[20];
                strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
                if (manager->photos[i].transfer_time) {
                    free(manager->photos[i].transfer_time);
                    manager->photos[i].transfer_time = NULL;
                }
                manager->photos[i].transfer_time = strdup(time_str);
                if (!manager->photos[i].transfer_time) {
                    fprintf(stderr, "Error allocating memory for transfer time\n");
                }
            }
        }
    } else {
        // 模拟传输失败
        for (size_t i = 0; i < manager->photo_count; i++) {
            if (manager->photos[i].status == PHOTO_STATUS_TRANSFERRING) {
                manager->photos[i].status = PHOTO_STATUS_FAILED;
                if (manager->photos[i].error_message) {
                    free(manager->photos[i].error_message);
                    manager->photos[i].error_message = NULL;
                }
                manager->photos[i].error_message = strdup("Transfer failed");
                if (!manager->photos[i].error_message) {
                    fprintf(stderr, "Error allocating memory for error message\n");
                }
            }
        }
    }
    
    // 调用回调函数
    if (callback) {
        callback(callback_user_data, status, manager->photos);
    }
    
    // 清理资源
    free(user_data);
}

// 照片传输线程函数
static void* photo_transfer_thread(void* arg) {
    TransferThreadArgs* args = (TransferThreadArgs*)arg;
    PhotoTransferManager* manager = args->manager;
    PhotoTransferCallback callback = args->callback;
    void* user_data = args->user_data;
    
    // 模拟照片传输过程
    for (size_t i = 0; i < manager->photo_count; i++) {
        if (!manager->is_transferring) {
            break;
        }
        
        // 更新照片状态为传输中
        manager->photos[i].status = PHOTO_STATUS_TRANSFERRING;
        
        // 模拟传输进度
        for (int j = 0; j <= 100; j += 10) {
            if (!manager->is_transferring) {
                break;
            }
            manager->photos[i].progress = j;
            
            // 模拟网络延迟
            usleep(100000); // 100ms
        }
        
        // 模拟传输完成
        if (manager->is_transferring) {
            manager->photos[i].status = PHOTO_STATUS_COMPLETED;
            manager->photos[i].progress = 100.0;
            
            // 生成传输时间
        time_t now = time(NULL);
        struct tm* tm_info = localtime(&now);
        char time_str[20];
        strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
        if (manager->photos[i].transfer_time) {
            free(manager->photos[i].transfer_time);
            manager->photos[i].transfer_time = NULL;
        }
        manager->photos[i].transfer_time = strdup(time_str);
        if (!manager->photos[i].transfer_time) {
            fprintf(stderr, "Error allocating memory for transfer time\n");
        }
        }
    }
    
    // 传输完成
    manager->is_transferring = false;
    
    // 调用回调函数
    if (callback) {
        callback(user_data, 200, manager->photos);
    }
    
    free(args);
    return NULL;
}

// 初始化照片传输管理器
PhotoTransferManager* photo_transfer_manager_init() {
    PhotoTransferManager* manager = (PhotoTransferManager*)malloc(sizeof(PhotoTransferManager));
    if (!manager) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }
    
    manager->network_manager = NULL;
    manager->photos = NULL;
    manager->photo_count = 0;
    manager->max_photos = DEFAULT_MAX_PHOTOS;
    manager->is_transferring = false;
    manager->auto_save = true;
    
    return manager;
}

// 销毁照片传输管理器
void photo_transfer_manager_destroy(PhotoTransferManager* manager) {
    if (manager) {
        // 停止传输
        photo_transfer_manager_stop_transfer(manager);
        
        // 释放照片列表
        if (manager->photos) {
            for (size_t i = 0; i < manager->photo_count; i++) {
                if (manager->photos[i].photo_id) free(manager->photos[i].photo_id);
                if (manager->photos[i].file_path) free(manager->photos[i].file_path);
                if (manager->photos[i].file_name) free(manager->photos[i].file_name);
                if (manager->photos[i].create_time) free(manager->photos[i].create_time);
                if (manager->photos[i].transfer_time) free(manager->photos[i].transfer_time);
                if (manager->photos[i].error_message) free(manager->photos[i].error_message);
            }
            free(manager->photos);
        }
        free(manager);
    }
}

// 添加照片到传输列表
bool photo_transfer_manager_add_photo(PhotoTransferManager* manager, const char* file_path, const char* file_name) {
    if (!manager || !file_path || !file_name) {
        return false;
    }
    
    // 检查是否达到最大照片数
    if (manager->photo_count >= manager->max_photos) {
        fprintf(stderr, "Maximum number of photos reached (%zu)\n", manager->max_photos);
        return false;
    }
    
    // 检查是否正在传输
    if (manager->is_transferring) {
        fprintf(stderr, "Transfer is in progress\n");
        return false;
    }
    
    // 重新分配照片列表
    Photo* new_photos = (Photo*)realloc(manager->photos, sizeof(Photo) * (manager->photo_count + 1));
    if (!new_photos) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    manager->photos = new_photos;
    
    // 初始化新照片
    Photo* photo = &manager->photos[manager->photo_count];
    photo->photo_id = (char*)malloc(32);
    if (!photo->photo_id) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    snprintf(photo->photo_id, 32, "photo_%d", (int)time(NULL));
    
    photo->file_path = strdup(file_path);
    photo->file_name = strdup(file_name);
    photo->file_size = 0; // 实际应用中应该获取文件大小
    photo->status = PHOTO_STATUS_PENDING;
    photo->progress = 0.0;
    
    // 生成创建时间
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_str[20];
    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    photo->create_time = strdup(time_str);
    
    photo->transfer_time = NULL;
    photo->error_message = NULL;
    
    manager->photo_count++;
    
    return true;
}

// 批量添加照片到传输列表
bool photo_transfer_manager_add_photos(PhotoTransferManager* manager, const char** file_paths, const char** file_names, size_t count) {
    if (!manager || !file_paths || !file_names || count == 0) {
        return false;
    }
    
    // 检查是否达到最大照片数
    if (manager->photo_count + count > manager->max_photos) {
        fprintf(stderr, "Maximum number of photos reached (%zu)\n", manager->max_photos);
        return false;
    }
    
    // 检查是否正在传输
    if (manager->is_transferring) {
        fprintf(stderr, "Transfer is in progress\n");
        return false;
    }
    
    // 重新分配照片列表
    Photo* new_photos = (Photo*)realloc(manager->photos, sizeof(Photo) * (manager->photo_count + count));
    if (!new_photos) {
        fprintf(stderr, "Error allocating memory\n");
        return false;
    }
    manager->photos = new_photos;
    
    // 生成创建时间
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_str[20];
    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", tm_info);
    
    // 添加照片
    for (size_t i = 0; i < count; i++) {
        Photo* photo = &manager->photos[manager->photo_count + i];
        photo->photo_id = (char*)malloc(32);
        if (!photo->photo_id) {
            fprintf(stderr, "Error allocating memory\n");
            return false;
        }
        snprintf(photo->photo_id, 32, "photo_%d_%zu", (int)time(NULL), i);
        
        photo->file_path = strdup(file_paths[i]);
        photo->file_name = strdup(file_names[i]);
        photo->file_size = 0; // 实际应用中应该获取文件大小
        photo->status = PHOTO_STATUS_PENDING;
        photo->progress = 0.0;
        photo->create_time = strdup(time_str);
        photo->transfer_time = NULL;
        photo->error_message = NULL;
    }
    
    manager->photo_count += count;
    
    return true;
}

// 开始传输照片
bool photo_transfer_manager_start_transfer(PhotoTransferManager* manager, PhotoTransferCallback callback, void* user_data) {
    if (!manager || manager->is_transferring || manager->photo_count == 0) {
        return false;
    }
    
    manager->is_transferring = true;
    
    // 创建传输线程
    TransferThreadArgs* args = (TransferThreadArgs*)malloc(sizeof(TransferThreadArgs));
    if (!args) {
        fprintf(stderr, "Error allocating memory\n");
        manager->is_transferring = false;
        return false;
    }
    args->manager = manager;
    args->callback = callback;
    args->user_data = user_data;
    
    pthread_t thread;
    if (pthread_create(&thread, NULL, photo_transfer_thread, args) != 0) {
        fprintf(stderr, "Error creating transfer thread\n");
        free(args);
        manager->is_transferring = false;
        return false;
    }
    
    // 分离线程，让它自行清理
    pthread_detach(thread);
    
    return true;
}

// 停止传输照片
bool photo_transfer_manager_stop_transfer(PhotoTransferManager* manager) {
    if (!manager || !manager->is_transferring) {
        return false;
    }
    
    manager->is_transferring = false;
    
    // 更新未完成的照片状态
        for (size_t i = 0; i < manager->photo_count; i++) {
            if (manager->photos[i].status == PHOTO_STATUS_TRANSFERRING) {
                manager->photos[i].status = PHOTO_STATUS_FAILED;
                if (manager->photos[i].error_message) {
                    free(manager->photos[i].error_message);
                    manager->photos[i].error_message = NULL;
                }
                manager->photos[i].error_message = strdup("Transfer stopped");
                if (!manager->photos[i].error_message) {
                    fprintf(stderr, "Error allocating memory for error message\n");
                }
            }
        }
    
    return true;
}

// 取消传输照片
bool photo_transfer_manager_cancel_transfer(PhotoTransferManager* manager, const char* photo_id) {
    if (!manager || !photo_id) {
        return false;
    }
    
    // 查找照片
    for (size_t i = 0; i < manager->photo_count; i++) {
        if (strcmp(manager->photos[i].photo_id, photo_id) == 0) {
            // 更新照片状态
            manager->photos[i].status = PHOTO_STATUS_FAILED;
            if (manager->photos[i].error_message) {
                free(manager->photos[i].error_message);
                manager->photos[i].error_message = NULL;
            }
            manager->photos[i].error_message = strdup("Transfer cancelled");
            if (!manager->photos[i].error_message) {
                fprintf(stderr, "Error allocating memory for error message\n");
            }
            return true;
        }
    }
    
    return false;
}

// 获取传输状态
bool photo_transfer_manager_get_transfer_status(PhotoTransferManager* manager, PhotoTransferCallback callback, void* user_data) {
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
    photo_transfer_response_callback(callback_data, 200, NULL, 0);
    
    return true;
}

// 清理传输列表
bool photo_transfer_manager_clear_photos(PhotoTransferManager* manager) {
    if (!manager || manager->is_transferring) {
        return false;
    }
    
    // 释放照片列表
    if (manager->photos) {
        for (size_t i = 0; i < manager->photo_count; i++) {
            if (manager->photos[i].photo_id) free(manager->photos[i].photo_id);
            if (manager->photos[i].file_path) free(manager->photos[i].file_path);
            if (manager->photos[i].file_name) free(manager->photos[i].file_name);
            if (manager->photos[i].create_time) free(manager->photos[i].create_time);
            if (manager->photos[i].transfer_time) free(manager->photos[i].transfer_time);
            if (manager->photos[i].error_message) free(manager->photos[i].error_message);
        }
        free(manager->photos);
        manager->photos = NULL;
    }
    
    manager->photo_count = 0;
    
    return true;
}

// 设置传输进度回调
void photo_transfer_manager_set_progress_callback(PhotoTransferManager* manager, PhotoProgressCallback callback, void* user_data) {
    // 实际应用中应该保存回调函数和用户数据
    (void)manager;
    (void)callback;
    (void)user_data;
}

// 设置自动保存到相册
void photo_transfer_manager_set_auto_save(PhotoTransferManager* manager, bool auto_save) {
    if (manager) {
        manager->auto_save = auto_save;
    }
}

// 获取最大照片数
size_t photo_transfer_manager_get_max_photos(PhotoTransferManager* manager) {
    if (!manager) {
        return 0;
    }
    return manager->max_photos;
}

// 设置最大照片数
bool photo_transfer_manager_set_max_photos(PhotoTransferManager* manager, size_t max_photos) {
    if (!manager || max_photos == 0) {
        return false;
    }
    
    // 检查是否正在传输
    if (manager->is_transferring) {
        fprintf(stderr, "Transfer is in progress\n");
        return false;
    }
    
    // 检查是否超过当前照片数
    if (max_photos < manager->photo_count) {
        fprintf(stderr, "Cannot set max photos less than current photo count\n");
        return false;
    }
    
    manager->max_photos = max_photos;
    
    return true;
}
