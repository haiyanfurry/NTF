#ifndef SECURITY_H
#define SECURITY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 唯一全局安全初始化函数
void security_init(void);

// 安全模块状态检查
bool security_check_status(void);

// 安全模块清理函数
void security_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif // SECURITY_H