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
#include "CSCsdkCloudHelper.h"
#include "CSCsdkUtilityHelper.h"

int g_isCbInvoked = CALLBACK_NOT_INVOKED;
std::string g_uid = "";
std::string g_accesstoken = "";
std::string g_refreshToken = "";
std::string g_groupID = "";
std::string g_aclResponse = "";

int CSCsdkCloudHelper::waitCallbackRet()
{
    IOTIVITYTEST_LOG(DEBUG, "Waiting for Callback to be invoked");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        if (CALLBACK_INVOKED == g_isCbInvoked)
        {
            return CALLBACK_INVOKED;
        }

        sleep(DELAY_SHORT);
        printf("Second Elapsed : %d seconds\n", i);

        if (OC_STACK_OK != OCProcess())
        {
            printf("OCStack process error\n");
            return CALLBACK_NOT_INVOKED;
        }
    }

    return CALLBACK_NOT_INVOKED;
}

void CSCsdkCloudHelper::printRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;

        if (itr->attrname().compare("accesstoken") == 0)
        {
            g_accesstoken = itr->getValueToString();
        }

        if (itr->attrname().compare("uid") == 0)
        {
            g_uid = itr->getValueToString();
        }

        if (itr->attrname().compare("gid") == 0)
        {
            g_groupID = itr->getValueToString();
        }
    }
}

FILE* controllerOpen(const char * /*path*/, const char *mode)
{
    return fopen(CLOUD_ACL_CONTROLLER_DAT, mode);
}

CSCsdkCloudHelper::CSCsdkCloudHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[CSCsdkCloudHelper] Constructor Called");
}

void CSCsdkCloudHelper::handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep,
        const int ecode)
{
    IOTIVITYTEST_LOG(INFO, "Auth response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void CSCsdkCloudHelper::onPublish(const OCRepresentation &, const int &eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Publish resource response received code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

bool CSCsdkCloudHelper::initCloudACLClient()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] initCloudACLClient IN");

    static OCPersistentStorage pstStr;
    pstStr.open = controllerOpen;
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

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] initCloudACLClient OUT");
    return true;
}

bool CSCsdkCloudHelper::signUp(OCAccountManager::Ptr accountMgr, std::string authprovider,
        std::string authCode, PostCallback cloudConnectHandler, std::string &uid,
        std::string &devAccessToken, std::string &devRefreshToken, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "SignUp IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = accountMgr->signUp(authprovider, authCode, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signUp returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    uid = g_uid;
    devAccessToken = g_accesstoken;
    OC_UNUSED(devRefreshToken);
    //devRefreshToken = g_refreshToken; /* Will be enabled if a cloud Service that returns refresh token is available*/
    IOTIVITYTEST_LOG(DEBUG, "SignUp OUT");
    return true;

}

bool CSCsdkCloudHelper::signIn(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
        const std::string& accesstoken, PostCallback cloudConnectHandler,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "signIn IN");

    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->signIn(userUuid, accesstoken, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signIn returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "signIn OUT");
    return true;
}

bool CSCsdkCloudHelper::signOut(OCAccountManager::Ptr accountMgr, const std::string accessToken,
        PostCallback cloudConnectHandler, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "signOut IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->signOut(accessToken, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signIn returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "signOut OUT");
    return true;
}

bool CSCsdkCloudHelper::provisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId,
        const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "provisionTrustCertChain IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCProvisionTrustCertChain(ctx, type, credId, selectedDeviceInfo,
            resultCallback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] provisionTrustCertChain returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "provisionTrustCertChain OUT");
    return true;
}

bool readFile(const char *name, OCByteString *out)
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

bool CSCsdkCloudHelper::saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
        OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "saveTrustCertChain IN");
    OIC_LOG_BUFFER(DEBUG, "CLOUD ACL", trustCertChain, chainSize);
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCSaveTrustCertChain(trustCertChain, chainSize, encodingType, credId);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCSaveTrustCertChain returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    IOTIVITYTEST_LOG(INFO, "CredId of Saved Trust Cert. Chain into Cred of SVR : %d", *credId);

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "saveTrustCertChain OUT");
    return true;
}

bool CSCsdkCloudHelper::cloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudCertificateIssueRequest IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCCloudCertificateIssueRequest(ctx, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudCertificateIssueRequest returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudCertificateIssueRequest OUT");
    return true;
}

bool CSCsdkCloudHelper::cloudGetCRL(void* ctx, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudGetCRL IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudGetCRL(ctx, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudGetCRL returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudGetCRL OUT");
    return true;
}

bool CSCsdkCloudHelper::cloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudAclIdCreate IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIdCreate(ctx, ownerId, deviceId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIdCreate returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudAclIdCreate OUT");
    return true;
}

bool CSCsdkCloudHelper::cloudAclIdGetByDevice(void* ctx, const char *deviceId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, std::string &devAclID,
        OCStackResult expectedResult, bool checkCallback)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudAclIdGetByDevice IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudGetAclIdByDevice(ctx, deviceId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] cloudAclIdGetByDevice returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }

        devAclID = g_aclResponse;

        if ("" == devAclID)
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Retrieving ACL ID failed");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudAclIdGetByDevice OUT");
    return true;
}

bool CSCsdkCloudHelper::cloudAclIndividualGetInfo(void* ctx, const char *aclId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult,
        bool checkCallback)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudAclIndividualGetInfo IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIndividualGetInfo(ctx, aclId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIndividualGetInfo returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudAclIndividualGetInfo OUT");
    return true;
}

bool CSCsdkCloudHelper::cloudAclIndividualUpdateAce(void* ctx, const char *aclId,
        const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback,
        OCStackResult expectedResult, bool checkCallback)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudAclIndividualUpdateAce IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIndividualUpdateAce(ctx, aclId, aces, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIndividualUpdateAce returns %s",
            CSCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == CSCsdkCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCsdkUtilityHelper::setFailureMessage(
                    "[Cloud] Callback Not Invoked");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudAclIndividualUpdateAce OUT");
    return true;
}

void CSCsdkCloudHelper::provisionResultCB(void* ctx, int nOfRes, OCProvisionResult_t* arr,
bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Provision SUCCEEDED - ctx: %s\n", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Provision Failed - ctx: %s", (char* ) ctx);
        CSCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_isCbInvoked = true;
}

void CSCsdkCloudHelper::cloudResponseCB(void* ctx, OCStackResult result, void* data)
{
    OC_UNUSED(ctx);
    OC_UNUSED(data);

    char* dataChar = (char*) data;

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d for ctx : %s", __func__, result, (char* )ctx);
    IOTIVITYTEST_LOG(INFO, "Received Data: %s", dataChar);

    if (result <= OC_STACK_RESOURCE_CHANGED)
    {
        g_isCbInvoked = true;
    }
}

void CSCsdkCloudHelper::aclResponseCB(void* ctx, OCStackResult result, void* data)
{
    OC_UNUSED(ctx);
    OC_UNUSED(data);

    g_aclResponse = string((char*) data);

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d for ctx : %s", __func__, result, (char* )ctx);
    IOTIVITYTEST_LOG(INFO, "Received Data: %s", g_aclResponse.c_str());

    if (result <= OC_STACK_RESOURCE_CHANGED)
    {
        g_isCbInvoked = true;
    }
}

std::string CSCsdkCloudHelper::getFailureMessage()
{
    return m_failureMessage;
}

