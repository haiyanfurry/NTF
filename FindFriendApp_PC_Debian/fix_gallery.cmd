#!/bin/bash
cd ~/汇总/FindFriendApp_PC_Debian
# 备份原文件
cp src/ui/MainWindow.c src/ui/MainWindow.c.bak
# 使用 sed 修复所有不完整的 </spa 标签
sed -i 's/<\/spa/<\/span/g' src/ui/MainWindow.c
