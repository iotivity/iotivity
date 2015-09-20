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

#include "jni_group_synchronization.h"
#include "JniOcResource.h"
#include "JniOcResourceHandle.h"
#include "GroupManager.h"
#include "ActionSet.h"
#include "jni_things_manager_jvm.h"
#include "jni_things_manager_util.h"
#include "jni_group_synchronization_callbacks.h"
#include "jni_action_set.h"

using namespace OC;
using namespace OIC;

/**
 * GroupSynchronization static object
 */
static GroupSynchronization g_GroupSynchronization;

JNIEXPORT jint JNICALL JNIGroupSynchronizationFindGroup(JNIEnv *env, jobject interfaceObject,
        jobject jResourceTypes)
{
    LOGI("JNIGroupSynchronizationFindGroup: Enter");

    if (!jResourceTypes)
    {
        LOGE("JNIGroupSynchronizationFindGroup: jResourceTypes is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;
    try
    {
        ocResult = g_GroupSynchronization.findGroup((convertStringVector(env, jResourceTypes)),
                   &GroupSynchronizationCallbacks::onFoundGroup);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIGroupSynchronizationFindGroup: findGroup failed!");
            return ocResult;
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIGroupSynchronizationFindGroup: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
        return ocResult;
    }
    LOGI("JNIGroupSynchronizationFindGroup: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIGroupSynchronizationCreateGroup(JNIEnv *env, jobject interfaceObject,
        jstring jResourceType)
{
    LOGI("JNIGroupSynchronizationCreateGroup: Enter");

    if (!jResourceType)
    {
        LOGE("JNIGroupSynchronizationCreateGroup: jResourceType is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    const char *resourceTypePointer = env->GetStringUTFChars(jResourceType, NULL);
    if (NULL == resourceTypePointer)
    {
        LOGE("JNIGroupSynchronizationCreateGroup: Failed to convert jstring to char string!");
        return OC_STACK_NO_MEMORY;
    }

    std::string resourceType(resourceTypePointer);
    try
    {
        ocResult =  g_GroupSynchronization.createGroup(resourceType);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIGroupSynchronizationCreateGroup: CreateGroup failed!");
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIGroupSynchronizationCreateGroup: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
    }
    env->ReleaseStringUTFChars(jResourceType, resourceTypePointer);
    LOGI("JNIGroupSynchronizationCreateGroup: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIGroupSynchronizationJoinGroupString(JNIEnv *env, jobject interfaceObject,
        jstring jResourceType, jobject jResourceHandle)
{
    LOGI("JNIGroupSynchronizationJoinGroupString: Enter");

    if ((!jResourceType) || (!jResourceHandle))
    {
        LOGE("JNIGroupSynchronizationJoinGroupString: jResourceType or jResourceHandle is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    const char *resourceTypePointer = env->GetStringUTFChars(jResourceType, NULL);
    if (NULL == resourceTypePointer)
    {
        LOGE("JNIGroupSynchronizationJoinGroupString: Failed to convert jstring to char string!");
        return OC_STACK_NO_MEMORY;
    }

    std::string resourceType(resourceTypePointer);

    JniOcResourceHandle *jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(env,
            jResourceHandle);
    try
    {
        OCResourceHandle ocResourceHandle = jniOcResourceHandle->getOCResourceHandle();
        ocResult = g_GroupSynchronization.joinGroup(resourceType, ocResourceHandle);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIGroupSynchronizationJoinGroupString: joinGroup failed!");
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIGroupSynchronizationJoinGroupString: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
    }
    env->ReleaseStringUTFChars(jResourceType, resourceTypePointer);
    LOGI("JNIGroupSynchronizationJoinGroupString: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIGroupSynchronizationJoinGroupObject(JNIEnv *env, jobject interfaceObject,
        jobject jResource, jobject jResourceHandle)
{
    LOGI("JNIGroupSynchronizationJoinGroupObject: Enter");

    if ((!jResource) || (!jResourceHandle))
    {
        LOGE("JNIGroupSynchronizationJoinGroupObject: jResource or jResourceHandle is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, jResource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIGroupSynchronizationJoinGroupObject: Failed to get OCResource object!");
        return ocResult;
    }

    JniOcResourceHandle *jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(env,
            jResourceHandle);

    try
    {
        OCResourceHandle resHandle = jniOcResourceHandle->getOCResourceHandle();

        ocResult = g_GroupSynchronization.joinGroup(ocResource, resHandle);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIGroupSynchronizationJoinGroupObject: joinGroup failed!");
            return ocResult;
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIGroupSynchronizationJoinGroupObject: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
        return ocResult;
    }
    LOGI("JNIGroupSynchronizationJoinGroupObject: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIGroupSynchronizationLeaveGroup(JNIEnv *env, jobject interfaceObject,
        jstring jResourceType,
        jobject jResourceHandle)
{
    LOGI("JNIGroupSynchronizationLeaveGroup: Enter");

    if ((!jResourceType) || (!jResourceHandle))
    {
        LOGE("JNIGroupSynchronizationLeaveGroup: jResourceType or jResourceHandle is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    const char *resourceTypePointer = env->GetStringUTFChars(jResourceType, NULL);
    if (NULL == resourceTypePointer)
    {
        LOGE("JNIGroupSynchronizationLeaveGroup: Failed to convert jstring to char string!");
        return OC_STACK_NO_MEMORY;
    }

    std::string resourceType(resourceTypePointer);
    JniOcResourceHandle *jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(env,
            jResourceHandle);

    try
    {
        OCResourceHandle ocResourceHandle = jniOcResourceHandle->getOCResourceHandle();

        ocResult = g_GroupSynchronization.leaveGroup(resourceType, ocResourceHandle);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIGroupSynchronizationLeaveGroup: leaveGroup failed!");
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIGroupSynchronizationLeaveGroup: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
    }
    env->ReleaseStringUTFChars(jResourceType, resourceTypePointer);
    LOGI("JNIGroupSynchronizationLeaveGroup: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIGroupSynchronizationLeaveGroupForResource(JNIEnv *env,
        jobject interfaceObject,
        jobject jResource,
        jstring jResourceType,
        jobject jResourceHandle)
{
    LOGI("JNIGroupSynchronizationLeaveGroupForResource: Enter");

    if ((!jResource) || (!jResourceType) || (!jResourceHandle))
    {
        LOGE("JNIGroupSynchronizationLeaveGroupForResource: jResourceType or jResourceHandle is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, jResource);
    if (NULL == jniOcResource)
    {
        LOGE("JNIGroupSynchronizationLeaveGroupForResource: Failed to get jni OcResource!");
        return ocResult;
    }

    std::shared_ptr<OCResource> ocResource = jniOcResource->getOCResource();
    if (NULL == ocResource.get())
    {
        LOGE("JNIGroupSynchronizationLeaveGroupForResource: Failed to get OCResource object!");
        return ocResult;
    }

    const char *resourceTypePointer = env->GetStringUTFChars(jResourceType, NULL);
    if (NULL == resourceTypePointer)
    {
        LOGE("JNIGroupSynchronizationLeaveGroupForResource: Failed to convert jstring to char string!");
        return OC_STACK_NO_MEMORY;
    }

    std::string resourceType(resourceTypePointer);
    JniOcResourceHandle *jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(env,
            jResourceHandle);

    try
    {
        OCResourceHandle ocResourceHandle = jniOcResourceHandle->getOCResourceHandle();

        ocResult = g_GroupSynchronization.leaveGroup(ocResource, resourceType, ocResourceHandle);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIGroupSynchronizationLeaveGroupForResource: leaveGroup failed!");
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIGroupSynchronizationLeaveGroupForResource: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
    }

    env->ReleaseStringUTFChars(jResourceType, resourceTypePointer);
    LOGI("JNIGroupSynchronizationLeaveGroupForResource: Exit");
    return ocResult;
}

JNIEXPORT void JNICALL JNIGroupSynchronizationDeleteGroup(JNIEnv *env, jobject interfaceObject,
        jstring jcollectionResourceType)
{
    LOGI("JNIGroupSynchronizationDeleteGroup: Enter");

    if (!jcollectionResourceType)
    {
        LOGE("JNIGroupSynchronizationDeleteGroup: jcollectionResourceType is NULL!");
        return;
    }

    const char *collectionResourceTypePointer = env->GetStringUTFChars(jcollectionResourceType, NULL);
    if (NULL == collectionResourceTypePointer)
    {
        LOGE("JNIGroupSynchronizationDeleteGroup: Failed to convert jstring to char string!");
        return;
    }

    std::string collectionResourceType(collectionResourceTypePointer);
    try
    {
        g_GroupSynchronization.deleteGroup(collectionResourceType);
    }
    catch (InitializeException &e)
    {
        LOGE("JNIGroupSynchronizationDeleteGroup: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
    }

    env->ReleaseStringUTFChars(jcollectionResourceType, collectionResourceTypePointer);
    LOGI("JNIGroupSynchronizationDeleteGroup: Exit");
}

JNIEXPORT jobject JNICALL JNIGroupSynchronizationGetGroupList(JNIEnv *env, jobject interfaceObject)
{
    LOGI("JNIGroupSynchronizationGetGroupList: Enter");

    std::map< std::string, OCResourceHandle> groupListMap;
    jobject jGroupListMap;

    groupListMap = g_GroupSynchronization.getGroupList();
    if (groupListMap.empty())
    {
        LOGD("getGroupList Map is empty");
        return NULL;
    }

    jclass clazz = env->FindClass("java/util/HashMap");
    jmethodID init = env->GetMethodID(clazz, "<init>", "()V");
    jGroupListMap = env->NewObject(clazz, init);
    jmethodID putMethod = env->GetMethodID(clazz, "put",
                                           "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    for (auto it = groupListMap.begin(); it != groupListMap.end(); ++it)
    {
        jstring key = (*env).NewStringUTF( (*it).first.c_str() );
        JniOcResourceHandle *jniOcResourceHandle = new JniOcResourceHandle(((*it).second));
        jobject value = ocResourceHandleToJava(env, reinterpret_cast<jlong>(jniOcResourceHandle));
        env->CallObjectMethod(jGroupListMap, putMethod, key, value);
    }

    LOGI("JNIGroupSynchronizationGetGroupList: Exit");
    return jGroupListMap;
}


