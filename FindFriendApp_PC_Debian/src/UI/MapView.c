#include "MapView.h"
#include "../Network/GoodSamaritan.h"
#include "../Models/User.h"
#include <webkit2/webkit2.h>
#include <string.h>
#include <stdio.h>

static WebKitWebView *webview;
static UserClickCallback click_callback = NULL;
static uint16_t my_tags = 1;

// JavaScript 回调
static void on_js_message(WebKitUserContentManager *manager, 
                          WebKitJavascriptResult *result, 
                          gpointer data) {
    JSCValue *value = webkit_javascript_result_get_js_value(result);
    gchar *msg = jsc_value_to_string(value);
    
    if (msg && strstr(msg, "user_")) {
        uint32_t user_id = 0;
        sscanf(msg, "user_%u", &user_id);
        if (click_callback) {
            click_callback(user_id);
        }
    }
    g_free(msg);
}

// 更新附近用户
void map_view_update_nearby_users(UserList *list) {
    if (!webview || !list) return;
    
    // 构建 JSON
    char json_buffer[65536] = "[";
    int pos = 1;
    
    for (int i = 0; i < list->count && i < 100; i++) {
        if (i > 0) {
            if (pos < sizeof(json_buffer) - 2) json_buffer[pos++] = ',';
        }
        
        pos += snprintf(json_buffer + pos, sizeof(json_buffer) - pos,
            "{\"id\":%u,\"name\":\"%s\",\"emoji\":\"%s\",\"sig\":\"%s\","
            "\"lat\":%f,\"lng\":%f,\"tags\":%u}",
            list->users[i].id,
            list->users[i].nickname,
            list->users[i].emoji,
            list->users[i].signature,
            list->users[i].latitude,
            list->users[i].longitude,
            list->users[i].tags);
        if (pos >= sizeof(json_buffer) - 500) break;
    }
    if (pos < sizeof(json_buffer) - 2) json_buffer[pos++] = ']';
    json_buffer[pos] = '\0';
    
    char script[131072];
    snprintf(script, sizeof(script), 
        "if(window.updateNearbyUsers) window.updateNearbyUsers('%s');", json_buffer);
    
    webkit_web_view_evaluate_javascript(webview, script, -1, NULL, NULL, NULL, NULL, NULL);
}

// 设置地图中心
void map_view_set_center(double lat, double lng, int zoom) {
    if (!webview) return;
    char script[256];
    snprintf(script, sizeof(script), 
        "if(map) map.setView([%f, %f], %d);", lat, lng, zoom);
    webkit_web_view_evaluate_javascript(webview, script, -1, NULL, NULL, NULL, NULL, NULL);
}

void map_view_set_on_user_click_callback(UserClickCallback cb) {
    click_callback = cb;
}

void map_view_set_tags(uint16_t tags) {
    my_tags = tags;
}

// 创建地图
GtkWidget* map_view_create(void) {
    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);
    
    // 完整的地图 HTML
    const char *html = 
        "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
        "<link rel='stylesheet' href='https://unpkg.com/leaflet@1.9.4/dist/leaflet.css'/>"
        "<script src='https://unpkg.com/leaflet@1.9.4/dist/leaflet.js'></script>"
        "<style>"
        "*{margin:0;padding:0;}body{background:#1a1a1a;}#map{height:100vh;width:100%;}"
        ".custom-marker{width:44px;height:44px;border-radius:50%;display:flex;align-items:center;justify-content:center;font-size:26px;cursor:pointer;transition:transform 0.2s;box-shadow:0 2px 8px rgba(0,0,0,0.3);}"
        ".custom-marker:hover{transform:scale(1.15);z-index:1000;}"
        ".furry-marker{background:linear-gradient(135deg,#ff9800,#f57c00);border:2px solid #fff;}"
        ".anime-marker{background:linear-gradient(135deg,#e91e63,#c2185b);border:2px solid #fff;}"
        ".cos-marker{background:linear-gradient(135deg,#9c27b0,#7b1fa2);border:2px solid #fff;}"
        ".info-window{background:#2d2d2d;color:#e0e0e0;border-radius:12px;padding:12px;min-width:180px;border-left:4px solid #ff9800;}"
        ".info-window h4{margin:0 0 5px;color:#ff9800;}"
        ".info-window p{margin:5px 0;font-size:12px;color:#aaa;}"
        ".info-window .distance{color:#4caf50;font-size:11px;}"
        ".info-window button{background:#ff9800;border:none;padding:6px 16px;border-radius:20px;cursor:pointer;margin-top:8px;font-weight:bold;color:#1a1a1a;}"
        ".info-window button:hover{background:#ffb74d;}"
        ".legend{position:absolute;bottom:20px;right:20px;background:rgba(45,45,45,0.95);padding:12px;border-radius:12px;z-index:1000;font-size:12px;backdrop-filter:blur(4px);}"
        ".legend span{display:inline-block;width:12px;height:12px;border-radius:50%;margin-right:6px;}"
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
        "function updateUserOnMap(user){"
        "if(markers[user.id]){markers[user.id].setLatLng([user.lat,user.lng]);}else{"
        "var cls=getMarkerColor(user.tags);var emoji=getEmoji(user.tags);"
        "var icon=L.divIcon({className:'custom-marker '+cls,html:'<span>'+emoji+'</span>',iconSize:[44,44],popupAnchor:[0,-22]});"
        "var center=map.getCenter();var dist=calculateDistance(center.lat,center.lng,user.lat,user.lng);"
        "markers[user.id]=L.marker([user.lat,user.lng],{icon:icon}).addTo(map).bindPopup("
        "'<div class=\"info-window\"><h4>'+user.name+'</h4><p>'+user.sig+'</p>'"
        "+'<p class=\"distance\">📍 距离 '+dist.toFixed(1)+' km</p>'"
        "+'<button onclick=\"window.startChat('+user.id+')\">💬 添加好友</button></div>');}}"
        "}"
        "function clearAllMarkers(){for(var id in markers){map.removeLayer(markers[id]);}markers={};}"
        "window.updateNearbyUsers=function(usersJson){clearAllMarkers();try{var usersList=JSON.parse(usersJson);for(var i=0;i<usersList.length;i++){updateUserOnMap(usersList[i]);}}catch(e){console.log(e);}};"
        "window.startChat=function(userId){if(window.webkit&&window.webkit.messageHandlers){window.webkit.messageHandlers.startChat.postMessage('user_'+userId);}};"
        "map.on('zoomend',function(){var zoom=map.getZoom();if(window.external&&window.external.onZoomChange){window.external.onZoomChange(zoom);}});"
        "</script></body></html>";
    
    webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    
    // 设置 JavaScript 通信
    WebKitUserContentManager *manager = webkit_web_view_get_user_content_manager(webview);
    webkit_user_content_manager_register_script_message_handler(manager, "startChat");
    g_signal_connect(manager, "script-message-received::startChat", 
                     G_CALLBACK(on_js_message), NULL);
    
    webkit_web_view_load_html(webview, html, NULL);
    
    gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(webview));
    
    return frame;
}
