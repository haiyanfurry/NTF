#ifndef SAFE_MEMORY_H
#define SAFE_MEMORY_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 安全内存分配宏 - 自动检查返回值并清零内存
 */
#define SAFE_MALLOC(size) safe_malloc(size, __FILE__, __LINE__)
#define SAFE_CALLOC(nmemb, size) safe_calloc(nmemb, size, __FILE__, __LINE__)
#define SAFE_REALLOC(ptr, size) safe_realloc(ptr, size, __FILE__, __LINE__)
#define SAFE_FREE(ptr) safe_free((void**)&(ptr))
#define SAFE_STRDUP(s) safe_strdup(s, __FILE__, __LINE__)

/**
 * @brief 安全内存分配函数
 * @param size 分配大小
 * @param file 源文件
 * @param line 行号
 * @return 分配成功的内存指针，失败返回NULL
 */
void* safe_malloc(size_t size, const char* file, int line);

/**
 * @brief 安全内存分配并清零函数
 * @param nmemb 元素个数
 * @param size 元素大小
 * @param file 源文件
 * @param line 行号
 * @return 分配成功的内存指针，失败返回NULL
 */
void* safe_calloc(size_t nmemb, size_t size, const char* file, int line);

/**
 * @brief 安全内存重新分配函数
 * @param ptr 原内存指针
 * @param size 新大小
 * @param file 源文件
 * @param line 行号
 * @return 分配成功的内存指针，失败返回NULL
 */
void* safe_realloc(void* ptr, size_t size, const char* file, int line);

/**
 * @brief 安全释放内存函数 - 自动置NULL
 * @param ptr 内存指针的指针
 */
void safe_free(void** ptr);

/**
 * @brief 安全字符串复制函数
 * @param s 源字符串
 * @param file 源文件
 * @param line 行号
 * @return 复制后的字符串指针，失败返回NULL
 */
char* safe_strdup(const char* s, const char* file, int line);

/**
 * @brief 安全内存清零函数
 * @param ptr 内存指针
 * @param size 大小
 */
void safe_memzero(void* ptr, size_t size);

/**
 * @brief 检查指针是否有效
 * @param ptr 指针
 * @return 是否有效
 */
bool safe_ptr_valid(const void* ptr);

/**
 * @brief 安全字符串拷贝
 * @param dest 目标缓冲区
 * @param src 源字符串
 * @param dest_size 目标缓冲区大小
 * @return 是否成功
 */
bool safe_strcpy(char* dest, const char* src, size_t dest_size);

/**
 * @brief 安全字符串连接
 * @param dest 目标缓冲区
 * @param src 源字符串
 * @param dest_size 目标缓冲区大小
 * @return 是否成功
 */
bool safe_strcat(char* dest, const char* src, size_t dest_size);

/**
 * @brief 内存泄漏检测初始化
 */
void safe_memory_init(void);

/**
 * @brief 内存泄漏检测清理
 */
void safe_memory_cleanup(void);

/**
 * @brief 获取未释放的内存块数量
 * @return 未释放的内存块数量
 */
int safe_memory_get_unfreed_count(void);

#ifdef __cplusplus
}
#endif

#endif /* SAFE_MEMORY_H */
