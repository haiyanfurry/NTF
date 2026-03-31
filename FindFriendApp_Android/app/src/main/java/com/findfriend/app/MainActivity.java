package com.findfriend.app;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "FindFriendApp";

    // 加载C++库
    static {
        System.loadLibrary("findfriendapp");
    }

    // 声明native方法
    public native String stringFromJNI();
    public native int initApp();
    public native int login(String username, String password);
    public native int startLocalDiscovery();
    public native void stopLocalDiscovery();
    public native int setServerIP(String ip);

    private EditText mUsernameEdit;
    private EditText mPasswordEdit;
    private Button mLoginButton;
    private TextView mResultText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // 初始化UI组件
        mUsernameEdit = findViewById(R.id.username_edit);
        mPasswordEdit = findViewById(R.id.password_edit);
        mLoginButton = findViewById(R.id.login_button);
        mResultText = findViewById(R.id.result_text);

        // 初始化应用
        int initResult = initApp();
        Log.d(TAG, "Init result: " + initResult);

        // 设置登录按钮点击事件
        mLoginButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String username = mUsernameEdit.getText().toString();
                String password = mPasswordEdit.getText().toString();
                int loginResult = login(username, password);
                mResultText.setText("Login result: " + loginResult);
            }
        });

        // 测试C++调用
        String message = stringFromJNI();
        Log.d(TAG, "Message from C++: " + message);
    }

    @Override
    protected void onStart() {
        super.onStart();
        // 开始同城用户发现
        int discoveryResult = startLocalDiscovery();
        Log.d(TAG, "Start discovery result: " + discoveryResult);
    }

    @Override
    protected void onStop() {
        super.onStop();
        // 停止同城用户发现
        stopLocalDiscovery();
        Log.d(TAG, "Stop discovery");
    }
}
