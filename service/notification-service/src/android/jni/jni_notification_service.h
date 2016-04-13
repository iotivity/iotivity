#include <stdio.h>
#include <string.h>

#include <jni.h>
#include <jni_string.h>

#include "JniOcStack.h"

#ifdef __cplusplus
extern "C++" {
#endif

    JNIEXPORT void JNICALL JNIDiscover(JNIEnv *env, jobject thisObj);

    JNIEXPORT void JNICALL JNIStartNotificationProducer(JNIEnv *env, jobject thisObj, jstring notifyDeviceName);

    JNIEXPORT jint JNICALL  JNISendNotification(JNIEnv *env, jobject thisObj, jstring URL, jstring Message, jstring Sender, jint MessageType);

    JNIEXPORT void JNICALL JNIStartSubscribeNotifications(JNIEnv *env, jobject thisObj, jint resourceIndex);

    JNIEXPORT void JNICALL  JNIStopSubscribeNotifications(JNIEnv *env, jobject thisObj);

#ifdef __cplusplus
}
#endif

