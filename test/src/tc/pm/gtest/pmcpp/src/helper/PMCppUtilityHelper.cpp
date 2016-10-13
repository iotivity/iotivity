/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      LICENSE-2.0" target="_blank">http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

#include "PMCppHelper.h"
#include "PMCppUtilityHelper.h"

OCDevAddr PMCppUtilityHelper::getOCDevAddrEndPoint()
{
    OCDevAddr endPoint;

    memset(&endPoint, 0, sizeof(endPoint));
    strncpy(endPoint.addr, DEFAULT_HOST, sizeof(endPoint.addr));
    endPoint.port = OC_MULTICAST_PORT;

    return endPoint;
}

OCProvisionDev_t* PMCppUtilityHelper::getDevInst(OCProvisionDev_t* dev_lst, const int dev_num)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getDevInst IN");

    if (!dev_lst || 0 >= dev_num)
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] Device List is Empty");
        return NULL;
    }

    OCProvisionDev_t* lst = (OCProvisionDev_t*) dev_lst;

    for (int i = 0; lst;)
    {
        if (dev_num == ++i)
        {
            IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getDevInst OUT");
            return lst;
        }
        lst = lst->next;
    }

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] getDevInst OUT");
    return NULL; // in here |lst| is always |NULL|
}

int PMCppUtilityHelper::printDevList(OCProvisionDev_t* dev_lst)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printDevList IN");

    if (!dev_lst)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] Device List is Empty...");
        return 0;
    }

    OCProvisionDev_t* lst = (OCProvisionDev_t*) dev_lst;
    int lst_cnt = 0;

    for (; lst;)
    {
        printf("     [%d] ", ++lst_cnt);
        printUuid((const OicUuid_t*) &lst->doxm->deviceID);
        printf("\n");
        lst = lst->next;
    }
    printf("\n");

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printDevList OUT");
    return lst_cnt;
}

size_t PMCppUtilityHelper::printUuidList(const OCUuidList_t* uid_lst)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printUuidList IN");

    if (!uid_lst)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] Device List is Empty...");
        return 0;
    }

    OCUuidList_t* lst = (OCUuidList_t*) uid_lst;
    size_t lst_cnt = 0;

    for (; lst;)
    {
        printf("     [%zu] ", ++lst_cnt);
        printUuid((const OicUuid_t*) &lst->dev);
        printf("\n");
        lst = lst->next;
    }
    printf("\n");

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printUuidList OUT");
    return lst_cnt;
}

int PMCppUtilityHelper::printResultList(const OCProvisionResult_t* rslt_lst, const int rslt_cnt)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printResultList IN");

    if (!rslt_lst || 0 >= rslt_cnt)
    {
        IOTIVITYTEST_LOG(INFO, "[PMHelper] Device List is Empty...");
        return 0;
    }

    int lst_cnt = 0;
    for (; rslt_cnt > lst_cnt; ++lst_cnt)
    {
        printf("     [%d] ", lst_cnt + 1);
        printUuid((const OicUuid_t*) &rslt_lst[lst_cnt].deviceId);
        printf(" - result: %s\n", PMCppUtilityHelper::getOCStackResult(rslt_lst[lst_cnt].res));
    }
    printf("\n");

    IOTIVITYTEST_LOG(INFO, "[PMHelper] printResultList IN");
    return lst_cnt;
}

void PMCppUtilityHelper::printUuid(const OicUuid_t* uid)
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printUuid IN");

    for (int i = 0; i < UUID_LENGTH;)
    {
        printf("%02X", (*uid).id[i++]);
        if (i == 4 || i == 6 || i == 8 || i == 10) // canonical format for UUID has '8-4-4-4-12'
        {
            printf(DASH);
        }
    }
    printf("\n");

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printUuid OUT");
}

char* PMCppUtilityHelper::getOCStackResult(OCStackResult ocstackresult)
{
    char* resultString = NULL;

    switch (ocstackresult)
    {
        case OC_STACK_OK:
            resultString = (char*) "OC_STACK_OK";
            break;
        case OC_STACK_RESOURCE_CREATED:
            resultString = (char*) "OC_STACK_RESOURCE_CREATED";
            break;
        case OC_STACK_RESOURCE_DELETED:
            resultString = (char*) "OC_STACK_RESOURCE_DELETED";
            break;
        case OC_STACK_INVALID_URI:
            resultString = (char*) "OC_STACK_INVALID_URI";
            break;
        case OC_STACK_INVALID_QUERY:
            resultString = (char*) "OC_STACK_INVALID_QUERY";
            break;
        case OC_STACK_INVALID_IP:
            resultString = (char*) "OC_STACK_INVALID_IP";
            break;
        case OC_STACK_INVALID_PORT:
            resultString = (char*) "OC_STACK_INVALID_PORT";
            break;
        case OC_STACK_INVALID_CALLBACK:
            resultString = (char*) "OC_STACK_INVALID_CALLBACK";
            break;
        case OC_STACK_INVALID_METHOD:
            resultString = (char*) "OC_STACK_INVALID_METHOD";
            break;
        case OC_STACK_INVALID_PARAM:
            resultString = (char*) "OC_STACK_INVALID_PARAM";
            break;
        case OC_STACK_INVALID_OBSERVE_PARAM:
            resultString = (char*) "OC_STACK_INVALID_OBSERVE_PARAM";
            break;
        case OC_STACK_NO_MEMORY:
            resultString = (char*) "OC_STACK_NO_MEMORY";
            break;
        case OC_STACK_COMM_ERROR:
            resultString = (char*) "OC_STACK_COMM_ERROR";
            break;
        case OC_STACK_TIMEOUT:
            resultString = (char*) "OC_STACK_TIMEOUT";
            break;
        case OC_STACK_ADAPTER_NOT_ENABLED:
            resultString = (char*) "OC_STACK_ADAPTER_NOT_ENABLED";
            break;
        case OC_STACK_NOTIMPL:
            resultString = (char*) "OC_STACK_NOTIMPL";
            break;
        case OC_STACK_NO_RESOURCE:
            resultString = (char*) "OC_STACK_NO_RESOURCE";
            break;
        case OC_STACK_UNAUTHORIZED_REQ:
            resultString = (char*) "OC_STACK_UNAUTHORIZED_REQ";
            break;
        case OC_STACK_ERROR:
            resultString = (char*) "OC_STACK_ERROR";
            break;
        default:
            resultString = (char*) "UNKNOWN_STATE";
    }

    return resultString;
}
