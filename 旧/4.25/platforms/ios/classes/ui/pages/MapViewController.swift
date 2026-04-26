import UIKit
import WebKit

class MapViewController: UIViewController {

    private var mapWebView: WKWebView!
    private var userTableView: UITableView!
    private var addFriendButton: UIButton!
    private var selectedUserId: String?
    
    // 模拟用户数据
    private let users = [
        (id: "user1", name: "张三", distance: "100m"),
        (id: "user2", name: "李四", distance: "200m"),
        (id: "user3", name: "王五", distance: "300m"),
        (id: "user4", name: "赵六", distance: "400m"),
        (id: "user5", name: "钱七", distance: "500m")
    ]

    override func viewDidLoad() {
        super.viewDidLoad()
        setupUI()
        loadMapHTML()
    }

    private func setupUI() {
        view.backgroundColor = .white
        title = "同城地图"

        // 创建水平布局
        let horizontalStackView = UIStackView()
        horizontalStackView.axis = .horizontal
        horizontalStackView.distribution = .fill
        horizontalStackView.alignment = .fill
        horizontalStackView.spacing = 0
        
        // 创建地图WebView
        let webViewConfiguration = WKWebViewConfiguration()
        mapWebView = WKWebView(frame: .zero, configuration: webViewConfiguration)
        mapWebView.translatesAutoresizingMaskIntoConstraints = false
        horizontalStackView.addArrangedSubview(mapWebView)
        mapWebView.widthAnchor.constraint(equalTo: horizontalStackView.widthAnchor, multiplier: 2.0/3.0).isActive = true

        // 创建用户列表
        userTableView = UITableView()
        userTableView.dataSource = self
        userTableView.delegate = self
        userTableView.translatesAutoresizingMaskIntoConstraints = false
        horizontalStackView.addArrangedSubview(userTableView)
        userTableView.widthAnchor.constraint(equalTo: horizontalStackView.widthAnchor, multiplier: 1.0/3.0).isActive = true

        // 创建添加好友按钮
        addFriendButton = UIButton(type: .system)
        addFriendButton.setTitle("添加好友", for: .normal)
        addFriendButton.backgroundColor = .systemGreen
        addFriendButton.setTitleColor(.white, for: .normal)
        addFriendButton.isEnabled = false
        addFriendButton.addTarget(self, action: #selector(addFriendTapped), for: .touchUpInside)
        addFriendButton.translatesAutoresizingMaskIntoConstraints = false

        // 添加到主视图
        view.addSubview(horizontalStackView)
        view.addSubview(addFriendButton)

        // 设置约束
        horizontalStackView.translatesAutoresizingMaskIntoConstraints = false
        NSLayoutConstraint.activate([
            horizontalStackView.topAnchor.constraint(equalTo: view.safeAreaLayoutGuide.topAnchor),
            horizontalStackView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            horizontalStackView.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            horizontalStackView.bottomAnchor.constraint(equalTo: addFriendButton.topAnchor),
            
            addFriendButton.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            addFriendButton.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            addFriendButton.bottomAnchor.constraint(equalTo: view.safeAreaLayoutGuide.bottomAnchor),
            addFriendButton.heightAnchor.constraint(equalToConstant: 50)
        ])
    }

    private func loadMapHTML() {
        let html = """<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>同城地图</title>
<style>
body { margin: 0; padding: 0; width: 100%; height: 100%; }
#map { width: 100%; height: 100%; background-color: #f0f0f0; position: relative; }
.user-marker {
    position: absolute;
    width: 40px;
    height: 40px;
    border-radius: 50%;
    background-color: #4CAF50;
    color: white;
    display: flex;
    align-items: center;
    justify-content: center;
    cursor: pointer;
    box-shadow: 0 2px 5px rgba(0,0,0,0.3);
    transition: all 0.3s ease;
}
.user-marker:hover {
    transform: scale(1.1);
    box-shadow: 0 4px 10px rgba(0,0,0,0.5);
}
.user-info {
    position: absolute;
    background-color: white;
    padding: 10px;
    border-radius: 5px;
    box-shadow: 0 2px 10px rgba(0,0,0,0.2);
    display: none;
    z-index: 1000;
}
.map-control {
    position: absolute;
    top: 10px;
    left: 10px;
    background-color: white;
    padding: 10px;
    border-radius: 5px;
    box-shadow: 0 2px 5px rgba(0,0,0,0.2);
}
</style>
</head>
<body>
<div id="map">
    <div class="map-control">
        <h3>同城地图</h3>
        <p>显示附近的用户</p>
    </div>
</div>
<script>
// 地图中心点
var center = {lat: 39.9042, lng: 116.4074};
var users = {};

// 初始化地图
function initMap() {
    // 简单的地图背景
    var map = document.getElementById('map');
    map.style.backgroundImage = 'linear-gradient(to bottom, #e3f2fd, #bbdefb)';
    
    // 添加自己的位置
    addMarker(center.lat, center.lng, '我的位置', 'me');
}

// 添加标记
function addMarker(lat, lng, title, userId) {
    var map = document.getElementById('map');
    var marker = document.createElement('div');
    marker.className = 'user-marker';
    marker.id = 'marker_' + userId;
    marker.title = title;
    
    // 简单的坐标转换
    var x = (lng - (center.lng - 0.1)) * 5000;
    var y = (center.lat - 0.1 - lat) * 5000;
    
    marker.style.left = x + 'px';
    marker.style.top = y + 'px';
    marker.innerHTML = userId === 'me' ? '我' : 'U';
    
    // 点击事件
    marker.onclick = function() {
        showUserInfo(x, y, title, userId);
    };
    
    map.appendChild(marker);
    users[userId] = {lat: lat, lng: lng, element: marker};
}

// 显示用户信息
function showUserInfo(x, y, title, userId) {
    // 移除之前的信息框
    var oldInfo = document.getElementById('userInfo');
    if (oldInfo) {
        oldInfo.remove();
    }
    
    var map = document.getElementById('map');
    var info = document.createElement('div');
    info.id = 'userInfo';
    info.className = 'user-info';
    info.style.left = (x + 50) + 'px';
    info.style.top = y + 'px';
    info.innerHTML = '<strong>' + title + '</strong><br>ID: ' + userId + '<br><button onclick="addFriend(\'' + userId + '\')">添加好友</button>';
    
    map.appendChild(info);
    
    // 点击其他地方关闭信息框
    setTimeout(function() {
        document.addEventListener('click', function closeInfo(e) {
            if (e.target !== info && !info.contains(e.target)) {
                info.remove();
                document.removeEventListener('click', closeInfo);
            }
        });
    }, 100);
}

// 高亮用户
function highlightUser(userId) {
    // 重置所有标记
    for (var id in users) {
        if (users.hasOwnProperty(id)) {
            users[id].element.style.backgroundColor = '#4CAF50';
        }
    }
    
    // 高亮选中的用户
    if (users[userId]) {
        users[userId].element.style.backgroundColor = '#FF9800';
        users[userId].element.style.transform = 'scale(1.2)';
    }
}

// 更新自己的位置
function updateMyLocation(lat, lng) {
    var marker = document.getElementById('marker_me');
    if (marker) {
        var x = (lng - (center.lng - 0.1)) * 5000;
        var y = (center.lat - 0.1 - lat) * 5000;
        marker.style.left = x + 'px';
        marker.style.top = y + 'px';
    }
}

// 添加好友
function addFriend(userId) {
    alert('发送好友请求给: ' + userId);
}

// 初始化地图
initMap();
</script>
</body>
</html>"""
        
        mapWebView.loadHTMLString(html, baseURL: nil)
    }

    @objc private func addFriendTapped() {
        if let userId = selectedUserId {
            addFriend(userId)
        }
    }

    // 预留接口：获取同城用户
    func getNearbyUsers(latitude: Double, longitude: Double, radius: Double) {
        // 预留接口，当前仅做本地模拟
        print("获取同城用户：\(latitude),\(longitude) 半径：\(radius)")
        
        // 模拟添加用户到地图
        mapWebView.evaluateJavaScript("addMarker(39.9042, 116.4074, '张三', 'user1');")
        mapWebView.evaluateJavaScript("addMarker(39.9142, 116.4174, '李四', 'user2');")
        mapWebView.evaluateJavaScript("addMarker(39.8942, 116.3974, '王五', 'user3');")
    }
    
    // 预留接口：获取用户位置
    func getUserLocation(userId: String) {
        // 预留接口，当前仅做本地模拟
        print("获取用户位置：\(userId)")
        
        // 模拟返回位置
        let latitude = 39.9042
        let longitude = 116.4074
        
        // 在地图上标记用户位置
        mapWebView.evaluateJavaScript("addMarker(\(latitude), \(longitude), '用户位置', 'avatar.png');")
    }
    
    // 预留接口：发送位置
    func sendLocation(userId: String, latitude: Double, longitude: Double) {
        // 预留接口，当前仅做本地模拟
        print("发送位置给用户：\(userId) 位置：\(latitude),\(longitude)")
        
        let alert = UIAlertController(title: "提示", message: "位置已发送", preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "确定", style: .default))
        present(alert, animated: true)
    }
    
    // 预留接口：更新位置
    func updateLocation(latitude: Double, longitude: Double) {
        // 预留接口，当前仅做本地模拟
        print("更新位置：\(latitude),\(longitude)")
        
        // 在地图上更新自己的位置
        mapWebView.evaluateJavaScript("updateMyLocation(\(latitude), \(longitude));")
    }
    
    // 预留接口：添加好友
    func addFriend(_ userId: String) {
        // 预留接口，当前仅做本地模拟
        print("添加好友：\(userId)")
        
        let alert = UIAlertController(title: "提示", message: "好友请求已发送", preferredStyle: .alert)
        alert.addAction(UIAlertAction(title: "确定", style: .default))
        present(alert, animated: true)
    }
}

extension MapViewController: UITableViewDataSource, UITableViewDelegate {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return users.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "UserCell") ?? UITableViewCell(style: .default, reuseIdentifier: "UserCell")
        let user = users[indexPath.row]
        cell.textLabel?.text = "\(user.name) (\(user.distance))"
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        let user = users[indexPath.row]
        selectedUserId = user.id
        addFriendButton.isEnabled = true
        
        // 在地图上高亮显示该用户
        mapWebView.evaluateJavaScript("highlightUser('\(user.id)');")
    }
}