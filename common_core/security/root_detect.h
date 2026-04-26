#ifndef ROOT_DETECT_H
#define ROOT_DETECT_H

// 检查是否以 root 权限运行
int check_root_privileges(void);

// 处理 root 权限检查
int handle_root_check(void);

#endif // ROOT_DETECT_H