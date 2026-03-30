package com.yourname.findfriend;

import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import com.yourname.findfriend.util.WebSocketClient;

public class ChatActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle b) {
        super.onCreate(b);
        setContentView(R.layout.activity_chat);
        EditText input=findViewById(R.id.input);

        findViewById(R.id.send).setOnClickListener(v->{
            String t=input.getText().toString();
            WebSocketClient.get().send(t);
            Toast.makeText(this,"已发送",Toast.LENGTH_SHORT).show();
        });
    }
}
