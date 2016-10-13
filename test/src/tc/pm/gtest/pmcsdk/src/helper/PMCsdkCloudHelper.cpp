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
#include "PMCsdkCloudHelper.h"
#include "PMCsdkUtilityHelper.h"
#if defined(__CLOUD__)
int g_isCbInvoked = CALLBACK_NOT_INVOKED;
std::string g_uid = "";
std::string g_accesstoken = "";
std::string g_refreshToken = "";
std::string g_groupID = "";

int PMCsdkCloudHelper::waitCallbackRet()
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

void printRepresentation(OCRepresentation rep)
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

PMCsdkCloudHelper::PMCsdkCloudHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMCsdkCloudHelper] Constructor Called");
}

void PMCsdkCloudHelper::handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep,
        const int ecode)
{
    IOTIVITYTEST_LOG(INFO, "Auth response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void PMCsdkCloudHelper::onPublish(const OCRepresentation &, const int &eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Publish resource response received code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

bool PMCsdkCloudHelper::initCloudACLClient()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] initProvisionClient IN");

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

    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] initProvisionClient OUT");
    return true;
}

bool PMCsdkCloudHelper::signUp(OCAccountManager::Ptr accountMgr, std::string authprovider,
        std::string authCode, PostCallback cloudConnectHandler, std::string &devID,
        std::string &devAccessToken, std::string &devRefreshToken, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "SignUp IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = accountMgr->signUp(authprovider, authCode, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signUp returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    devID = g_uid;
    devAccessToken = g_accesstoken;
    //devRefreshToken = g_refreshToken; /* Will be enabled if a cloud Service that returns refresh token is available*/
    IOTIVITYTEST_LOG(DEBUG, "SignUp OUT");
    return true;

}

bool PMCsdkCloudHelper::signIn(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
        const std::string& accessToken, PostCallback cloudConnectHandler,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "signIn IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->signIn(g_uid, g_accesstoken, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signIn returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "signIn OUT");
    return true;
}

bool PMCsdkCloudHelper::signOut(OCAccountManager::Ptr accountMgr, PostCallback cloudConnectHandler,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "signOut IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->signOut(cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signIn returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "signOut OUT");
    return true;
}

bool PMCsdkCloudHelper::publishResourceToRD(std::string host, OCConnectivityType connectivityType,
        PublishResourceCallback callback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "publishResourceToRD IN");

    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCPlatform::publishResourceToRD(host, connectivityType, callback);
    IOTIVITYTEST_LOG(DEBUG, "[Cloud API Return Code ] publishResourceToRD returns %d", result);

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "publishResourceToRD OUT");
    return true;
}

bool PMCsdkCloudHelper::provisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId,
        const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "provisionTrustCertChain IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCProvisionTrustCertChain(ctx, type, credId, selectedDeviceInfo,
            resultCallback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] provisionTrustCertChain returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "provisionTrustCertChain OUT");
    return true;
}

ByteArray PMCsdkCloudHelper::getTrustCertChainArray()
{

    IOTIVITYTEST_LOG(INFO, "Save Trust Cert. Chain into Cred of SVR");

    ByteArray trustCertChainArray =
    { 0, 0 };

    FILE *fp = fopen(ROOT_CERT_FILE, "rb+");

    if (fp)
    {
        size_t fsize;
        if (fseeko(fp, 0, SEEK_END) == 0 && (fsize = ftello(fp)))
        {
            trustCertChainArray.data = OICCalloc(1, fsize + 1);
            trustCertChainArray.len = fsize + 1;
            if (NULL == trustCertChainArray.data)
            {
                IOTIVITYTEST_LOG(ERROR, "OICCalloc");
                fclose(fp);
            }
            rewind(fp);
            fsize = fread(trustCertChainArray.data, 1, fsize, fp);
            fclose(fp);
        }
    }

    //OIC_LOG_BUFFER(DEBUG, "CLOUD ACL", trustCertChainArray.data, trustCertChainArray.len);

    return trustCertChainArray;
}

bool PMCsdkCloudHelper::saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
        OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "saveTrustCertChain IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCSaveTrustCertChain(trustCertChain, chainSize, encodingType, credId);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCSaveTrustCertChain returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    IOTIVITYTEST_LOG(INFO, "CredId of Saved Trust Cert. Chain into Cred of SVR : %d", *credId);

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "saveTrustCertChain OUT");
    return true;
}

bool PMCsdkCloudHelper::cloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback = true)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudCertificateIssueRequest IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCCloudCertificateIssueRequest(ctx, endPoint, &cloudResponseCB);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudCertificateIssueRequest returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudCertificateIssueRequest OUT");
    return true;
}

bool PMCsdkCloudHelper::cloudGetCRL(void* ctx, const OCDevAddr *endPoint,
        OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback = true)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudGetCRL IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudGetCRL(ctx, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudGetCRL returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudGetCRL OUT");
    return true;
}

bool PMCsdkCloudHelper::cloudAclIdCreate(void* ctx, const char *ownerId, const char *deviceId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback = true)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudAclIdCreate IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIdCreate(ctx, ownerId, deviceId, endPoint, cloudResponseCB);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIdCreate returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudAclIdCreate OUT");
    return true;
}

bool PMCsdkCloudHelper::cloudAclIdGetByDevice(void* ctx, const char *deviceId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback = true)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudAclIdGetByDevice IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudGetAclIdByDevice(ctx, deviceId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] cloudAclIdGetByDevice returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudAclIdGetByDevice OUT");
    return true;
}

bool PMCsdkCloudHelper::cloudAclIndividualGetInfo(void* ctx, const char *aclId,
        const OCDevAddr *endPoint, OCCloudResponseCB callback, OCStackResult expectedResult, bool checkCallback = true)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudAclIndividualGetInfo IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIndividualGetInfo(ctx, aclId, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIndividualGetInfo returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudAclIndividualGetInfo OUT");
    return true;
}

bool PMCsdkCloudHelper::cloudAclIndividualUpdateAce(void* ctx, const char *aclId,
        const cloudAce_t *aces, const OCDevAddr *endPoint, OCCloudResponseCB callback,
        OCStackResult expectedResult, bool checkCallback = true)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudAclIndividualUpdateAce IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCCloudAclIndividualUpdateAce(ctx, aclId, aces, endPoint, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudAclIndividualUpdateAce returns %s",
            PMCsdkUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result && checkCallback == true)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudAclIndividualUpdateAce OUT");
    return true;
}

void PMCsdkCloudHelper::provisionResultCB(void* ctx, int nOfRes, OCProvisionResult_t* arr,
        bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Provision SUCCEEDED - ctx: %s\n", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Provision Failed - ctx: %s", (char* ) ctx);
        PMCsdkUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_isCbInvoked = true;
}

void PMCsdkCloudHelper::cloudResponseCB(void* ctx, OCStackResult result, void* data)
{
    OC_UNUSED(ctx);
    OC_UNUSED(data);

    char* dataChar = (char*)data;

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d for ctx : %s", __func__, result, (char* )ctx);
    IOTIVITYTEST_LOG(INFO, "Received Data: %s", dataChar);

    if (result < 5)
    {
        g_isCbInvoked = true;
    }
}

std::string PMCsdkCloudHelper::getFailureMessage()
{
    return m_failureMessage;
}

/**
 * Function to set failure message
 */
std::string PMCsdkCloudHelper::setFailureMessage(OCStackResult actualResult,
        OCStackResult expectedResult)
{
    std::string errorMessage("\033[1;31m[Error] Expected : ");
    errorMessage.append(PMCsdkUtilityHelper::getOCStackResult(expectedResult));
    errorMessage.append("\033[0m  \033[1;31mActual : ");
    errorMessage.append(PMCsdkUtilityHelper::getOCStackResult(actualResult));
    errorMessage.append("\033[0m");
    return errorMessage;
}

/**
 * Function to set failure message
 */
std::string PMCsdkCloudHelper::setFailureMessage(std::string errorMessage)
{
    std::string retErrorMessage("\033[1;31m[Error] Expected : ");
    retErrorMessage.append(errorMessage);
    retErrorMessage.append("\033[0m");

    return retErrorMessage;
}
#endif /*__CLOUD__*/
