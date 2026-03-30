package com.yourname.findfriend.util;

import android.content.Context;
import android.content.SharedPreferences;

public class SPUtil {
    private static SharedPreferences sp;

    public static void init(Context c) {
        if(sp==null) sp=c.getSharedPreferences("friend",0);
    }

    public static void put(String k,boolean v) {
        sp.edit().putBoolean(k,v).apply();
    }

    public static boolean get(String k,boolean def) {
        return sp.getBoolean(k,def);
    }
}
