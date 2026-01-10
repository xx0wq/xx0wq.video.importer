#include <jni.h>
#include <string>
#include <android/log.h>

// Simple logging macro for Android
#define LOG_TAG "VideoImporter"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Example: pretend we are importing a video file and returning metadata
extern "C" JNIEXPORT jstring JNICALL
Java_com_xx0wq_videoimporter_NativeBridge_importVideo(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);

    // Log the path we received
    LOGI("Importing video from path: %s", cpath);

    // Here you would normally call FFmpeg APIs to probe the file.
    // For demonstration, we just return a fake metadata string.
    std::string metadata = "VideoImporter: successfully imported file at ";
    metadata += cpath;

    env->ReleaseStringUTFChars(path, cpath);
    return env->NewStringUTF(metadata.c_str());
}

// Example: return dummy video duration
extern "C" JNIEXPORT jint JNICALL
Java_com_xx0wq_videoimporter_NativeBridge_getDuration(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, nullptr);

    LOGI("Getting duration for video: %s", cpath);

    // Normally you’d use FFmpeg to read duration.
    // Here we just return a placeholder value.
    int durationSeconds = 120; // 2 minutes

    env->ReleaseStringUTFChars(path, cpath);
    return durationSeconds;
}

// Example: convert video to another format (stub)
extern "C" JNIEXPORT jboolean JNICALL
Java_com_xx0wq_videoimporter_NativeBridge_convertVideo(JNIEnv* env, jobject thiz,
                                                       jstring inputPath, jstring outputPath) {
    const char* inPath = env->GetStringUTFChars(inputPath, nullptr);
    const char* outPath = env->GetStringUTFChars(outputPath, nullptr);

    LOGI("Converting video from %s to %s", inPath, outPath);

    // Stub: pretend conversion succeeded
    bool success = true;

    env->ReleaseStringUTFChars(inputPath, inPath);
    env->ReleaseStringUTFChars(outputPath, outPath);

    return success ? JNI_TRUE : JNI_FALSE;
}
