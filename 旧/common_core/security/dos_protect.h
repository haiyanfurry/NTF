#ifndef DOS_PROTECT_H
#define DOS_PROTECT_H

#include <stddef.h>

// 检查循环迭代次数
int check_loop_iterations(int iterations);

// 检查文件大小
int check_file_size(size_t file_size);

// 检查字符串长度
int check_string_length(const char* str);

// 检查数组越界
int check_array_bounds(size_t index, size_t array_size);

// 检查空指针
int check_null_pointer(const void* ptr);

// 检查内存分配
void* safe_malloc(size_t size);

// 检查内存释放
void safe_free(void** ptr);

// 检查系统资源使用
int check_system_resources(void);

// 综合系统保护检查
int system_protect_check(int iterations, size_t file_size, const char* str, size_t index, size_t array_size, const void* ptr);

// 检测黑客渗透系统
int detect_hacker_system(void);

// 强制拦截黑客渗透行为
int block_hacker_activities(void);

#endif // DOS_PROTECT_H