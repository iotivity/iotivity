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

class CSCsdkGroupTest_btc: public ::testing::Test
{
protected:
    CSCsdkCloudHelper m_CloudAclHelper;
    string m_hostAddress = COAP_HOST_ADDRESS;
    string m_authProvider = "github";
    string m_authCode = "";
    string m_uidControlee = "";
    string m_authTokenControlee = "";
    string m_groupId = "";
    string m_aclId = "";
    OCAccountManager::Ptr m_accountMgrControlee = nullptr;
    std::vector< std::string > m_deviceIdAddded;
    std::vector< std::string > m_deviceIdRemoving;
    uint16_t m_credId = 0;
    ByteArray_t m_trustCertChainArray =
    { 0, 0 };

    OCDevAddr m_endPoint =
    { 0, 0 };
    stringArray_t *m_memberArray = NULL;
    stringArray_t *m_deviceArray = NULL;
    stringArray_t *m_groupArray = NULL;

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
        m_endPoint = CSCsdkUtilityHelper::getOCDevAddrEndPoint();

        m_accountMgrControlee = OCPlatform::constructAccountManagerObject(m_hostAddress,
                CT_ADAPTER_TCP);

        m_memberArray = (stringArray_t*) OICCalloc(memberNumber, sizeof(stringArray_t*));
        m_memberArray->array = memberIDs;
        m_memberArray->length = memberNumber;

        m_deviceArray = (stringArray_t*) OICCalloc(deviceNumber, sizeof(stringArray_t*));
        m_deviceArray->array = deviceIDs;
        m_deviceArray->length = deviceNumber;

        m_groupArray = (stringArray_t*) OICCalloc(1, sizeof(stringArray_t*));
        m_groupArray->array = groupIds;
        m_groupArray->length = groupNumber;
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
 * @objective       Test OCCloudAclCreateGroup negatively with groupType as NULL
 * @target          OCStackResult OCCloudAclCreateGroup(void* ctx, const char *groupType,const char *groupMasterId, const OCDevAddr *endPoint,OCCloudResponseCB callback)
 * @test_data       groupType as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclCreateGroup
 * @post_condition  none
 * @expected        OCCloudAclCreateGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclCreateGroupGroupType_NV_N)
{
    if (!m_CloudAclHelper.cloudAclCreateGroup((void*)CTX_CREATE_GROUP, NULL , GROUP_MASTER_ID,
                    &m_endPoint, CSCsdkCloudHelper::createGroupResponseCB, m_groupId, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclCreateGroup negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclCreateGroup(void* ctx, const char *groupType,const char *groupMasterId, const OCDevAddr *endPoint,OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclCreateGroup
 * @post_condition  none
 * @expected        OCCloudAclCreateGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclCreateGroupDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclCreateGroup((void*)CTX_CREATE_GROUP, GROUP_TYPE_PUBLIC, GROUP_MASTER_ID,
                    NULL, CSCsdkCloudHelper::createGroupResponseCB, m_groupId, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclFindMyGroup negatively with memberId as NULL
 * @target          OCStackResult OCCloudAclFindMyGroup(void* ctx, const char *memberId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       memberId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclFindMyGroup
 * @post_condition  none
 * @expected        OCCloudAclFindMyGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclFindMyGroupMemberId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclFindMyGroup((void*)CTX_FIND_GROUP, NULL,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclFindMyGroup negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclFindMyGroup(void* ctx, const char *memberId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclFindMyGroup
 * @post_condition  none
 * @expected        OCCloudAclFindMyGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclFindMyGroupDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclFindMyGroup((void*)CTX_FIND_GROUP, GROUP_MASTER_ID,
                    NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclFindMyGroup negatively with groupId as NULL
 * @target          OCStackResult OCCloudAclDeleteGroup(void* ctx, const char *groupId, const char *groupMasterId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclDeleteGroup
 * @post_condition  none
 * @expected        OCCloudAclDeleteGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclDeleteGroupGroupId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclDeleteGroup((void*)CTX_DELETE_GROUP, NULL, GROUP_MASTER_ID,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclFindMyGroup negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclDeleteGroup(void* ctx, const char *groupId, const char *groupMasterId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclDeleteGroup
 * @post_condition  none
 * @expected        OCCloudAclDeleteGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclDeleteGroupDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclDeleteGroup((void*)CTX_DELETE_GROUP, m_groupId.c_str(), GROUP_MASTER_ID,
                    NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclGroupGetInfo negatively with groupId as NULL
 * @target          OCStackResult OCCloudAclGroupGetInfo(void* ctx, const char *groupId, const char *memberId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclGroupGetInfo
 * @post_condition  none
 * @expected        OCCloudAclGroupGetInfo will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclGroupGetInfoGroupId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclGroupGetInfo((void*)CTX_GET_GROUP_INFO, NULL, GROUP_MASTER_ID,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclGroupGetInfo negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclGroupGetInfo(void* ctx, const char *groupId, const char *memberId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclGroupGetInfo
 * @post_condition  none
 * @expected        OCCloudAclGroupGetInfo will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclGroupGetInfoDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclGroupGetInfo((void*)CTX_GET_GROUP_INFO, m_groupId.c_str(), GROUP_MASTER_ID,
                    NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclInviteUser negatively with groupIds as NULL
 * @target          OCStackResult OCCloudAclInviteUser(void* ctx, const char *userId, const stringArray_t *groupIds, const stringArray_t *memberIds, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupIds as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclInviteUser
 * @post_condition  none
 * @expected        OCCloudAclInviteUser will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclInviteUserGroupIds_NV_N)
{
    if (!m_CloudAclHelper.cloudAclInviteUser((void*)CTX_INVITE_USER, NULL, NULL, m_memberArray,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclInviteUser negatively with memberIds as NULL
 * @target          OCStackResult OCCloudAclInviteUser(void* ctx, const char *userId, const stringArray_t *groupIds, const stringArray_t *memberIds, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       memberIds as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclInviteUser
 * @post_condition  none
 * @expected        OCCloudAclInviteUser will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclInviteUserMemberIds_NV_N)
{
    if (!m_CloudAclHelper.cloudAclInviteUser((void*)CTX_INVITE_USER, NULL, m_groupArray, NULL,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclInviteUser negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclInviteUser(void* ctx, const char *userId, const stringArray_t *groupIds, const stringArray_t *memberIds, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclInviteUser
 * @post_condition  none
 * @expected        OCCloudAclInviteUser will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclInviteUserDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclInviteUser((void*)CTX_INVITE_USER, NULL, m_groupArray, m_memberArray,
                    NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclCancelInvitation negatively with groupId as NULL
 * @target          OCStackResult OCCloudAclCancelInvitation(void* ctx, const char *userId, const char *groupId, const char *memberId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclCancelInvitation
 * @post_condition  none
 * @expected        OCCloudAclCancelInvitation will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudCancelInvitationGroupId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclCancelInvitation((void*)CTX_INVITE_USER, NULL, NULL, GROUP_MEMBER_ID_01,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclCancelInvitation negatively with memberId as NULL
 * @target          OCStackResult OCCloudAclCancelInvitation(void* ctx, const char *userId, const char *groupId, const char *memberId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       memberId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclCancelInvitation
 * @post_condition  none
 * @expected        OCCloudAclCancelInvitation will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudCancelInvitationMemberId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclCancelInvitation((void*)CTX_INVITE_USER, NULL, m_groupId.c_str(), NULL,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclCancelInvitation negatively with groupId as NULL
 * @target          OCStackResult OCCloudAclCancelInvitation(void* ctx, const char *userId, const char *groupId, const char *memberId, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclCancelInvitation
 * @post_condition  none
 * @expected        OCCloudAclCancelInvitation will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudCancelInvitationDevaddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclCancelInvitation((void*)CTX_INVITE_USER, NULL, m_groupId.c_str(), GROUP_MEMBER_ID_01,
                    NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclShareDeviceIntoGroup negatively with groupId as NULL
 * @target          OCStackResult OCCloudAclShareDeviceIntoGroup(void* ctx, const char *groupId, const stringArray_t *memberIds, const stringArray_t *deviceIds, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclShareDeviceIntoGroup
 * @post_condition  none
 * @expected        OCCloudAclShareDeviceIntoGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclShareDeviceIntoGroupGroupId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclShareDeviceIntoGroup((void*)CTX_SHARE_DEVICE, NULL, m_memberArray, m_deviceArray,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclShareDeviceIntoGroup negatively with groupId as NULL
 * @target          OCStackResult OCCloudAclShareDeviceIntoGroup(void* ctx, const char *groupId, const stringArray_t *memberIds, const stringArray_t *deviceIds, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclShareDeviceIntoGroup
 * @post_condition  none
 * @expected        OCCloudAclShareDeviceIntoGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclShareDeviceIntoGroupMemberId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclShareDeviceIntoGroup((void*)CTX_SHARE_DEVICE, m_groupId.c_str(), NULL, m_deviceArray,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclShareDeviceIntoGroup negatively with deviceId as NULL
 * @target          OCStackResult OCCloudAclShareDeviceIntoGroup(void* ctx, const char *groupId, const stringArray_t *memberIds, const stringArray_t *deviceIds, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       deviceId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclShareDeviceIntoGroup
 * @post_condition  none
 * @expected        OCCloudAclShareDeviceIntoGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclShareDeviceIntoGroupDeviceId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclShareDeviceIntoGroup((void*)CTX_SHARE_DEVICE, m_groupId.c_str(), m_memberArray, NULL,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclShareDeviceIntoGroup negatively with OCDevAddr as NULL
 * @target          OCStackResult OCCloudAclShareDeviceIntoGroup(void* ctx, const char *groupId, const stringArray_t *memberIds, const stringArray_t *deviceIds, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       OCDevAddr as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclShareDeviceIntoGroup
 * @post_condition  none
 * @expected        OCCloudAclShareDeviceIntoGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclShareDeviceIntoGroupDevAddr_NV_N)
{
    if (!m_CloudAclHelper.cloudAclShareDeviceIntoGroup((void*)CTX_SHARE_DEVICE, m_groupId.c_str(), m_memberArray, m_deviceArray,
                    NULL, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclDeleteDeviceFromGroup negatively with groupId as NULL
 * @target          OCStackResult OCCloudAclDeleteDeviceFromGroup(void* ctx, const char *groupId, const stringArray_t *memberIds, const stringArray_t *deviceIds, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       groupId as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclDeleteDeviceFromGroup
 * @post_condition  none
 * @expected        OCCloudAclDeleteDeviceFromGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclDeleteDeviceFromGroupGroupId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclDeleteDeviceFromGroup((void*)CTX_DELETE_SHARED_DEVICE, NULL, m_memberArray, m_deviceArray,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclDeleteDeviceFromGroup negatively with memberIds as NULL
 * @target          OCStackResult OCCloudAclDeleteDeviceFromGroup(void* ctx, const char *groupId, const stringArray_t *memberIds, const stringArray_t *deviceIds, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       memberIds as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclDeleteDeviceFromGroup
 * @post_condition  none
 * @expected        OCCloudAclDeleteDeviceFromGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclDeleteDeviceFromGroupMemberIds_NV_N)
{
    if (!m_CloudAclHelper.cloudAclDeleteDeviceFromGroup((void*)CTX_DELETE_SHARED_DEVICE, m_groupId.c_str(), NULL, m_deviceArray,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-01
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @objective       Test OCCloudAclDeleteDeviceFromGroup negatively with deviceIds as NULL
 * @target          OCStackResult OCCloudAclDeleteDeviceFromGroup(void* ctx, const char *groupId, const stringArray_t *memberIds, const stringArray_t *deviceIds, const OCDevAddr *endPoint, OCCloudResponseCB callback)
 * @test_data       deviceIds as NULL
 * @pre_condition   none
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCCloudAclDeleteDeviceFromGroup
 * @post_condition  none
 * @expected        OCCloudAclDeleteDeviceFromGroup will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__)
TEST_F(CSCsdkGroupTest_btc, OCCloudAclDeleteDeviceFromGroupDeviceId_NV_N)
{
    if (!m_CloudAclHelper.cloudAclDeleteDeviceFromGroup((void*)CTX_DELETE_SHARED_DEVICE, m_groupId.c_str(), m_memberArray, NULL,
                    &m_endPoint, CSCsdkCloudHelper::cloudResponseCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_CloudAclHelper.getFailureMessage());
    }
}
#endif
