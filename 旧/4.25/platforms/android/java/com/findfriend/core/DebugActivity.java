package com.findfriend.core;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;

/**
 * FindFriend 调试控制台 - Android 主界面
 */
public class DebugActivity extends Activity {
    
    // UI 组件
    private EditText mUserEdit;
    private EditText mPassEdit;
    private Button mLoginButton;
    private Button mLogoutButton;
    private EditText mMessageEdit;
    private Button mSendButton;
    private EditText mLatEdit;
    private EditText mLonEdit;
    private Button mReportLocationButton;
    private Button mDebugButton;
    private Button mClearLogsButton;
    private TextView mLogTextView;
    private ScrollView mLogScrollView;
    private TextView mStatusTextView;
    
    // 核心接口
    private CoreInterface mCoreInterface;
    
    // 线程处理
    private Handler mHandler = new Handler(Looper.getMainLooper());
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_debug);
        
        // 初始化核心接口
        mCoreInterface = new CoreInterface();
        
        // 绑定 UI 组件
        initViews();
        
        // 设置监听器
        setupListeners();
        
        // 初始化核心接口
        appendLog("正在初始化核心接口...");
        new Thread(() -> {
            boolean success = mCoreInterface.init("localhost", 8080);
            mHandler.post(() -> {
                if (success) {
                    appendLog("核心接口初始化成功");
                    updateStatus("已就绪");
                } else {
                    appendLog("核心接口初始化失败");
                    updateStatus("初始化失败");
                }
            });
        }).start();
    }
    
    private void initViews() {
        mUserEdit = findViewById(R.id.et_username);
        mPassEdit = findViewById(R.id.et_password);
        mLoginButton = findViewById(R.id.btn_login);
        mLogoutButton = findViewById(R.id.btn_logout);
        mMessageEdit = findViewById(R.id.et_message);
        mSendButton = findViewById(R.id.btn_send);
        mLatEdit = findViewById(R.id.et_latitude);
        mLonEdit = findViewById(R.id.et_longitude);
        mReportLocationButton = findViewById(R.id.btn_report_location);
        mDebugButton = findViewById(R.id.btn_debug);
        mClearLogsButton = findViewById(R.id.btn_clear_logs);
        mLogTextView = findViewById(R.id.tv_logs);
        mLogScrollView = findViewById(R.id.sv_logs);
        mStatusTextView = findViewById(R.id.tv_status);
        
        // 设置默认值
        mUserEdit.setText("test_user");
        mPassEdit.setText("123456");
        mLatEdit.setText("39.9042");
        mLonEdit.setText("116.4074");
        mMessageEdit.setText("Hello from Android!");
    }
    
    private void setupListeners() {
        mCoreInterface.setOnCoreListener(new CoreInterface.OnCoreListener() {
            @Override
            public void onLoginResult(int status, String message) {
                String logText = String.format("登录结果: %d - %s", status, message);
                appendLog(logText);
                updateStatus(status == 0 ? "已登录" : "登录失败");
            }
        });
        
        mLoginButton.setOnClickListener(v -> handleLogin());
        mLogoutButton.setOnClickListener(v -> handleLogout());
        mSendButton.setOnClickListener(v -> handleSendMessage());
        mReportLocationButton.setOnClickListener(v -> handleReportLocation());
        mDebugButton.setOnClickListener(v -> handleDebugCommand());
        mClearLogsButton.setOnClickListener(v -> clearLogs());
    }
    
    private void handleLogin() {
        String user = mUserEdit.getText().toString();
        String pass = mPassEdit.getText().toString();
        appendLog(String.format("尝试登录: %s", user));
        updateStatus("正在登录...");
        
        new Thread(() -> {
            mCoreInterface.login(user, pass);
        }).start();
    }
    
    private void handleLogout() {
        appendLog("正在登出...");
        updateStatus("正在登出...");
        
        new Thread(() -> {
            mCoreInterface.logout();
            mHandler.post(() -> {
                appendLog("已登出");
                updateStatus("未登录");
            });
        }).start();
    }
    
    private void handleSendMessage() {
        String message = mMessageEdit.getText().toString();
        appendLog(String.format("发送消息: %s", message));
        updateStatus("发送中...");
        
        new Thread(() -> {
            mCoreInterface.sendMessage(1, message);
            mHandler.post(() -> {
                updateStatus("消息已发送");
            });
        }).start();
    }
    
    private void handleReportLocation() {
        try {
            double lat = Double.parseDouble(mLatEdit.getText().toString());
            double lon = Double.parseDouble(mLonEdit.getText().toString());
            appendLog(String.format("上报位置: %.6f, %.6f", lat, lon));
            updateStatus("上报中...");
            
            new Thread(() -> {
                mCoreInterface.reportLocation(lat, lon);
                mHandler.post(() -> {
                    updateStatus("位置已上报");
                });
            }).start();
        } catch (NumberFormatException e) {
            appendLog("错误: 无效的经纬度");
        }
    }
    
    private void handleDebugCommand() {
        appendLog("执行调试命令: status");
        
        new Thread(() -> {
            String status = mCoreInterface.getModuleStatus();
            mHandler.post(() -> {
                appendLog(status);
            });
        }).start();
    }
    
    private void appendLog(String text) {
        mHandler.post(() -> {
            String currentText = mLogTextView.getText().toString();
            String newText = currentText + (currentText.isEmpty() ? "" : "\n") + text;
            mLogTextView.setText(newText);
            mLogScrollView.post(() -> mLogScrollView.fullScroll(View.FOCUS_DOWN));
        });
    }
    
    private void updateStatus(String status) {
        mHandler.post(() -> mStatusTextView.setText(status));
    }
    
    private void clearLogs() {
        mLogTextView.setText("");
        appendLog("日志已清空");
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        appendLog("正在清理核心接口...");
        new Thread(() -> {
            mCoreInterface.destroy();
        }).start();
    }
}
