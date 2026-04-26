#ifdef _WIN32

#include <windows.h>
#include <stdio.h>
#include "core_interface.h"

// 窗口类名
#define WINDOW_CLASS L"FindFriendDebugWindow"
#define WINDOW_TITLE L"FindFriend 调试控制台"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// 控件ID
#define ID_LOGIN_BUTTON 1001
#define ID_USER_EDIT 1002
#define ID_PASS_EDIT 1003
#define ID_STATUS_TEXT 1004
#define ID_SEND_BUTTON 1005
#define ID_MESSAGE_EDIT 1006
#define ID_LOCATION_BUTTON 1007
#define ID_LAT_EDIT 1008
#define ID_LON_EDIT 1009
#define ID_DEBUG_BUTTON 1010
#define ID_LOG_EDIT 1011
#define ID_EXIT_BUTTON 1012

// 全局变量
HWND hMainWnd;
HWND hUserEdit;
HWND hPassEdit;
HWND hStatusText;
HWND hSendButton;
HWND hMessageEdit;
HWND hLocationButton;
HWND hLatEdit;
HWND hLonEdit;
HWND hDebugButton;
HWND hLogEdit;
HWND hExitButton;

// 日志函数
void append_log_text(const char* text) {
    if (hLogEdit && text) {
        int len = GetWindowTextLengthA(hLogEdit);
        SendMessageA(hLogEdit, EM_SETSEL, len, len);
        SendMessageA(hLogEdit, EM_REPLACESEL, FALSE, (LPARAM)text);
        SendMessageA(hLogEdit, EM_REPLACESEL, FALSE, (LPARAM)"\r\n");
    }
}

// 更新状态
void update_status(const char* status) {
    if (hStatusText && status) {
        SetWindowTextA(hStatusText, status);
    }
}

// 登录回调
void login_callback(void* user_data, int status, const char* message) {
    char buf[256];
    snprintf(buf, sizeof(buf), "登录结果: %d - %s", status, message ? message : "(null)");
    update_status(buf);
    append_log_text(buf);
}

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // 创建控件
            int y = 10;
            
            // 用户登录
            CreateWindowW(L"STATIC", L"用户名:", WS_CHILD | WS_VISIBLE, 10, y, 80, 20, hwnd, NULL, NULL, NULL);
            hUserEdit = CreateWindowW(L"EDIT", L"test", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, y, 200, 20, hwnd, (HMENU)ID_USER_EDIT, NULL, NULL);
            
            y += 30;
            CreateWindowW(L"STATIC", L"密码:", WS_CHILD | WS_VISIBLE, 10, y, 80, 20, hwnd, NULL, NULL, NULL);
            hPassEdit = CreateWindowW(L"EDIT", L"123456", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD, 100, y, 200, 20, hwnd, (HMENU)ID_PASS_EDIT, NULL, NULL);
            
            y += 30;
            hLoginButton = CreateWindowW(L"BUTTON", L"登录", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, y, 100, 30, hwnd, (HMENU)ID_LOGIN_BUTTON, NULL, NULL);
            
            y += 40;
            // 消息发送
            CreateWindowW(L"STATIC", L"发送消息:", WS_CHILD | WS_VISIBLE, 10, y, 100, 20, hwnd, NULL, NULL, NULL);
            hMessageEdit = CreateWindowW(L"EDIT", L"Hello, World!", WS_CHILD | WS_VISIBLE | WS_BORDER, 120, y, 300, 20, hwnd, (HMENU)ID_MESSAGE_EDIT, NULL, NULL);
            hSendButton = CreateWindowW(L"BUTTON", L"发送", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 430, y, 80, 30, hwnd, (HMENU)ID_SEND_BUTTON, NULL, NULL);
            
            y += 40;
            // 位置上报
            CreateWindowW(L"STATIC", L"纬度:", WS_CHILD | WS_VISIBLE, 10, y, 80, 20, hwnd, NULL, NULL, NULL);
            hLatEdit = CreateWindowW(L"EDIT", L"39.9042", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, y, 150, 20, hwnd, (HMENU)ID_LAT_EDIT, NULL, NULL);
            
            CreateWindowW(L"STATIC", L"经度:", WS_CHILD | WS_VISIBLE, 270, y, 80, 20, hwnd, NULL, NULL, NULL);
            hLonEdit = CreateWindowW(L"EDIT", L"116.4074", WS_CHILD | WS_VISIBLE | WS_BORDER, 360, y, 150, 20, hwnd, (HMENU)ID_LON_EDIT, NULL, NULL);
            
            y += 30;
            hLocationButton = CreateWindowW(L"BUTTON", L"上报位置", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, y, 120, 30, hwnd, (HMENU)ID_LOCATION_BUTTON, NULL, NULL);
            hDebugButton = CreateWindowW(L"BUTTON", L"调试命令", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 140, y, 120, 30, hwnd, (HMENU)ID_DEBUG_BUTTON, NULL, NULL);
            hExitButton = CreateWindowW(L"BUTTON", L"退出", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 270, y, 100, 30, hwnd, (HMENU)ID_EXIT_BUTTON, NULL, NULL);
            
            y += 50;
            CreateWindowW(L"STATIC", L"状态:", WS_CHILD | WS_VISIBLE, 10, y, 80, 20, hwnd, NULL, NULL, NULL);
            hStatusText = CreateWindowW(L"STATIC", L"就绪", WS_CHILD | WS_VISIBLE, 100, y, 500, 20, hwnd, (HMENU)ID_STATUS_TEXT, NULL, NULL);
            
            y += 30;
            CreateWindowW(L"STATIC", L"日志:", WS_CHILD | WS_VISIBLE, 10, y, 80, 20, hwnd, NULL, NULL, NULL);
            hLogEdit = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, 10, y + 20, WINDOW_WIDTH - 30, WINDOW_HEIGHT - y - 50, hwnd, (HMENU)ID_LOG_EDIT, NULL, NULL);
            
            // 初始化核心接口
            core_set_hwnd(hwnd);
            if (core_interface_init("localhost", 8080)) {
                append_log_text("核心接口初始化成功");
                update_status("核心接口已就绪");
            } else {
                append_log_text("核心接口初始化失败");
                update_status("核心接口初始化失败");
            }
            break;
        }
        
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case ID_LOGIN_BUTTON: {
                    char user[256], pass[256];
                    GetWindowTextA(hUserEdit, user, sizeof(user));
                    GetWindowTextA(hPassEdit, pass, sizeof(pass));
                    char log[512];
                    snprintf(log, sizeof(log), "尝试登录: %s", user);
                    append_log_text(log);
                    update_status("正在登录...");
                    core_login(user, pass, login_callback, NULL);
                    break;
                }
                
                case ID_SEND_BUTTON: {
                    char msg[512];
                    GetWindowTextA(hMessageEdit, msg, sizeof(msg));
                    char log[512];
                    snprintf(log, sizeof(log), "发送消息: %s", msg);
                    append_log_text(log);
                    core_send_message(1, msg, NULL, NULL);
                    update_status("消息已发送");
                    break;
                }
                
                case ID_LOCATION_BUTTON: {
                    char lat_str[32], lon_str[32];
                    GetWindowTextA(hLatEdit, lat_str, sizeof(lat_str));
                    GetWindowTextA(hLonEdit, lon_str, sizeof(lon_str));
                    double lat = atof(lat_str);
                    double lon = atof(lon_str);
                    char log[512];
                    snprintf(log, sizeof(log), "上报位置: %.6f, %.6f", lat, lon);
                    append_log_text(log);
                    core_report_location(lat, lon, NULL, NULL);
                    update_status("位置已上报");
                    break;
                }
                
                case ID_DEBUG_BUTTON: {
                    append_log_text("执行调试命令: status");
                    const char* status = core_debug_get_module_status();
                    append_log_text(status);
                    break;
                }
                
                case ID_EXIT_BUTTON:
                    PostQuitMessage(0);
                    break;
            }
            break;
        }
        
        case WM_DESTROY:
            append_log_text("清理核心接口...");
            core_interface_destroy();
            PostQuitMessage(0);
            break;
        
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Windows 入口函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 注册窗口类
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    if (!RegisterClass(&wc)) {
        MessageBoxA(NULL, "无法注册窗口类", "错误", MB_ICONERROR);
        return 1;
    }
    
    // 创建窗口
    hMainWnd = CreateWindowEx(
        0,
        WINDOW_CLASS,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );
    
    if (!hMainWnd) {
        MessageBoxA(NULL, "无法创建窗口", "错误", MB_ICONERROR);
        return 1;
    }
    
    // 设置实例句柄
    core_set_instance(hInstance);
    
    // 显示窗口
    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);
    
    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}

#endif // _WIN32
