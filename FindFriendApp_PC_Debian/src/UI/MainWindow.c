#include "MainWindow.h"
#include "FriendList.h"
#include "MapView.h"
#include "ChatView.h"
#include "../Settings/Settings.h"
#include <gtk/gtk.h>
#include <math.h>

static GtkWidget *main_window;
static GtkWidget *friend_list_widget;
static GtkWidget *map_widget;
static GtkWidget *chat_widget;
static AppSettings settings;

// 好友选中回调函数
static void on_friend_selected(uint32_t user_id) {
    chat_view_show(user_id);
    gtk_widget_show(chat_widget);
}

// 地图用户点击回调
static void on_user_clicked(uint32_t user_id) {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
        "是否向对方发送好友请求？"
    );
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
        GtkWidget *msg = gtk_message_dialog_new(
            GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
            GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
            "好友请求已发送，等待对方同意~"
        );
        gtk_dialog_run(GTK_DIALOG(msg));
        gtk_widget_destroy(msg);
    }
    gtk_widget_destroy(dialog);
}

// 贝塞尔缓动函数
static double ease_out_cubic(double t) {
    double t2 = 1 - t;
    return 1 - t2 * t2 * t2;
}

// 动画显示窗口
static void animate_window_show(GtkWidget *widget, int duration_ms) {
    if (!settings.animation_enabled) {
        gtk_widget_show_all(widget);
        return;
    }
    
    gtk_widget_set_opacity(widget, 0);
    gtk_widget_show_all(widget);
    
    for (int i = 1; i <= 20; i++) {
        double opacity = ease_out_cubic(i / 20.0);
        gtk_widget_set_opacity(widget, opacity);
        while (gtk_events_pending()) gtk_main_iteration();
        g_usleep(duration_ms * 1000 / 20);
    }
    gtk_widget_set_opacity(widget, 1);
}

// 设置对话框
void show_settings_dialog(GtkWidget *parent) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "设置", GTK_WINDOW(parent),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "取消", GTK_RESPONSE_CANCEL,
        "保存", GTK_RESPONSE_OK,
        NULL
    );
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 350);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content), 20);
    
    // 昵称
    GtkWidget *nickname_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *nickname_label = gtk_label_new("昵称:");
    GtkWidget *nickname_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(nickname_entry), settings.nickname);
    gtk_box_pack_start(GTK_BOX(nickname_box), nickname_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(nickname_box), nickname_entry, TRUE, TRUE, 0);
    
    // 个性签名
    GtkWidget *sig_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *sig_label = gtk_label_new("个性签名:");
    gtk_widget_set_halign(sig_label, GTK_ALIGN_START);
    GtkWidget *sig_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(sig_entry), settings.signature);
    gtk_box_pack_start(GTK_BOX(sig_box), sig_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(sig_box), sig_entry, FALSE, FALSE, 0);
    
    // 老好人模式开关
    GtkWidget *gs_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *gs_label = gtk_label_new("老好人模式 (帮助其他用户发现彼此):");
    GtkWidget *gs_switch = gtk_switch_new();
    gtk_switch_set_active(GTK_SWITCH(gs_switch), settings.good_samaritan_mode);
    gtk_box_pack_start(GTK_BOX(gs_box), gs_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(gs_box), gs_switch, FALSE, FALSE, 0);
    
    // 动画开关
    GtkWidget *anim_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *anim_label = gtk_label_new("启用动画效果:");
    GtkWidget *anim_switch = gtk_switch_new();
    gtk_switch_set_active(GTK_SWITCH(anim_switch), settings.animation_enabled);
    gtk_box_pack_start(GTK_BOX(anim_box), anim_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(anim_box), anim_switch, FALSE, FALSE, 0);
    
    // 标签选择
    GtkWidget *tags_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);
    GtkWidget *tags_label = gtk_label_new("我的标签:");
    GtkWidget *furry_btn = gtk_check_button_new_with_label("🐺 福瑞");
    GtkWidget *anime_btn = gtk_check_button_new_with_label("🦊 二次元");
    GtkWidget *cos_btn = gtk_check_button_new_with_label("🎭 COS");
    
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(furry_btn), settings.tags & 1);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(anime_btn), settings.tags & 2);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cos_btn), settings.tags & 4);
    
    gtk_box_pack_start(GTK_BOX(tags_box), tags_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tags_box), furry_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tags_box), anime_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tags_box), cos_btn, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(content), nickname_box, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(content), sig_box, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(content), tags_box, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(content), gs_box, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(content), anim_box, FALSE, FALSE, 10);
    
    gtk_widget_show_all(content);
    
    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    if (result == GTK_RESPONSE_OK) {
        strncpy(settings.nickname, gtk_entry_get_text(GTK_ENTRY(nickname_entry)), 63);
        strncpy(settings.signature, gtk_entry_get_text(GTK_ENTRY(sig_entry)), 255);
        settings.tags = 0;
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(furry_btn))) settings.tags |= 1;
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(anime_btn))) settings.tags |= 2;
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cos_btn))) settings.tags |= 4;
        settings.good_samaritan_mode = gtk_switch_get_active(GTK_SWITCH(gs_switch));
        settings.animation_enabled = gtk_switch_get_active(GTK_SWITCH(anim_switch));
        settings_save(&settings);
        
        friend_list_update_user_info(settings.nickname, settings.signature, settings.tags);
        map_view_set_tags(settings.tags);
    }
    gtk_widget_destroy(dialog);
}

// 个人中心
void show_personal_center(GtkWidget *parent) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        "个人中心", GTK_WINDOW(parent),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "关闭", GTK_RESPONSE_CLOSE,
        NULL
    );
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 400);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content), 20);
    
    char info[1024];
    snprintf(info, sizeof(info),
        "<span size='xx-large'>🐺</span>\n\n"
        "<b><span size='large'>%s</span></b>\n\n"
        "📝 %s\n\n"
        "🏷️ 标签: %s%s%s\n\n"
        "👤 用户ID: %d\n\n"
        "🌍 位置: %.4f, %.4f",
        settings.nickname,
        settings.signature,
        (settings.tags & 1) ? "🐺 福瑞 " : "",
        (settings.tags & 2) ? "🦊 二次元 " : "",
        (settings.tags & 4) ? "🎭 COS " : "",
        10000,
        settings.map_lat, settings.map_lng
    );
    
    GtkWidget *label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), info);
    gtk_box_pack_start(GTK_BOX(content), label, FALSE, FALSE, 10);
    
    GtkWidget *edit_btn = gtk_button_new_with_label("编辑资料");
    g_signal_connect_swapped(edit_btn, "clicked", G_CALLBACK(show_settings_dialog), parent);
    gtk_box_pack_start(GTK_BOX(content), edit_btn, FALSE, FALSE, 10);
    
    gtk_widget_show_all(content);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// 创建标题栏
static GtkWidget* create_header_bar(void) {
    GtkWidget *header = gtk_header_bar_new();
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "FindFriend");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    
    // 设置按钮
    GtkWidget *settings_btn = gtk_button_new_from_icon_name("preferences-system", GTK_ICON_SIZE_BUTTON);
    g_signal_connect_swapped(settings_btn, "clicked", G_CALLBACK(show_settings_dialog), main_window);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), settings_btn);
    
    // 个人中心按钮
    GtkWidget *profile_btn = gtk_button_new_from_icon_name("avatar-default", GTK_ICON_SIZE_BUTTON);
    g_signal_connect_swapped(profile_btn, "clicked", G_CALLBACK(show_personal_center), main_window);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(header), profile_btn);
    
    // 老好人模式指示器
    GtkWidget *gs_indicator = gtk_label_new(NULL);
    if (settings.good_samaritan_mode) {
        gtk_label_set_markup(GTK_LABEL(gs_indicator), "🐺 老好人模式已开启");
    }
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header), gs_indicator);
    
    return header;
}

// 创建主布局
static GtkWidget* create_main_layout(void) {
    // 主水平布局
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    
    // 左侧好友栏 (1/5 宽度)
    friend_list_widget = friend_list_create();
    gtk_widget_set_hexpand(friend_list_widget, FALSE);
    gtk_widget_set_size_request(friend_list_widget, 200, -1);
    
    // 中间地图区域 (自适应)
    map_widget = map_view_create();
    gtk_widget_set_hexpand(map_widget, TRUE);
    gtk_widget_set_vexpand(map_widget, TRUE);
    
    // 右侧聊天区域 (隐藏，点击好友后展开)
    chat_widget = chat_view_create();
    gtk_widget_set_hexpand(chat_widget, FALSE);
    gtk_widget_set_size_request(chat_widget, 320, -1);
    gtk_widget_hide(chat_widget);
    
    gtk_box_pack_start(GTK_BOX(hbox), friend_list_widget, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), map_widget, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), chat_widget, FALSE, FALSE, 0);
    
    // 设置回调
    friend_list_set_on_friend_selected_callback(on_friend_selected);
    map_view_set_on_user_click_callback(on_user_clicked);
    
    return hbox;
}

GtkWidget* create_main_window(void) {
    settings_load(&settings);
    
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "FindFriend");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1280, 720);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
    
    // 设置深色主题
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css = 
        "* { background-color: #1a1a1a; color: #e0e0e0; }"
        "window { background-color: #1a1a1a; }"
        "button { background-color: #2d2d2d; border: none; border-radius: 6px; padding: 6px 12px; }"
        "button:hover { background-color: #3d3d3d; }"
        "entry { background-color: #2d2d2d; border: 1px solid #3d3d3d; border-radius: 6px; padding: 6px; }"
        "list-row { background-color: #2d2d2d; border-bottom: 1px solid #3d3d3d; }"
        "list-row:hover { background-color: #3d3d3d; }"
        ".dialog-vbox { background-color: #2d2d2d; }";
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    // 创建标题栏
    GtkWidget *header = create_header_bar();
    gtk_window_set_titlebar(GTK_WINDOW(main_window), header);
    
    // 创建主内容
    GtkWidget *content = create_main_layout();
    gtk_container_add(GTK_CONTAINER(main_window), content);
    
    // 动画显示主窗口
    animate_window_show(main_window, 300);
    
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    return main_window;
}
