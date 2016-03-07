/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include <jni.h>
#include "cacommon.h"
#include "logger.h"
#include "cacommonutil.h"
#include "camanagerleutil.h"
#include "uarraylist.h"
#include "camutex.h"
#include "camanagerdevice.h"
#include "oic_malloc.h"

#define TAG "OIC_CA_MANAGER_DEVICE"

static u_arraylist_t *g_deviceACDataList = NULL;
static ca_mutex g_deviceACDataListMutex = NULL;
static bool g_isBTRecovery = false;

jstring CAManagerGetLEAddressFromACData(JNIEnv *env, size_t idx)
{
    OIC_LOG(DEBUG, TAG, "CAManagerGetLEAddressFromACData");
    OIC_LOG_V(DEBUG, TAG, "idx : %d", idx);
    if (idx <= u_arraylist_length(g_deviceACDataList))
    {
        CAManagerACData_t *curData = (CAManagerACData_t *) u_arraylist_get(
                g_deviceACDataList, idx);
        if (!curData)
        {
            OIC_LOG(ERROR, TAG, "curData is null");
            return NULL;
        }

        const char* address = (*env)->GetStringUTFChars(env, curData->address, NULL);
        if (!address)
        {
            OIC_LOG(ERROR, TAG, "address is null");
            return NULL;
        }
        OIC_LOG_V(INFO, TAG, "found out target address : %s", address);
        (*env)->ReleaseStringUTFChars(env, curData->address, address);

        return curData->address;
    }
    return NULL;
}

void CAManagerCreateACDataList(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "CAManagerCreateACDataList");
    VERIFY_NON_NULL_VOID(env, TAG, "env");

    ca_mutex_lock(g_deviceACDataListMutex);
    // create new object array
    if (g_deviceACDataList == NULL)
    {
        OIC_LOG(DEBUG, TAG, "Create AC Data list");

        g_deviceACDataList = u_arraylist_create();
    }
    ca_mutex_unlock(g_deviceACDataListMutex);
}

CAResult_t CAManagerInitMutexVaraibles()
{
    if (NULL == g_deviceACDataListMutex)
    {
        g_deviceACDataListMutex = ca_mutex_new();
        if (NULL == g_deviceACDataListMutex)
        {
            OIC_LOG(ERROR, TAG, "ca_mutex_new has failed");
            return CA_STATUS_FAILED;
        }
    }
    return CA_STATUS_OK;
}

void CAManagerTerminateMutexVariables()
{
    if (g_deviceACDataListMutex)
    {
        ca_mutex_free(g_deviceACDataListMutex);
        g_deviceACDataListMutex = NULL;
    }
}

static CAManagerACData_t *CAManagerCreateACData(jstring jaddress)
{
    OIC_LOG(DEBUG, TAG, "IN-CAManagerCreateACData");
    VERIFY_NON_NULL_RET(jaddress, TAG, "jaddress", NULL);

    // create block data
    CAManagerACData_t *data = (CAManagerACData_t *) OICCalloc(1, sizeof(*data));
    if (!data)
    {
        OIC_LOG(ERROR, TAG, "memory alloc has failed");
        return NULL;
    }

    data->address = jaddress;
    data->isAutoConnect = false;

    OIC_LOG(DEBUG, TAG, "OUT-CAManagerCreateACData");
    return data;
}

bool CAManagerIsMatchedACData(JNIEnv *env, jstring jaddress)
{
    VERIFY_NON_NULL_RET(env, TAG, "env", NULL);
    VERIFY_NON_NULL_RET(jaddress, TAG, "jaddress", false);

    const char* address = (*env)->GetStringUTFChars(env, jaddress, NULL);
    if (!address)
    {
        OIC_LOG(ERROR, TAG, "address is null");
        return false;
    }

    size_t length = u_arraylist_length(g_deviceACDataList);
    for (size_t idx = 0; idx < length; idx++)
    {
        CAManagerACData_t *curData = (CAManagerACData_t *) u_arraylist_get(g_deviceACDataList,
                                                                           idx);
        if (!curData)
        {
            OIC_LOG(ERROR, TAG, "curData is null");
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            return false;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, curData->address, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "address is null");
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            return false;
        }

        if (!strcmp(setAddress, address))
        {
            (*env)->ReleaseStringUTFChars(env, curData->address, setAddress);
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            return true;
        }
        (*env)->ReleaseStringUTFChars(env, curData->address, setAddress);
    }
    (*env)->ReleaseStringUTFChars(env, jaddress, address);

    return false;
}

void CAManagerAddACData(JNIEnv *env, jstring jaddress)
{
    OIC_LOG(DEBUG, TAG, "IN-CAManagerAddACData");

    VERIFY_NON_NULL_VOID(env, TAG, "env");
    VERIFY_NON_NULL_VOID(jaddress, TAG, "jaddress");

    ca_mutex_lock(g_deviceACDataListMutex);

    if(!CAManagerIsMatchedACData(env, jaddress))
    {
        OIC_LOG(DEBUG, TAG, "ACdata will be added");
        // add CAManagerACData
        jobject gaddress = (*env)->NewGlobalRef(env, jaddress);

        CAManagerACData_t *data = CAManagerCreateACData(gaddress);
        u_arraylist_add(g_deviceACDataList, data);
    }
    ca_mutex_unlock(g_deviceACDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-CAManagerAddACData");
}

CAResult_t CAManagerRemoveData(JNIEnv *env, jstring jaddress)
{
    OIC_LOG(DEBUG, TAG, "IN-CAManagerRemoveData");
    VERIFY_NON_NULL(env, TAG, "env");
    VERIFY_NON_NULL(jaddress, TAG, "jaddress");

    ca_mutex_lock(g_deviceACDataListMutex);

    const char* address = (*env)->GetStringUTFChars(env, jaddress, NULL);
    if (!address)
    {
        OIC_LOG(ERROR, TAG, "address is null");
        ca_mutex_unlock(g_deviceACDataListMutex);
        return CA_STATUS_FAILED;
    }

    OIC_LOG_V(DEBUG, TAG, "(%s) will be removed", address);

    size_t length = u_arraylist_length(g_deviceACDataList);
    for (size_t idx = 0; idx < length; idx++)
    {
        CAManagerACData_t *curData = (CAManagerACData_t *) u_arraylist_get(g_deviceACDataList,
                                                                           idx);
        if (!curData)
        {
            OIC_LOG(ERROR, TAG, "curData is null");
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            ca_mutex_unlock(g_deviceACDataListMutex);
            return CA_STATUS_FAILED;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, curData->address, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "address is null");
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            ca_mutex_unlock(g_deviceACDataListMutex);
            return CA_STATUS_FAILED;
        }

        if (!strcmp(setAddress, address))
        {
            if (NULL == u_arraylist_remove(g_deviceACDataList, idx))
            {
                OIC_LOG(ERROR, TAG, "removal has failed.");
                (*env)->ReleaseStringUTFChars(env, jaddress, address);
                (*env)->ReleaseStringUTFChars(env, curData->address, setAddress);
                ca_mutex_unlock(g_deviceACDataListMutex);
                return CA_STATUS_FAILED;
            }

            (*env)->ReleaseStringUTFChars(env, curData->address, setAddress);

            if (curData->address)
            {
                (*env)->DeleteGlobalRef(env, curData->address);
            }

            OICFree(curData);
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            ca_mutex_unlock(g_deviceACDataListMutex);
            OIC_LOG(DEBUG, TAG, "remove done");
            return CA_STATUS_OK;
        }
        (*env)->ReleaseStringUTFChars(env, curData->address, setAddress);
    }

    (*env)->ReleaseStringUTFChars(env, jaddress, address);
    ca_mutex_unlock(g_deviceACDataListMutex);
    OIC_LOG(DEBUG, TAG, "OUT-CAManagerRemoveData");
    return CA_STATUS_OK;
}

CAResult_t CAManagerRemoveAllData(JNIEnv *env)
{
    OIC_LOG(DEBUG, TAG, "IN-CAManagerRemoveAllData");
    VERIFY_NON_NULL(env, TAG, "env");

    ca_mutex_lock(g_deviceACDataListMutex);

    size_t length = u_arraylist_length(g_deviceACDataList);
    for (size_t idx = 0; idx < length; idx++)
    {
        CAManagerACData_t *curData = (CAManagerACData_t *) u_arraylist_get(g_deviceACDataList,
                                                                           idx);
        if (!curData)
        {
            OIC_LOG(ERROR, TAG, "curData is null");
            ca_mutex_unlock(g_deviceACDataListMutex);
            return CA_STATUS_FAILED;
        }

        if (NULL == u_arraylist_remove(g_deviceACDataList, idx))
        {
            OIC_LOG(ERROR, TAG, "removal has failed.");
            ca_mutex_unlock(g_deviceACDataListMutex);
            return CA_STATUS_FAILED;
        }

        if (curData->address)
        {
            (*env)->DeleteGlobalRef(env, curData->address);
        }

        OICFree(curData);
    }
    ca_mutex_unlock(g_deviceACDataListMutex);
    OIC_LOG(DEBUG, TAG, "OUT-CAManagerRemoveAllData");
    return CA_STATUS_OK;
}

bool CAManagerGetAutoConnectionFlag(JNIEnv *env, jstring jaddress)
{
    OIC_LOG(DEBUG, TAG, "IN-CAManagerGetAutoConnectionFlag");
    VERIFY_NON_NULL_RET(env, TAG, "env", NULL);
    VERIFY_NON_NULL_RET(jaddress, TAG, "jaddress", NULL);

    ca_mutex_lock(g_deviceACDataListMutex);

    const char* address = (*env)->GetStringUTFChars(env, jaddress, NULL);
    if (!address)
    {
        OIC_LOG(ERROR, TAG, "address is null");
        ca_mutex_unlock(g_deviceACDataListMutex);
        return CA_STATUS_FAILED;
    }

    size_t length = u_arraylist_length(g_deviceACDataList);
    for (size_t idx = 0; idx < length; idx++)
    {
        CAManagerACData_t *curData = (CAManagerACData_t *) u_arraylist_get(g_deviceACDataList,
                                                                           idx);
        if (!curData)
        {
            OIC_LOG(ERROR, TAG, "curData is null");
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            ca_mutex_unlock(g_deviceACDataListMutex);
            return CA_STATUS_FAILED;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, curData->address, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "setAddress is null");
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            ca_mutex_unlock(g_deviceACDataListMutex);
            return CA_STATUS_FAILED;
        }

        if (!strcmp(setAddress, address))
        {
            (*env)->ReleaseStringUTFChars(env, curData->address, setAddress);
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            ca_mutex_unlock(g_deviceACDataListMutex);
            OIC_LOG_V(DEBUG, TAG, "flag is %d", curData->isAutoConnect);
            return curData->isAutoConnect;
        }
        (*env)->ReleaseStringUTFChars(env, curData->address, setAddress);
    }
    (*env)->ReleaseStringUTFChars(env, jaddress, address);
    ca_mutex_unlock(g_deviceACDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-CAManagerGetAutoConnectionFlag");
    return false;
}

void CAManagerSetAutoConnectionFlag(JNIEnv *env, jstring jaddress, bool flag)
{
    OIC_LOG(DEBUG, TAG, "IN-CAManagerSetAutoConnectionFlag");
    VERIFY_NON_NULL_VOID(env, TAG, "env");
    VERIFY_NON_NULL_VOID(jaddress, TAG, "jaddress");

    ca_mutex_lock(g_deviceACDataListMutex);

    const char* address = (*env)->GetStringUTFChars(env, jaddress, NULL);
    if (!address)
    {
        OIC_LOG(ERROR, TAG, "address is null");
        ca_mutex_unlock(g_deviceACDataListMutex);
        return;
    }

    size_t length = u_arraylist_length(g_deviceACDataList);
    for (size_t idx = 0; idx < length; idx++)
    {
        CAManagerACData_t *curData = (CAManagerACData_t *) u_arraylist_get(g_deviceACDataList,
                                                                           idx);
        if (!curData)
        {
            OIC_LOG(ERROR, TAG, "curData is null");
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            ca_mutex_unlock(g_deviceACDataListMutex);
            return;
        }

        const char* setAddress = (*env)->GetStringUTFChars(env, curData->address, NULL);
        if (!setAddress)
        {
            OIC_LOG(ERROR, TAG, "address is null");
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            ca_mutex_unlock(g_deviceACDataListMutex);
            return;
        }

        if (!strcmp(setAddress, address))
        {
            OIC_LOG_V(DEBUG, TAG, "flag is set to %d", flag);
            curData->isAutoConnect = flag;
            (*env)->ReleaseStringUTFChars(env, curData->address, setAddress);
            (*env)->ReleaseStringUTFChars(env, jaddress, address);
            ca_mutex_unlock(g_deviceACDataListMutex);
            return;
        }
        (*env)->ReleaseStringUTFChars(env, curData->address, setAddress);
    }
    (*env)->ReleaseStringUTFChars(env, jaddress, address);
    ca_mutex_unlock(g_deviceACDataListMutex);

    OIC_LOG(DEBUG, TAG, "OUT-CAManagerSetAutoConnectionFlag");
}

size_t CAManagerGetACDataLength()
{
    return u_arraylist_length(g_deviceACDataList);
}

void CAManagerSetBTRecovery(bool flag)
{
    g_isBTRecovery = flag;
    OIC_LOG_V(DEBUG, TAG, "BT recovery flag : %d", g_isBTRecovery);
}

bool CAManagerIsRecoveryFlagSet()
{
    return g_isBTRecovery;
}
