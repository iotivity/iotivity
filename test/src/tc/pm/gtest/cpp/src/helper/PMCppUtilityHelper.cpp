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

bool PMCppUtilityHelper::readFile(const char *name, OCByteString *out)
{
    FILE *file = NULL;
    int length = 0;
    uint8_t *buffer = NULL;
    bool result = false;
    size_t count, realCount;
    //Open file
    file = fopen(name, "rb");
    if (!file)
    {
        OIC_LOG_V(ERROR, TAG, "Unable to open file %s", name);
        return result;
    }

    //Get file length
    if (fseek(file, 0, SEEK_END))
    {
        OIC_LOG(ERROR, TAG, "Failed to SEEK_END");
        goto exit;
    }

    length = ftell(file);
    if (length < 0)
    {
        OIC_LOG(ERROR, TAG, "Failed to ftell");
        goto exit;
    }

    if (fseek(file, 0, SEEK_SET))
    {
        OIC_LOG(ERROR, TAG, "Failed to SEEK_SET");
        goto exit;
    }

    //Allocate memory
    buffer = (uint8_t *) malloc(length);
    if (!buffer)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate buffer");
        goto exit;
    }

    //Read file contents into buffer
    count = 1;
    realCount = fread(buffer, length, count, file);
    if (realCount != count)
    {
        OIC_LOG_V(ERROR, TAG, "Read %d bytes %zu times instead of %zu", length, realCount, count);
        goto exit;
    }

    out->bytes = buffer;
    out->len = length;

    result = true;
    exit: fclose(file);
    return result;
}

void PMCppUtilityHelper::printDevices(DeviceList_t &list)
{
    for (unsigned int i = 0; i < list.size(); i++)
    {
        std::cout << "Device " << i + 1 << " ID: ";
        std::cout << list[i]->getDeviceID() << " From IP: ";
        std::cout << list[i]->getDevAddr() << " Device Status(On/ Off) ";
        std::cout << list[i]->getDeviceStatus() << " Owned Status: ";
        std::cout << list[i]->getOwnedStatus() << std::endl;
    }
}

char* PMCppUtilityHelper::getOxmType(OicSecOxm_t oxmType)
{

    char* resultString = NULL;

    switch (oxmType)
    {
        case OIC_JUST_WORKS:
            resultString = (char*) "OIC_JUST_WORKS";
            break;
        case OIC_RANDOM_DEVICE_PIN:
            resultString = (char*) "OIC_RANDOM_DEVICE_PIN";
            break;
        case OIC_MANUFACTURER_CERTIFICATE:
            resultString = (char*) "OIC_MANUFACTURER_CERTIFICATE";
            break;
        case OIC_DECENTRALIZED_PUBLIC_KEY:
            resultString = (char*) "OIC_DECENTRALIZED_PUBLIC_KEY";
            break;
        case OIC_OXM_COUNT:
            resultString = (char*) "OIC_OXM_COUNT";
            break;
        case OIC_PRECONFIG_PIN:
            resultString = (char*) "OIC_PRECONFIG_PIN";
            break;
        case OIC_MV_JUST_WORKS:
            resultString = (char*) "OC_STACK_RESOURCE_CREATED";
            break;
        case OIC_CON_MFG_CERT:
            resultString = (char*) "OIC_CON_MFG_CERT";
            break;
        default:
            resultString = (char*) "UNKNOWN_OXM_TYPE";
    }

    return resultString;
}

void PMCppUtilityHelper::printUuid(OicUuid_t uuid)
{
    for (int i = 0; i < UUID_LENGTH; i++)
    {
        std::cout << std::hex << uuid.id[i] << " ";
    }
    std::cout << std::endl;
}

void PMCppUtilityHelper::removeAllResFile()
{
    CommonUtil::rmFile(JUSTWORKS_SERVER1_CBOR);
    CommonUtil::rmFile(JUSTWORKS_SERVER2_CBOR);
    CommonUtil::rmFile(RANDOMPIN_SERVER_CBOR);
    CommonUtil::rmFile(PRECONFIG_SERVER1_CBOR);
    CommonUtil::rmFile(PRECONFIG_SERVER2_CBOR);
    CommonUtil::rmFile(JUSTWORKS_SERVER7_CBOR);
    CommonUtil::rmFile(MV_JUSTWORKS_SERVER_CBOR);

    CommonUtil::rmFile(CLIENT_DATABASE);
    CommonUtil::rmFile(CLIENT_CBOR);

    CommonUtil::rmFile(MOT_CLIENT_DATABASE);
    CommonUtil::rmFile(MOT_CLIENT_CBOR);
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
        printf(" - result: %s\n", CommonUtil::getOCStackResult(rslt_lst[lst_cnt].res));
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
            printf("%c", DASH);
        }
    }
    printf("\n");

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] printUuid OUT");
}

/**
 * Function to set failure message
 */
std::string PMCppUtilityHelper::setFailureMessage(OCStackResult actualResult,
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
std::string PMCppUtilityHelper::setFailureMessage(OicSecOxm_t actualResult,
        OicSecOxm_t expectedResult)
{
    std::string errorMessage("\033[1;31m[Error] Expected : ");
    errorMessage.append(PMCppUtilityHelper::getOxmType(expectedResult));
    errorMessage.append("\033[0m  \033[1;31mActual : ");
    errorMessage.append(PMCppUtilityHelper::getOxmType(actualResult));
    errorMessage.append("\033[0m");
    return errorMessage;
}

/**
 * Function to set failure message
 */
std::string PMCppUtilityHelper::setFailureMessage(std::string errorMessage)
{
    std::string retErrorMessage("\033[1;31m[Error] Expected : ");
    retErrorMessage.append(errorMessage);
    retErrorMessage.append("\033[0m");

    return retErrorMessage;
}

