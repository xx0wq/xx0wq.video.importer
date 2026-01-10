#include <jni.h>
#include <android/log.h>
#include <string>

#define LOG_TAG "VideoImporter"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Example native function: import a video path and return metadata
extern "C" JNIEXPORT jstring JNICALL
Java_com_xx0wq_videoimporter_NativeBridge_importVideo(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    LOGI("Importing video from path: %s", cpath);

    std::string metadata = "VideoImporter: successfully imported file at ";
    metadata += cpath;

    env->ReleaseStringUTFChars(path, cpath);
    return env->NewStringUTF(metadata.c_str());
}

// Example native function: return dummy duration
extern "C" JNIEXPORT jint JNICALL
Java_com_xx0wq_videoimporter_NativeBridge_getDuration(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    LOGI("Getting duration for video: %s", cpath);

    int durationSeconds = 120; // placeholder
    env->ReleaseStringUTFChars(path, cpath);
    return durationSeconds;
}

// Example native function: pretend to convert video
extern "C" JNIEXPORT jboolean JNICALL
Java_com_xx0wq_videoimporter_NativeBridge_convertVideo(JNIEnv* env, jobject thiz,
                                                       jstring inputPath, jstring outputPath) {
    const char* inPath = env->GetStringUTFChars(inputPath, nullptr);
    const char* outPath = env->GetStringUTFChars(outputPath, nullptr);

    LOGI("Converting video from %s to %s", inPath, outPath);

    bool success = true; // stubbed conversion
    env->ReleaseStringUTFChars(inputPath, inPath);
    env->ReleaseStringUTFChars(outputPath, outPath);

    return success ? JNI_TRUE : JNI_FALSE;
}
