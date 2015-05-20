/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#include "jni_things_manager_callbacks.h"
#include "JniOcResource.h"
#include "ThingsManager.h"
#include "jni_things_manager_jvm.h"
#include "jni_string.h"

#define LOG_TAG "ThingsManagerCallbacks"

#define METHOD_ONRESOURCE_CALLBACK    "(" TM_JAVA_VECTOR_TYPE")V"
#define METHOD_ONGROUP_FIND_CALLBACK    "(" TM_SERVICE_OCRESOURCE_TYPE")V"
#define METHOD_ONPRESENCE_CALLBACK    "(" TM_JAVA_STRING_TYPE"I)V"
#define METHOD_ONUPDATE_CONF_CALLBACK    "(" TM_JAVA_VECTOR_TYPE TM_SERVICE_OCREPRESENTATION_TYPE"I)V"
#define METHOD_ONGET_CONF_CALLBACK    "(" TM_JAVA_VECTOR_TYPE TM_SERVICE_OCREPRESENTATION_TYPE"I)V"
#define METHOD_ONBOOT_STRAP_CALLBACK    "(" TM_JAVA_VECTOR_TYPE TM_SERVICE_OCREPRESENTATION_TYPE"I)V"
#define METHOD_ONFACTORY_RESET_CALLBACK    "(" TM_JAVA_VECTOR_TYPE TM_SERVICE_OCREPRESENTATION_TYPE"I)V"
#define METHOD_ONREBOOT_CALLBACK    "(" TM_JAVA_VECTOR_TYPE TM_SERVICE_OCREPRESENTATION_TYPE"I)V"
#define METHOD_ONPOST_CALLBACK    "(" TM_JAVA_VECTOR_TYPE TM_SERVICE_OCREPRESENTATION_TYPE"I)V"
#define METHOD_ONPUT_CALLBACK    "(" TM_JAVA_VECTOR_TYPE TM_SERVICE_OCREPRESENTATION_TYPE"I)V"
#define METHOD_ONGET_CALLBACK    "(" TM_JAVA_VECTOR_TYPE TM_SERVICE_OCREPRESENTATION_TYPE"I)V"
#define METHOD_TMCALLBACK_GETINSTANCE    "()" TM_SERVICE_CALLBACK_CLASS_TYPE

jobject OcResourceToJava(JNIEnv *env, jlong resource)
{
    jclass resourceClass = GetJClass(TM_SERVICE_OCRESOURCE_PATH);
    if (NULL == resourceClass)
    {
        LOGE("OcResourceToJava : failed to find OCResouce java class!");
        return NULL;
    }

    jmethodID constructor = env->GetMethodID(resourceClass, "<init>", "(J)V");
    if (NULL == constructor)
    {
        LOGE("OcResourceToJava: Failed to get constructor method!");
        return NULL;
    }

    jobject resourceObj = (jobject) env->NewObject(resourceClass, constructor, resource);
    if (NULL == resourceObj)
    {
        LOGE("OcResourceToJava: Failed to create OCResouce java object!");
        return NULL;
    }

    return resourceObj;
}

jobject OcHeaderOptionToJava(JNIEnv *env, OC::HeaderOption::OCHeaderOption headerOption)
{
    JString *optionData = new JString(env, headerOption.getOptionData());
    if (!optionData)
    {
        LOGE("OcHeaderOptionToJava : failed to get JString!");
        return NULL;
    }

    jclass headerOptionClass = GetJClass(TM_SERVICE_HEADER_OPTION_PATH);
    if (NULL == headerOptionClass)
    {
        LOGE("OcHeaderOptionToJava : failed to find OCRepresentation java class!");
        return NULL;
    }

    jmethodID constructor = env->GetMethodID(headerOptionClass, "<init>", "(JLjava/lang/String;)V");
    if (NULL == constructor)
    {
        LOGE("OcHeaderOptionToJava: Failed to get constructor method!");
        return NULL;
    }

    jobject headerOptionObj = (jobject) env->NewObject(headerOptionClass, constructor,
                              (jlong)headerOption.getOptionID(), optionData);
    if (NULL == headerOptionObj)
    {
        LOGE("OcHeaderOptionToJava: Failed to create OCRepresentation java object!");
        return NULL;
    }

    return headerOptionObj;
}

jobject OcRepresentationToJava(JNIEnv *env, jlong ocRepresentation)
{
    jclass ocRepresentationClass = GetJClass(TM_SERVICE_OCREPRESENTATION_PATH);
    if (NULL == ocRepresentationClass)
    {
        LOGE("OcRepresentationToJava : failed to find OCRepresentation java class!");
        return NULL;
    }

    jmethodID constructor = env->GetMethodID(ocRepresentationClass, "<init>", "(J)V");
    if (NULL == constructor)
    {
        LOGE("OcRepresentationToJava: Failed to get constructor method!");
        return NULL;
    }

    jobject ocRepresentationObj = (jobject) env->NewObject(ocRepresentationClass, constructor,
                                  ocRepresentation);
    if (NULL == ocRepresentationObj)
    {
        LOGE("OcRepresentationToJava: Failed to create OCRepresentation java object!");
        return NULL;
    }

    return ocRepresentationObj;
}

void ThingsManagerCallbacks::onFoundCandidateResource(
    std::vector< std::shared_ptr<OC::OCResource>> resources)
{
    LOGI("findCandidateResource : Enter");

    if (resources.size() == 0)
    {
        LOGE("findCandidateResource : found resources zero");
        return;
    }

    JNIEnv *env = ThingsManagerJVM::getEnv();
    if (env == NULL)
    {
        LOGE("findCandidateResource : Getting JNIEnv failed");
        return;
    }

    // Get ThingsManagerCallback class reference
    jclass thingsManagerCallbacks = GetJClass(TM_SERVICE_CALLBACK_CLASS_PATH);
    if (NULL == thingsManagerCallbacks)
    {
        LOGE("findCandidateResource : GetJClass TMServiceCallbackInterface failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Get the ThingsManagerCallback class instance
    jobject jobjectCallback = GetJObjectInstance(TM_SERVICE_CALLBACK_CLASS_PATH);
    if (NULL == jobjectCallback)
    {
        LOGE("getInstance( %s) failed!", TM_SERVICE_CALLBACK_CLASS_PATH);
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Get onResourceCallback method reference
    jmethodID method_id = env->GetMethodID(thingsManagerCallbacks,
                                           "onResourceCallback", METHOD_ONRESOURCE_CALLBACK);
    if (NULL == method_id)
    {
        LOGE("findCandidateResource: onResourceCallback : GetMethodID failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    if ((env)->ExceptionCheck())
    {
        LOGE("findCandidateResource : ExceptionCheck failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jclass vectorCls = env->FindClass(TM_JAVA_VECTOR_CLASS_PATH);
    if (!vectorCls)
    {
        LOGE("findCandidateResource: failed to get %s class reference", TM_JAVA_VECTOR_CLASS_PATH);
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jmethodID constr = env->GetMethodID(vectorCls, "<init>", "()V");
    if (!constr)
    {
        LOGE("findCandidateResource: failed to get %s constructor", TM_JAVA_VECTOR_CLASS_PATH);
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jobject vectorObj = env->NewObject(vectorCls, constr);
    if (!vectorObj)
    {
        LOGE("findCandidateResource: failed to create a %s object", TM_JAVA_VECTOR_CLASS_PATH);
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jmethodID addElement = env->GetMethodID(vectorCls, "addElement", "(Ljava/lang/Object;)V");
    if (NULL == addElement)
    {
        LOGE("findCandidateResource: failed to create a addElement method");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Convert to java OCResource object
    for (int i = 0; i < resources.size(); i++)
    {
        JniOcResource *jniOcResource = new JniOcResource(resources[i]);
        if (!jniOcResource)
        {
            LOGE("findCandidateResource: failed to create a JniOcResource");
            ThingsManagerJVM::releaseEnv();
            return;
        }

        jobject resource = OcResourceToJava(env, reinterpret_cast<jlong>(jniOcResource));
        env->CallVoidMethod(vectorObj, addElement, resource);
    }

    env->CallVoidMethod(jobjectCallback, method_id, vectorObj);

    if ((env)->ExceptionCheck())
    {
        LOGE("findCandidateResource : CallVoidMethod failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    ThingsManagerJVM::releaseEnv();
    LOGI("findCandidateResource : Exit");
}

void ThingsManagerCallbacks::onFoundGroup(std::shared_ptr<OC::OCResource> groupResource)
{
    LOGI("FindGroup : Enter");

    if (NULL == groupResource.get())
    {
        LOGE("FindGroup : Invalid received GroupResource!");
        return;
    }

    JNIEnv *env = ThingsManagerJVM::getEnv();
    if (env == NULL)
    {
        LOGE("FindGroup : Getting JNIEnv failed");
        return;
    }

    // Get ThingsManagerCallback class reference
    jclass thingsManagerCallbacks = GetJClass(TM_SERVICE_CALLBACK_CLASS_PATH);
    if (NULL == thingsManagerCallbacks)
    {
        LOGE("FindGroup : GetJClass TMServiceCallbackInterface failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Get the ThingsManagerCallback class instance
    jobject jobjectCallback = GetJObjectInstance(TM_SERVICE_CALLBACK_CLASS_PATH);
    if (NULL == jobjectCallback)
    {
        LOGE("FindGroup: getInstance( %s) failed!", TM_SERVICE_CALLBACK_CLASS_PATH);
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Get onGroupFindCallback method reference
    jmethodID method_id = env->GetMethodID(thingsManagerCallbacks,
                                           "onGroupFindCallback",
                                           METHOD_ONGROUP_FIND_CALLBACK);
    if (NULL == method_id)
    {
        LOGE("FindGroup : GetMethodID failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    if ((env)->ExceptionCheck())
    {
        LOGE("FindGroup : ExceptionCheck failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    JniOcResource *jniOcResource = new JniOcResource(groupResource);
    if (!jniOcResource)
    {
        LOGE("FindGroup : groupResource is invalid!");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jobject resource = OcResourceToJava(env, reinterpret_cast<jlong>(jniOcResource));

    env->CallVoidMethod(jobjectCallback, method_id, resource);

    if ((env)->ExceptionCheck())
    {
        LOGE("FindGroup : CallVoidMethod failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    ThingsManagerJVM::releaseEnv();
    LOGI("FindGroup : Exit");
}

void ThingsManagerCallbacks::onSubscribePresence(std::string resource, OCStackResult result)
{
    LOGI("SubscribePresence : Entry");

    JNIEnv *env = ThingsManagerJVM::getEnv();
    if (env == NULL)
    {
        LOGE("SubscribePresence : Getting JNIEnv failed");
        return;
    }

    // Get ThingsManagerCallback class reference
    jclass thingsManagerCallbacks = GetJClass(TM_SERVICE_CALLBACK_CLASS_PATH);
    if (NULL == thingsManagerCallbacks)
    {
        LOGE("findCandidateResource : GetJClass TMServiceCallbackInterface failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Get getInstance method reference
    jmethodID getinstance_method_id = env->GetStaticMethodID(thingsManagerCallbacks,
                                      "getInstance", METHOD_TMCALLBACK_GETINSTANCE);
    if (NULL == getinstance_method_id)
    {
        LOGE("getInstance : GetMethodID failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Get the ThingsManagerCallback class instance
    jobject jobjectCallback = env->CallStaticObjectMethod(thingsManagerCallbacks,
                              getinstance_method_id);
    if (NULL == jobjectCallback)
    {
        LOGE("getInstance( %s) failed!", TM_SERVICE_CALLBACK_CLASS_PATH);
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jmethodID method_id = env->GetMethodID(thingsManagerCallbacks,
                                           "onPresenceCallback",
                                           METHOD_ONPRESENCE_CALLBACK);
    if (NULL == method_id)
    {
        LOGE("SubscribePresence : GetMethodID failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    if ((env)->ExceptionCheck())
    {
        LOGE("SubscribePresence : ExceptionCheck failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    JString *jresource = new JString(env, resource);
    if (jresource == NULL)
    {
        LOGE("resource value is invalid");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    env->CallVoidMethod(jobjectCallback, method_id, jresource->getObject(), (jint)result);

    delete jresource;

    if ((env)->ExceptionCheck())
    {
        LOGE("SubscribePresence : CallVoidMethod failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    LOGI("SubscribePresence : Exit");

//METHOD_FAILURE:
    ThingsManagerJVM::releaseEnv();
}

void ThingsManagerCallbacks::onUpdateConfigurationsResponse(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    LOGI("UpdateConfigurations : Enter");

    ThingsManagerCallbacks::invokeCallback(headerOptions, rep, eCode, "onUpdateConfigurationsCallback",
                                           METHOD_ONUPDATE_CONF_CALLBACK);

    LOGI("UpdateConfigurations : Exit");
}

void ThingsManagerCallbacks::onGetConfigurationsResponse(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    LOGI("GetConfigurations : Enter");

    ThingsManagerCallbacks::invokeCallback(headerOptions, rep, eCode, "onGetConfigurationsCallback",
                                           METHOD_ONGET_CONF_CALLBACK);

    LOGI("GetConfigurations : Exit");
}

void ThingsManagerCallbacks::onBootStrapResponse(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    LOGI("BootStrap : Enter");

    ThingsManagerCallbacks::invokeCallback(headerOptions, rep, eCode, "onBootStrapCallback",
                                           METHOD_ONBOOT_STRAP_CALLBACK);

    LOGI("BootStrap : Exit");
}

void ThingsManagerCallbacks::onRebootResponse(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    LOGI("OnReboot : Enter");

    ThingsManagerCallbacks::invokeCallback(headerOptions, rep, eCode, "onRebootCallback",
                                           METHOD_ONREBOOT_CALLBACK);

    LOGI("OnReboot : Exit");
}

void ThingsManagerCallbacks::onFactoryResetResponse(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    LOGI("OnFactoryReset : Enter");

    ThingsManagerCallbacks::invokeCallback(headerOptions, rep, eCode, "onFactoryResetCallback",
                                           METHOD_ONFACTORY_RESET_CALLBACK);

    LOGI("OnFactoryReset : Exit");
}

void ThingsManagerCallbacks::onPostResponse(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    LOGI("PostResponse : Enter");

    ThingsManagerCallbacks::invokeCallback(headerOptions, rep, eCode, "onPostResponseCallback",
                                           METHOD_ONPOST_CALLBACK);

    LOGI("PostResponse : Exit");
}

void ThingsManagerCallbacks::onPutResponse(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    LOGI("OnPutResponse : Enter");

    ThingsManagerCallbacks::invokeCallback(headerOptions, rep, eCode, "onPutResponseCallback",
                                           METHOD_ONPUT_CALLBACK);

    LOGI("OnPutResponse : Exit");
}


void ThingsManagerCallbacks::onGetResponse(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode)
{
    LOGI("OnGetResponse : Enter");

    ThingsManagerCallbacks::invokeCallback(headerOptions, rep, eCode, "onGetResponseCallback",
                                           METHOD_ONGET_CALLBACK);

    LOGI("OnGetResponse : Exit");
}

void ThingsManagerCallbacks::invokeCallback(const OC::HeaderOptions &headerOptions,
        const OC::OCRepresentation &rep, const int eCode, const char  *callbackName,
        const char *signature)
{
    LOGI("InvokeCallback : Enter %s", callbackName);

    JNIEnv *env = ThingsManagerJVM::getEnv();
    if (env == NULL)
    {
        LOGE("InvokeCallback : Getting JNIEnv failed");
        return;
    }

    // Get ThingsManagerCallback class reference
    jclass thingsManagerCallbacks = GetJClass(TM_SERVICE_CALLBACK_CLASS_PATH);
    if (NULL == thingsManagerCallbacks)
    {
        LOGE("InvokeCallback : GetJClass TMServiceCallbackInterface failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Get the ThingsManagerCallback class instance
    jobject jobjectCallback = GetJObjectInstance(TM_SERVICE_CALLBACK_CLASS_PATH);
    if (NULL == jobjectCallback)
    {
        LOGE("InvokeCallback: getInstance( %s) failed!", TM_SERVICE_CALLBACK_CLASS_PATH);
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jmethodID method_id = env->GetMethodID(thingsManagerCallbacks, callbackName, signature);
    if (!method_id)
    {
        LOGE("InvokeCallback : GetMethodID failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    if ((env)->ExceptionCheck())
    {
        LOGE("InvokeCallback : ExceptionCheck failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Convert vector<OC:HeaderOption::OCHeaderOption> to java type
    jclass vectorCls = env->FindClass(TM_JAVA_VECTOR_CLASS_PATH);
    if (!vectorCls)
    {
        LOGE("InvokeCallback: failed to get %s class reference", TM_JAVA_VECTOR_CLASS_PATH);
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jmethodID constr = env->GetMethodID(vectorCls, "<init>", "()V");
    if (!constr)
    {
        LOGE("InvokeCallback: failed to get %s constructor", TM_JAVA_VECTOR_CLASS_PATH);
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jobject vectorObj = env->NewObject(vectorCls, constr);
    if (!vectorObj)
    {
        LOGE("InvokeCallback: failed to create a %s object", TM_JAVA_VECTOR_CLASS_PATH);
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jmethodID addElement = env->GetMethodID(vectorCls, "addElement", "(Ljava/lang/Object;)V");
    if (NULL == addElement)
    {
        LOGE("InvokeCallback: failed to create a addElement method");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    jobject headerOptionTemp;
    for (int i = 0; i < headerOptions.size(); i++)
    {
        headerOptionTemp = OcHeaderOptionToJava(env, headerOptions[i]);
        env->CallVoidMethod(vectorObj, addElement, headerOptionTemp);
    }

    // Convert OCRepresentation to java type
    jobject jrepresentation = OcRepresentationToJava(env, (jlong) reinterpret_cast<jlong>(&rep));
    if (!jrepresentation)
    {
        LOGE("InvokeCallback : cannot create OCRepresentation class");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    env->CallVoidMethod(jobjectCallback, method_id, vectorObj, jrepresentation, (jint)eCode);

    if ((env)->ExceptionCheck())
    {
        LOGE("InvokeCallback : CallVoidMethod failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    ThingsManagerJVM::releaseEnv();
    LOGI("InvokeCallback : Exit %s", callbackName);
}