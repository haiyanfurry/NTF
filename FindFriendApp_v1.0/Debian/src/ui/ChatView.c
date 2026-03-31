#include "ChatView.h"
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static GtkWidget *chat_window;
static GtkWidget *chat_display;
static GtkWidget *chat_input;
static GtkWidget *send_button;
static GtkWidget *close_button;
static GtkWidget *title_label;
static uint32_t current_user_id = 0;
static char current_user_name[64];

// 添加消息到聊天记录
void chat_view_add_message(uint32_t from_id, const char* message, int is_self) {
    if (!chat_display) return;
    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_display));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    
    // 获取当前时间
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[16];
    strftime(time_str, sizeof(time_str), "%H:%M", tm_info);
    
    // 构建消息气泡
    char formatted[2048];
    if (is_self) {
        snprintf(formatted, sizeof(formatted),
            "\n<tt><span foreground='#888888'>%s</span></tt>\n"
            "<span background='#ff9800' foreground='#1a1a1a'>  %s  </span>\n\n",
            time_str, message);
    } else {
        snprintf(formatted, sizeof(formatted),
            "\n<tt><span foreground='#888888'>%s</span></tt>\n"
            "<span background='#2d2d2d' foreground='#e0e0e0'>  %s  </span>\n\n",
            time_str, message);
    }
    
    gtk_text_buffer_insert_markup(buffer, &end, formatted, -1);
    
    // 滚动到底部
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(chat_display), &end, 0, FALSE, 0, 0);
}

// 发送消息
static void send_message(GtkWidget *widget, gpointer data) {
    const char *msg = gtk_entry_get_text(GTK_ENTRY(chat_input));
    if (strlen(msg) == 0) return;
    
    // 显示自己发送的消息
    chat_view_add_message(0, msg, 1);
    
    // 清空输入框
    gtk_entry_set_text(GTK_ENTRY(chat_input), "");
    
    // TODO: 通过网络发送消息
    // send_encrypted_message(current_user_id, msg);
}

// 关闭聊天
static void close_chat(GtkWidget *widget, gpointer data) {
    chat_view_hide();
}

// 显示聊天窗口
void chat_view_show(uint32_t user_id) {
    current_user_id = user_id;
    snprintf(current_user_name, sizeof(current_user_name), "用户 %u", user_id);
    
    char title[128];
    snprintf(title, sizeof(title), "💬 与 %s 聊天中", current_user_name);
    gtk_label_set_text(GTK_LABEL(title_label), title);
    
    // 清空聊天记录
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_display));
    gtk_text_buffer_set_text(buffer, "", -1);
    
    // 添加欢迎消息
    chat_view_add_message(0, "你们已经成为好友，开始聊天吧~", 0);
    
    gtk_widget_show_all(chat_window);
}

void chat_view_hide(void) {
    gtk_widget_hide(chat_window);
    current_user_id = 0;
}

// 创建聊天视图
GtkWidget* chat_view_create(void) {
    chat_window = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(chat_window, "chat-panel");
    
    // 标题栏
    GtkWidget *title_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_start(title_bar, 8);
    gtk_widget_set_margin_end(title_bar, 8);
    gtk_widget_set_margin_top(title_bar, 8);
    gtk_widget_set_margin_bottom(title_bar, 8);
    
    title_label = gtk_label_new("💬 聊天");
    gtk_widget_set_halign(title_label, GTK_ALIGN_START);
    
    close_button = gtk_button_new_with_label("✕");
    gtk_widget_set_halign(close_button, GTK_ALIGN_END);
    gtk_widget_set_size_request(close_button, 30, 30);
    g_signal_connect(close_button, "clicked", G_CALLBACK(close_chat), NULL);
    
    gtk_box_pack_start(GTK_BOX(title_bar), title_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(title_bar), close_button, FALSE, FALSE, 0);
    
    // 聊天记录区域
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled, -1, 400);
    
    chat_display = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_display), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_display), GTK_WRAP_WORD);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(chat_display), FALSE);
    
    // 设置聊天记录支持 markup
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_display));
    gtk_text_buffer_create_tag(buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
    
    gtk_container_add(GTK_CONTAINER(scrolled), chat_display);
    
    // 输入区域
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_margin_start(input_box, 8);
    gtk_widget_set_margin_end(input_box, 8);
    gtk_widget_set_margin_bottom(input_box, 8);
    
    chat_input = gtk_entry_new();
    gtk_widget_set_hexpand(chat_input, TRUE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(chat_input), "输入消息...");
    
    send_button = gtk_button_new_with_label("发送");
    gtk_widget_set_size_request(send_button, 60, -1);
    g_signal_connect(send_button, "clicked", G_CALLBACK(send_message), NULL);
    g_signal_connect(chat_input, "activate", G_CALLBACK(send_message), NULL);
    
    gtk_box_pack_start(GTK_BOX(input_box), chat_input, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(input_box), send_button, FALSE, FALSE, 0);
    
    // 组装
    gtk_box_pack_start(GTK_BOX(chat_window), title_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(chat_window), scrolled, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(chat_window), input_box, FALSE, FALSE, 0);
    
    return chat_window;
}
