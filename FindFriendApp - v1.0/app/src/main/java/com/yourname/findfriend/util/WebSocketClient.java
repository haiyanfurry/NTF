package com.yourname.findfriend.util;

import android.util.Log;

public class WebSocketClient {
    private static WebSocketClient ins;
    private MsgListener listener;

    private WebSocketClient() {}
    public static WebSocketClient get() {
        if(ins==null) ins=new WebSocketClient();
        return ins;
    }

    public void connect(String url) { Log.d("WS","connect"); }
    public void send(String msg) { Log.d("WS","send:"+msg); }
    public void setListener(MsgListener l) { listener=l; }

    public interface MsgListener {
        void onMsg(String s);
    }
}
