/* *****************************************************************
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

#include "jni_group_synchronization_callbacks.h"
#include "JniOcResource.h"
#include "GroupSynchronization.h"
#include "jni_things_manager_jvm.h"
#include "jni_things_manager_util.h"
#include "jni_string.h"

#define LOG_TAG "GroupSynchronizationCallbacks"

#define METHOD_ONGROUP_FIND_CALLBACK    "(" TM_SERVICE_OCRESOURCE_TYPE")V"

void GroupSynchronizationCallbacks::onFoundGroup(std::shared_ptr<OC::OCResource> groupResource)
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
    jclass groupSynchronizationCallbacks = GetJClass(TM_SERVICE_GROUP_SYNCHRONIZATION_CLASS_PATH);
    if (NULL == groupSynchronizationCallbacks)
    {
        LOGE("FindGroup : GetJClass TMServiceCallbackInterface failed");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Get the ThingsManagerCallback class instance
    jobject jobjectCallback = GetJObjectInstance(TM_SERVICE_GROUP_SYNCHRONIZATION_CLASS_PATH);
    if (NULL == jobjectCallback)
    {
        LOGE("FindGroup: getInstance failed!");
        ThingsManagerJVM::releaseEnv();
        return;
    }

    // Get onGroupFindCallback method reference
    jmethodID method_id = env->GetMethodID(groupSynchronizationCallbacks,
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
