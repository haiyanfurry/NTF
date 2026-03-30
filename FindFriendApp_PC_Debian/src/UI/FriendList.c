#include "FriendList.h"
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

static GtkWidget *list_box;
static GtkWidget *expander;
static FriendSelectedCallback callback = NULL;

// 好友项数据结构
typedef struct {
    uint32_t id;
    char name[64];
    char emoji[8];
    char signature[128];
    int online;
    GtkWidget *row;
} FriendItem;

static FriendItem friends[100];
static int friend_count = 0;

// 好友点击回调函数定义
static gboolean on_friend_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    uint32_t user_id = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(widget), "friend_id"));
    if (callback) {
        callback(user_id);
    }
    return TRUE;
}

// 创建好友列表项
static GtkWidget* create_friend_row(FriendItem *item) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_margin_start(box, 8);
    gtk_widget_set_margin_end(box, 8);
    gtk_widget_set_margin_top(box, 8);
    gtk_widget_set_margin_bottom(box, 8);
    
    // 头像
    GtkWidget *avatar_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *avatar = gtk_label_new(item->emoji);
    gtk_widget_set_size_request(avatar, 40, 40);
    gtk_style_context_add_class(gtk_widget_get_style_context(avatar), "avatar");
    
    // 在线状态指示器
    GtkWidget *status = gtk_label_new(NULL);
    char *status_markup = g_markup_printf_escaped(
        "<span foreground='%s' size='x-small'>%s</span>",
        item->online ? "#4caf50" : "#9e9e9e",
        item->online ? "● 在线" : "○ 离线"
    );
    gtk_label_set_markup(GTK_LABEL(status), status_markup);
    g_free(status_markup);
    
    gtk_box_pack_start(GTK_BOX(avatar_box), avatar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(avatar_box), status, FALSE, FALSE, 0);
    
    // 信息区域
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *name_label = gtk_label_new(NULL);
    char *name_markup = g_markup_printf_escaped(
        "<b><span size='large'>%s</span></b>",
        item->name
    );
    gtk_label_set_markup(GTK_LABEL(name_label), name_markup);
    g_free(name_markup);
    gtk_widget_set_halign(name_label, GTK_ALIGN_START);
    
    GtkWidget *sig_label = gtk_label_new(item->signature);
    gtk_widget_set_halign(sig_label, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(sig_label), "signature");
    gtk_label_set_ellipsize(GTK_LABEL(sig_label), PANGO_ELLIPSIZE_END);
    gtk_widget_set_size_request(sig_label, 140, -1);
    
    gtk_box_pack_start(GTK_BOX(info_box), name_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(info_box), sig_label, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(box), avatar_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), info_box, TRUE, TRUE, 0);
    
    // 添加点击事件
    GtkWidget *event_box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(event_box), box);
    g_object_set_data(G_OBJECT(event_box), "friend_id", GUINT_TO_POINTER(item->id));
    g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_friend_clicked), NULL);
    
    return event_box;
}

// 刷新列表
static void refresh_list(void) {
    // 清空列表
    GList *children = gtk_container_get_children(GTK_CONTAINER(list_box));
    for (GList *child = children; child != NULL; child = child->next) {
        gtk_container_remove(GTK_CONTAINER(list_box), GTK_WIDGET(child->data));
    }
    g_list_free(children);
    
    // 重新添加所有好友
    for (int i = 0; i < friend_count; i++) {
        GtkWidget *row = create_friend_row(&friends[i]);
        gtk_list_box_insert(GTK_LIST_BOX(list_box), row, -1);
        friends[i].row = row;
    }
    gtk_widget_show_all(list_box);
}

// 添加好友
void friend_list_add_user(uint32_t id, const char* name, const char* emoji, const char* signature, int online) {
    if (friend_count >= 100) return;
    
    FriendItem *item = &friends[friend_count];
    item->id = id;
    strncpy(item->name, name, 63);
    strncpy(item->emoji, emoji, 7);
    strncpy(item->signature, signature, 127);
    item->online = online;
    friend_count++;
    
    refresh_list();
}

// 清空好友列表
void friend_list_clear(void) {
    friend_count = 0;
    refresh_list();
}

// 设置回调
void friend_list_set_on_friend_selected_callback(FriendSelectedCallback cb) {
    callback = cb;
}

// 更新用户信息
void friend_list_update_user_info(const char* name, const char* signature, uint16_t tags) {
    // 更新自己的信息（如果需要的话）
}

// 创建好友列表
GtkWidget* friend_list_create(void) {
    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
    
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    // 可展开的标题栏
    expander = gtk_expander_new_with_mnemonic("📱 _好友列表");
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);
    
    // 列表容器
    list_box = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(list_box), GTK_SELECTION_NONE);
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), list_box);
    
    gtk_container_add(GTK_CONTAINER(expander), scrolled);
    gtk_box_pack_start(GTK_BOX(vbox), expander, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    
    // 默认清空好友列表
    friend_list_clear();
    
    return frame;
}
