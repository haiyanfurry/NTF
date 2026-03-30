package com.yourname.findfriend;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import com.yourname.findfriend.util.SPUtil;
import com.yourname.findfriend.util.WebSocketClient;

public class MainActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle b) {
        super.onCreate(b);
        setContentView(R.layout.activity_main);
        SPUtil.init(this);
        WebSocketClient.get().connect("ws://server");

        findViewById(R.id.menu).setOnClickListener(v->{
            startActivity(new Intent(this,PersonalActivity.class));
        });

        Toast.makeText(this,"定位已开启（毛坯）",Toast.LENGTH_SHORT).show();
    }
}
