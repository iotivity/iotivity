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
#include "PMCsdkHelper.h"
#include "PMCsdkCertHelper.h"
#include "PMCsdkAutoProvisionHelper.h"
#include "PMCsdkUtilityHelper.h"

class PMCsdkAutoProvision_btc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_UnownList, *m_OwnList;
    PMCsdkHelper m_PMHelper;
    PMCsdkCertHelper m_PMCertHelper;
    PMCsdkAutoProvisionHelper m_PMAutoProvisionHelper;
    OicSecRole_t role;
    OCProvisionDev_t *device1 = NULL;

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        PMCsdkUtilityHelper::removeAllResFile();
        CommonUtil::copyFile(ROOT_CERT_FILE_BACKUP, ROOT_CERT_FILE);
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O1_UNOWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O1);
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O2_UNOWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O2);
        CommonUtil::copyFile(CLIENT_CBOR_01_UNOWNED_BACKUP, CLIENT_CBOR_01);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::launchApp(JUSTWORKS_SERVER2);
        CommonUtil::waitInSecond(DELAY_LONG);

        m_UnownList = NULL;
        m_OwnList = NULL;

        if (!m_PMHelper.initProvisionClient(OTM_JUSTWORK, (char*) CLIENT_DB_01))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }

        if (!m_PMHelper.discoverUnownedDevices(DISCOVERY_TIMEOUT,
                    &m_UnownList, OC_STACK_OK))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
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

        device1 = m_OwnList;
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since           2017-04-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetCSRResource positively
 * @target          OCStackResult OCGetCSRResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OCGetCSRResultCB resultCallback)
 * @test_data       Regular data of  OCGetCSRResource API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetCSRResource
 * @post_condition  None
 * @expected        OCGetCSRResource will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkAutoProvision_btc, OCGetCSRResource_RV_SRC_P)
{
    if (!m_PMAutoProvisionHelper.getCSRResource(NULL, device1, (OCGetCSRResultCB)PMCsdkCallbackHelper::provisionGetCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-12
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetRolesResource positively
 * @target          OCStackResult OCGetRolesResource(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OCGetRolesResultCB resultCallback)
 * @test_data       Regular data of  OCGetRolesResource API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetRolesResource
 * @post_condition  None
 * @expected        OCGetRolesResource will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkAutoProvision_btc, OCGetRolesResource_RV_SRC_P)
{
    if (!m_PMAutoProvisionHelper.getRolesResource(NULL, device1, (OCGetRolesResultCB)PMCsdkCallbackHelper::provisionGetCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-12
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCGetRolesResource negatively
 * @target          OCStackResult OCGetRolesResource(void *ctx, const OCProvisionDev_t *selectedDeviceInfo, OCGetRolesResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGetRolesResource
 * @post_condition  None
 * @expected        OCGetRolesResource will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkAutoProvision_btc, OCGetRolesResource_NV_N)
{
    if (!m_PMAutoProvisionHelper.getRolesResource(NULL, device1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionSymmetricRoleCredentials positively
 * @target          OCStackResult OCProvisionSymmetricRoleCredentials(void *ctx, OicSecCredType_t type, size_t keySize,
 *                                                  const OCProvisionDev_t *pDev1,
 *                                                  const OCProvisionDev_t *pDev2,
 *                                                  const OicSecRole_t *role1,
 *                                                  const OicSecRole_t *role2,
 *                                                  OCProvisionResultCB resultCallback)
 * @test_data       Regular data of  OCProvisionSymmetricRoleCredentials API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionSymmetricRoleCredentials
 * @post_condition  None
 * @expected        OCProvisionSymmetricRoleCredentials will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkAutoProvision_btc, OCProvisionSymmetricRoleCredentials_RV_SRC_P)
{
    memset(&role, 0, sizeof(role));
    OICStrcpy(role.id, sizeof(role.id), TEST_CERT_ROLE1);

    if (!m_PMAutoProvisionHelper.provisionSymmetricRoleCredentials(NULL, SYMMETRIC_PAIR_WISE_KEY, OWNER_PSK_LENGTH_128, device1, NULL, NULL, &role, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-20
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCSaveTrustCertChain(const uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCGetCSRResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OCGetCSRResultCB resultCallback)
 * @see             OCStackResult OCVerifyCSRSignature(const char* csr)
 * @see             OCStackResult OCGetPublicKeyFromCSR(const char* csr, char** publicKey)
 * @see             OCStackResult OCGenerateRandomSerialNumber(char **serial, size_t *serialLen)
 * @see             OCStackResult OCGenerateIdentityCertificate(const OicUuid_t *subjectUuid, const char *subjectPublicKey, const char *issuerCert, const char *issuerPrivateKey, const char *serial, const char *notValidBefore, const char *notValidAfter, char **certificate, size_t *certificateLen)
 * @objective       Test OCProvisionCertificate positively with regular data
 * @target          OCStackResult OCProvisionCertificate(void *ctx, const OCProvisionDev_t *pDev, const char* pemCert, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for the API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCProvisionTrustCertChain
 *                  7. call OCGetCSRResource
 *                  8. call OCVerifyCSRSignature
 *                  9. call OCGetPublicKeyFromCSR
 *                 10. call OCGenerateRandomSerialNumber
 *                 11. call OCGenerateIdentityCertificate
 *                 12. call OCProvisionCertificate
 * @post_condition  None
 * @expected        OCProvisionCertificate will return OC_STACK_OK
 */
//#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
//TEST_F(PMCsdkAutoProvision_btc, OCProvisionCertificate_RV_SRC_P)
//{
//    uint16_t caCredId = 0;
//
//    if (!m_PMCertHelper.saveTrustCertChain((uint8_t*) PMCsdkAutoProvisionHelper::g_caCertPem, strlen(PMCsdkAutoProvisionHelper::g_caCertPem) + 1, OIC_ENCODING_PEM, &caCredId, OC_STACK_OK))
//    {
//        SET_FAILURE(m_PMCertHelper.getFailureMessage());
//        return;
//    }
//
//    OicSecCredType_t type = SIGNED_ASYMMETRIC_KEY;
//
//    if (!m_PMCertHelper.provisionTrustCertChain((void*)g_ctx, type, caCredId, device1, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
//    {
//        SET_FAILURE(m_PMCertHelper.getFailureMessage());
//        return;
//    }
//
//    char* csr = NULL;
//
//    if (!m_PMAutoProvisionHelper.getCSRResource(NULL, device1, PMCsdkCallbackHelper::provisionGetCB, OC_STACK_OK))
//    {
//        SET_FAILURE(m_PMHelper.getFailureMessage());
//        return;
//    }
//
//    if (!m_PMCertHelper.verifyCSRSignature(PMCsdkAutoProvisionHelper::g_csr, OC_STACK_OK))
//    {
//        SET_FAILURE(m_PMHelper.getFailureMessage());
//        return;
//    }
//
//    OicUuid_t uuidFromCSR =
//    {   0};
//    char* publicKeyFromCSR = NULL;
//    char* serialFromCSR = NULL;
//    size_t serialLenFromCSR;
//    char* deviceCert = NULL;
//    size_t deviceCertLen;
//
//    if (!m_PMCertHelper.getUuidFromCSR(PMCsdkAutoProvisionHelper::g_csr, &uuidFromCSR, OC_STACK_OK))
//    {
//        SET_FAILURE(m_PMHelper.getFailureMessage());
//        return;
//    }
//
//    if (!m_PMCertHelper.getPublicKeyFromCSR(PMCsdkAutoProvisionHelper::g_csr, &publicKeyFromCSR, OC_STACK_OK))
//    {
//        SET_FAILURE(m_PMHelper.getFailureMessage());
//        return;
//    }
//
//    if(!m_PMCertHelper.generateRandomSerialNumber(&serialFromCSR, &serialLenFromCSR, OC_STACK_OK))
//    {
//        SET_FAILURE(m_PMCertHelper.getFailureMessage());
//        return;
//    }
//
//    if(!m_PMCertHelper.generateIdentityCertificate(&uuidFromCSR, publicKeyFromCSR, PMCsdkAutoProvisionHelper::g_caCertPem, PMCsdkAutoProvisionHelper::g_caKeyPem, serialFromCSR, TEST_CERT_NOT_BEFORE,
//                    TEST_CERT_NOT_AFTER, &deviceCert, &deviceCertLen, OC_STACK_OK))
//    {
//        SET_FAILURE(m_PMCertHelper.getFailureMessage());
//        return;
//    }
//
//    if(!m_PMCertHelper.provisionCertificate(NULL, device1, deviceCert, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
//    {
//        SET_FAILURE(m_PMCertHelper.getFailureMessage());
//        return;
//    }
//}
//#endif

/**
 * @since           2017-04-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCSaveTrustCertChain(const uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult OCGenerateKeyPair(char **publicKey, size_t *publicKeyLen, char **privateKey, size_t *privateKeyLen)
 * @see             OCStackResult OCGenerateRandomSerialNumber(char **serial, size_t *serialLen)
 * @see             OCStackResult OCGenerateIdentityCertificate(const OicUuid_t *subjectUuid, const char *subjectPublicKey, const char *issuerCert, const char *issuerPrivateKey, const char *serial, const char *notValidBefore, const char *notValidAfter, char **certificate, size_t *certificateLen)
 * @objective       Test OCSaveOwnCertChain positively with regular data
 * @target          OCStackResult OCSaveOwnCertChain(const char* cert, const char* key, uint16_t *credId)
 * @test_data       Regular data for the API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCGenerateKeyPair
 *                  7. call OCGenerateRandomSerialNumber
 *                  8. call OCGenerateIdentityCertificate
 *                  9. call OCSaveOwnCertChain
 * @post_condition  None
 * @expected        OCSaveOwnCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkAutoProvision_btc, OCSaveOwnCertChain_RV_SRC_P)
{
    uint16_t caCredId = 0;

    if (!m_PMCertHelper.saveTrustCertChain((uint8_t*) PMCsdkAutoProvisionHelper::g_caCertPem, strlen(PMCsdkAutoProvisionHelper::g_caCertPem) + 1, OIC_ENCODING_PEM, &caCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    char* serial = NULL;
    size_t serialLen = 0;
    size_t idPublicKeyLen = 0;
    char* idKey = NULL;
    size_t idKeyLen = 0;
    char* idCert = NULL;
    size_t idCertLen = 0;
    char* g_idPublicKey = NULL;

    if(!m_PMCertHelper.generateKeyPair(&g_idPublicKey, &idPublicKeyLen, &idKey, &idKeyLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.generateRandomSerialNumber(&serial, &serialLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    OCUUIdentity deviceId =
    {   0};

    if(!m_PMCertHelper.getDeviceId(&deviceId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    OicUuid_t uuid =
    {   0};
    OicUuid_t* uuidForCert;
    memcpy(uuid.id, deviceId.id, sizeof(uuid.id));
    uuidForCert = &uuid;

    PMCsdkUtilityHelper::printUuid(uuidForCert);

    if(!m_PMCertHelper.generateIdentityCertificate(&uuid, g_idPublicKey, PMCsdkAutoProvisionHelper::g_caCertPem, PMCsdkAutoProvisionHelper::g_caKeyPem, serial, TEST_CERT_NOT_BEFORE,
                    TEST_CERT_NOT_AFTER, &idCert, &idCertLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    uint16_t idCertCredId = 0;

    if(!m_PMCertHelper.saveOwnCertChain(idCert, idKey, &idCertCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-04-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCSaveTrustCertChain(const uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult OCGenerateKeyPair(char **publicKey, size_t *publicKeyLen, char **privateKey, size_t *privateKeyLen)
 * @see             OCStackResult OCGenerateRandomSerialNumber(char **serial, size_t *serialLen)
 * @see             OCStackResult OCGenerateIdentityCertificate(const OicUuid_t *subjectUuid, const char *subjectPublicKey, const char *issuerCert, const char *issuerPrivateKey, const char *serial, const char *notValidBefore, const char *notValidAfter, char **certificate, size_t *certificateLen)
 * @see             OCStackResult OCSaveOwnCertChain(const char* cert, const char* key, uint16_t *credId)
 * @see             OCStackResult OCGenerateRandomSerialNumber(char **serial, size_t *serialLen)
 * @see             OCStackResult OCGenerateRoleCertificate(const OicUuid_t *subjectUuid, const char *subjectPublicKey, const char *issuerCert, const char *issuerPrivateKey, const char *serial, const char *notValidBefore, const char *notValidAfter, const char *role, const char *authority, char **certificate, size_t *certificateLen)
 * @objective       Test OCSaveOwnRoleCert positively with regular data
 * @target          OCStackResult OCSaveOwnRoleCert(const char* cert, uint16_t *credId)
 * @test_data       Regular data for the API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCSaveTrustCertChain
 *                  6. call OCGenerateKeyPair
 *                  7. call OCGenerateRandomSerialNumber
 *                  8. call OCGenerateIdentityCertificate
 *                  9. call OCSaveOwnCertChain
 *                 10. call OCGenerateRandomSerialNumber
 *                 11. call OCGenerateRoleCertificate
 *                 12. call OCSaveOwnRoleCert
 * @post_condition  None
 * @expected        OCSaveOwnRoleCert will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkAutoProvision_btc, OCSaveOwnRoleCert_RV_SRC_P)
{
    uint16_t caCredId = 0;

    if (!m_PMCertHelper.saveTrustCertChain((uint8_t*) PMCsdkAutoProvisionHelper::g_caCertPem, strlen(PMCsdkAutoProvisionHelper::g_caCertPem) + 1, OIC_ENCODING_PEM, &caCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    char* serial = NULL;
    size_t serialLen = 0;
    size_t idPublicKeyLen = 0;
    char* idKey = NULL;
    size_t idKeyLen = 0;
    char* idCert = NULL;
    size_t idCertLen = 0;
    char* g_idPublicKey = NULL;

    if(!m_PMCertHelper.generateKeyPair(&g_idPublicKey, &idPublicKeyLen, &idKey, &idKeyLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.generateRandomSerialNumber(&serial, &serialLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    OCUUIdentity deviceId =
    {   0};

    if(!m_PMCertHelper.getDeviceId(&deviceId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    OicUuid_t uuid =
    {   0};
    OicUuid_t* uuidForCert;
    memcpy(uuid.id, deviceId.id, sizeof(uuid.id));
    uuidForCert = &uuid;

    PMCsdkUtilityHelper::printUuid(uuidForCert);

    if(!m_PMCertHelper.generateIdentityCertificate(&uuid, g_idPublicKey, PMCsdkAutoProvisionHelper::g_caCertPem, PMCsdkAutoProvisionHelper::g_caKeyPem, serial, TEST_CERT_NOT_BEFORE,
                    TEST_CERT_NOT_AFTER, &idCert, &idCertLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    uint16_t idCertCredId = 0;

    if(!m_PMCertHelper.saveOwnCertChain(idCert, idKey, &idCertCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    char* serialRole = NULL;
    size_t serialLenRole = 0;
    char* roleCert = NULL;
    size_t roleCertLen = 0;

    if(!m_PMCertHelper.generateRandomSerialNumber(&serialRole, &serialLenRole, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.getDeviceId(&deviceId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    OicUuid_t uuidRole =
    {   0};
    OicUuid_t* uuidForCertRole;
    memcpy(uuidRole.id, deviceId.id, sizeof(uuidRole.id));
    uuidForCertRole = &uuidRole;

    PMCsdkUtilityHelper::printUuid(uuidForCert);

    const char *role = TEST_CERT_ROLE1;
    const char *authority = NULL;
    if(!m_PMAutoProvisionHelper.generateRoleCertificate(&uuidRole, g_idPublicKey, PMCsdkAutoProvisionHelper::g_caCertPem, PMCsdkAutoProvisionHelper::g_caKeyPem, serial, TEST_CERT_NOT_BEFORE,
                    TEST_CERT_NOT_AFTER, role, authority, &roleCert, &roleCertLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    uint16_t roleCertCredId = 0;
    if(!m_PMAutoProvisionHelper.saveOwnRoleCert(roleCert, &roleCertCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-04-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCRemoveCredential positively with regular data
 * @target          OCStackResult OCRemoveCredential(const OicUuid_t* subjectUuid)
 * @test_data       Regular data for the API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCRemoveCredential
 * @post_condition  None
 * @expected        OCRemoveCredential will return OC_STACK_RESOURCE_DELETED
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkAutoProvision_btc, OCRemoveCredential_RV_SRC_P)
{
    OicUuid_t deviceUuid = device1->doxm->deviceID;

    if(!m_PMAutoProvisionHelper.removeCredential(&deviceUuid, OC_STACK_RESOURCE_DELETED))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif

/**
 * @since           2017-04-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCSaveOwnRoleCert(const char* cert, uint16_t *credId)
 * @objective       Test OCDeleteRoleCertificateByCredId positively with regular data
 * @target          OCStackResult OCRemoveCredential(const OicUuid_t* subjectUuid)
 * @test_data       Regular data for the API
 * @pre_condition   1. run iotivity_pm_client
 *                  2. start two justworks simulators using cbor files from 'owned' direcotry
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCSaveOwnRoleCert
 *                  6. call OCDeleteRoleCertificateByCredId
 * @post_condition  None
 * @expected        OCDeleteRoleCertificateByCredId will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkAutoProvision_btc, OCDeleteRoleCertificateByCredId_RV_SRC_P)
{
    uint16_t caCredId = 0;

    if (!m_PMCertHelper.saveTrustCertChain((uint8_t*) PMCsdkAutoProvisionHelper::g_caCertPem, strlen(PMCsdkAutoProvisionHelper::g_caCertPem) + 1, OIC_ENCODING_PEM, &caCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    char* serial = NULL;
    size_t serialLen = 0;
    size_t idPublicKeyLen = 0;
    char* idKey = NULL;
    size_t idKeyLen = 0;
    char* idCert = NULL;
    size_t idCertLen = 0;
    char* g_idPublicKey = NULL;

    if(!m_PMCertHelper.generateKeyPair(&g_idPublicKey, &idPublicKeyLen, &idKey, &idKeyLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.generateRandomSerialNumber(&serial, &serialLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    OCUUIdentity deviceId =
    {   0};

    if(!m_PMCertHelper.getDeviceId(&deviceId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    OicUuid_t uuid =
    {   0};
    OicUuid_t* uuidForCert;
    memcpy(uuid.id, deviceId.id, sizeof(uuid.id));
    uuidForCert = &uuid;

    PMCsdkUtilityHelper::printUuid(uuidForCert);

    if(!m_PMCertHelper.generateIdentityCertificate(&uuid, g_idPublicKey, PMCsdkAutoProvisionHelper::g_caCertPem, PMCsdkAutoProvisionHelper::g_caKeyPem, serial, TEST_CERT_NOT_BEFORE,
                    TEST_CERT_NOT_AFTER, &idCert, &idCertLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    uint16_t idCertCredId = 0;

    if(!m_PMCertHelper.saveOwnCertChain(idCert, idKey, &idCertCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    char* serialRole = NULL;
    size_t serialLenRole = 0;
    char* roleCert = NULL;
    size_t roleCertLen = 0;

    if(!m_PMCertHelper.generateRandomSerialNumber(&serialRole, &serialLenRole, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.getDeviceId(&deviceId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    OicUuid_t uuidRole =
    {   0};
    OicUuid_t* uuidForCertRole;
    memcpy(uuidRole.id, deviceId.id, sizeof(uuidRole.id));
    uuidForCertRole = &uuidRole;

    PMCsdkUtilityHelper::printUuid(uuidForCert);

    const char *role = TEST_CERT_ROLE1;
    const char *authority = NULL;
    if(!m_PMAutoProvisionHelper.generateRoleCertificate(&uuidRole, g_idPublicKey, PMCsdkAutoProvisionHelper::g_caCertPem, PMCsdkAutoProvisionHelper::g_caKeyPem, serial, TEST_CERT_NOT_BEFORE,
                    TEST_CERT_NOT_AFTER, role, authority, &roleCert, &roleCertLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    uint16_t roleCertCredId = 0;
    if(!m_PMAutoProvisionHelper.saveOwnRoleCert(roleCert, &roleCertCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMAutoProvisionHelper.deleteRoleCertificateByCredId(NULL, device1, PMCsdkCallbackHelper::provisionPostCB, roleCertCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }
}
#endif
