/*
* ******************************************************************
*
*  Copyright 2015 Samsung Electronics All Rights Reserved.
*
* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

#include "JniSecureUtils.h"
#include "JniOcStack.h"
#include "JniOcSecureResource.h"
#include "oic_malloc.h"
#include "srmutility.h"
#include "mbedtls/base64.h"

using namespace OC;

jobject JniSecureUtils::convertProvisionresultVectorToJavaList(JNIEnv *env, const OC::PMResultList_t *result)
{
    jobject jResultList = env->NewObject(g_cls_LinkedList, g_mid_LinkedList_ctor);
    if (!jResultList)
    {
        return nullptr;
    }

    for (size_t i = 0; i < result->size(); ++i)
    {
        char *Str = NULL;

        if (OC_STACK_OK != ConvertUuidToStr(&(result->at(i).deviceId), &Str))
        {
            return nullptr;
        }
        jstring jStr = env->NewStringUTF(Str);
        OICFree(Str);

        if (!jStr)
        {
            return nullptr;
        }
        jobject jresult = env->NewObject(
                g_cls_OcProvisionResult,
                g_mid_OcProvisionResult_ctor,
                jStr,
                static_cast<jint>(result->at(i).res)
                );
        if (!jresult)
        {
            return nullptr;
        }

        env->CallBooleanMethod(jResultList, g_mid_LinkedList_add_object, jresult);
        if (env->ExceptionCheck())
        {
            return nullptr;
        }
        env->DeleteLocalRef(jresult);
        env->DeleteLocalRef(jStr);
    }
    return jResultList;
}

jobjectArray JniSecureUtils::convertDeviceVectorToJavaArray(JNIEnv *env,
    std::vector<std::shared_ptr<OC::OCSecureResource>> &deviceListVector)
{
    jsize len = static_cast<jsize>(deviceListVector.size());
    jobjectArray devArr = env->NewObjectArray(len, g_cls_OcSecureResource, NULL);

    if (!devArr)
    {
        return nullptr;
    }

    for (jsize i = 0; i < len; ++i)
    {
        JniOcSecureResource *device = new JniOcSecureResource(deviceListVector[i]);
        jobject jDevice = env->NewObject(g_cls_OcSecureResource, g_mid_OcSecureResource_ctor);

        SetHandle<JniOcSecureResource>(env, jDevice, device);
        if (!jDevice)
        {
            return nullptr;
        }

        env->SetObjectArrayElement(devArr, i, jDevice);
        if (env->ExceptionCheck())
        {
            return nullptr;
        }
        env->DeleteLocalRef(jDevice);
    }
    return devArr;
}

jobject JniSecureUtils::convertUUIDVectorToJavaStrList(JNIEnv *env, UuidList_t &vector)
{
    jobject jList = env->NewObject(g_cls_LinkedList, g_mid_LinkedList_ctor);
    if (!jList)
    {
        return nullptr;
    }
    for (size_t i = 0; i < vector.size(); ++i)
    {
        char *Str = NULL;

        if (OC_STACK_OK != ConvertUuidToStr(&(vector[i]), &Str))
        {
            return nullptr;
        }
        jstring jStr = env->NewStringUTF(Str);
        OICFree(Str);

        if (!jStr)
        {
            return nullptr;
        }
        env->CallBooleanMethod(jList, g_mid_LinkedList_add_object, jStr);
        if (env->ExceptionCheck())
        {
            return nullptr;
        }
        env->DeleteLocalRef(jStr);
    }
    return jList;
}

static OicSecValidity_t* getValiditiesList(JNIEnv *env, jobject validityObject)
{
    jstring jData;
    jobjectArray  valList = (jobjectArray)env->CallObjectMethod(validityObject, g_mid_OcOicSecAcl_ace_get_validities);
    if (!valList || env->ExceptionCheck())
    {
        return nullptr;
    }
    int nr_validities = env->GetArrayLength(valList);

    OicSecValidity_t *valHead = NULL;

    for (int i = 0 ; i < nr_validities; i++)
    {
        OicSecValidity_t *tmp = (OicSecValidity_t*)OICCalloc(1, sizeof(OicSecValidity_t));
        jobject element = env->GetObjectArrayElement(valList, i);
        if (!element || env->ExceptionCheck())
        {
            return nullptr;
        }

        jData = (jstring)env->CallObjectMethod(element, g_mid_OcOicSecAcl_validity_get_getPeriod);
        if (!jData || env->ExceptionCheck())
        {
            return nullptr;
        }
        tmp->period = (char*)env->GetStringUTFChars(jData, 0);

        jint jrecurrenceLen = (jint) env->CallIntMethod(element,
                g_mid_OcOicSecAcl_validity_get_recurrenceLen);
        tmp->recurrenceLen = (int)jrecurrenceLen;

        if (jrecurrenceLen > 0)
        {
            jvalue argv[1];
            tmp->recurrences = (char**)OICCalloc(jrecurrenceLen, sizeof(char*));

            for (int j = 0 ; j < jrecurrenceLen; j++)
            {
                argv[0].i = j;
                jData = (jstring)env->CallObjectMethodA(element, g_mid_OcOicSecAcl_validity_get_recurrences, argv);
                if (!jData || env->ExceptionCheck())
                {
                    return nullptr;
                }
                tmp->recurrences[j] = (char*)env->GetStringUTFChars(jData, 0);
            }
        }
        if (NULL == valHead)
        {
            valHead = tmp;
        }
        else
        {
            OicSecValidity_t *ptr = valHead;
            while(ptr->next != NULL) ptr = ptr->next;
            ptr->next = tmp;
            tmp->next = NULL;
        }
        env->DeleteLocalRef(element);
    }
    return valHead;
}

static OicSecRsrc_t * getResourcesList(JNIEnv *env, jobject resourceObject)
{
    jstring jData;

    jobjectArray rescList = (jobjectArray)env->CallObjectMethod(resourceObject, g_mid_OcOicSecAcl_ace_get_resources);
    if (!rescList || env->ExceptionCheck())
    {
        return nullptr;
    }

    int nr_resc = env->GetArrayLength(rescList);
    OicSecRsrc_t *rescHead = NULL;

    for (int i = 0 ; i < nr_resc; i++)
    {
        OicSecRsrc_t *tmp = (OicSecRsrc_t*)OICCalloc(1, sizeof(OicSecRsrc_t));
        jobject element = env->GetObjectArrayElement(rescList, i);
        if (!element || env->ExceptionCheck())
        {
            return nullptr;
        }
        jData = (jstring)env->CallObjectMethod(element, g_mid_OcOicSecAcl_resr_get_href);
        if (!jData || env->ExceptionCheck())
        {
            return nullptr;
        }
        tmp->href = (char*)env->GetStringUTFChars(jData, 0);

        jData = (jstring)env->CallObjectMethod(element, g_mid_OcOicSecAcl_resr_get_rel);
        if (!jData || env->ExceptionCheck())
        {
            return nullptr;
        }
        tmp->rel = (char*)env->GetStringUTFChars(jData, 0);

        jint len = (jint) env->CallIntMethod(element, g_mid_OcOicSecAcl_resr_get_typeLen);
        tmp->typeLen = (int)len;
        if (len > 0)
        {
            jvalue argv[1];
            tmp->types = (char**)OICCalloc(len, sizeof(char*));

            for (int j = 0 ; j < len; j++)
            {
                argv[0].i = j;
                jData = (jstring)env->CallObjectMethodA(element, g_mid_OcOicSecAcl_resr_get_types, argv);
                if (!jData || env->ExceptionCheck())
                {
                    return nullptr;
                }
                tmp->types[j] = (char*)env->GetStringUTFChars(jData, 0);
            }
        }

        len = (jint) env->CallIntMethod(element, g_mid_OcOicSecAcl_resr_get_interfaceLen);
        tmp->interfaceLen = len;
        if (len > 0)
        {
            jvalue argv[1];
            tmp->interfaces = (char**)OICCalloc(len, sizeof(char*));

            for (int j = 0 ; j < len; j++)
            {
                argv[0].i = j;
                jData = (jstring)env->CallObjectMethodA(element, g_mid_OcOicSecAcl_resr_get_interfaces, argv);
                if (!jData || env->ExceptionCheck())
                {
                    return nullptr;
                }
                tmp->interfaces[j] = (char*)env->GetStringUTFChars(jData, 0);
            }
        }

        if (NULL == rescHead)
        {
            rescHead = tmp;
        }
        else
        {
            OicSecRsrc_t *ptr = rescHead;
            while(ptr->next != NULL) ptr = ptr->next;
            ptr->next = tmp;
            tmp->next = NULL;
        }
        env->DeleteLocalRef(element);
    }
    return rescHead;
}

OCStackResult JniSecureUtils::convertJavaACLToOCAcl(JNIEnv *env, jobject in, OicSecAcl_t *acl)
{
    jstring jData;

    jData = (jstring) env->CallObjectMethod(in, g_mid_OcOicSecAcl_get_rownerID);
    if (!jData || env->ExceptionCheck())
    {
        return OC_STACK_ERROR;
    }

    char *str = (char*) env->GetStringUTFChars(jData, 0);
    if (OC_STACK_OK == ConvertStrToUuid(str, &acl->rownerID))
    {
        env->ReleaseStringUTFChars(jData, str);
    }
    else
    {
        return OC_STACK_ERROR;
    }

    jobjectArray acesList = (jobjectArray)env->CallObjectMethod(in, g_mid_OcOicSecAcl_get_aces);

    if (!acesList || env->ExceptionCheck())
    {
        return OC_STACK_ERROR;
    }

    int nr_aces = env->GetArrayLength(acesList);

    OicSecAce_t *acesHead = NULL;

    for (int i = 0 ; i < nr_aces; i++)
    {
        OicSecAce_t *tmp = (OicSecAce_t*)OICCalloc(1, sizeof(OicSecAce_t));

        jobject element = env->GetObjectArrayElement(acesList, i);
        if (!element || env->ExceptionCheck())
        {
            return OC_STACK_ERROR;
        }

        jData = (jstring) env->CallObjectMethod(element, g_mid_OcOicSecAcl_ace_get_subjectID);
        if (!jData || env->ExceptionCheck())
        {
            return OC_STACK_ERROR;
        }

        str = (char*) env->GetStringUTFChars(jData, 0);
        if (OC_STACK_OK == ConvertStrToUuid(str, &tmp->subjectuuid))
        {
            env->ReleaseStringUTFChars(jData, str);
        }
        else
        {
            return OC_STACK_ERROR;
        }

        jint perm = (jint)env->CallIntMethod(element, g_mid_OcOicSecAcl_ace_get_permissions);
        tmp->permission = (uint16_t)perm;
        if (nullptr == (tmp->resources = getResourcesList(env, element)))
        {
            return OC_STACK_ERROR;
        }

        tmp->validities = NULL; //TODO Seems Validities CBOR conversion is broken in C stack
#if 0
        if (nullptr == (tmp->validities = getValiditiesList(env, element)))
        {
            return OC_STACK_ERROR;
        }
#endif
        if (NULL == acesHead)
        {
            acesHead = tmp;
        }
        else
        {
            OicSecAce_t *ptr = acesHead;
            while(ptr->next != NULL) ptr = ptr->next;
            ptr->next = tmp;
            tmp->next = NULL;
        }
    }
    acl->aces = acesHead;
    return OC_STACK_OK;
}

OCStackResult JniSecureUtils::convertJavaPdACLToOCAcl(JNIEnv *env, jobject in, OicSecPdAcl_t *pdacl)
{
    jstring jData;
    jvalue args[1];

    jint jCount = (jint) env->CallIntMethod(in, g_mid_OcOicSecPdAcl_get_resources_cnt);
    if (!jCount || env->ExceptionCheck())
    {
        return OC_STACK_ERROR;
    }

    pdacl->resourcesLen = jCount;
    pdacl->resources = new char*[jCount];

    if (!pdacl->resources)
    {
        return OC_STACK_ERROR;
    }
    for (jint i = 0; i < jCount; ++i)
    {
        args[0].i = i;
        jData = (jstring) env->CallObjectMethodA(in, g_mid_OcOicSecPdAcl_get_resources, args);
        if (!jData || env->ExceptionCheck())
        {
            return OC_STACK_ERROR;
        }

        pdacl->resources[i] = (char*) env->GetStringUTFChars(jData, 0);
    }

    jCount = (jint) env->CallIntMethod(in, g_mid_OcOicSecPdAcl_get_permission);
    if (env->ExceptionCheck())
    {
        return OC_STACK_ERROR;
    }

    pdacl->permission = static_cast<uint16_t>(jCount);
    jCount = (jint) env->CallIntMethod(in, g_mid_OcOicSecPdAcl_get_periods_cnt);
    if (env->ExceptionCheck())
    {
        return OC_STACK_ERROR;
    }

    pdacl->prdRecrLen = jCount;
    if (jCount)
    {
        pdacl->periods = new char*[jCount];
        if (!pdacl->periods)
        {
            return OC_STACK_ERROR;
        }
    }
    for (jint i = 0; i < jCount; ++i)
    {
        args[0].i = i;
        jData = (jstring) env->CallObjectMethodA(in, g_mid_OcOicSecPdAcl_get_periods, args);
        if (!jData || env->ExceptionCheck())
        {
            return OC_STACK_ERROR;
        }

        pdacl->periods[i] = (char*) env->GetStringUTFChars(jData, 0);
    }

    if (jCount)
    {
        pdacl->recurrences = new char*[jCount];
        if (!pdacl->recurrences)
        {
            return OC_STACK_ERROR;
        }
    }
    for (jint i = 0; i < jCount; ++i)
    {
        args[0].i = i;
        jData = (jstring) env->CallObjectMethodA(in, g_mid_OcOicSecPdAcl_get_recurrences, args);
        if (!jData ||  env->ExceptionCheck())
        {
            return OC_STACK_ERROR;
        }

        pdacl->recurrences[i] = (char*) env->GetStringUTFChars(jData, 0);
    }
    return OC_STACK_OK;
}
