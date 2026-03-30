package com.yourname.findfriend.jni;

public class NativeUtil {
    static {
        System.loadLibrary("native-lib");
    }

    public static native double calcDistance(double lat1, double lon1, double lat2, double lon2);
    public static native String getCurrentLocation();
    public static native boolean isInRange(double userLat, double userLon, double friendLat, double friendLon, double rangeM);
}
