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
#include "PMCsdkHelper.h"
#include "PMCsdkUtilityHelper.h"
#include "SecurityCommonUtil.h"

class PMCsdkProvTest_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_OwnList, *m_UnownList;
    OicSecAcl_t *m_Acl;
    PMCsdkHelper m_PMHelper;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCsdkUtilityHelper::removeAllResFile();
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O1_UNOWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O1);
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O2_UNOWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O2);
        CommonUtil::copyFile(CLIENT_CBOR_01_UNOWNED_BACKUP, CLIENT_CBOR_01);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::launchApp(JUSTWORKS_SERVER2);
        CommonUtil::waitInSecond(DELAY_LONG);
        m_UnownList = NULL;
        m_OwnList = NULL;
        m_Acl = NULL;

        if (!m_PMHelper.initProvisionClient(OTM_JUSTWORK, (char*) CLIENT_DB_01))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }

        if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    &m_UnownList, OC_STACK_OK))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }

        if (!m_PMHelper.doOwnerShipTransfer((void*)g_ctx, &m_UnownList, PMCsdkHelper::ownershipTransferCB, OC_STACK_OK))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }

        if (!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);

        if (!m_PMHelper.deleteACLList(m_Acl))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
        }

        if(!m_PMHelper.deleteDiscoveredDevices(m_OwnList))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }
    }
};

/**
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials positively.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       Regular data OCProvisionCredentials
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredential_SRC_RV_P)
{
    OCProvisionDev_t *device1 = m_OwnList;
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials positively with Cred Type with Representative value of SYMMETRIC_PAIR_WISE_KEY.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       type = representative value of SYMMETRIC_PAIR_WISE_KEY
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialCredType_REV_P)
{
    OCProvisionDev_t *device1 = m_OwnList;
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = (OicSecCredType_t)SYM_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-23
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials positively with Upper Boundary Value of Keysize.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       keySize = OWNER_PSK_LENGTH_256
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialKeySize_UBV_P)
{
    OCProvisionDev_t *device1 = m_OwnList;
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-12-08
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with callback as NULL.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialCB_ECRC_NV_N)
{
    OCProvisionDev_t *device1 = m_OwnList;
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Device1 ID as NULL.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       pDev1 = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialDev1_NV_N)
{
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, NULL,
                    device2, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Device2 ID as NULL.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       pDev2 = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialDev2_NV_N)
{
    OCProvisionDev_t *device1 = m_OwnList;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    NULL, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Lower Outer Boundary Value of KeySize.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       keySize is Out of Lower Boundary Value
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialKeysize_LOBV_N)
{

    OCProvisionDev_t *device1 = m_OwnList;
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_128 - 1;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively Upper Outer Boundary Value of KeySize
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       keySize is Out of Upper Boundary Value
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialKeysize_UOBV_N)
{
    OCProvisionDev_t *device1 = m_OwnList;
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
    size_t keySize = OWNER_PSK_LENGTH_256 + 1;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Lower Outer Boundary Value of Cred Type.
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       OicSecCredType_t is out of Lower Boundary Value
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialCredType_LOBV_N)
{
    OCProvisionDev_t *device1 = m_OwnList;
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = (OicSecCredType_t)CRED_TYPE_LOBV;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2015-02-24
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionCredentials negatively with Upper Outer Boundary Value of Cred Type
 * @target          OCStackResult OCProvisionCredentials(void *ctx, OicSecCredType_t type, size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, OCProvisionResultCB resultCallback)
 * @test_data       OicSecCredType_t is out of Upper Boundary Value
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionCredentials
 * @post_condition  None
 * @expected        OCProvisionCredentials will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionCredentialCredType_UOBV_N)
{
    OCProvisionDev_t *device1 = m_OwnList;
    OCProvisionDev_t *device2 = m_OwnList->next;
    OicSecCredType_t type = (OicSecCredType_t)CRED_TYPE_UOBV;
    size_t keySize = OWNER_PSK_LENGTH_128;

    if (!m_PMHelper.provisionCredentials((void*)ctxProvCreadential, type, keySize, device1,
                    device2, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL positively.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for OCProvisionACL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionACL
 *                  6. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionAcl_SRC_RV_P)
{
    m_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionACL((void*)g_ctx, m_OwnList, m_Acl,
                    PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL negatively with Device List NULL.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       selectedDeviceInfo = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionACL
 *                  6. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionAclDeviceList_ECRC_NV_N)
{
    m_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionACL((void*)g_ctx, NULL, m_Acl,
                    PMCsdkCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL negatively with Access Control List NULL.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       acl = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionACL
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionAclAcl_NV_N)
{
    if (!m_PMHelper.provisionACL((void*)g_ctx, m_OwnList, NULL,
                    PMCsdkCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL negatively with callback as NULL.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionACL
 *                  6. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionAclCB_NV_N)
{
    m_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionACL((void*)g_ctx, m_OwnList, m_Acl,
                    NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL positively by calling provision acl multiple time.
 * @target          OCStackResult OCProvisionACL(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       Call OCProvisionACL multiple time
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionACL
 *                  6. call OCProvisionACL
 *                  7. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCProvisionACL will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionAclMultipleTime_P)
{
    m_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionACL((void*)g_ctx, m_OwnList, m_Acl,
                    PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMHelper.provisionACL((void*)g_ctx, m_OwnList, m_Acl,
                    PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-02-13
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionACL2 positively.
 * @target          OCStackResult OCProvisionACL2(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecAcl_t *acl, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for OCProvisionACL2
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionACL2
 *                  6. call OCDeleteACLList for m_Acl
 * @post_condition  None
 * @expected        OCProvisionACL2 will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkProvTest_btc, ProvisionAcl2_SRC_RV_P)
{
    m_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &m_OwnList);

    if (!m_PMHelper.provisionACL2((void*)g_ctx, m_OwnList, m_Acl,
                    PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif
