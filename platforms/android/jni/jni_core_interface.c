#include "jni_core_interface.h"
#include "core_interface.h"
#include <string.h>
#include <stdlib.h>

static JavaVM* g_jvm = NULL;
static jobject g_global_ref = NULL;

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
    g_jvm = vm;
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM* vm, void* reserved) {
    g_jvm = NULL;
    if (g_global_ref) {
        JNIEnv* env;
        if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6) == JNI_OK) {
            (*env)->DeleteGlobalRef(env, g_global_ref);
        }
        g_global_ref = NULL;
    }
}

JNIEXPORT jboolean JNICALL
Java_com_findfriend_core_CoreInterface_nativeInit(JNIEnv* env, jobject thiz, jstring host, jint port) {
    const char* host_str = host ? (*env)->GetStringUTFChars(env, host, NULL) : NULL;
    bool result = core_interface_init(host_str, port);
    if (host_str) {
        (*env)->ReleaseStringUTFChars(env, host, host_str);
    }
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeDestroy(JNIEnv* env, jobject thiz) {
    core_interface_destroy();
}

JNIEXPORT jstring JNICALL
Java_com_findfriend_core_CoreInterface_nativeGetVersion(JNIEnv* env, jobject thiz) {
    return (*env)->NewStringUTF(env, core_interface_get_version());
}

JNIEXPORT jboolean JNICALL
Java_com_findfriend_core_CoreInterface_nativeIsInitialized(JNIEnv* env, jobject thiz) {
    return core_interface_is_initialized() ? JNI_TRUE : JNI_FALSE;
}

static void login_callback(void* user_data, int status, const char* message) {
    if (g_jvm && g_global_ref) {
        JNIEnv* env;
        if ((*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6) == JNI_OK) {
            jclass cls = (*env)->GetObjectClass(env, g_global_ref);
            jmethodID mid = (*env)->GetMethodID(env, cls, "onLoginResult", "(ILjava/lang/String;)V");
            if (mid) {
                jstring j_msg = message ? (*env)->NewStringUTF(env, message) : NULL;
                (*env)->CallVoidMethod(env, g_global_ref, mid, status, j_msg);
                if (j_msg) (*env)->DeleteLocalRef(env, j_msg);
            }
        }
    }
}

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeLogin(JNIEnv* env, jobject thiz, jstring username, jstring password) {
    if (!g_global_ref) {
        g_global_ref = (*env)->NewGlobalRef(env, thiz);
    }

    const char* user_str = (*env)->GetStringUTFChars(env, username, NULL);
    const char* pass_str = (*env)->GetStringUTFChars(env, password, NULL);
    core_login(user_str, pass_str, login_callback, NULL);
    (*env)->ReleaseStringUTFChars(env, username, user_str);
    (*env)->ReleaseStringUTFChars(env, password, pass_str);
}

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeLogout(JNIEnv* env, jobject thiz) {
    core_logout();
}

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeSendMessage(JNIEnv* env, jobject thiz, jint toId, jstring message) {
    const char* msg_str = (*env)->GetStringUTFChars(env, message, NULL);
    core_send_message((uint32_t)toId, msg_str, NULL, NULL);
    (*env)->ReleaseStringUTFChars(env, message, msg_str);
}

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeReportLocation(JNIEnv* env, jobject thiz, jdouble latitude, jdouble longitude) {
    core_report_location(latitude, longitude, NULL, NULL);
}

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeGetNearbyUsers(JNIEnv* env, jobject thiz, jdouble latitude, jdouble longitude, jdouble radius) {
    core_get_nearby_users(latitude, longitude, radius, NULL, NULL);
}

JNIEXPORT jstring JNICALL
Java_com_findfriend_core_CoreInterface_nativeGetModuleStatus(JNIEnv* env, jobject thiz) {
    return (*env)->NewStringUTF(env, core_debug_get_module_status());
}

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeDebugCommand(JNIEnv* env, jobject thiz, jstring command) {
    const char* cmd_str = (*env)->GetStringUTFChars(env, command, NULL);
    core_debug_command(cmd_str, NULL, NULL);
    (*env)->ReleaseStringUTFChars(env, command, cmd_str);
}
