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
#include "PMCsdkCertHelper.h"
#include "PMCsdkUtilityHelper.h"

int PMCsdkCertHelper::g_OwnDevCount = 0;
int PMCsdkCertHelper::g_UnownDevCount = 0;
int PMCsdkCertHelper::g_motPMDevCount = 0;
bool PMCsdkCertHelper::g_CBInvoked = CALLBACK_NOT_INVOKED;
OCPersistentStorage PMCsdkCertHelper::pstStr = {0, 0, 0, 0, 0};
ByteArray s_trustCertChainArray =
{ 0, 0 };

FILE* PMCsdkCertHelper::fopenProvManager(const char* path, const char* mode)
{
    OC_UNUSED(path);
    return fopen(SVR_DB_FILE_NAME, mode);
}

int PMCsdkCertHelper::waitCallbackRet(void)
{
    __FUNC_IN__

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        IOTIVITYTEST_LOG(DEBUG, "PMCsdkCertHelper::waitCallbackRet Loop = %d", i);

        if (true == PMCsdkCertHelper::g_CBInvoked)
        {
            return CALLBACK_INVOKED;
        }

        CommonUtil::waitInSecond(DELAY_SHORT);

        if (OC_STACK_OK != OCProcess())
        {
            IOTIVITYTEST_LOG(ERROR, "OCStack process error");
            return CALLBACK_NOT_INVOKED;
        }
    }

    __FUNC_OUT__
    return CALLBACK_NOT_INVOKED;
}

void PMCsdkCertHelper::provisionCertCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr,
bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Provision Cert SUCCEEDED - ctx: %s", (char* ) ctx);
        PMCsdkCertHelper::g_CBInvoked = true;
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Ownership Transfer FAILED - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
}

void PMCsdkCertHelper::trustCertChainChangeCB(void *ctx, uint16_t credId, uint8_t *trustCertChain,
        size_t chainSize)
{
    __FUNC_IN__

    IOTIVITYTEST_LOG(INFO, "Provision Cert SUCCEEDED - ctx: %s", (char* ) ctx);

    IOTIVITYTEST_LOG(INFO, "CRED ID : %d", credId);

    OIC_LOG_BUFFER(DEBUG, "Cert Cb", trustCertChain, chainSize);

    PMCsdkCertHelper::g_CBInvoked = true;

    __FUNC_OUT__
}

// CAPI for Provisioning Manager

PMCsdkCertHelper::PMCsdkCertHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] Constructor");
}

bool PMCsdkCertHelper::initProvisionClient(int clientOTMType, char* chDBPath)
{
    __FUNC_IN__

    pstStr.open = PMCsdkCertHelper::fopenProvManager;
    pstStr.read = fread;
    pstStr.write = fwrite;
    pstStr.close = fclose;
    pstStr.unlink = unlink;

    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&pstStr))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCRegisterPersistentStorageHandler error");
        return false;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCStack init error");
        return false;
    }

    if (access(PRVN_DB_FILE_NAME, F_OK) != -1)
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMHelper] Provisioning DB file already exists.");
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMHelper] No provisioning DB file, creating new ");
    }

    IOTIVITYTEST_LOG(INFO, "************OCInitPM In************");

    if (OC_STACK_OK != OCInitPM(chDBPath))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCInitPM Error");
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCsdkCertHelper::provisionCRL(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
        OicSecCrl_t *crl, OCProvisionResultCB resultCallback, OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCertHelper::g_CBInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OC_STACK_OK;
    //OCProvisionCRL(ctx, selectedDeviceInfo, crl, resultCallback);

    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCProvisionCRL returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCertHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool PMCsdkCertHelper::provisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId,
        const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCertHelper::g_CBInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCProvisionTrustCertChain(ctx, type, credId, selectedDeviceInfo,
            resultCallback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] provisionTrustCertChain returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCertHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool PMCsdkCertHelper::saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
        OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCertHelper::g_CBInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCSaveTrustCertChain(trustCertChain, chainSize, encodingType, credId);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCSaveTrustCertChain returns %s",
            CommonUtil::getOCStackResult(result));

    IOTIVITYTEST_LOG(INFO, "CredId of Saved Trust Cert. Chain into Cred of SVR : %d", *credId);

    if (expectedResult != result)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "saveTrustCertChain OUT");
    return true;
}

bool PMCsdkCertHelper::registerTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCertHelper::g_CBInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCRegisterTrustCertChainNotifier(cb, CB);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCRegisterTrustCertChainNotifier returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCsdkCertHelper::removeTrustCertChainNotifier()
{
    __FUNC_IN__

    OCRemoveTrustCertChainNotifier();

    __FUNC_OUT__
    return true;
}

bool PMCsdkCertHelper::readTrustCertChain(uint16_t credId, uint8_t **trustCertChain,
        size_t *chainSize, OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCertHelper::g_CBInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCReadTrustCertChain(credId, trustCertChain, chainSize);

    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCReadTrustCertChain returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if( OC_STACK_OK == result && OC_STACK_OK == expectedResult) {
        OIC_LOG_BUFFER(DEBUG, TAG, *trustCertChain, *chainSize);
    }

    __FUNC_OUT__
    return true;
}

bool PMCsdkCertHelper::readFile(const char *name, OCByteString *out)
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
        IOTIVITYTEST_LOG(ERROR, "Unable to open file %s", name);
        return result;
    }

    //Get file length
    if (fseek(file, 0, SEEK_END))
    {
        IOTIVITYTEST_LOG(ERROR, "Failed to SEEK_END");
        goto exit;
    }

    length = ftell(file);
    if (length < 0)
    {
        IOTIVITYTEST_LOG(ERROR, "Failed to ftell");
        goto exit;
    }

    if (fseek(file, 0, SEEK_SET))
    {
        IOTIVITYTEST_LOG(ERROR, "Failed to SEEK_SET");
        goto exit;
    }

    //Allocate memory
    buffer = (uint8_t *) malloc(length);
    if (!buffer)
    {
        IOTIVITYTEST_LOG(ERROR, "Failed to allocate buffer");
        goto exit;
    }

    //Read file contents into buffer
    count = 1;
    realCount = fread(buffer, length, count, file);
    if (realCount != count)
    {
        IOTIVITYTEST_LOG(ERROR, "Read %d bytes %zu times instead of %zu", length, realCount, count);
        goto exit;
    }

    out->bytes = buffer;
    out->len = length;

    result = true;
    exit: fclose(file);
    return result;
}

ByteArray_t PMCsdkCertHelper::getTrustCertChainArray()
{
    __FUNC_IN__

    ByteArray trustCertChainArray =
    { 0, 0 };

    FILE *fp = fopen("rootca.crt", "rb+");

    if (fp)
    {
        size_t fsize;
        if (fseeko(fp, 0, SEEK_END) == 0 && (fsize = ftello(fp)) > 0)
        {
            trustCertChainArray.data = (uint8_t *) malloc(fsize);
            trustCertChainArray.len = fsize;
            if (NULL == trustCertChainArray.data)
            {
                cout << "Failed to allocate memory" << endl;
                fclose(fp);
            }
            rewind(fp);
            if (fsize != fread(trustCertChainArray.data, 1, fsize, fp))
            {
                cout << "Certiface not read completely" << endl;
            }
            fclose(fp);
        }
    }

    __FUNC_OUT__
    return trustCertChainArray;
}

std::string PMCsdkCertHelper::getFailureMessage()
{
    return m_failureMessage;
}
