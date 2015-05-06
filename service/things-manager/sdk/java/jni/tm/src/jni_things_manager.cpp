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
#include "jni_things_manager.h"

#include "JniOcResource.h"
#include "JniOcResourceHandle.h"
#include "ThingsManager.h"
#include "ActionSet.h"
#include "jni_things_manager_jvm.h"
#include "jni_things_manager_util.h"
#include "jni_things_manager_callbacks.h"
#include "jni_action_set.h"

using namespace OC;
using namespace OIC;

/**
 * @var g_ThingsManager
 * @brief ThingsManager static object
 */
static ThingsManager g_ThingsManager;

jobject ocResourceHandleToJava(JNIEnv *env, jlong resourceHandle);

JNIEXPORT jint JNICALL JNIThingsManagerFindCandidateResource(JNIEnv *env, jobject interfaceObject,
        jobject jResourceTypes, jint waitSec)
{
    LOGI("JNIThingsManagerFindCandidateResource: Enter");

    if (!jResourceTypes)
    {
        LOGE("JNIThingsManagerFindCandidateResource: jResourceTypes is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;
    try
    {
        ocResult = g_ThingsManager.findCandidateResources(convertStringVector(env, jResourceTypes),
                   &ThingsManagerCallbacks::onFoundCandidateResource, (int)waitSec);

        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIThingsManagerFindCandidateResource: findCandidateResources failed!");
            return ocResult;
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIThingsManagerFindCandidateResource: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
        return ocResult;
    }

    LOGI("JNIThingsManagerFindCandidateResource: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerSubscribeCollectionPresence(JNIEnv *env,
        jobject interfaceObject,
        jobject jResource)
{
    LOGI("JNIThingsManagerSubscribeCollectionPresence: Enter");

    if (!jResource)
    {
        LOGE("JNIThingsManagerSubscribeCollectionPresence: jResource is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, jResource);
    if (NULL == jniOcResource)
    {
        LOGE("JNIThingsManagerSubscribeCollectionPresence: Failed to get jni OcResource!");
        return ocResult;
    }

    std::shared_ptr<OCResource> ocResource = jniOcResource->getOCResource();
    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerSubscribeCollectionPresence: Failed to get OCResource object!");
        return ocResult;
    }

    ocResult = g_ThingsManager.subscribeCollectionPresence(ocResource,
               &ThingsManagerCallbacks::onSubscribePresence);
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerSubscribeCollectionPresence: subscribeCollectionPresence failed!");
        return ocResult;
    }

    LOGI("JNIThingsManagerSubscribeCollectionPresence: Exit");
    return ocResult;
}

JNIEXPORT jobject JNICALL JNIThingsManagerBindResourceToGroup(JNIEnv *env, jobject interfaceObject,
        jobject jResource, jobject jCollectionHandle)
{
    LOGI("JNIThingsManagerBindResourceToGroup: Enter");

    if (!jResource || !jCollectionHandle)
    {
        LOGE("JNIThingsManagerBindResourceToGroup: Invalid parameter!");
        return NULL;
    }

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, jResource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerBindResourceToGroup: Failed to get OCResource object!");
        return NULL;
    }

    JniOcResourceHandle *jniOcCollectionHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(env,
            jCollectionHandle);
    if (NULL == jniOcCollectionHandle)
    {
        LOGE("JNIThingsManagerBindResourceToGroup: collection handle is null!");
        return NULL;
    }

    jobject jResourceHandle = NULL;
    try
    {
        OCResourceHandle ocChildHandle = NULL;
        OCResourceHandle ocCollectionHandle = jniOcCollectionHandle->getOCResourceHandle();
        OCStackResult ocResult = g_ThingsManager.bindResourceToGroup(ocChildHandle, ocResource, ocCollectionHandle);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIThingsManagerBindResourceToGroup: bindResourceToGroup failed!");
            return NULL;
        }

        // Convert OCResourceHandle to java type
        JniOcResourceHandle* jniHandle = new JniOcResourceHandle(ocChildHandle);
        jlong handle = reinterpret_cast<jlong>(jniHandle);
        jResourceHandle = env->NewObject(g_cls_OcResourceHandle, g_mid_OcResourceHandle_N_ctor, handle);
        if (env->ExceptionCheck())
        {
            LOGE("JNIThingsManagerBindResourceToGroup: Failed to create OcResourceHandle");
            delete jniHandle;
            return NULL;
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIThingsManagerBindResourceToGroup: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
        return NULL;
    }

    LOGI("JNIThingsManagerBindResourceToGroup: Exit");
    return jResourceHandle;
}

JNIEXPORT jint JNICALL JNIThingsManagerFindGroup(JNIEnv *env, jobject interfaceObject,
        jobject jResourceTypes)
{
    LOGI("JNIThingsManagerFindGroup: Enter");

    if (!jResourceTypes)
    {
        LOGE("JNIThingsManagerFindGroup: jResourceTypes is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;
    try
    {
        ocResult = g_ThingsManager.findGroup((convertStringVector(env, jResourceTypes)),
                                             &ThingsManagerCallbacks::onFoundGroup);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIThingsManagerFindGroup: findGroup failed!");
            return ocResult;
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIThingsManagerFindGroup: Exception occurred! %s, %d", e.reason().c_str(),
            e.code());
        return ocResult;
    }
    LOGI("JNIThingsManagerFindGroup: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerCreateGroup(JNIEnv *env, jobject interfaceObject,
        jstring jResourceType)
{
    LOGI("JNIThingsManagerCreateGroup: Enter");

    if (!jResourceType)
    {
        LOGE("JNIThingsManagerCreateGroup: jResourceType is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    const char *resourceTypePointer = env->GetStringUTFChars(jResourceType, NULL);
    if (NULL == resourceTypePointer)
    {
        LOGE("JNIThingsManagerCreateGroup: Failed to convert jstring to char string!");
        return OC_STACK_NO_MEMORY;
    }

    std::string resourceType(resourceTypePointer);
    try
    {
        ocResult =  g_ThingsManager.createGroup(resourceType);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIThingsManagerCreateGroup: CreateGroup failed!");
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIThingsManagerCreateGroup: Exception occurred! %s, %d", e.reason().c_str(),
            e.code());
    }
    env->ReleaseStringUTFChars(jResourceType, resourceTypePointer);
    LOGI("JNIThingsManagerCreateGroup: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerJoinGroupString(JNIEnv *env, jobject interfaceObject,
        jstring jResourceType, jobject jResourceHandle)
{
    LOGI("JNIThingsManagerJoinGroupString: Enter");

    if ((!jResourceType) || (!jResourceHandle))
    {
        LOGE("JNIThingsManagerJoinGroupString: jResourceType or jResourceHandle is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    const char *resourceTypePointer = env->GetStringUTFChars(jResourceType, NULL);
    if (NULL == resourceTypePointer)
    {
        LOGE("JNIThingsManagerJoinGroupString: Failed to convert jstring to char string!");
        return OC_STACK_NO_MEMORY;
    }

    std::string resourceType(resourceTypePointer);

    JniOcResourceHandle *jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(env,
            jResourceHandle);
    try
    {
        OCResourceHandle ocResourceHandle = jniOcResourceHandle->getOCResourceHandle();
        ocResult = g_ThingsManager.joinGroup(resourceType, ocResourceHandle);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIThingsManagerJoinGroupString: joinGroup failed!");
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIThingsManagerJoinGroupString: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
    }
    env->ReleaseStringUTFChars(jResourceType, resourceTypePointer);
    LOGI("JNIThingsManagerJoinGroupString: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerJoinGroupObject(JNIEnv *env, jobject interfaceObject,
        jobject jResource, jobject jResourceHandle)
{
    LOGI("JNIThingsManagerJoinGroupObject: Enter");

    if ((!jResource) || (!jResourceHandle))
    {
        LOGE("JNIThingsManagerJoinGroupObject: jResource or jResourceHandle is NULL!");
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
        LOGE("JNIThingsManagerJoinGroupObject: Failed to get OCResource object!");
        return ocResult;
    }

    JniOcResourceHandle *jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(env,
            jResourceHandle);

    try
    {
        OCResourceHandle resHandle = jniOcResourceHandle->getOCResourceHandle();

        ocResult = g_ThingsManager.joinGroup(ocResource, resHandle);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIThingsManagerJoinGroupObject: joinGroup failed!");
            return ocResult;
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIThingsManagerJoinGroupObject: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
        return ocResult;
    }
    LOGI("JNIThingsManagerJoinGroupObject: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerLeaveGroup(JNIEnv *env, jobject interfaceObject,
        jstring jResourceType,
        jobject jResourceHandle)
{
    LOGI("JNIThingsManagerLeaveGroup: Enter");

    if ((!jResourceType) || (!jResourceHandle))
    {
        LOGE("JNIThingsManagerLeaveGroup: jResourceType or jResourceHandle is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    const char *resourceTypePointer = env->GetStringUTFChars(jResourceType, NULL);
    if (NULL == resourceTypePointer)
    {
        LOGE("JNIThingsManagerLeaveGroup: Failed to convert jstring to char string!");
        return OC_STACK_NO_MEMORY;
    }

    std::string resourceType(resourceTypePointer);
    JniOcResourceHandle *jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(env,
            jResourceHandle);

    try
    {
        OCResourceHandle ocResourceHandle = jniOcResourceHandle->getOCResourceHandle();

        ocResult = g_ThingsManager.leaveGroup(resourceType, ocResourceHandle);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIThingsManagerLeaveGroup: leaveGroup failed!");
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIThingsManagerLeaveGroup: Exception occurred! %s, %d", e.reason().c_str(),
            e.code());
    }
    env->ReleaseStringUTFChars(jResourceType, resourceTypePointer);
    LOGI("JNIThingsManagerLeaveGroup: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerLeaveGroupForResource(JNIEnv *env, jobject interfaceObject,
        jobject jResource,
        jstring jResourceType,
        jobject jResourceHandle)
{
    LOGI("JNIThingsManagerLeaveGroupForResource: Enter");

    if ((!jResource) || (!jResourceType) || (!jResourceHandle))
    {
        LOGE("JNIThingsManagerLeaveGroupForResource: jResourceType or jResourceHandle is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, jResource);
    if (NULL == jniOcResource)
    {
        LOGE("JNIThingsManagerLeaveGroupForResource: Failed to get jni OcResource!");
        return ocResult;
    }

    std::shared_ptr<OCResource> ocResource = jniOcResource->getOCResource();
    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerLeaveGroupForResource: Failed to get OCResource object!");
        return ocResult;
    }

    const char *resourceTypePointer = env->GetStringUTFChars(jResourceType, NULL);
    if (NULL == resourceTypePointer)
    {
        LOGE("JNIThingsManagerLeaveGroupForResource: Failed to convert jstring to char string!");
        return OC_STACK_NO_MEMORY;
    }

    std::string resourceType(resourceTypePointer);
    JniOcResourceHandle *jniOcResourceHandle = JniOcResourceHandle::getJniOcResourceHandlePtr(env,
            jResourceHandle);

    try
    {
        OCResourceHandle ocResourceHandle = jniOcResourceHandle->getOCResourceHandle();

        ocResult = g_ThingsManager.leaveGroup(ocResource, resourceType, ocResourceHandle);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIThingsManagerLeaveGroupForResource: leaveGroup failed!");
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIThingsManagerLeaveGroupForResource: Exception occurred! %s, %d", e.reason().c_str(),
             e.code());
    }

    env->ReleaseStringUTFChars(jResourceType, resourceTypePointer);
    LOGI("JNIThingsManagerLeaveGroupForResource: Exit");
    return ocResult;
}

JNIEXPORT void JNICALL JNIThingsManagerDeleteGroup(JNIEnv *env, jobject interfaceObject,
        jstring jcollectionResourceType)
{
    LOGI("JNIThingsManagerDeleteGroup: Enter");

    if (!jcollectionResourceType)
    {
        LOGE("JNIThingsManagerDeleteGroup: jcollectionResourceType is NULL!");
        return;
    }

    const char *collectionResourceTypePointer = env->GetStringUTFChars(jcollectionResourceType, NULL);
    if (NULL == collectionResourceTypePointer)
    {
        LOGE("JNIThingsManagerDeleteGroup: Failed to convert jstring to char string!");
        return;
    }

    std::string collectionResourceType(collectionResourceTypePointer);
    try
    {
        g_ThingsManager.deleteGroup(collectionResourceType);
    }
    catch (InitializeException &e)
    {
        LOGE("JNIThingsManagerDeleteGroup: Exception occurred! %s, %d", e.reason().c_str(),
            e.code());
    }

    env->ReleaseStringUTFChars(jcollectionResourceType, collectionResourceTypePointer);
    LOGI("JNIThingsManagerDeleteGroup: Exit");
}

JNIEXPORT jobject JNICALL JNIThingsManagerGetGroupList(JNIEnv *env, jobject interfaceObject)
{
    LOGI("JNIThingsManagerGetGroupList: Enter");

    std::map< std::string, OCResourceHandle> groupListMap;
    jobject jGroupListMap;

    groupListMap = g_ThingsManager.getGroupList();
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

    LOGI("JNIThingsManagerGetGroupList: Exit");
    return jGroupListMap;
}

JNIEXPORT jint JNICALL JNIThingsManagerUpdateConfigurations(JNIEnv *env, jobject interfaceObject,
        jobject resource, jobject configurations)
{
    LOGI("JNIThingsManagerUpdateConfigurations: Enter");

    if ((!resource) || (!configurations))
    {
        LOGE("JNIThingsManagerUpdateConfigurations: resource or configurations is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, resource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerUpdateConfigurations: Failed to get OCResource object!");
        return ocResult;
    }

    std::map<std::string, std::string> configurationsMap;
    configurationsMap = convertStringMap(env, configurations);
    ocResult =  g_ThingsManager.updateConfigurations(ocResource, configurationsMap,
                &ThingsManagerCallbacks::onUpdateConfigurationsResponse);
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerUpdateConfigurations: updateConfigurations failed!");
        return ocResult;
    }

    LOGI("JNIThingsManagerUpdateConfigurations: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerGetConfigurations(JNIEnv *env, jobject interfaceObject,
        jobject resource, jobject configurations)
{
    LOGI("JNIThingsManagerGetConfigurations: Enter");

    if ((!resource) || (!configurations))
    {
        LOGE("JNIThingsManagerGetConfigurations: resource or configurations is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, resource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerGetConfigurations: Failed to get OCResource object!");
        return ocResult;
    }

    ocResult = g_ThingsManager.getConfigurations(ocResource,
               (convertStringVector(env, configurations)),
               &ThingsManagerCallbacks::onGetConfigurationsResponse);
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerGetConfigurations: getConfigurations failed!");
        return ocResult;
    }

    LOGI("JNIThingsManagerGetConfigurations: Exit");
    return ocResult;
}

JNIEXPORT jstring JNICALL JNIThingsManagerGetListOfSupportedConfigurationUnits(JNIEnv *env,
        jobject interfaceObject)
{
    LOGI("JNIThingsManagerGetListOfSupportedConfigurationUnits: Enter");

    std::string configListString = g_ThingsManager.getListOfSupportedConfigurationUnits();
    jstring jConfigListString =  env->NewStringUTF(configListString.c_str());

    LOGI("JNIThingsManagerGetListOfSupportedConfigurationUnits: Exit");
    return jConfigListString;
}

JNIEXPORT jint JNICALL JNIThingsManagerDoBootstrap(JNIEnv *env, jobject interfaceObject)
{
    LOGI("JNIThingsManagerDoBootstrap: Enter");

    OCStackResult ocResult = OC_STACK_ERROR;
    try
    {
        ocResult  = g_ThingsManager.doBootstrap(&ThingsManagerCallbacks::onBootStrapResponse);
        if (OC_STACK_OK != ocResult)
        {
            LOGE("JNIThingsManagerDoBootstrap: doBootstrap failed!");
            return ocResult;
        }
    }
    catch (InitializeException &e)
    {
        LOGE("JNIThingsManagerDoBootstrap: Exception occurred! %s, %d", e.reason().c_str(),
            e.code());
        return ocResult;
    }

    LOGI("JNIThingsManagerDoBootstrap: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerReboot(JNIEnv *env, jobject interfaceObject,
        jobject resource)
{
    LOGI("JNIThingsManagerReboot: Enter");

    if (!resource)
    {
        LOGE("JNIThingsManagerReboot: resource is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, resource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerReboot: Failed to get OCResource object!");
        return ocResult;
    }

    ocResult = g_ThingsManager.reboot(ocResource, &ThingsManagerCallbacks::onRebootResponse);
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerReboot: reboot failed!");
        return ocResult;
    }

    LOGI("JNIThingsManagerReboot: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerFactoryReset(JNIEnv *env, jobject interfaceObject,
        jobject resource)
{
    LOGI("JNIThingsManagerFactoryReset: Enter");

    if (!resource)
    {
        LOGE("JNIThingsManagerFactoryReset: resource is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, resource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerFactoryReset: Failed to get OCResource object!");
        return ocResult;
    }

    ocResult = g_ThingsManager.factoryReset(ocResource,
                                            &ThingsManagerCallbacks::onFactoryResetResponse);
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerFactoryReset: factoryReset failed!");
        return ocResult;
    }

    LOGI("JNIThingsManagerFactoryReset: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerAddActionSet(JNIEnv *env, jobject interfaceObject,
        jobject resource,
        jobject newActionSet)
{
    LOGI("JNIThingsManagerAddActionSet: Entry");

    if ((!resource) || (!newActionSet))
    {
        LOGE("JNIThingsManagerAddActionSet: resource or newActionSet is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, resource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManageraAdActionSet: Failed to get OCResource object!");
        return ocResult;
    }

    JniActionSet *jActionSet = new JniActionSet(env, newActionSet);
    ActionSet *pActionSet = jActionSet->getActionSet(env, newActionSet);
    if (NULL == pActionSet)
    {
        LOGE("JNIThingsManageraAdActionSet: Failed to convert ActionSet!");
        return ocResult;
    }

    ocResult = g_ThingsManager.addActionSet(ocResource, pActionSet,
                                            &ThingsManagerCallbacks::onPutResponse);
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerAddActionSet: addActionSet is failed!");
    }

    delete pActionSet;
    LOGI("JNIThingsManagerAddActionSet: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerExecuteActionSet(JNIEnv *env, jobject interfaceObject,
        jobject resource, jstring jActionSetName)
{
    LOGI("JNIThingsManagerExecuteActionSet: Entry");

    if ((!resource) || (!jActionSetName))
    {
        LOGE("JNIThingsManagerExecuteActionSet: resource or jActionSetName is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, resource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerExecuteActionSet: Failed to get OCResource object!");
        return ocResult;
    }

    const char *actionSetNamePointer = env->GetStringUTFChars(jActionSetName, 0);
    if (NULL == actionSetNamePointer)
    {
        LOGE("JNIThingsManagerExecuteActionSet: Failed to convert jstring to char string!");
        return OC_STACK_NO_MEMORY;
    }

    std::string actionSetName(actionSetNamePointer);

    ocResult = g_ThingsManager.executeActionSet(ocResource, actionSetName,
               &ThingsManagerCallbacks::onPostResponse);
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerExecuteActionSet: executeActionSet is failed!");
    }

    env->ReleaseStringUTFChars(jActionSetName, actionSetNamePointer);
    LOGI("JNIThingsManagerExecuteActionSet: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerExecuteActionSetWithDelay(JNIEnv *env,
        jobject interfaceObject,
        jobject resource, jstring jActionSetName, jlong delay)
{
    LOGI("JNIThingsManagerExecuteActionSet: Entry");

    if ((!resource) || (!jActionSetName))
    {
        LOGE("JNIThingsManagerExecuteActionSet: resource or jActionSetName is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, resource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerExecuteActionSet: Failed to get OCResource object!");
        return ocResult;
    }

    const char *actionSetNamePointer = env->GetStringUTFChars(jActionSetName, 0);
    if (NULL == actionSetNamePointer)
    {
        LOGE("JNIThingsManagerExecuteActionSet: Failed to convert jstring to char string!");
        return OC_STACK_NO_MEMORY;
    }

    std::string actionSetName(actionSetNamePointer);
    if (0 == delay)
    {
        ocResult = g_ThingsManager.executeActionSet(ocResource, actionSetName,
                   &ThingsManagerCallbacks::onPostResponse);
    }
    else
    {
        ocResult = g_ThingsManager.executeActionSet(ocResource, actionSetName,
                   (long int)delay,
                   &ThingsManagerCallbacks::onPostResponse);
    }
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerExecuteActionSet: executeActionSet is failed!");
    }

    env->ReleaseStringUTFChars(jActionSetName, actionSetNamePointer);
    LOGI("JNIThingsManagerExecuteActionSet: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerCancelActionSet(JNIEnv *env, jobject interfaceObject,
        jobject resource, jstring jActionSetName)
{
    LOGI("JNIThingsManagerCancelActionSet: Entry");

    if ((!resource) || (!jActionSetName))
    {
        LOGE("JNIThingsManagerCancelActionSet: resource or jActionSetName is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, resource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerCancelActionSet: Failed to get OCResource object!");
        return ocResult;
    }

    const char *actionSetNamePointer = env->GetStringUTFChars(jActionSetName, 0);
    if (NULL == actionSetNamePointer)
    {
        LOGE("JNIThingsManagerCancelActionSet: Failed to get character sequence from jstring!");
        return OC_STACK_NO_MEMORY;
    }

    std::string actionSetName(actionSetNamePointer);

    ocResult = g_ThingsManager.cancelActionSet(ocResource, actionSetName,
               &ThingsManagerCallbacks::onPostResponse);
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerCancelActionSet: cancelActionSet is failed!");
    }

    env->ReleaseStringUTFChars(jActionSetName, actionSetNamePointer);
    LOGI("JNIThingsManagerCancelActionSet: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerGetActionSet(JNIEnv *env, jobject interfaceObject,
        jobject resource,
        jstring jActionSetName)
{
    LOGI("JNIThingsManagerGetActionSet: Entry");

    if ((!resource) || (!jActionSetName))
    {
        LOGE("JNIThingsManagerGetActionSet: resource or jActionSetName is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, resource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerGetActionSet: Failed to get OCResource object!");
        return ocResult;
    }

    const char *actionSetNamePointer = env->GetStringUTFChars(jActionSetName, 0);
    std::string actionSetName(actionSetNamePointer);

    ocResult = g_ThingsManager.getActionSet(ocResource, actionSetName,
                                            &ThingsManagerCallbacks::onGetResponse);
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerGetActionSet: getActionSet is failed!");
    }

    env->ReleaseStringUTFChars(jActionSetName, actionSetNamePointer);
    LOGI("JNIThingsManagerGetActionSet: Exit");
    return ocResult;
}

JNIEXPORT jint JNICALL JNIThingsManagerDeleteActionSet(JNIEnv *env, jobject interfaceObject,
        jobject resource,
        jstring jActionSetName)
{
    LOGI("JNIThingsManagerDeleteActionSet: Entry");

    if ((!resource) || (!jActionSetName))
    {
        LOGE("JNIThingsManagerDeleteActionSet: resource or jActionSetName is NULL!");
        return OC_STACK_INVALID_PARAM;
    }

    OCStackResult ocResult = OC_STACK_ERROR;

    std::shared_ptr<OCResource> ocResource;
    JniOcResource *jniOcResource = JniOcResource::getJniOcResourcePtr(env, resource);
    if (jniOcResource)
    {
        ocResource = jniOcResource->getOCResource();
    }

    if (NULL == ocResource.get())
    {
        LOGE("JNIThingsManagerDeleteActionSet: Failed to get OCResource object!");
        return ocResult;
    }

    const char *actionSetNamePointer = env->GetStringUTFChars(jActionSetName, 0);
    std::string actionSetName(actionSetNamePointer);

    ocResult = g_ThingsManager.deleteActionSet(ocResource, actionSetName,
               &ThingsManagerCallbacks::onPutResponse);
    if (OC_STACK_OK != ocResult)
    {
        LOGE("JNIThingsManagerDeleteActionSet: deleteActionSet is failed!");
    }

    env->ReleaseStringUTFChars(jActionSetName, actionSetNamePointer);
    LOGI("JNIThingsManagerDeleteActionSet: Exit");
    return ocResult;
}

jobject ocResourceHandleToJava(JNIEnv *env, jlong resourceHandle)
{
    jclass resourceClass = GetJClass(TM_SERVICE_OCRESOURCEHANDLE_PATH);
    if (NULL == resourceClass)
    {
        LOGE("ocResourceHandleToJava : failed to find OCResourceHandle java class!");
        return NULL;
    }

    jmethodID constructor = env->GetMethodID(resourceClass, "<init>", "(J)V");
    if (NULL == constructor)
    {
        LOGE("ocResourceHandleToJava: Failed to get constructor method!");
        return NULL;
    }

    jobject resourceObj = (jobject) env->NewObject(resourceClass, constructor, resourceHandle);
    if (NULL == resourceObj)
    {
        LOGE("ocResourceHandleToJava: Failed to create OCResouceHandle java object!");
        return NULL;
    }

    return resourceObj;
}
