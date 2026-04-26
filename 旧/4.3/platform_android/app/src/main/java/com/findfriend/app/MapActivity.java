package com.findfriend.app;

import android.os.Bundle;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ListView;
import android.widget.Button;
import android.view.View;
import androidx.appcompat.app.AppCompatActivity;

public class MapActivity extends AppCompatActivity {

    private WebView mapWebView;
    private ListView userListView;
    private Button addFriendButton;
    private String selectedUserId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map);

        mapWebView = findViewById(R.id.map_webview);
        userListView = findViewById(R.id.user_list);
        addFriendButton = findViewById(R.id.add_friend_button);

        // 配置WebView
        mapWebView.getSettings().setJavaScriptEnabled(true);
        mapWebView.setWebViewClient(new WebViewClient());

        // 加载本地地图HTML
        loadMapHTML();

        // 初始化用户列表
        initUserList();

        // 添加好友按钮点击事件
        addFriendButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (selectedUserId != null) {
                    addFriend(selectedUserId);
                }
            }
        });
    }

    // 加载地图HTML
    private void loadMapHTML() {
        String html = "" +
                "<!DOCTYPE html>" +
                "<html>" +
                "<head>" +
                "<meta charset=\"utf-8\">" +
                "<title>同城地图</title>" +
                "<style>" +
                "body { margin: 0; padding: 0; width: 100%; height: 100%; }" +
                "#map { width: 100%; height: 100%; background-color: #f0f0f0; position: relative; }" +
                ".user-marker {" +
                "position: absolute;" +
                "width: 40px;" +
                "height: 40px;" +
                "border-radius: 50%;" +
                "background-color: #4CAF50;" +
                "color: white;" +
                "display: flex;" +
                "align-items: center;" +
                "justify-content: center;" +
                "cursor: pointer;" +
                "box-shadow: 0 2px 5px rgba(0,0,0,0.3);" +
                "transition: all 0.3s ease;" +
                "}" +
                ".user-marker:hover {" +
                "transform: scale(1.1);" +
                "box-shadow: 0 4px 10px rgba(0,0,0,0.5);" +
                "}" +
                ".user-info {" +
                "position: absolute;" +
                "background-color: white;" +
                "padding: 10px;" +
                "border-radius: 5px;" +
                "box-shadow: 0 2px 10px rgba(0,0,0,0.2);" +
                "display: none;" +
                "z-index: 1000;" +
                "}" +
                ".map-control {" +
                "position: absolute;" +
                "top: 10px;" +
                "left: 10px;" +
                "background-color: white;" +
                "padding: 10px;" +
                "border-radius: 5px;" +
                "box-shadow: 0 2px 5px rgba(0,0,0,0.2);" +
                "}" +
                "</style>" +
                "</head>" +
                "<body>" +
                "<div id=\"map\">" +
                "<div class=\"map-control\">" +
                "<h3>同城地图</h3>" +
                "<p>显示附近的用户</p>" +
                "</div>" +
                "</div>" +
                "<script>" +
                "// 地图中心点" +
                "var center = {lat: 39.9042, lng: 116.4074};" +
                "var users = {};" +
                "" +
                "// 初始化地图" +
                "function initMap() {" +
                "// 简单的地图背景" +
                "var map = document.getElementById('map');" +
                "map.style.backgroundImage = 'linear-gradient(to bottom, #e3f2fd, #bbdefb)';" +
                "" +
                "// 添加自己的位置" +
                "addMarker(center.lat, center.lng, '我的位置', 'me');" +
                "}" +
                "" +
                "// 添加标记" +
                "function addMarker(lat, lng, title, userId) {" +
                "var map = document.getElementById('map');" +
                "var marker = document.createElement('div');" +
                "marker.className = 'user-marker';" +
                "marker.id = 'marker_' + userId;" +
                "marker.title = title;" +
                "" +
                "// 简单的坐标转换" +
                "var x = (lng - (center.lng - 0.1)) * 5000;" +
                "var y = (center.lat - 0.1 - lat) * 5000;" +
                "" +
                "marker.style.left = x + 'px';" +
                "marker.style.top = y + 'px';" +
                "marker.innerHTML = userId === 'me' ? '我' : 'U';" +
                "" +
                "// 点击事件" +
                "marker.onclick = function() {" +
                "showUserInfo(x, y, title, userId);" +
                "}" +
                "" +
                "map.appendChild(marker);" +
                "users[userId] = {lat: lat, lng: lng, element: marker};" +
                "}" +
                "" +
                "// 显示用户信息" +
                "function showUserInfo(x, y, title, userId) {" +
                "// 移除之前的信息框" +
                "var oldInfo = document.getElementById('userInfo');" +
                "if (oldInfo) {" +
                "oldInfo.remove();" +
                "}" +
                "" +
                "var map = document.getElementById('map');" +
                "var info = document.createElement('div');" +
                "info.id = 'userInfo';" +
                "info.className = 'user-info';" +
                "info.style.left = (x + 50) + 'px';" +
                "info.style.top = y + 'px';" +
                "info.innerHTML = '<strong>' + title + '</strong><br>ID: ' + userId + '<br><button onclick=\"addFriend(\\'' + userId + '\\')\">添加好友</button>';" +
                "" +
                "map.appendChild(info);" +
                "" +
                "// 点击其他地方关闭信息框" +
                "setTimeout(function() {" +
                "document.addEventListener('click', function closeInfo(e) {" +
                "if (e.target !== info && !info.contains(e.target)) {" +
                "info.remove();" +
                "document.removeEventListener('click', closeInfo);" +
                "}" +
                "});" +
                "}, 100);" +
                "}" +
                "" +
                "// 高亮用户" +
                "function highlightUser(userId) {" +
                "// 重置所有标记" +
                "for (var id in users) {
                    if (users.hasOwnProperty(id)) {
                        users[id].element.style.backgroundColor = '#4CAF50';
                    }
                }" +
                "" +
                "// 高亮选中的用户" +
                "if (users[userId]) {
                    users[userId].element.style.backgroundColor = '#FF9800';
                    users[userId].element.style.transform = 'scale(1.2)';
                }" +
                "}" +
                "" +
                "// 更新自己的位置" +
                "function updateMyLocation(lat, lng) {
                    var marker = document.getElementById('marker_me');
                    if (marker) {
                        var x = (lng - (center.lng - 0.1)) * 5000;
                        var y = (center.lat - 0.1 - lat) * 5000;
                        marker.style.left = x + 'px';
                        marker.style.top = y + 'px';
                    }
                }" +
                "" +
                "// 添加好友" +
                "function addFriend(userId) {
                    alert('发送好友请求给: ' + userId);
                }" +
                "" +
                "// 初始化地图" +
                "initMap();" +
                "</script>" +
                "</body>" +
                "</html>";

        mapWebView.loadDataWithBaseURL(null, html, "text/html", "UTF-8", null);
    }

    // 初始化用户列表
    private void initUserList() {
        // 模拟用户数据
        String[] users = {"张三 (100m)", "李四 (200m)", "王五 (300m)", "赵六 (400m)", "钱七 (500m)"};
        String[] userIds = {"user1", "user2", "user3", "user4", "user5"};

        // 创建适配器并设置给ListView
        // 这里使用简单的ArrayAdapter作为示例
        android.widget.ArrayAdapter<String> adapter = new android.widget.ArrayAdapter<>(this,
                android.R.layout.simple_list_item_1, users);
        userListView.setAdapter(adapter);

        // 设置列表项点击事件
        userListView.setOnItemClickListener(new android.widget.AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(android.widget.AdapterView<?> parent, View view, int position, long id) {
                selectedUserId = userIds[position];
                addFriendButton.setEnabled(true);

                // 在地图上高亮显示该用户
                mapWebView.evaluateJavascript("highlightUser('" + selectedUserId + "');", null);
            }
        });
    }

    // 预留接口：获取同城用户
    public void getNearbyUsers(double latitude, double longitude, double radius) {
        // 预留接口，当前仅做本地模拟
        android.util.Log.d("MapActivity", "获取同城用户：" + latitude + "," + longitude + "半径：" + radius);

        // 模拟添加用户到地图
        mapWebView.evaluateJavascript("addMarker(39.9042, 116.4074, '张三', 'user1');", null);
        mapWebView.evaluateJavascript("addMarker(39.9142, 116.4174, '李四', 'user2');", null);
        mapWebView.evaluateJavascript("addMarker(39.8942, 116.3974, '王五', 'user3');", null);
    }

    // 预留接口：获取用户位置
    public void getUserLocation(String userId) {
        // 预留接口，当前仅做本地模拟
        android.util.Log.d("MapActivity", "获取用户位置：" + userId);

        // 模拟返回位置
        double latitude = 39.9042;
        double longitude = 116.4074;

        // 在地图上标记用户位置
        mapWebView.evaluateJavascript("addMarker(" + latitude + ", " + longitude + ", '用户位置', 'avatar.png');", null);
    }

    // 预留接口：发送位置
    public void sendLocation(String userId, double latitude, double longitude) {
        // 预留接口，当前仅做本地模拟
        android.util.Log.d("MapActivity", "发送位置给用户：" + userId + "位置：" + latitude + "," + longitude);

        android.widget.Toast.makeText(this, "位置已发送", android.widget.Toast.LENGTH_SHORT).show();
    }

    // 预留接口：更新位置
    public void updateLocation(double latitude, double longitude) {
        // 预留接口，当前仅做本地模拟
        android.util.Log.d("MapActivity", "更新位置：" + latitude + "," + longitude);

        // 在地图上更新自己的位置
        mapWebView.evaluateJavascript("updateMyLocation(" + latitude + ", " + longitude + ");", null);
    }

    // 预留接口：添加好友
    public void addFriend(String userId) {
        // 预留接口，当前仅做本地模拟
        android.util.Log.d("MapActivity", "添加好友：" + userId);

        android.widget.Toast.makeText(this, "好友请求已发送", android.widget.Toast.LENGTH_SHORT).show();
    }
}