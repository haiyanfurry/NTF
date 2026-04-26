package com.findfriend.core;

public class CoreInterface {
    
    static {
        System.loadLibrary("FindFriendCore");
        System.loadLibrary("FindFriendJNI");
    }
    
    private native boolean nativeInit(String host, int port);
    private native void nativeDestroy();
    private native String nativeGetVersion();
    private native boolean nativeIsInitialized();
    private native void nativeLogin(String username, String password);
    private native void nativeLogout();
    private native void nativeSendMessage(int toId, String message);
    private native void nativeReportLocation(double latitude, double longitude);
    private native void nativeGetNearbyUsers(double latitude, double longitude, double radius);
    private native String nativeGetModuleStatus();
    private native void nativeDebugCommand(String command);
    
    private OnCoreListener listener;
    
    public interface OnCoreListener {
        void onLoginResult(int status, String message);
    }
    
    public void setOnCoreListener(OnCoreListener listener) {
        this.listener = listener;
    }
    
    public boolean init(String host, int port) {
        return nativeInit(host, port);
    }
    
    public void destroy() {
        nativeDestroy();
    }
    
    public String getVersion() {
        return nativeGetVersion();
    }
    
    public boolean isInitialized() {
        return nativeIsInitialized();
    }
    
    public void login(String username, String password) {
        nativeLogin(username, password);
    }
    
    public void logout() {
        nativeLogout();
    }
    
    public void sendMessage(int toId, String message) {
        nativeSendMessage(toId, message);
    }
    
    public void reportLocation(double latitude, double longitude) {
        nativeReportLocation(latitude, longitude);
    }
    
    public void getNearbyUsers(double latitude, double longitude, double radius) {
        nativeGetNearbyUsers(latitude, longitude, radius);
    }
    
    public String getModuleStatus() {
        return nativeGetModuleStatus();
    }
    
    public void debugCommand(String command) {
        nativeDebugCommand(command);
    }
    
    private void onLoginResult(int status, String message) {
        if (listener != null) {
            listener.onLoginResult(status, message);
        }
    }
}
