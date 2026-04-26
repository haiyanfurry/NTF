#ifndef BOOT_CHECK_H
#define BOOT_CHECK_H

// 检查文件是否为恶意程序
int check_malicious_file(const char* filename);

// 检查核心文件是否被篡改
int check_core_files(const char* directory);

// 扫描目录
int scan_directory(const char* directory);

// 启动前检查
int boot_check(void);

#endif // BOOT_CHECK_H