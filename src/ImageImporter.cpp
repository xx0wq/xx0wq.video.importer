#include <jni.h>
#include <android/log.h>

#define LOG_TAG "VideoImporter"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT void JNICALL
Java_com_xx0wq_videoimporter_NativeBridge_dummy(JNIEnv* env, jobject thiz) {
    LOGI("VideoImporter dummy function called");
}
