#include "../UI/MainWindow.h"
#include "../Network/GoodSamaritan.h"
#include <gtk/gtk.h>
#include <pthread.h>
#include <unistd.h>

static void* start_server_thread(void *arg) {
    start_good_samaritan(8888);
    return NULL;
}

int main(int argc, char *argv[]) {
    // 初始化 GTK
    gtk_init(&argc, &argv);
    
    // 启动老好人服务器（后台线程）
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, start_server_thread, NULL);
    
    // 创建主窗口
    GtkWidget *window = create_main_window();
    
    // 显示窗口
    gtk_widget_show_all(window);
    
    // 进入主循环
    gtk_main();
    
    // 清理
    stop_good_samaritan();
    
    return 0;
}
