#include "../ui/MainWindow.h"
#include "../network/MediaTransfer.h"
#include "../models/Database.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

// 声明外部函数
void show_login_dialog(void);

int main(int argc, char *argv[]) {
    // 初始化随机数
    srand(time(NULL));
    
    // 初始化 GTK
    gtk_init(&argc, &argv);
    
    // 初始化数据库
    if (!db_init()) {
        g_print("数据库初始化失败\n");
        return 1;
    }
    
    // 初始化媒体传输模块（占位）
    media_transfer_init();
    
    // 创建主窗口
    GtkWidget *window = create_main_window();
    
    // 显示窗口
    gtk_widget_show_all(window);
    
    // 显示登录对话框
    show_login_dialog();
    
    // 进入主循环
    gtk_main();
    
    // 关闭数据库
    db_close();
    
    return 0;
}
