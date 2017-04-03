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

class CSCsdkGetInvitationTest_btc: public ::testing::Test
{
protected:

    CSCsdkCloudHelper m_CloudAclHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    OCDevAddr m_endPoint;
    string m_authProvider = "github";
    string m_authCode = "";
    string m_groupId = "";
    string m_aclId = "";
    string m_uidControlee = "";
    string m_authTokenControlee = "";

    OCAccountManager::Ptr m_accountMgrControlee = nullptr;

    std::vector< std::string > m_deviceIdAddded;
    std::vector< std::string > m_deviceIdRemoving;
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
 * @objective       Test OCCloudAclGetInvitation negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclGetInvitation(void* ctx, const char *userId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclGetInvitation
 * @post_condition  none
 * @expected        OCCloudAclGetInvitation will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_btc, OCCloudAclGetInvitationDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclGetInvitation((void*)CTX_GET_GROUP_INVITATION, NULL, NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclGetInvitation negatively with groupId as NULL
 * @target          OCStackResult OCCloudAclDeleteInvitation(void* ctx, const char *userId, const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclDeleteInvitation
 * @post_condition  none
 * @expected        OCCloudAclDeleteInvitation will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_btc, OCCloudAclDeleteInvitationGroupId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclDeleteInvitation((void*)CTX_DELETE_INVITATION, NULL, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclGetInvitation negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclDeleteInvitation(void* ctx, const char *userId, const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclDeleteInvitation
 * @post_condition  none
 * @expected        OCCloudAclDeleteInvitation will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_btc, OCCloudAclDeleteInvitationDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclDeleteInvitation((void*)CTX_DELETE_INVITATION, NULL, m_groupId.c_str(), NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclJoinToInvitedGroup negatively with groupId as NULL
 * @target          OCStackResult OCCloudAclJoinToInvitedGroup(void* ctx, const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclJoinToInvitedGroup
 * @post_condition  none
 * @expected        OCCloudAclJoinToInvitedGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_btc, OCCloudAclJoinToInvitedGroupGroupId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclJoinToInvitedGroup((void*)CTX_JOIN_GROUP, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclJoinToInvitedGroup negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclJoinToInvitedGroup(void* ctx, const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclJoinToInvitedGroup
 * @post_condition  none
 * @expected        OCCloudAclJoinToInvitedGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_btc, OCCloudAclJoinToInvitedGroupDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclJoinToInvitedGroup((void*)CTX_JOIN_GROUP, m_groupId.c_str(), NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCCloudAclJoinToInvitedGroup(void* ctx, const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclObserveGroup negatively with groupId as NULL
 * @target          OCStackResult OCCloudAclObserveGroup(void* ctx,const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  8. call OCCloudAclObserveGroup
 * @post_condition  none
 * @expected        OCCloudAclObserveGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_btc, OCCloudAclObserveGroupGroupId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclObserveGroup((void*)CTX_OBSERVER_GROUP, NULL, &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCCloudAclJoinToInvitedGroup(void* ctx, const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @objective       Test OCCloudAclObserveGroup negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclObserveGroup(void* ctx,const char *groupId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  8. call OCCloudAclObserveGroup
 * @post_condition  none
 * @expected        OCCloudAclObserveGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGetInvitationTest_btc, OCCloudAclObserveGroupDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclObserveGroup((void*)CTX_OBSERVER_GROUP, m_groupId.c_str(), NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
