//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifdef WITH_ARDUINO
#define __STDC_LIMIT_MACROS
#endif

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ocstack.h"
#include "logger.h"
#include "oic_malloc.h"
#include "cainterface.h"
#include "secureresourcemanager.h"
#include "resourcemanager.h"
#include "aclresource.h"
#include "psinterface.h"
#include "pstatresource.h"
#include "doxmresource.h"
#include "credresource.h"
#include "srmresourcestrings.h"
#include "srmutility.h"
#include "ocpayloadcbor.h"

#define TAG  PCF("SRM-PSI")

/**
 * Writes the Secure Virtual Database to PS
 *
 * @param[in] svrData is a pointer of OicSvr_t.
 * @retval  reference to memory buffer containing SVR databaspstat1e.
 */
OCStackResult WriteSVRDatabase(OicSvr_t* svrData)
{

    OCStackResult result = OC_STACK_ERROR;
    OCPersistentStorage* ps = SRMGetPersistentStorageHandler();
    if (!ps)
    {
        return result;
    }
    FILE* fp = ps->open(SVR_DB_FILE, "wb+");
    if (!fp)
    {
        OC_LOG (ERROR, TAG, PCF("Unable to open SVR database file!! "));
        return result;
    }
    size_t bytes = fwrite(&svrData->pstatSize, sizeof(size_t), 1, fp);
    VERIFY_SUCCESS(TAG, bytes, ERROR);
    if (svrData->pstatSize)
    {
        bytes = fwrite(svrData->pstatPayload, svrData->pstatSize, 1, fp);
        VERIFY_SUCCESS(TAG, bytes, ERROR);
    }
    bytes = fwrite(&svrData->doxmSize, sizeof(size_t), 1, fp);
    VERIFY_SUCCESS(TAG, bytes, ERROR);
    if (svrData->doxmSize)
    {

        bytes = fwrite(svrData->doxmPayload, svrData->doxmSize, 1, fp);
        VERIFY_SUCCESS(TAG, bytes, ERROR);
    }
    bytes = fwrite(&svrData->aclSize, sizeof(size_t), 1, fp);
    VERIFY_SUCCESS(TAG, bytes, ERROR);
    if (svrData->aclSize)
    {
        bytes = fwrite(svrData->aclPayload, svrData->aclSize, 1, fp);
        VERIFY_SUCCESS(TAG, bytes, ERROR);
    }
    bytes = fwrite(&svrData->credSize, sizeof(size_t), 1, fp);
    VERIFY_SUCCESS(TAG, bytes, ERROR);
    if (svrData->credSize)
    {
        bytes = fwrite(svrData->credPayload, svrData->credSize, 1, fp);
        VERIFY_SUCCESS(TAG, bytes, ERROR);
    }
    result = OC_STACK_OK;
exit:
    if (ps && fp)
    {
        ps->close(fp);
    }
    return result;
}

/**
 * This method converts SVR buffers in to OCRepPayload and updates the persistent storage.
 *
  * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult UpdateSVRData()
{
    OCStackResult ret = OC_STACK_ERROR;
    OicSvr_t svrData = {.aclPayload = NULL};

    ret = ConvertAclData(&svrData.aclPayload, &svrData.aclSize);
    if(ret != OC_STACK_OK)
    {
        OC_LOG (INFO, TAG, PCF("No Acl data!! "));
    }
    ret = ConvertCredData(&svrData.credPayload, &svrData.credSize);
    if(ret != OC_STACK_OK)
    {
        OC_LOG (INFO, TAG, PCF("No Cred data!! "));
    }
    ret = ConvertDoxmData(&svrData.doxmPayload, &svrData.doxmSize);
    if(ret != OC_STACK_OK)
    {
        OC_LOG (INFO, TAG, PCF("No Doxm data!! "));
    }
    ret = ConvertPstatData(&svrData.pstatPayload, &svrData.pstatSize);
    if (ret != OC_STACK_OK)
    {
        OC_LOG (INFO, TAG, PCF("No Pstat data!! "));
    }
    //update persistent storage
    ret = WriteSVRDatabase(&svrData);
    if(ret != OC_STACK_OK)
    {
        OC_LOG (ERROR, TAG, PCF("UpdateSVDatabase failed!! "));
    }
    return ret;
}

/**
 * Reads the Secure Virtual Database from PS
 *
 * @note Caller of this method MUST use OCFree() method to release memory
 *       referenced by return value.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult ReadSVDataFromPS(OicSvr_t* svrData)
{
    OCStackResult ret = OC_STACK_ERROR;
    if(!svrData)
    {
        OC_LOG (ERROR, TAG, PCF("Invalid parameter "));
        return OC_STACK_INVALID_PARAM;
    }
    OCPersistentStorage* ps = SRMGetPersistentStorageHandler();
    if (!ps)
    {
        return ret;
    }
    FILE* readFile = ps->open(SVR_DB_FILE, "r");
    if (!readFile)
    {
        OC_LOG (ERROR, TAG, PCF("Unable to open SVR database file!! "));
        return ret;
    }
    size_t bytes = fread(&svrData->pstatSize, sizeof(size_t), 1, readFile);
    if (svrData->pstatSize && svrData->pstatSize < UINT32_MAX)
    {
        VERIFY_SUCCESS(TAG, bytes, ERROR);
        svrData->pstatPayload = (uint8_t*)OICMalloc(svrData->pstatSize);
        VERIFY_NON_NULL(TAG, svrData->pstatPayload, INFO);
        bytes = fread(svrData->pstatPayload, svrData->pstatSize, 1, readFile);
        VERIFY_SUCCESS(TAG, bytes, ERROR);
    }
    bytes = fread(&svrData->doxmSize, sizeof(size_t), 1, readFile);
    if (svrData->doxmSize && svrData->doxmSize < UINT32_MAX)
    {
        VERIFY_SUCCESS(TAG, bytes, ERROR);
        svrData->doxmPayload = (uint8_t*)OICMalloc(svrData->doxmSize);
        VERIFY_NON_NULL(TAG, svrData->doxmPayload, INFO);
        bytes = fread(svrData->doxmPayload, svrData->doxmSize, 1, readFile);
        VERIFY_SUCCESS(TAG, bytes, ERROR);
    }
    bytes = fread(&svrData->aclSize, sizeof(size_t), 1, readFile);
    if (svrData->aclSize && svrData->aclSize < UINT32_MAX)
    {
        VERIFY_SUCCESS(TAG, bytes, ERROR);
        svrData->aclPayload = (uint8_t*)OICMalloc(svrData->aclSize);
        VERIFY_NON_NULL(TAG, svrData->aclPayload, INFO);
        bytes = fread(svrData->aclPayload, svrData->aclSize, 1, readFile);
        VERIFY_SUCCESS(TAG, bytes, ERROR);
    }
    bytes = fread(&svrData->credSize, sizeof(size_t), 1, readFile);
    if (svrData->credSize && svrData->credSize < UINT32_MAX)
    {
        VERIFY_SUCCESS(TAG, bytes, ERROR);
        svrData->credPayload = (uint8_t*)OICMalloc(svrData->credSize);
        VERIFY_NON_NULL(TAG, svrData->credPayload, INFO);
        bytes = fread(svrData->credPayload, svrData->credSize, 1, readFile);
        VERIFY_SUCCESS(TAG, bytes, ERROR);
    }
    //All is good
    ret = OC_STACK_OK;
exit:
    if (ps && readFile)
    {
        ps->close(readFile);
    }
    if(ret != OC_STACK_OK)
    {
        OICFree(svrData->aclPayload);
        OICFree(svrData->credPayload);
        OICFree(svrData->doxmPayload);
        OICFree(svrData->pstatPayload);
    }
    return ret;
}
