#include "MainWindow.h"
#include "../models/User.h"
#include "../models/Database.h"
#include "../models/Message.h"
#include "../models/Exhibition.h"
#include "../network/MediaTransfer.h"
#include "../network/GoodSamaritan.h"
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static GtkWidget *main_window;
static GtkWidget *notebook;
static User current_user;
static int is_logged_in = 0;

// 回调函数声明
static void on_set_ip_clicked(GtkWidget *widget, gpointer data);
static void on_add_server_clicked(GtkWidget *widget, gpointer data);
static void on_start_discovery_clicked(GtkWidget *widget, gpointer data);
static void on_stop_discovery_clicked(GtkWidget *widget, gpointer data);

// 登录对话框
void show_login_dialog(void) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "登录", GTK_WINDOW(main_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "取消", GTK_RESPONSE_CANCEL,
        "登录", GTK_RESPONSE_OK,
        "注册", GTK_RESPONSE_APPLY,
        NULL
    );
    gtk_window_set_default_size(GTK_WINDOW(dialog), 350, 220);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content), 20);
    
    // 用户名
    GtkWidget *user_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *user_label = gtk_label_new("用户名:");
    GtkWidget *user_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(user_entry), "输入用户名");
    gtk_box_pack_start(GTK_BOX(user_box), user_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(user_box), user_entry, TRUE, TRUE, 0);
    
    // 密码
    GtkWidget *pass_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *pass_label = gtk_label_new("密码:");
    GtkWidget *pass_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(pass_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(pass_entry), "输入密码");
    gtk_box_pack_start(GTK_BOX(pass_box), pass_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(pass_box), pass_entry, TRUE, TRUE, 0);
    
    gtk_box_pack_start(GTK_BOX(content), user_box, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(content), pass_box, FALSE, FALSE, 10);
    
    gtk_widget_show_all(content);
    
    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_OK || result == GTK_RESPONSE_APPLY) {
        const char *username = gtk_entry_get_text(GTK_ENTRY(user_entry));
        const char *password = gtk_entry_get_text(GTK_ENTRY(pass_entry));
        
        if (strlen(username) == 0 || strlen(password) == 0) {
            GtkWidget *error_dialog = gtk_message_dialog_new(
                GTK_WINDOW(dialog),
                GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "用户名和密码不能为空"
            );
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        } else {
            if (result == GTK_RESPONSE_APPLY) {
                // 注册新用户
                // 检查用户名是否存在
                if (db_username_exists(username)) {
                    // 生成带编号的用户名
                    char new_username[64];
                    int i = 1;
                    do {
                        sprintf(new_username, "%s_%d", username, i);
                        i++;
                    } while (db_username_exists(new_username) && i < 1000);
                    
                    GtkWidget *info_dialog = gtk_message_dialog_new(
                        GTK_WINDOW(dialog),
                        GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "用户名已存在，将使用 %s", new_username
                    );
                    gtk_dialog_run(GTK_DIALOG(info_dialog));
                    gtk_widget_destroy(info_dialog);
                    
                    DBResult db_result = db_register_user(new_username, password, IDENTITY_NORMAL);
                    if (db_result == DB_SUCCESS) {
                        GtkWidget *success_dialog = gtk_message_dialog_new(
                            GTK_WINDOW(dialog),
                            GTK_DIALOG_DESTROY_WITH_PARENT,
                            GTK_MESSAGE_INFO,
                            GTK_BUTTONS_OK,
                            "注册成功，请使用新用户名登录"
                        );
                        gtk_dialog_run(GTK_DIALOG(success_dialog));
                        gtk_widget_destroy(success_dialog);
                    } else {
                        GtkWidget *error_dialog = gtk_message_dialog_new(
                            GTK_WINDOW(dialog),
                            GTK_DIALOG_DESTROY_WITH_PARENT,
                            GTK_MESSAGE_ERROR,
                            GTK_BUTTONS_OK,
                            "注册失败"
                        );
                        gtk_dialog_run(GTK_DIALOG(error_dialog));
                        gtk_widget_destroy(error_dialog);
                    }
                } else {
                    // 用户名可用，直接注册
                    DBResult db_result = db_register_user(username, password, IDENTITY_NORMAL);
                    if (db_result == DB_SUCCESS) {
                        GtkWidget *success_dialog = gtk_message_dialog_new(
                            GTK_WINDOW(dialog),
                            GTK_DIALOG_DESTROY_WITH_PARENT,
                            GTK_MESSAGE_INFO,
                            GTK_BUTTONS_OK,
                            "注册成功，请登录"
                        );
                        gtk_dialog_run(GTK_DIALOG(success_dialog));
                        gtk_widget_destroy(success_dialog);
                    } else {
                        GtkWidget *error_dialog = gtk_message_dialog_new(
                            GTK_WINDOW(dialog),
                            GTK_DIALOG_DESTROY_WITH_PARENT,
                            GTK_MESSAGE_ERROR,
                            GTK_BUTTONS_OK,
                            "注册失败"
                        );
                        gtk_dialog_run(GTK_DIALOG(error_dialog));
                        gtk_widget_destroy(error_dialog);
                    }
                }
            } else if (result == GTK_RESPONSE_OK) {
                // 登录
                DBResult db_result = db_login_user(username, password, &current_user);
                if (db_result == DB_SUCCESS) {
                    is_logged_in = 1;
                    if (notebook) {
                        gtk_widget_set_sensitive(notebook, TRUE);
                    }
                    GtkWidget *success_dialog = gtk_message_dialog_new(
                        GTK_WINDOW(dialog),
                        GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "登录成功"
                    );
                    gtk_dialog_run(GTK_DIALOG(success_dialog));
                    gtk_widget_destroy(success_dialog);
                } else {
                    const char *error_msg;
                    if (db_result == DB_USER_NOT_FOUND) {
                        error_msg = "用户名或密码错误";
                    } else {
                        error_msg = "登录失败";
                    }
                    GtkWidget *error_dialog = gtk_message_dialog_new(
                        GTK_WINDOW(dialog),
                        GTK_DIALOG_DESTROY_WITH_PARENT,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_OK,
                        error_msg
                    );
                    gtk_dialog_run(GTK_DIALOG(error_dialog));
                    gtk_widget_destroy(error_dialog);
                }
            }
        }
    }
    gtk_widget_destroy(dialog);
}

// 展会面板
static GtkWidget* create_exhibition_panel(void) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), 
        "<span size='x-large' weight='bold'>🎪 展会与活动</spann>");
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    GtkWidget *list_box = gtk_list_box_new();
    
    const char *exhibitions[][5] = {
        {"COMICUP 30", "上海新国际博览中心", "2024-12-15", "🎫 票务紧张", "官方"},
        {"Furry 冬日祭", "北京国家会议中心", "2024-12-22", "🎫 热卖中", "官方"},
        {"Bilibili World", "广州保利世贸", "2025-01-05", "🎫 预售", "官方"},
        {"同人创作展", "杭州国际博览中心", "2025-01-12", "📝 征稿中", "合作"}
    };
    
    for (int i = 0; i < 4; i++) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
        gtk_widget_set_margin_start(hbox, 15);
        gtk_widget_set_margin_end(hbox, 15);
        gtk_widget_set_margin_top(hbox, 15);
        gtk_widget_set_margin_bottom(hbox, 15);
        
        GtkWidget *icon = gtk_label_new("🏢");
        gtk_widget_set_size_request(icon, 50, 50);
        
        GtkWidget *info = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        GtkWidget *name = gtk_label_new(NULL);
        char name_markup[256];
        snprintf(name_markup, sizeof(name_markup), 
            "<b><span size='large'>%s</spann></b>\n<span size='small'>%s</spann>",
            exhibitions[i][0], exhibitions[i][1]);
        gtk_label_set_markup(GTK_LABEL(name), name_markup);
        gtk_widget_set_halign(name, GTK_ALIGN_START);
        
        GtkWidget *details = gtk_label_new(NULL);
        char details_markup[256];
        snprintf(details_markup, sizeof(details_markup), 
            "📅 %s\n%s", exhibitions[i][2], exhibitions[i][3]);
        gtk_label_set_markup(GTK_LABEL(details), details_markup);
        gtk_widget_set_halign(details, GTK_ALIGN_START);
        
        GtkWidget *buy_btn = gtk_button_new_with_label("查看详情");
        
        gtk_box_pack_start(GTK_BOX(info), name, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(info), details, FALSE, FALSE, 0);
        
        gtk_box_pack_start(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), info, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), buy_btn, FALSE, FALSE, 0);
        
        gtk_container_add(GTK_CONTAINER(row), hbox);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
    }
    
    gtk_container_add(GTK_CONTAINER(scrolled), list_box);
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    return vbox;
}

// 票务面板
static GtkWidget* create_ticket_panel(void) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), 
        "<span size='x-large' weight='bold'>🎫 我的票务</spann>");
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    
    GtkWidget *list_box = gtk_list_box_new();
    
    GtkWidget *row = gtk_list_box_row_new();
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    gtk_widget_set_margin_start(hbox, 15);
    gtk_widget_set_margin_end(hbox, 15);
    gtk_widget_set_margin_top(hbox, 15);
    gtk_widget_set_margin_bottom(hbox, 15);
    
    GtkWidget *icon = gtk_label_new("🎟️");
    GtkWidget *info = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *name = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(name), 
        "<b>COMICUP 30 普通票</b>\n<span size='small'>2024-12-15 上海新国际博览中心</spann>");
    gtk_widget_set_halign(name, GTK_ALIGN_START);
    
    GtkWidget *status = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(status), 
        "<span foreground='#4caf50'>✓ 已支付 | 待使用</spann>");
    
    GtkWidget *code_btn = gtk_button_new_with_label("生成入场码");
    
    gtk_box_pack_start(GTK_BOX(info), name, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(info), status, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), info, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), code_btn, FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(row), hbox);
    gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
    
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), list_box, TRUE, TRUE, 0);
    
    return vbox;
}

// 交易面板
static GtkWidget* create_transaction_panel(void) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    
    GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), 
        "<span size='x-large' weight='bold'>💰 交易市场</spann>");
    GtkWidget *post_btn = gtk_button_new_with_label("+ 发布稿件");
    gtk_box_pack_start(GTK_BOX(title_box), title, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(title_box), post_btn, FALSE, FALSE, 0);
    
    GtkWidget *tabs = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *btn_all = gtk_button_new_with_label("全部");
    GtkWidget *btn_art = gtk_button_new_with_label("稿件");
    GtkWidget *btn_ticket = gtk_button_new_with_label("门票");
    GtkWidget *btn_goods = gtk_button_new_with_label("周边");
    
    gtk_box_pack_start(GTK_BOX(tabs), btn_all, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tabs), btn_art, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tabs), btn_ticket, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tabs), btn_goods, FALSE, FALSE, 0);
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *list_box = gtk_list_box_new();
    
    const char *transactions[][5] = {
        {"🎨", "福瑞插画约稿", "狼仔", "¥200", "待审核"},
        {"🎫", "COMICUP 30 门票", "狐酱", "¥80", "已上架"},
        {"🧸", "手工毛绒公仔", "喵喵", "¥150", "已售出"},
        {"📖", "原创漫画本", "龙龙", "¥45", "已上架"}
    };
    
    for (int i = 0; i < 4; i++) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
        gtk_widget_set_margin_start(hbox, 12);
        gtk_widget_set_margin_end(hbox, 12);
        gtk_widget_set_margin_top(hbox, 12);
        gtk_widget_set_margin_bottom(hbox, 12);
        
        GtkWidget *icon = gtk_label_new(transactions[i][0]);
        GtkWidget *info = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
        GtkWidget *name = gtk_label_new(NULL);
        char name_markup[256];
        snprintf(name_markup, sizeof(name_markup), 
            "<b>%s</b>\n<span size='small'>卖家: %s</spann>",
            transactions[i][1], transactions[i][2]);
        gtk_label_set_markup(GTK_LABEL(name), name_markup);
        gtk_widget_set_halign(name, GTK_ALIGN_START);
        
        GtkWidget *price = gtk_label_new(NULL);
        char price_markup[64];
        snprintf(price_markup, sizeof(price_markup), 
            "<span foreground='#ff9800'>%s</spann>", transactions[i][3]);
        gtk_label_set_markup(GTK_LABEL(price), price_markup);
        
        GtkWidget *status_btn = gtk_button_new_with_label(transactions[i][4]);
        
        gtk_box_pack_start(GTK_BOX(info), name, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(info), price, FALSE, FALSE, 0);
        
        gtk_box_pack_start(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), info, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), status_btn, FALSE, FALSE, 0);
        
        gtk_container_add(GTK_CONTAINER(row), hbox);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
    }
    
    gtk_container_add(GTK_CONTAINER(scrolled), list_box);
    
    gtk_box_pack_start(GTK_BOX(vbox), title_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), tabs, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    return vbox;
}

// 相册视图
static GtkWidget* create_gallery_view(void) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *select_btn = gtk_button_new_with_label("📷 选择照片");
    GtkWidget *send_btn = gtk_button_new_with_label("📤 发送");
    GtkWidget *album_btn = gtk_button_new_with_label("📁 相册");
    
    gtk_box_pack_start(GTK_BOX(toolbar), select_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), send_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(toolbar), album_btn, FALSE, FALSE, 0);
    
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), grid);
    
    const char *photos[] = {"📸", "🎨", "🐺", "🦊", "🐱", "🐉"};
    for (int i = 0; i < 6; i++) {
        GtkWidget *photo_box = gtk_event_box_new();
        gtk_widget_set_size_request(photo_box, 100, 100);
        GtkWidget *label = gtk_label_new(NULL);
        char label_markup[32];
        snprintf(label_markup, sizeof(label_markup), 
            "<span size=.xx-large.>%s</spannn>", photos[i]);
        gtk_label_set_markup(GTK_LABEL(label), label_markup);
        gtk_container_add(GTK_CONTAINER(photo_box), label);
        gtk_grid_attach(GTK_GRID(grid), photo_box, i % 3, i / 3, 1, 1);
    }
    
    GtkWidget *progress = gtk_progress_bar_new();
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), "传输进度");
    
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), progress, FALSE, FALSE, 10);
    
    return vbox;
}

// 消息视图
static GtkWidget* create_message_view(void) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_start(toolbar, 10);
    gtk_widget_set_margin_end(toolbar, 10);
    gtk_widget_set_margin_top(toolbar, 10);
    
    GtkWidget *settings_btn = gtk_button_new_with_label("⚙️ 消息设置");
    gtk_box_pack_end(GTK_BOX(toolbar), settings_btn, FALSE, FALSE, 0);
    
    GtkWidget *list_box = gtk_list_box_new();
    
    const char *msgs[][4] = {
        {"⭐", "官方公告", "COMICUP 30 即将开始，请准备好入场码", "10:30"},
        {"🐺", "狼仔", "明天一起去漫展吗？", "09:15"},
        {"🏢", "展会助手", "您的门票已出票，点击查看详情", "昨天"},
        {"🎨", "画师圈", "群内发布了新的约稿信息", "昨天"}
    };
    
    for (int i = 0; i < 4; i++) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
        gtk_widget_set_margin_start(hbox, 12);
        gtk_widget_set_margin_end(hbox, 12);
        gtk_widget_set_margin_top(hbox, 12);
        gtk_widget_set_margin_bottom(hbox, 12);
        
        GtkWidget *icon = gtk_label_new(msgs[i][0]);
        gtk_widget_set_size_request(icon, 40, 40);
        
        GtkWidget *info = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
        GtkWidget *name = gtk_label_new(NULL);
        char name_markup[256];
        snprintf(name_markup, sizeof(name_markup), 
            "<b>%s</b>  <span size='small'>%s</spann>",
            msgs[i][1], msgs[i][3]);
        gtk_label_set_markup(GTK_LABEL(name), name_markup);
        gtk_widget_set_halign(name, GTK_ALIGN_START);
        
        GtkWidget *content = gtk_label_new(msgs[i][2]);
        gtk_widget_set_halign(content, GTK_ALIGN_START);
        gtk_label_set_ellipsize(GTK_LABEL(content), PANGO_ELLIPSIZE_END);
        gtk_widget_set_size_request(content, 300, -1);
        
        gtk_box_pack_start(GTK_BOX(info), name, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(info), content, FALSE, FALSE, 0);
        
        gtk_box_pack_start(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), info, TRUE, TRUE, 0);
        
        gtk_container_add(GTK_CONTAINER(row), hbox);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
    }
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), list_box);
    
    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    
    return vbox;
}

// 创建主窗口
// 设置面板
static GtkWidget* create_settings_panel(void) {
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    
    // 标题
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), 
        "<span size='x-large' weight='bold'>⚙️ 系统设置</span>");
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    
    // IP设置区域
    GtkWidget *ip_frame = gtk_frame_new("服务器IP设置");
    gtk_frame_set_shadow_type(GTK_FRAME(ip_frame), GTK_SHADOW_ETCHED_IN);
    
    GtkWidget *ip_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(ip_vbox), 10);
    
    // 当前IP显示
    GtkWidget *current_ip_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *current_ip_label = gtk_label_new("当前服务器IP:");
    GtkWidget *current_ip_value = gtk_label_new(get_current_server_ip());
    gtk_box_pack_start(GTK_BOX(current_ip_hbox), current_ip_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(current_ip_hbox), current_ip_value, TRUE, TRUE, 0);
    
    // IP输入区域
    GtkWidget *ip_input_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *ip_label = gtk_label_new("服务器IP:");
    GtkWidget *ip_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ip_entry), "输入服务器IP地址");
    GtkWidget *set_ip_btn = gtk_button_new_with_label("设置IP");
    
    gtk_box_pack_start(GTK_BOX(ip_input_hbox), ip_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(ip_input_hbox), ip_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(ip_input_hbox), set_ip_btn, FALSE, FALSE, 0);
    
    // 服务器列表
    GtkWidget *server_list_frame = gtk_frame_new("服务器列表");
    GtkWidget *server_list_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(server_list_vbox), 10);
    
    // 服务器列表显示
    GtkWidget *server_list = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(server_list), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    // 添加服务器区域
    GtkWidget *add_server_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *server_name_label = gtk_label_new("服务器名称:");
    GtkWidget *server_name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(server_name_entry), "输入服务器名称");
    GtkWidget *server_ip_label = gtk_label_new("服务器IP:");
    GtkWidget *server_ip_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(server_ip_entry), "输入服务器IP地址");
    GtkWidget *add_server_btn = gtk_button_new_with_label("添加服务器");
    
    gtk_box_pack_start(GTK_BOX(add_server_hbox), server_name_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_server_hbox), server_name_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(add_server_hbox), server_ip_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(add_server_hbox), server_ip_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(add_server_hbox), add_server_btn, FALSE, FALSE, 0);
    
    // 组装IP设置区域
    gtk_box_pack_start(GTK_BOX(ip_vbox), current_ip_hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(ip_vbox), ip_input_hbox, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(ip_frame), ip_vbox);
    
    // 组装服务器列表区域
    gtk_box_pack_start(GTK_BOX(server_list_vbox), server_list, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(server_list_vbox), add_server_hbox, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(server_list_frame), server_list_vbox);
    
    // 同城用户发现区域
    GtkWidget *discovery_frame = gtk_frame_new("同城用户发现");
    gtk_frame_set_shadow_type(GTK_FRAME(discovery_frame), GTK_SHADOW_ETCHED_IN);
    
    GtkWidget *discovery_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(discovery_vbox), 10);
    
    // 发现控制按钮
    GtkWidget *discovery_control_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *start_discovery_btn = gtk_button_new_with_label("开始发现");
    GtkWidget *stop_discovery_btn = gtk_button_new_with_label("停止发现");
    gtk_box_pack_start(GTK_BOX(discovery_control_hbox), start_discovery_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(discovery_control_hbox), stop_discovery_btn, FALSE, FALSE, 0);
    
    // 设备列表
    GtkWidget *device_list_frame = gtk_frame_new("发现的设备");
    GtkWidget *device_list_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(device_list_vbox), 10);
    
    GtkWidget *device_list = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(device_list), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    // 组装同城用户发现区域
    gtk_box_pack_start(GTK_BOX(discovery_vbox), discovery_control_hbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(discovery_vbox), device_list_frame, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(device_list_frame), device_list_vbox);
    gtk_box_pack_start(GTK_BOX(device_list_vbox), device_list, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(discovery_frame), discovery_vbox);
    
    // 组装设置面板
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), ip_frame, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), server_list_frame, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), discovery_frame, TRUE, TRUE, 0);
    
    // 信号处理
    g_signal_connect(set_ip_btn, "clicked", G_CALLBACK(on_set_ip_clicked), ip_entry);
    g_signal_connect(add_server_btn, "clicked", G_CALLBACK(on_add_server_clicked), 
        (gpointer)server_name_entry);
    g_signal_connect(start_discovery_btn, "clicked", G_CALLBACK(on_start_discovery_clicked), NULL);
    g_signal_connect(stop_discovery_btn, "clicked", G_CALLBACK(on_stop_discovery_clicked), NULL);
    
    return vbox;
}

// 设置IP按钮点击回调
static void on_set_ip_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry *ip_entry = GTK_ENTRY(data);
    const char *ip = gtk_entry_get_text(ip_entry);
    if (strlen(ip) > 0) {
        set_server_ip(ip);
        // 刷新当前IP显示（简化实现）
        printf("当前服务器IP: %s\n", get_current_server_ip());
    }
}

// 添加服务器按钮点击回调
static void on_add_server_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry *name_entry = GTK_ENTRY(data);
    // 简化实现，直接从数据中获取IP输入框
    const char *name = gtk_entry_get_text(name_entry);
    if (strlen(name) > 0) {
        // 这里简化处理，实际项目中需要正确获取IP输入框
        printf("添加服务器: %s\n", name);
        // 清空输入框
        gtk_entry_set_text(name_entry, "");
    }
}

// 开始发现按钮点击回调
static void on_start_discovery_clicked(GtkWidget *widget, gpointer data) {
    start_local_discovery();
    printf("开始同城用户发现\n");
}

// 停止发现按钮点击回调
static void on_stop_discovery_clicked(GtkWidget *widget, gpointer data) {
    stop_local_discovery();
    printf("停止同城用户发现\n");
}

GtkWidget* create_main_window(void) {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "FindFriend - 同城交友·展会助手");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1400, 800);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
    
    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_TOP);
    
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
        create_message_view(), gtk_label_new("💬 消息"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
        create_exhibition_panel(), gtk_label_new("🎪 展会"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
        create_ticket_panel(), gtk_label_new("🎫 票务"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
        create_transaction_panel(), gtk_label_new("💰 交易"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
        create_gallery_view(), gtk_label_new("📸 相册"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
        create_settings_panel(), gtk_label_new("⚙️ 设置"));
    
    gtk_widget_set_sensitive(notebook, FALSE);
    
    GtkWidget *menubar = gtk_menu_bar_new();
    GtkWidget *user_menu = gtk_menu_new();
    GtkWidget *user_item = gtk_menu_item_new_with_label("👤 未登录");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(user_item), user_menu);
    
    GtkWidget *login_item = gtk_menu_item_new_with_label("登录");
    g_signal_connect(login_item, "activate", G_CALLBACK(show_login_dialog), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(user_menu), login_item);
    
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), user_item);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
    
    gtk_container_add(GTK_CONTAINER(main_window), vbox);
    
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    return main_window;
}
