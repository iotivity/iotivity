/******************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/
#include "CSCsdkUtilityHelper.h"

OCDevAddr CSCsdkUtilityHelper::getOCDevAddrEndPoint()
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
    OC_UNUSED(description);
    int i = 0;
    int count = 1;

    char **item = NULL;

    item = (char**) OICCalloc(count, sizeof(char*));

    for (i = 0; i < count; i++)
    {
        item[i] = (char*) OICCalloc(length, sizeof(char));
        OICStrdup(example);
    }
    list->array = item;
    list->length = count;
    return;
}

cloudAce_t* CSCsdkUtilityHelper::createCloudAces()
{

    int acllistCount = 1;
    cloudAce_t *aces = NULL;

    int i;

    for (i = 0; i < acllistCount; i++)
    {
        cloudAce_t *ace = (cloudAce_t*) OICCalloc(1, sizeof(cloudAce_t));
        if (!ace)
        {
            IOTIVITYTEST_LOG(ERROR, "Can't allocate memory for ace");
        }
        LL_APPEND(aces, ace);

        char aceid[MAX_ID_LENGTH] =
        { 0 };

        if (OC_STACK_OK != ConvertStrToUuid(SUBJECT_ID_EXAMPLE, &ace->subjectuuid))
        {
            IOTIVITYTEST_LOG(ERROR, "Adding Subject ID Failed");
        }

        ace->aceId = OICStrdup(aceid);
        ace->stype = 0;
        ace->permission = 0;

        int reslist_count = 1;

        int j;

        for (j = 0; j < reslist_count; j++)
        {
            OicSecRsrc_t *res = (OicSecRsrc_t*) OICCalloc(1, sizeof(OicSecRsrc_t));
            if (!res)
            {
                IOTIVITYTEST_LOG(ERROR, "Adding Resource ID Failed");
            }
            LL_APPEND(ace->resources, res);

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

OCProvisionDev_t* CSCsdkUtilityHelper::getDevInst(OCProvisionDev_t* dev_lst, const int dev_num)
{
    __FUNC_IN__

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

    __FUNC_OUT__
    return NULL; // in here |lst| is always |NULL|
}

int CSCsdkUtilityHelper::printDevList(OCProvisionDev_t* dev_lst)
{
    __FUNC_IN__

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

    __FUNC_OUT__
    return lst_cnt;
}

size_t CSCsdkUtilityHelper::printUuidList(const OCUuidList_t* uid_lst)
{
    __FUNC_IN__

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

    __FUNC_OUT__
    return lst_cnt;
}

int CSCsdkUtilityHelper::printResultList(const OCProvisionResult_t* rslt_lst, const int rslt_cnt)
{
    __FUNC_IN__

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
        printf(" - result: %s\n", CommonUtil::getOCStackResult(rslt_lst[lst_cnt].res));
    }
    printf("\n");

    __FUNC_OUT__
    return lst_cnt;
}

void CSCsdkUtilityHelper::printUuid(const OicUuid_t* uid)
{
    __FUNC_IN__

    for (int i = 0; i < UUID_LENGTH;)
    {
        printf("%02X", (*uid).id[i++]);
        if (i == 4 || i == 6 || i == 8 || i == 10) // canonical format for UUID has '8-4-4-4-12'
        {
            printf("%c", DASH);
        }
    }
    printf("\n");

    __FUNC_OUT__
}

/**
 * Function to set failure message
 */
std::string CSCsdkUtilityHelper::setFailureMessage(OCStackResult actualResult,
        OCStackResult expectedResult)
{
    std::string errorMessage("\033[1;31m[Error] Expected : ");
    errorMessage.append(CommonUtil::getOCStackResult(expectedResult));
    errorMessage.append("\033[0m  \033[1;31mActual : ");
    errorMessage.append(CommonUtil::getOCStackResult(actualResult));
    errorMessage.append("\033[0m");
    return errorMessage;
}

/**
 * Function to set failure message
 */
std::string CSCsdkUtilityHelper::setFailureMessage(std::string errorMessage)
{
    std::string retErrorMessage("\033[1;31m[Error] Expected : ");
    retErrorMessage.append(errorMessage);
    retErrorMessage.append("\033[0m");

    return retErrorMessage;
}

std::string CSCsdkUtilityHelper::readfile(std::string filename)
{
    __FUNC_IN__
    FILE *fp;
    char buff[100];

    fp = fopen(filename.c_str(), "r");

    if (NULL == fp)
    {
        IOTIVITYTEST_LOG(ERROR, "[CSC Helper] ERROR Opening File : %s", filename.c_str());
    }

    if (NULL == fgets(buff, 100, (FILE*) fp))
    {
        IOTIVITYTEST_LOG(ERROR, "[CSC Helper] Unable to Get input from File: %s", filename.c_str());
    }

    fclose(fp);

    IOTIVITYTEST_LOG(DEBUG, "[CSC Helper] Recieved Data : %s", buff);

    __FUNC_OUT__
    return std::string(buff);
}

