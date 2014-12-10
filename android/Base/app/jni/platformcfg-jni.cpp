#include "ocstack-jni.h"

static const char* TAG = "PLATFORMCFG-JNI";

OC::ServiceType getServiceType(int type)
{
    switch (type) {
        case 0:
            return OC::ServiceType::InProc;
        case 1:
        default:
            return OC::ServiceType::OutOfProc;
    };
}

OC::ModeType getModeType(int type)
{
    switch (type) {
        case 0:
            return OC::ModeType::Server;
        case 1:
            return OC::ModeType::Client;
        case 2:
        default:
            return OC::ModeType::Both;
    };
}

OC::QualityOfService getQOS(int type)
{
    switch (type) {
        case 0:
            return OC::QualityOfService::LowQos;
        case 1:
            return OC::QualityOfService::MidQos;
        case 2:
            return OC::QualityOfService::HighQos;
        case 3:
        default:
            return OC::QualityOfService::NaQos;
    };
}

OC::PlatformConfig *g_cfg;

JNIEXPORT jlong JNICALL createNativeInstance
  (JNIEnv *env, jobject obj, jint jServiceType, jint jModeType, jstring jIpAddress, jint port, jint jQOS)
{

    g_cfg = new OC::PlatformConfig{getServiceType(jServiceType),
                            getModeType(jModeType),
                            env->GetStringUTFChars(jIpAddress, 0),
                            (uint16_t) port,
                            getQOS(jQOS)
    };


    jlong instptr = reinterpret_cast<jlong>(g_cfg);
    return instptr;
}
