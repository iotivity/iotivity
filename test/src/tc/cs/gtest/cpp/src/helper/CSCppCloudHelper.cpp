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
#if defined(__CLOUD__)
#include "CSCppCloudHelper.h"
#include "CSCppUtilityHelper.h"

int CSCppCloudHelper::s_isCbInvoked = CALLBACK_NOT_INVOKED;
std::string CSCppCloudHelper::s_uid = "";
std::string CSCppCloudHelper::s_accesstoken = "";
std::string CSCppCloudHelper::s_refreshToken = "";
std::string CSCppCloudHelper::s_groupID = "";
std::string CSCppCloudHelper::s_aclId = "";
OCPersistentStorage CSCppCloudHelper::s_pst = {0, 0, 0, 0, 0};

int CSCppCloudHelper::waitCallbackRet()
{
    IOTIVITYTEST_LOG(DEBUG, "Waiting for Callback to be invoked");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        if (CALLBACK_INVOKED == s_isCbInvoked)
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
            CSCppCloudHelper::s_accesstoken = itr->getValueToString();
        }

        if (itr->attrname().compare("uid") == 0)
        {
            CSCppCloudHelper::s_uid = itr->getValueToString();
        }

        if (itr->attrname().compare("gid") == 0)
        {
            CSCppCloudHelper::s_groupID = itr->getValueToString();
        }
    }
}

FILE* CSCppCloudHelper::controllerOpen(const char * /*path*/, const char *mode)
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

    s_isCbInvoked = CALLBACK_INVOKED;
}

void CSCppCloudHelper::onPublish(const OCRepresentation &, const int &eCode)
{
    IOTIVITYTEST_LOG(DEBUG, "Publish resource response received code: %d", eCode);
    s_isCbInvoked = CALLBACK_INVOKED;
}

bool CSCppCloudHelper::initCloudACLClient()
{
    __FUNC_IN__

    CSCppCloudHelper::s_pst.open = CSCppCloudHelper::controllerOpen;
    CSCppCloudHelper::s_pst.read = fread;
    CSCppCloudHelper::s_pst.write = fwrite;
    CSCppCloudHelper::s_pst.close = fclose;
    CSCppCloudHelper::s_pst.unlink = unlink;

    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&CSCppCloudHelper::s_pst))
    {
        m_failureMessage = CSCppUtilityHelper::setFailureMessage("[Cloud] OCRegisterPersistentStorageHandler error");
        return false;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCStack init error");
        m_failureMessage = CSCppUtilityHelper::setFailureMessage("[Cloud] OCStack init error");
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool CSCppCloudHelper::signUp(OCAccountManager::Ptr accountMgr, std::string authprovider,
        std::string authCode, PostCallback cloudConnectHandler, std::string &devID,
        std::string &devAccessToken, std::string &devRefreshToken, OCStackResult expectedResult)
{
    __FUNC_IN__

    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = accountMgr->signUp(authprovider, authCode, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signUp returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCppUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCppUtilityHelper::setFailureMessage("[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    devID = s_uid;
    devAccessToken = s_accesstoken;
    //devRefreshToken = s_refreshToken; /* Will be enabled if a cloud Service that returns refresh token is available*/
    __FUNC_OUT__
    return true;

}

bool CSCppCloudHelper::signIn(OCAccountManager::Ptr accountMgr, const std::string& userUuid,
        const std::string& accessToken, PostCallback cloudConnectHandler,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = accountMgr->signIn(s_uid, s_accesstoken, cloudConnectHandler);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signIn returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCppUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCppUtilityHelper::setFailureMessage("[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCppCloudHelper::signOut(OCAccountManager::Ptr accountMgr, PostCallback cloudConnectHandler,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    /*
     * Need to fix as signOut API change
     */

//    OCStackResult result = accountMgr->signOut(cloudConnectHandler);
//    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] signIn returns %s",
//            CommonUtil::getOCStackResult(result));
//
//    if (expectedResult != result)
//    {
//        m_failureMessage = CSCppUtilityHelper::setFailureMessage(result, expectedResult);
//        return false;
//    }
//
//    if (OC_STACK_OK == result)
//    {
//        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
//        {
//            m_failureMessage = CSCppUtilityHelper::setFailureMessage("[Cloud] CALLBACK_NOT_INVOKED");
//            return false;
//        }
//    }
    __FUNC_OUT__
    return true;
}

bool CSCppCloudHelper::readFile(const char *name, OCByteString *out)
{
    __FUNC_IN__

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
        OIC_LOG_V(ERROR, TAG, "Read %d bytes %zu times instead of %zu", length,
                realCount, count);
        goto exit;
    }

    out->bytes = buffer;
    out->len = length;

    result = true;
    exit: fclose(file);
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
    __FUNC_IN__

    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = OCSecure::saveTrustCertChain(trustCertChain, chainSize, encodingType,
            credId);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCSaveTrustCertChain returns %s",
            CommonUtil::getOCStackResult(result));

    IOTIVITYTEST_LOG(INFO, "CredId of Saved Trust Cert. Chain into Cred of SVR : %d", *credId);

    if (expectedResult != result)
    {
        m_failureMessage = CSCppUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool CSCppCloudHelper::requestCertificate(OCCloudProvisioning& ptr, ResponseCallBack callback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = ptr.requestCertificate(callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudCertificateIssueRequest returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCppUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCppUtilityHelper::setFailureMessage("[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

bool CSCppCloudHelper::getCRL(OCCloudProvisioning& ptr, ResponseCallBack callback,
        OCStackResult expectedResult)
{
    try
    {
        IOTIVITYTEST_LOG(DEBUG, "cloudGetCRL IN");
        s_isCbInvoked = CALLBACK_NOT_INVOKED;

        OCStackResult result = ptr.getCRL(callback);
        IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCCloudGetCRL returns %s",
                CommonUtil::getOCStackResult(result));

        if (expectedResult != result)
        {
            m_failureMessage = CSCppUtilityHelper::setFailureMessage(result, expectedResult);
            return false;
        }

        if (OC_STACK_OK == result)
        {
            if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
            {
                m_failureMessage = CSCppUtilityHelper::setFailureMessage("[Cloud] CALLBACK_NOT_INVOKED");
                return false;
            }
        }

    }
    catch(...)
    {
        IOTIVITYTEST_LOG(ERROR, "[Cloud] Exceptions has occured");
        return true;
    }

    __FUNC_OUT__
    return true;
}

bool CSCppCloudHelper::postCRL(OCCloudProvisioning& ptr, const std::string thisUpdate, const std::string nextUpdate,
        const OCByteString *crl, const stringArray_t *serialNumbers, ResponseCallBack callback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = ptr.postCRL(thisUpdate, nextUpdate, crl, serialNumbers, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] postCRL returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCppUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCppUtilityHelper::setFailureMessage("[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    sleep(DELAY_MEDIUM);

    __FUNC_OUT__
    return true;
}

bool CSCppCloudHelper::getAclIdByDevice(OCCloudProvisioning& ptr, const std::string deviceId,
        AclIdResponseCallBack callback, std::string &aclId, OCStackResult expectedResult)
{
    __FUNC_IN__

    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = ptr.getAclIdByDevice(deviceId, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] cloudAclIdGetByDevice returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCppUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCppUtilityHelper::setFailureMessage("[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }

        aclId = s_aclId;
    }

    __FUNC_OUT__
    return true;
}

bool CSCppCloudHelper::getIndividualAclInfo(OCCloudProvisioning& ptr, const std::string aclId,
        ResponseCallBack callback, OCStackResult expectedResult)
{
    __FUNC_IN__

    s_isCbInvoked = CALLBACK_NOT_INVOKED;

    OCStackResult result = ptr.getIndividualAclInfo(aclId, callback);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] getIndividualAclInfo returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = CSCppUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == CSCppCloudHelper::waitCallbackRet())
        {
            m_failureMessage = CSCppUtilityHelper::setFailureMessage("[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }

        //aclId = s_aclId;
    }

    __FUNC_OUT__
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
        CSCppUtilityHelper::printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }

    s_isCbInvoked = true;
}

void CSCppCloudHelper::cloudResponseCB(OCStackResult result, void* data)
{
    //char* dataChar = (char*) data;

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d", __func__, result);

    if (result < 5)
    {
        s_isCbInvoked = true;
    }
}

void CSCppCloudHelper::cloudAclResponseCB(OCStackResult result, std::string aclID)
{

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d", __func__, result);
    IOTIVITYTEST_LOG(INFO, "ACL ID : %s", aclID.c_str());

    s_aclId = aclID;

    if (result < 5)
    {
        s_isCbInvoked = true;
    }

}
std::string CSCppCloudHelper::getFailureMessage()
{
    return m_failureMessage;
}

#endif /*__CLOUD__*/
