package com.yourname.findfriend.model;

public class ChatMessage {
    public static final int SEND=1;
    public static final int RECEIVE=2;

    public String content;
    public int type;
    public long time;

    public ChatMessage(String c,int t) {
        content=c;
        type=t;
        time=System.currentTimeMillis();
    }
}
