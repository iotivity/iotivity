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

#include "PMCsdkHelper.h"
#include "PMCsdkUtilityHelper.h"

OCDevAddr PMCsdkUtilityHelper::getOCDevAddrEndPoint()
{
    OCDevAddr endPoint;

    memset(&endPoint, 0, sizeof(endPoint));
    strncpy(endPoint.addr, DEFAULT_HOST, sizeof(endPoint.addr));
    endPoint.port = OC_MULTICAST_PORT;

    return endPoint;
}

static void readStringArray(stringArray_t *list, int length, const char* description,
        const char* example)
{
    int i = 0;
    int count = 1;
    char hint[MAX_STRING_LENGTH] =
    { 0 };

    char **item = NULL;

    item = OICCalloc(count, sizeof(char*));

    for (i = 0; i < count; i++)
    {
        item[i] = OICCalloc(length, sizeof(char));

        OICStrdup(example);
        //readString(item[i], length, "", example);
    }
    list->array = item;
    list->length = count;
    return;
}

cloudAce_t* PMCsdkUtilityHelper::createCloudAces()
{

    int acllistCount = 1;
    cloudAce_t *aces = OICCalloc(acllistCount, sizeof(cloudAce_t));
//    readString(aclid, sizeof(aclid), "ace id", ACL_ID_EXAMPLE);

    int i;

    for (i = 0; i < acllistCount; i++)
    {
        cloudAce_t *ace = &aces[i];
        if (i != acllistCount - 1)
            ace->next = &aces[i + 1];

        char aceid[MAX_ID_LENGTH] =
        { 0 };
        char subjectuuid[MAX_ID_LENGTH] =
        { 0 };
        int stype = 0;
        int permission = 0;

        ace->aceId = OICStrdup(ACE_ID_EXAMPLE);
        ace->stype = 0;
        ace->permission = 31;
        memcpy(&ace->subjectuuid, SUBJECT_ID_EXAMPLE, sizeof(OicUuid_t));

        int reslist_count = 1;

        ace->resources = OICCalloc(reslist_count, sizeof(OicSecRsrc_t));

        int j;

        for (j = 0; j < reslist_count; j++)
        {
            OicSecRsrc_t *res = &ace->resources[j];
            if (j != reslist_count - 1)
                res->next = &ace->resources[j + 1];

            char href[32] =
            { 0 };

            stringArray_t rt =
            { 0, 0 };
            readStringArray(&rt, MAX_ID_LENGTH, "resource type", RESOURCE_TYPE_EXAMPLE);

            stringArray_t _if =
            { 0, 0 };
            readStringArray(&_if, MAX_ID_LENGTH, "interface", INTERFACE_EXAMPLE);

            res->href = OICStrdup(RESOURCE_URI_EXAMPLE);
            res->types = rt.array;
            res->typeLen = rt.length;
            res->interfaces = _if.array;
            res->interfaceLen = _if.length;
        }
    }

    return aces;
}

OCProvisionDev_t* PMCsdkUtilityHelper::getDevInst(OCProvisionDev_t* dev_lst, const int dev_num)
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

int PMCsdkUtilityHelper::printDevList(OCProvisionDev_t* dev_lst)
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

size_t PMCsdkUtilityHelper::printUuidList(const OCUuidList_t* uid_lst)
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

int PMCsdkUtilityHelper::printResultList(const OCProvisionResult_t* rslt_lst, const int rslt_cnt)
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
        printf(" - result: %s\n", PMCsdkUtilityHelper::getOCStackResult(rslt_lst[lst_cnt].res));
    }
    printf("\n");

    IOTIVITYTEST_LOG(INFO, "[PMHelper] printResultList IN");
    return lst_cnt;
}

void PMCsdkUtilityHelper::printUuid(const OicUuid_t* uid)
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

char* PMCsdkUtilityHelper::getOCStackResult(OCStackResult ocstackresult)
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
            break;
    }

    return resultString;
}

bool PMCsdkUtilityHelper::isEqual(OicUuid_t a, OicUuid_t b)
{
    for (int i = 0; i < UUID_LENGTH; i++)
    {
        if (a.id[i] != b.id[i])
            return false;
    }
    return true;
}
