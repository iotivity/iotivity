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
#include "CSCsdkCloudHelper.h"
#include "CSCsdkUtilityHelper.h"

class CSCsdkCloudTest_btc: public ::testing::Test
{
protected:
    CSCsdkCloudHelper m_CloudAclHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    string m_uidControlee = "";
    string m_authTokenControlee = "";
    OCAccountManager::Ptr m_accountMgrControlee = nullptr;
    uint16_t m_credId = 0;
    ByteArray_t m_trustCertChainArray =
    { 0, 0 };

    virtual void SetUp()
    {
        CommonUtil::copyFile(CLOUD_ACL_CONTROLLER_DAT_BACKUP, CLOUD_ACL_CONTROLLER_DAT);
        CommonUtil::copyFile(ROOT_CERT_FILE_BACKUP, ROOT_CERT_FILE);
        CommonTestUtil::runCommonTCSetUpPart();

        if (!m_CloudAclHelper.initCloudACLClient())
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
            return;
        }

        m_hostAddress = CloudCommonUtil::getDefaultHostAddess();

        CSCsdkCloudHelper::readFile(ROOT_CERT_FILE, (OCByteString *) &m_trustCertChainArray);
        m_accountMgrControlee = OCPlatform::constructAccountManagerObject(m_hostAddress,
                CT_ADAPTER_TCP);
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
    }
};


/**
 * @since           2016-12-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudCertificateIssueRequest negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudCertificateIssueRequest(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudCertificateIssueRequest
 * @post_condition  none
 * @expected        OCCloudCertificateIssueRequest will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkCloudTest_btc, OCCloudCertificateIssueRequestDev_NV_N)
{
    if (!m_CloudAclHelper.cloudCertificateIssueRequest((void*) CTX_CERT_REQ_ISSUE, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-12-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudGetCRL negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudGetCRL(void* ctx, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudGetCRL
 * @post_condition  none
 * @expected        OCCloudGetCRL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkCloudTest_btc, OCCloudGetCRLDev_NV_N)
{
    if (!m_CloudAclHelper.cloudGetCRL((void*) CTX_GET_CRL, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-12-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudPostCRL negatively with thisUpdate as NULL
 * @target          OCStackResult OCCloudPostCRL(void* ctx, const char *thisUpdate, const char *nextUpdate, const OCByteString *crl, const stringArray_t *serialNumbers, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       thisUpdate as NULL
 * @pre_condition   nonecc_str()
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudPostCRL
 * @post_condition  none
 * @expected        OCCloudPostCRL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkCloudTest_btc, OCCloudPostCRLThisUpdate_NV_N)
{
    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudPostCRL((void*)CTX_POST_CRL, NULL, CRL_NEXT_DATE.c_str(), NULL, NULL, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-12-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudPostCRL negatively with nextUpdate as NULL
 * @target          OCStackResult OCCloudPostCRL(void* ctx, const char *thisUpdate, const char *nextUpdate, const OCByteString *crl, const stringArray_t *serialNumbers, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       nextUpdate as NULL
 * @pre_condition   nonecc_str()
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudPostCRL
 * @post_condition  none
 * @expected        OCCloudPostCRL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkCloudTest_btc, OCCloudPostCRLNextUpdate_NV_N)
{
    OCDevAddr endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

    if (!m_CloudAclHelper.cloudPostCRL((void*)CTX_POST_CRL, CRL_THIS_UPDATE.c_str(), NULL, NULL, NULL, &endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-12-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudPostCRL negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudPostCRL(void* ctx, const char *thisUpdate, const char *nextUpdate, const OCByteString *crl, const stringArray_t *serialNumbers, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   nonecc_str()
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudPostCRL
 * @post_condition  none
 * @expected        OCCloudPostCRL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkCloudTest_btc, OCCloudPostCRLDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudPostCRL((void*)CTX_POST_CRL, CRL_THIS_UPDATE.c_str(), CRL_NEXT_DATE.c_str(), NULL, NULL, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
