#ifndef JNI_CORE_INTERFACE_H
#define JNI_CORE_INTERFACE_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL
Java_com_findfriend_core_CoreInterface_nativeInit(JNIEnv* env, jobject thiz, jstring host, jint port);

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeDestroy(JNIEnv* env, jobject thiz);

JNIEXPORT jstring JNICALL
Java_com_findfriend_core_CoreInterface_nativeGetVersion(JNIEnv* env, jobject thiz);

JNIEXPORT jboolean JNICALL
Java_com_findfriend_core_CoreInterface_nativeIsInitialized(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeLogin(JNIEnv* env, jobject thiz, jstring username, jstring password);

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeLogout(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeSendMessage(JNIEnv* env, jobject thiz, jint toId, jstring message);

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeReportLocation(JNIEnv* env, jobject thiz, jdouble latitude, jdouble longitude);

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeGetNearbyUsers(JNIEnv* env, jobject thiz, jdouble latitude, jdouble longitude, jdouble radius);

JNIEXPORT jstring JNICALL
Java_com_findfriend_core_CoreInterface_nativeGetModuleStatus(JNIEnv* env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_findfriend_core_CoreInterface_nativeDebugCommand(JNIEnv* env, jobject thiz, jstring command);

#ifdef __cplusplus
}
#endif

#endif // JNI_CORE_INTERFACE_H
