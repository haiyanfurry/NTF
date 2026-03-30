#include "HomePage.h"
#include "../Server/GoodSamaritan.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static GtkWidget *main_window;
static GtkWidget *friend_list;
static WebKitWebView *map_view;
static GtkWidget *chat_display;
static GtkWidget *chat_input;

typedef struct {
    int id;
    char name[64];
    char emoji[8];
    char signature[128];
    int online;
    double lat;
    double lng;
} Friend;

static Friend friends[] = {
    {1, "狼仔", "🐺", "一只热爱自由的狼", 1, 39.9042, 116.4074},
    {2, "狐酱", "🦊", "二次元爱好者", 1, 39.9142, 116.4174},
    {3, "喵喵", "🐱", "福瑞控一枚", 0, 39.8942, 116.3974},
    {4, "龙龙", "🐉", "东方龙画师", 1, 39.9242, 116.4274},
    {5, "兔兔", "🐰", "软萌妹子", 1, 39.9342, 116.4374}
};

static int friend_count = 5;
static UserLocation my_location;

// JavaScript 回调
static void on_js_message(WebKitUserContentManager *manager, 
                          WebKitJavascriptResult *result, 
                          gpointer data) {
    JSCValue *value = webkit_javascript_result_get_js_value(result);
    gchar *msg = jsc_value_to_string(value);
    
    if (msg && strstr(msg, "chat_")) {
        int user_id = 0;
        sscanf(msg, "chat_%d", &user_id);
        
        for (int i = 0; i < friend_count; i++) {
            if (friends[i].id == user_id) {
                GtkWidget *dialog = gtk_message_dialog_new(
                    GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
                    GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
                    "%s 想加你为好友\n\n%s", 
                    friends[i].name, friends[i].signature);
                
                if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES) {
                    GtkWidget *msg_dialog = gtk_message_dialog_new(
                        GTK_WINDOW(main_window), GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                        "已发送好友请求给 %s", friends[i].name);
                    gtk_dialog_run(GTK_DIALOG(msg_dialog));
                    gtk_widget_destroy(msg_dialog);
                }
                gtk_widget_destroy(dialog);
                break;
            }
        }
    }
    g_free(msg);
}

// 更新地图上的用户（手动拼接 JSON）
static void update_map_users(NearbyResponse *resp) {
    if (!map_view || !resp) return;
    
    // 构建 JSON 字符串
    char json_buffer[32768] = "[";
    int pos = 1;
    
    for (int i = 0; i < resp->count && i < 50; i++) {
        if (i > 0) {
            if (pos < sizeof(json_buffer) - 2) json_buffer[pos++] = ',';
        }
        
        // 转义特殊字符
        char nickname_escaped[256];
        char signature_escaped[512];
        int ni = 0, si = 0;
        for (int j = 0; resp->users[i].nickname[j] && j < 200; j++) {
            if (resp->users[i].nickname[j] == '"' || resp->users[i].nickname[j] == '\\')
                nickname_escaped[ni++] = '\\';
            nickname_escaped[ni++] = resp->users[i].nickname[j];
        }
        nickname_escaped[ni] = '\0';
        
        for (int j = 0; resp->users[i].signature[j] && j < 400; j++) {
            if (resp->users[i].signature[j] == '"' || resp->users[i].signature[j] == '\\')
                signature_escaped[si++] = '\\';
            signature_escaped[si++] = resp->users[i].signature[j];
        }
        signature_escaped[si] = '\0';
        
        pos += snprintf(json_buffer + pos, sizeof(json_buffer) - pos,
            "{\"user_id\":%u,\"nickname\":\"%s\",\"signature\":\"%s\","
            "\"latitude\":%f,\"longitude\":%f,\"tags\":%u}",
            resp->users[i].user_id,
            nickname_escaped,
            signature_escaped,
            resp->users[i].latitude,
            resp->users[i].longitude,
            resp->users[i].tags);
        if (pos >= sizeof(json_buffer) - 500) break;
    }
    if (pos < sizeof(json_buffer) - 2) json_buffer[pos++] = ']';
    json_buffer[pos] = '\0';
    
    // 使用正确的 API 调用
    char script[65536];
    snprintf(script, sizeof(script), 
        "if(window.updateNearbyUsers) window.updateNearbyUsers('%s');", json_buffer);
    
    // webkit_web_view_evaluate_javascript 需要 7 个参数
    webkit_web_view_evaluate_javascript(map_view, script, -1, NULL, NULL, NULL, NULL, NULL);
}

// 定时更新附近用户
static gboolean update_nearby_timer(gpointer data) {
    if (my_location.user_id == 0) return TRUE;
    
    NearbyResponse resp;
    memset(&resp, 0, sizeof(resp));
    if (get_nearby_users(my_location.latitude, my_location.longitude, 50.0, &resp) > 0) {
        update_map_users(&resp);
    }
    return TRUE;
}

// 获取当前位置
static void get_current_location(void) {
    // 使用固定位置
    my_location.latitude = 39.9042;
    my_location.longitude = 116.4074;
    strcpy(my_location.city, "北京");
    my_location.user_id = 10000 + (int)time(NULL) % 1000;
    strcpy(my_location.nickname, "老好人");
    my_location.tags = 1;
    strcpy(my_location.signature, "欢迎加好友~");
    my_location.ip_addr = 0;
    my_location.port = 8889;
    my_location.last_seen = time(NULL);
    
    // 上报位置
    report_location(&my_location);
    
    // 更新地图中心
    if (map_view) {
        char script[256];
        snprintf(script, sizeof(script), "if(map) map.setView([%f, %f], 10);", 
                 my_location.latitude, my_location.longitude);
        webkit_web_view_evaluate_javascript(map_view, script, -1, NULL, NULL, NULL, NULL, NULL);
    }
    
    // 启动定时器
    g_timeout_add_seconds(10, update_nearby_timer, NULL);
}

// 创建地图区域
static GtkWidget* create_map_area(void) {
    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
    
    // 完整的地图 HTML
    const char *html = 
        "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
        "<link rel='stylesheet' href='https://unpkg.com/leaflet@1.9.4/dist/leaflet.css'/>"
        "<script src='https://unpkg.com/leaflet@1.9.4/dist/leaflet.js'></script>"
        "<style>"
        "*{margin:0;padding:0;}body{background:#1e1e1e;}#map{height:100vh;width:100%;}"
        ".custom-marker{width:40px;height:40px;border-radius:50%;background:#ff9800;border:2px solid white;display:flex;align-items:center;justify-content:center;font-size:24px;cursor:pointer;transition:transform 0.2s;}"
        ".custom-marker:hover{transform:scale(1.2);}"
        ".furry-marker{background:#ff9800;}.anime-marker{background:#e91e63;}.cos-marker{background:#9c27b0;}"
        ".info-window{background:#1e1e1e;color:white;border-radius:12px;padding:12px;min-width:180px;border-left:4px solid #ff9800;}"
        ".info-window h4{margin:0 0 5px;color:#ff9800;}.info-window p{margin:5px 0;font-size:12px;color:#aaa;}"
        ".info-window button{background:#ff9800;border:none;padding:6px 16px;border-radius:20px;cursor:pointer;margin-top:8px;font-weight:bold;}"
        ".legend{position:absolute;bottom:20px;right:20px;background:rgba(30,30,30,0.9);padding:10px;border-radius:8px;z-index:1000;font-size:12px;color:white;}"
        ".legend span{display:inline-block;width:12px;height:12px;border-radius:50%;margin-right:5px;}"
        "</style></head><body><div id='map'></div>"
        "<div class='legend'><div><span style='background:#ff9800'></span> 福瑞</div>"
        "<div><span style='background:#e91e63'></span> 二次元</div>"
        "<div><span style='background:#9c27b0'></span> COS</div></div>"
        "<script>"
        "var map=L.map('map').setView([39.9042,116.4074],10);"
        "L.tileLayer('https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png',{attribution:'© OpenStreetMap',subdomains:'abcd'}).addTo(map);"
        "var markers={};"
        "function getMarkerColor(tags){if(tags&1)return'furry-marker';if(tags&2)return'anime-marker';if(tags&4)return'cos-marker';return'furry-marker';}"
        "function getEmoji(tags){if(tags&1)return'🐺';if(tags&2)return'🦊';if(tags&4)return'🎭';return'🐾';}"
        "function calculateDistance(lat1,lon1,lat2,lon2){var R=6371;var dLat=(lat2-lat1)*Math.PI/180;var dLon=(lon2-lon1)*Math.PI/180;var a=Math.sin(dLat/2)*Math.sin(dLat/2)+Math.cos(lat1*Math.PI/180)*Math.cos(lat2*Math.PI/180)*Math.sin(dLon/2)*Math.sin(dLon/2);var c=2*Math.atan2(Math.sqrt(a),Math.sqrt(1-a));return R*c;}"
        "function updateUserOnMap(user){if(markers[user.user_id]){markers[user.user_id].setLatLng([user.latitude,user.longitude]);}else{var cls=getMarkerColor(user.tags);var emoji=getEmoji(user.tags);var icon=L.divIcon({className:'custom-marker '+cls,html:'<span>'+emoji+'</span>',iconSize:[40,40],popupAnchor:[0,-20]});var center=map.getCenter();var dist=calculateDistance(center.lat,center.lng,user.latitude,user.longitude);markers[user.user_id]=L.marker([user.latitude,user.longitude],{icon:icon}).addTo(map).bindPopup('<div class=\"info-window\"><h4>'+user.nickname+'</h4><p>'+user.signature+'</p><p class=\"distance\">📍 距离 '+dist.toFixed(1)+' km</p><button onclick=\"window.startChat('+user.user_id+')\">💬 添加好友</button></div>');}}"
        "function clearAllMarkers(){for(var id in markers){map.removeLayer(markers[id]);}markers={};}"
        "window.updateNearbyUsers=function(usersJson){clearAllMarkers();try{var usersList=JSON.parse(usersJson);for(var i=0;i<usersList.length;i++){updateUserOnMap(usersList[i]);}}catch(e){console.log(e);}};"
        "window.startChat=function(userId){if(window.webkit&&window.webkit.messageHandlers&&window.webkit.messageHandlers.startChat){window.webkit.messageHandlers.startChat.postMessage('chat_'+userId);}console.log('Start chat with:',userId);};"
        "</script></body></html>";
    
    map_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    
    // 设置 JavaScript 与 C 通信
    WebKitUserContentManager *manager = webkit_web_view_get_user_content_manager(map_view);
    webkit_user_content_manager_register_script_message_handler(manager, "startChat");
    g_signal_connect(manager, "script-message-received::startChat", 
                     G_CALLBACK(on_js_message), NULL);
    
    webkit_web_view_load_html(map_view, html, NULL);
    
    gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(map_view));
    
    // 延迟获取位置
    g_timeout_add_seconds(1, (GSourceFunc)get_current_location, NULL);
    
    return frame;
}

// 创建左侧列表
static GtkWidget* create_friend_list(void) {
    GtkWidget *frame = gtk_frame_new(NULL);
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<b><span size='large'>📱 附近的人</span></b>");
    gtk_widget_set_margin_start(title, 12);
    gtk_widget_set_margin_top(title, 12);
    gtk_widget_set_margin_bottom(title, 8);
    
    friend_list = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(friend_list), GTK_SELECTION_SINGLE);
    
    for (int i = 0; i < friend_count; i++) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_widget_set_margin_start(hbox, 8);
        gtk_widget_set_margin_end(hbox, 8);
        gtk_widget_set_margin_top(hbox, 8);
        gtk_widget_set_margin_bottom(hbox, 8);
        
        GtkWidget *avatar = gtk_label_new(friends[i].emoji);
        gtk_widget_set_size_request(avatar, 40, 40);
        
        GtkWidget *info = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        GtkWidget *name = gtk_label_new(friends[i].name);
        GtkWidget *sig = gtk_label_new(friends[i].signature);
        gtk_label_set_xalign(GTK_LABEL(sig), 0);
        gtk_widget_set_halign(name, GTK_ALIGN_START);
        gtk_widget_set_halign(sig, GTK_ALIGN_START);
        
        gtk_box_pack_start(GTK_BOX(info), name, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(info), sig, FALSE, FALSE, 0);
        
        gtk_box_pack_start(GTK_BOX(hbox), avatar, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(hbox), info, TRUE, TRUE, 0);
        
        gtk_container_add(GTK_CONTAINER(row), hbox);
        gtk_list_box_insert(GTK_LIST_BOX(friend_list), row, -1);
    }
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), friend_list);
    
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    
    return frame;
}

// 创建聊天区域
static GtkWidget* create_chat_area(void) {
    GtkWidget *frame = gtk_frame_new(NULL);
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);
    
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<b>💬 聊天</b>");
    gtk_widget_set_halign(title, GTK_ALIGN_START);
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled, -1, 300);
    
    chat_display = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(chat_display), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(chat_display), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(scrolled), chat_display);
    
    GtkWidget *input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    chat_input = gtk_entry_new();
    gtk_widget_set_hexpand(chat_input, TRUE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(chat_input), "选择好友后开始聊天...");
    gtk_widget_set_sensitive(chat_input, FALSE);
    
    GtkWidget *send_btn = gtk_button_new_with_label("发送");
    gtk_widget_set_sensitive(send_btn, FALSE);
    
    gtk_box_pack_start(GTK_BOX(input_box), chat_input, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(input_box), send_btn, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), input_box, FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    
    return frame;
}

// CSS 样式
static void apply_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    const char *css = 
        "window, frame { background-color: #1e1e1e; }"
        "label { color: #ffffff; }"
        "list-row { background-color: #2d2d2d; }"
        "list-row:hover { background-color: #3d3d3d; }"
        "entry { background: #2d2d2d; border: 1px solid #3d3d3d; border-radius: 20px; padding: 8px 12px; color: white; }"
        "button { background: #ff9800; border: none; border-radius: 20px; padding: 8px 16px; font-weight: bold; color: #1e1e1e; }"
        "button:hover { background: #ffb74d; }"
        "textview { background: #2d2d2d; border-radius: 12px; padding: 8px; color: white; }";
    
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

GtkWidget* create_home_page(void) {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "FindFriend - 老好人模式");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1400, 800);
    gtk_window_set_position(GTK_WINDOW(main_window), GTK_WIN_POS_CENTER);
    
    apply_css();
    
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    
    GtkWidget *left_panel = create_friend_list();
    gtk_widget_set_size_request(left_panel, 260, -1);
    
    GtkWidget *map_area = create_map_area();
    
    GtkWidget *chat_area = create_chat_area();
    gtk_widget_set_size_request(chat_area, 320, -1);
    
    gtk_box_pack_start(GTK_BOX(hbox), left_panel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), map_area, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), chat_area, FALSE, FALSE, 0);
    
    gtk_container_add(GTK_CONTAINER(main_window), hbox);
    
    return main_window;
}
