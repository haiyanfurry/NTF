#include <jni.h>
#include <string>
#include <cmath>
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,"NATIVE",__VA_ARGS__)
const double EARTH = 6371000.0;

static double rad(double d) { return d * M_PI / 180.0; }

extern "C" JNIEXPORT jdouble JNICALL
Java_com_yourname_findfriend_jni_NativeUtil_calcDistance(
        JNIEnv* env, jclass,
        jdouble lat1,jdouble lon1,jdouble lat2,jdouble lon2
) {
    double a = rad(lat1)-rad(lat2);
    double b = rad(lon1)-rad(lon2);
    double s = 2 * asin(sqrt(pow(sin(a/2),2)
        + cos(rad(lat1))*cos(rad(lat2))*pow(sin(b/2),2)));
    return s * EARTH;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_yourname_findfriend_jni_NativeUtil_getCurrentLocation(
        JNIEnv* env,jclass
) {
    return env->NewStringUTF("30.5928,104.0668");
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_yourname_findfriend_jni_NativeUtil_isInRange(
        JNIEnv* env,jclass,
        jdouble uLat,jdouble uLon,jdouble fLat,jdouble fLon,jdouble range
) {
    double d = Java_com_yourname_findfriend_jni_NativeUtil_calcDistance(env,nullptr,uLat,uLon,fLat,fLon);
    return d <= range;
}
