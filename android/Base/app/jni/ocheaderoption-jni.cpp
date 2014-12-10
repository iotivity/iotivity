#include "ocstack-jni.h"

static const char* TAG = "OCHEADEROPTION-JNI";

JNIEXPORT jint JNICALL getOptionID(JNIEnv *env, jobject jobj)
{
    OC::HeaderOption::OCHeaderOption *option = getHandle<OC::HeaderOption::OCHeaderOption>(env, jobj);
    return option->getOptionID();
}

JNIEXPORT jstring JNICALL getOptionData(JNIEnv *env, jobject jobj)
{
    OC::HeaderOption::OCHeaderOption *option = getHandle<OC::HeaderOption::OCHeaderOption>(env, jobj);
    std::string data = option->getOptionData();
    return env->NewStringUTF(data.c_str());
}

JNIEXPORT jlong JNICALL OCHeaderOptionConstructor(JNIEnv *env, jobject jobj, jint joption, jstring jdata)
{
    OC::HeaderOption::OCHeaderOption *option = new OC::HeaderOption::OCHeaderOption(joption, env->GetStringUTFChars(jdata, 0));
    jlong instptr = reinterpret_cast<jlong>(option);
    return instptr;
}

