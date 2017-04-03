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

class CSCsdkPolicyCheckTest_btc: public ::testing::Test
{
protected:

    CSCsdkCloudHelper m_CloudAclHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    OCAccountManager::Ptr m_accountMgrControlee = nullptr;
    OCDevAddr m_endPoint = {0, 0};
    virtual void SetUp()
    {
        if (!m_CloudAclHelper.initCloudACLClient())
        {
            SET_FAILURE(m_CloudAclHelper.getFailureMessage());
            return;
        }

        m_hostAddress = CloudCommonUtil::getDefaultHostAddess();
        m_endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

        m_accountMgrControlee = OCPlatform::constructAccountManagerObject(m_hostAddress,
                CT_ADAPTER_TCP);
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
    }
};

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclPolicyCheck negatively with subjectId as NULL
 * @target          OCStackResult OCCloudAclPolicyCheck(void* ctx, const char *subjectId, const char *deviceId, const char *method, const char *uri, const OCDevAddr *endPoint, OCCloudResponseCB callback);
 * @test_data       subjectId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  7. call OCCloudAclPolicyCheck
 * @post_condition  none
 * @expected        OCCloudAclPolicyCheck will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkPolicyCheckTest_btc, OCCloudAclPolicyCheckGetAllowedSubjectId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclPolicyCheck((void*) CTX_INDIVIDUAL_GET_INFO, NULL,
                    CSCsdkCloudHelper::s_deviceId.c_str(), m_CloudAclHelper.GET_REQUEST.c_str(),
                    CSCsdkCloudHelper::s_href.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclPolicyCheck negatively with deviceId as NULL
 * @target          OCStackResult OCCloudAclPolicyCheck(void* ctx, const char *subjectId, const char *deviceId, const char *method, const char *uri, const OCDevAddr *endPoint, OCCloudResponseCB callback);
 * @test_data       deviceId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  7. call OCCloudAclPolicyCheck
 * @post_condition  none
 * @expected        OCCloudAclPolicyCheck will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkPolicyCheckTest_btc, OCCloudAclPolicyCheckGetAllowedDeviceId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclPolicyCheck((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_subjectuuid.c_str(),
                    NULL, m_CloudAclHelper.GET_REQUEST.c_str(),
                    CSCsdkCloudHelper::s_href.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclPolicyCheck negatively with method as NULL
 * @target          OCStackResult OCCloudAclPolicyCheck(void* ctx, const char *subjectId, const char *deviceId, const char *method, const char *uri, const OCDevAddr *endPoint, OCCloudResponseCB callback);
 * @test_data       method as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  7. call OCCloudAclPolicyCheck
 * @post_condition  none
 * @expected        OCCloudAclPolicyCheck will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkPolicyCheckTest_btc, OCCloudAclPolicyCheckGetAllowedMethod_NV_N)
{
    if (!m_CloudAclHelper.cloudAclPolicyCheck((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_subjectuuid.c_str(),
                    CSCsdkCloudHelper::s_deviceId.c_str(), NULL,
                    CSCsdkCloudHelper::s_href.c_str(), &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclPolicyCheck negatively with uri as NULL
 * @target          OCStackResult OCCloudAclPolicyCheck(void* ctx, const char *subjectId, const char *deviceId, const char *method, const char *uri, const OCDevAddr *endPoint, OCCloudResponseCB callback);
 * @test_data       uri as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  7. call OCCloudAclPolicyCheck
 * @post_condition  none
 * @expected        OCCloudAclPolicyCheck will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkPolicyCheckTest_btc, OCCloudAclPolicyCheckGetAllowedUri_NV_N)
{
    if (!m_CloudAclHelper.cloudAclPolicyCheck((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_subjectuuid.c_str(),
                    CSCsdkCloudHelper::s_deviceId.c_str(), m_CloudAclHelper.GET_REQUEST.c_str(),
                    NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclPolicyCheck negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclPolicyCheck(void* ctx, const char *subjectId, const char *deviceId, const char *method, const char *uri, const OCDevAddr *endPoint, OCCloudResponseCB callback);
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  7. call OCCloudAclPolicyCheck
 * @post_condition  none
 * @expected        OCCloudAclPolicyCheck will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkPolicyCheckTest_btc, OCCloudAclPolicyCheckGetAllowedDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclPolicyCheck((void*) CTX_INDIVIDUAL_GET_INFO, CSCsdkCloudHelper::s_subjectuuid.c_str(),
                    CSCsdkCloudHelper::s_deviceId.c_str(), m_CloudAclHelper.GET_REQUEST.c_str(),
                    CSCsdkCloudHelper::s_href.c_str(), NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

