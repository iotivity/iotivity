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
#if defined(__CLOUD__)
#include "CSCppCloudHelper.h"
#include "CSCppUtilityHelper.h"

int g_isCbInvoked = CALLBACK_NOT_INVOKED;
std::string g_uid = "";
std::string g_accesstoken = "";
std::string g_refreshToken = "";
std::string g_groupID = "";
std::string g_aclId = "";

int CSCppCloudHelper::waitCallbackRet()
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
        IOTIVITYTEST_LOG(DEBUG, "[CSCppCloudHelper] \t%s:\t%s", itr->attrname().c_str(), itr->getValueToString().c_str());

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

CSCppCloudHelper::CSCppCloudHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[CSCppCloudHelper] Constructor Called");
}

void CSCppCloudHelper::handleLoginoutCB(const HeaderOptions &, const OCRepresentation &rep,
        const int ecode)
{
    IOTIVITYTEST_LOG(INFO, "Auth response received code: %d", ecode);
    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    g_isCbInvoked = CALLBACK_INVOKED;
}

void CSCppCloudHelper::onPublish(const OCRepresentation &, const int &eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Publish resource response received code: %d", eCode);
    g_isCbInvoked = CALLBACK_INVOKED;
}

bool CSCppCloudHelper::initCloudACLClient()
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

bool CSCppCloudHelper::signUp(OCAccountManager::Ptr accountMgr, std::string authprovider,
        std::string authCode, PostCallback cloudConnectHandler, std::string &devID,
        std::string &devAccessToken, std::string &devRefreshToken, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "SignUp IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = accountMgr->signUp(authprovider, authCode, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signUp returns %s",
            PMCppUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
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

bool CSCppCloudHelper::signIn(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
        const std::string& accessToken, PostCallback cloudConnectHandler,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "signIn IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = accountMgr->signIn(g_uid, g_accesstoken, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signIn returns %s",
            PMCppUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "signIn OUT");
    return true;
}

bool CSCppCloudHelper::signOut(OCAccountManager::Ptr accountMgr, PostCallback cloudConnectHandler,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "signOut IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    /*
     * Need to fix as signOut API change
     */
    IOTIVITYTEST_LOG(DEBUG, "signOut OUT");
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
    buffer = (uint8_t *)malloc(length);
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
    out->len   = length;

    result = true;
exit:
    fclose(file);
    return result;
}

ByteArray CSCppCloudHelper::getTrustCertChainArray()
{

    IOTIVITYTEST_LOG(INFO, "Save Trust Cert. Chain into Cred of SVR");

    ByteArray trustCertChainArray =
    {   0, 0};

    FILE *fp = fopen(ROOT_CERT_FILE, "rb+");

    if (fp)
    {
        size_t fsize;
        if (fseeko(fp, 0, SEEK_END) == 0 && (fsize = ftello(fp)))
        {
            trustCertChainArray.data = (uint8_t*)OICCalloc(1, fsize + 1);
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

    OIC_LOG_BUFFER(DEBUG, "CLOUD ACL", trustCertChainArray.data, trustCertChainArray.len);

    return trustCertChainArray;
}

bool CSCppCloudHelper::saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
        OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "saveTrustCertChain IN");

    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCSecure::saveTrustCertChain(trustCertChain, chainSize, encodingType,
            credId);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCSaveTrustCertChain returns %s",
            PMCppUtilityHelper::getOCStackResult(result));

    IOTIVITYTEST_LOG(INFO, "CredId of Saved Trust Cert. Chain into Cred of SVR : %d", *credId);

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    IOTIVITYTEST_LOG(DEBUG, "saveTrustCertChain OUT");
    return true;
}

bool CSCppCloudHelper::requestCertificate(OCCloudProvisioning& ptr, ResponseCallBack callback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudCertificateIssueRequest IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = ptr.requestCertificate(callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudCertificateIssueRequest returns %s",
            PMCppUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudCertificateIssueRequest OUT");
    return true;
}

bool CSCppCloudHelper::getCRL(OCCloudProvisioning& ptr, ResponseCallBack callback,
        OCStackResult expectedResult)
{
    try
    {
        IOTIVITYTEST_LOG(DEBUG, "cloudGetCRL IN");
        g_isCbInvoked = CALLBACK_NOT_INVOKED;

        OCStackResult result = ptr.getCRL(callback);
        IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudGetCRL returns %s",
                PMCppUtilityHelper::getOCStackResult(result));

        if (expectedResult != result)
        {
            m_failureMessage = setFailureMessage(result, expectedResult);
            return false;
        }

        if (OC_STACK_OK == result)
        {
            if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
            {
                IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
                return false;
            }
        }

    }
    catch(...)
    {
        IOTIVITYTEST_LOG(ERROR, "[Cloud] Exceptions has occured");
        return true;
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudGetCRL OUT");
    return true;
}

bool CSCppCloudHelper::postCRL(OCCloudProvisioning& ptr, const std::string thisUpdate, const std::string nextUpdate,
        const OCByteString *crl, const stringArray_t *serialNumbers, ResponseCallBack callback,
        OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "PostCRL IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = ptr.postCRL(thisUpdate, nextUpdate, crl, serialNumbers, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] postCRL returns %s",
            PMCppUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    sleep(DELAY_MEDIUM);

    IOTIVITYTEST_LOG(DEBUG, "PostCRL OUT");
    return true;
}

bool CSCppCloudHelper::getAclIdByDevice(OCCloudProvisioning& ptr, const std::string deviceId,
        AclIdResponseCallBack callback, std::string &aclId, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "cloudAclIdGetByDevice IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = ptr.getAclIdByDevice(deviceId, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] cloudAclIdGetByDevice returns %s",
            PMCppUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }

        aclId = g_aclId;
    }

    IOTIVITYTEST_LOG(DEBUG, "cloudAclIdGetByDevice OUT");
    return true;
}

bool CSCppCloudHelper::getIndividualAclInfo(OCCloudProvisioning& ptr, const std::string aclId,
        ResponseCallBack callback, OCStackResult expectedResult)
{
    IOTIVITYTEST_LOG(DEBUG, "GetIndividualAclInfo IN");
    g_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = ptr.getIndividualAclInfo(aclId, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] getIndividualAclInfo returns %s",
            PMCppUtilityHelper::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }

        //aclId = g_aclId;
    }

    IOTIVITYTEST_LOG(DEBUG, "GetIndividualAclInfo OUT");
    return true;
}

void CSCppCloudHelper::provisionResultCB(void* ctx, int nOfRes, OCProvisionResult_t* arr,
        bool hasError)
{
    if (!hasError)
    {
        IOTIVITYTEST_LOG(INFO, "Provision SUCCEEDED - ctx: %s\n", (char* ) ctx);
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "Provision Failed - ctx: %s", (char* ) ctx);
        PMCppUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    g_isCbInvoked = true;
}

void CSCppCloudHelper::cloudResponseCB(OCStackResult result, void* data)
{
    //char* dataChar = (char*) data;

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d", __func__, result);

    if (result < 5)
    {
        g_isCbInvoked = true;
    }
}

void CSCppCloudHelper::cloudAclResponseCB(OCStackResult result, std::string aclID)
{

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d", __func__, result);
    IOTIVITYTEST_LOG(INFO, "ACL ID : %s", aclID.c_str());

    g_aclId = aclID;

    if (result < 5)
    {
        g_isCbInvoked = true;
    }

}
std::string CSCppCloudHelper::getFailureMessage()
{
    return m_failureMessage;
}

/**
 * Function to set failure message
 */
std::string CSCppCloudHelper::setFailureMessage(OCStackResult actualResult,
        OCStackResult expectedResult)
{
    std::string errorMessage("\033[1;31m[Error] Expected : ");
    errorMessage.append(PMCppUtilityHelper::getOCStackResult(expectedResult));
    errorMessage.append("\033[0m  \033[1;31mActual : ");
    errorMessage.append(PMCppUtilityHelper::getOCStackResult(actualResult));
    errorMessage.append("\033[0m");
    return errorMessage;
}

/**
 * Function to set failure message
 */
std::string CSCppCloudHelper::setFailureMessage(std::string errorMessage)
{
    std::string retErrorMessage("\033[1;31m[Error] Expected : ");
    retErrorMessage.append(errorMessage);
    retErrorMessage.append("\033[0m");

    return retErrorMessage;
}
#endif /*__CLOUD__*/
