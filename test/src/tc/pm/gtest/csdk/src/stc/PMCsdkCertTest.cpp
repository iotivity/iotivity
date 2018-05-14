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
#include "PMCsdkCertHelper.h"
#include "PMCsdkUtilityHelper.h"

class PMCsdkCertTest_stc: public ::testing::Test
{
protected:

    OCProvisionDev_t *m_OwnList;
    OicSecAcl_t *m_Acl, *m_Acl1, *m_Acl2;
    PMCsdkHelper m_PMHelper;
    PMCsdkCertHelper m_PMCertHelper;

    char* g_caKeyPem; /* Test CA private key */
    char* g_caCertPem; /* Test CA certificate */
    uint16_t g_caCredId = 0; /* ID of CA's OCF identity cert */

    virtual void SetUp()
    {
        CommonTestUtil::runCommonTCSetUpPart();
        CommonUtil::killApp(KILL_SERVERS);
        CommonUtil::waitInSecond(DELAY_MEDIUM);
        PMCsdkUtilityHelper::removeAllResFile();
        CommonUtil::waitInSecond(DELAY_LONG);
        CommonUtil::copyFile(JUSTWORKS_SERVER_CBOR_O1_OWNED_BACKUP, JUSTWORKS_SERVER_CBOR_O1);
        CommonUtil::copyFile(CLIENT_CBOR_01_OWNED_BACKUP, CLIENT_CBOR_01);
        CommonUtil::copyFile(DEVICE_PROP_CBOR_01_OWNED_BACKUP, DEVICE_PROP_CBOR_01);
        CommonUtil::copyFile(CLIENT_DB_01_OWNED_BACKUP, CLIENT_DB_01);
        CommonUtil::copyFile(ROOT_CERT_UNOWNED_BACKUP, ROOT_CERT);
        CommonUtil::launchApp(JUSTWORKS_SERVER1);
        CommonUtil::waitInSecond(DELAY_LONG);

        m_OwnList = NULL;
        g_caKeyPem = NULL;
        g_caCertPem = NULL;

        if (!m_PMHelper.initProvisionClient())
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }

        if(!m_PMHelper.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList, OC_STACK_OK))
        {
            SET_FAILURE(m_PMHelper.getFailureMessage());
            return;
        }
    }

    virtual void TearDown()
    {
        CommonTestUtil::runCommonTCTearDownPart();
        CommonUtil::killApp(KILL_SERVERS);
    }
};

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain positively with regular data
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for the API
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_stc, ProvisionTrustCertChain_SRC_RV_P)
{
    if (!m_PMCertHelper.registerTrustCertChainNotifier((void*)CTX_PROV_TRUST_CERT, PMCsdkCertHelper::trustCertChainChangeCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    ByteArray_t g_trustCertChainArray =
    {   0, 0};

    m_PMCertHelper.readFile(ROOT_CERT_FILE, (OCByteString *) &g_trustCertChainArray);

    uint16_t g_credId = 0;
    if (!m_PMCertHelper.saveTrustCertChain(g_trustCertChainArray.data, g_trustCertChainArray.len,
                    OIC_ENCODING_PEM, &g_credId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }

    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, g_credId, device1, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain negatively with regular data
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       OCProvisionDev_t = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_INVALID_PARAM
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_stc, ProvisionTrustCertChainDev_NV_N)
{
    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, 1, NULL, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_INVALID_PARAM))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain negatively with credId as -1
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       credId = -1
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_stc, ProvisionTrustCertChainCredId_LOBV_NV_N)
{
    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, CRED_ID_NEGATIVE, device1, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2016-10-19
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @objective       Test OCProvisionTrustCertChain negatively with resultCallback as NULL
 * @target          OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @test_data       resultCallback = NULL
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCProvisionTrustCertChain
 * @post_condition  None
 * @expected        OCProvisionTrustCertChain will return OC_STACK_INVALID_CALLBACK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_stc, ProvisionTrustCertChainCb_NV_N)
{
    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, 1, device1, NULL, OC_STACK_INVALID_CALLBACK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
    }
}
#endif

/**
 * @since           2017-04-21
 * @see             OCStackResult OCRegisterPersistentStorageHandler(OCPersistentStorage* persistentStorageHandler)
 * @see             OCStackResult OCInit(const char *ipAddr, uint16_t port, OCMode mode)
 * @see             OCStackResult OCInitPM(const char* dbPath)
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCGenerateKeyPair(char **publicKey, size_t *publicKeyLen, char **privateKey, size_t *privateKeyLen)
 * @see             OCStackResult OCGenerateRandomSerialNumber(char **serial, size_t *serialLen)
 * @see             OCStackResult OCGenerateRootCACertificate(const char *subject, const char *subjectPublicKey, const char *issuerCert, const char *issuerPrivateKey, const char *serial, const char *notValidBefore, const char *notValidAfter, char **certificate, size_t *certificateLen)
 * @see             OCStackResult OCSaveTrustCertChain(const uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult OCGenerateKeyPair(char **publicKey, size_t *publicKeyLen, char **privateKey, size_t *privateKeyLen)
 * @see             OCStackResult OCGenerateRandomSerialNumber(char **serial, size_t *serialLen)
 * @see             OCStackResult OCGenerateIdentityCertificate(const OicUuid_t *subjectUuid, const char *subjectPublicKey, const char *issuerCert, const char *issuerPrivateKey, const char *serial, const char *notValidBefore, const char *notValidAfter, char **certificate, size_t *certificateLen)
 * @see             OCStackResult OCSaveOwnCertChain(const char* cert, const char* key, uint16_t *credId)
 * @see             OCStackResult OCProvisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId, const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCGetCSRResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OCGetCSRResultCB resultCallback)
 * @see             OCStackResult OCVerifyCSRSignature(const char* csr)
 * @see             OCStackResult OCGetPublicKeyFromCSR(const char* csr, char** publicKey)
 * @see             OCStackResult OCGenerateRandomSerialNumber(char **serial, size_t *serialLen)
 * @see             OCStackResult OCGenerateIdentityCertificate(const OicUuid_t *subjectUuid, const char *subjectPublicKey, const char *issuerCert, const char *issuerPrivateKey, const char *serial, const char *notValidBefore, const char *notValidAfter, char **certificate, size_t *certificateLen)
 * @objective       Test OCProvisionCertificate positively with regular data
 * @target          OCStackResult OCProvisionCertificate(void *ctx, const OCProvisionDev_t *pDev, const char* pemCert, OCProvisionResultCB resultCallback)
 * @test_data       Regular data for the API
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGenerateKeyPair
 *                  6. call OCGenerateRandomSerialNumber
 *                  7. call OCGenerateRootCACertificate
 *                  8. call OCSaveTrustCertChain
 *                  9. call OCGenerateKeyPair
 *                 10. call OCGenerateRandomSerialNumber
 *                 11. call OCGenerateIdentityCertificate
 *                 12. call OCSaveOwnCertChain
 *                 13. call OCProvisionTrustCertChain
 *                 14. call OCGetCSRResource
 *                 15. call OCVerifyCSRSignature
 *                 16. call OCGetPublicKeyFromCSR
 *                 17. call OCGenerateRandomSerialNumber
 *                 18. call OCGenerateIdentityCertificate
 *                 19. call OCProvisionCertificate
 * @post_condition  None
 * @expected        OCProvisionCertificate will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_stc, ProvisionCertificate_SRC_RV_P)
{
    char* publicKey = NULL;
    size_t publicKeyLen = 0;
    size_t caKeyLen = 0;
    char* serial = NULL;
    size_t serialLen = 0;
    size_t caCertLen = 0;
    char* idPublicKey = NULL;
    char* idKey = NULL;
    char* idCert = NULL;
    size_t idCertLen = 0;
    size_t idKeyLen = 0;

    /* Create CA keypair, serial number and Create a CA certificate */

    if(!m_PMCertHelper.generateKeyPair(&publicKey, &publicKeyLen, &g_caKeyPem, &caKeyLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.generateRandomSerialNumber(&serial, &serialLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.generateCACertificate(CA_SUBJECT, publicKey, NULL, g_caKeyPem, serial, TEST_CERT_NOT_BEFORE,
                    TEST_CERT_NOT_AFTER, &g_caCertPem, &caCertLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    uint16_t g_credId = 0;

    if (!m_PMCertHelper.saveTrustCertChain((uint8_t*) g_caCertPem, caCertLen, OIC_ENCODING_PEM, &g_caCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    /* Create identity certificate for use by the CA. */

    if(!m_PMCertHelper.generateKeyPair(&idPublicKey, &publicKeyLen, &idKey, &idKeyLen, OC_STACK_OK))
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
    memcpy(uuid.id, deviceId.id, sizeof(uuid.id));

    if(!m_PMCertHelper.generateIdentityCertificate(&uuid, idPublicKey, g_caCertPem, g_caKeyPem, serial, TEST_CERT_NOT_BEFORE,
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

    OCProvisionDev_t *device1 = m_OwnList;

    if (!m_PMCertHelper.provisionTrustCertChain((void*)CTX_PROV_TRUST_CERT, SIGNED_ASYMMETRIC_KEY, g_caCredId, device1, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if (!m_PMCertHelper.getCSRResource(NULL, device1, PMCsdkCertHelper::getCsrForCertProvCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMCertHelper.verifyCSRSignature(PMCsdkCertHelper::g_csrVal, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    char* deviceCert;
    size_t deviceCertLen;
    OicUuid_t uuidFromCSR =
    {   0};
    char* publicKeyFromCSR = NULL;
    char* serialFromCSR = NULL;
    size_t serialLenFromCSR;

    if (!m_PMCertHelper.getUuidFromCSR(PMCsdkCertHelper::g_csrVal, &uuidFromCSR, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if (!m_PMCertHelper.getPublicKeyFromCSR(PMCsdkCertHelper::g_csrVal, &publicKeyFromCSR, OC_STACK_OK))
    {
        SET_FAILURE(m_PMHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.generateRandomSerialNumber(&serialFromCSR, &serialLenFromCSR, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.generateIdentityCertificate(&uuidFromCSR, publicKeyFromCSR, g_caCertPem, g_caKeyPem, serialFromCSR, TEST_CERT_NOT_BEFORE,
                    TEST_CERT_NOT_AFTER, &deviceCert, &deviceCertLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.provisionCertificate(NULL, device1, deviceCert, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }
if(!m_PMCertHelper.provisionCertificate(NULL, device1, deviceCert, PMCsdkCallbackHelper::provisionPostCB, OC_STACK_OK))
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
 * @see             OCStackResult OCDoOwnershipTransfer(void* ctx, OCProvisionDev_t *targetDevices, OCProvisionResultCB resultCallback)
 * @see             OCStackResult OCDiscoverOwnedDevices(unsigned short timeout, OCProvisionDev_t **ppList)
 * @see             OCStackResult OCGenerateKeyPair(char **publicKey, size_t *publicKeyLen, char **privateKey, size_t *privateKeyLen)
 * @see             OCStackResult OCGenerateRandomSerialNumber(char **serial, size_t *serialLen)
 * @see             OCStackResult OCGenerateRootCACertificate(const char *subject, const char *subjectPublicKey, const char *issuerCert, const char *issuerPrivateKey, const char *serial, const char *notValidBefore, const char *notValidAfter, char **certificate, size_t *certificateLen)
 * @see             OCStackResult OCSaveTrustCertChain(const uint8_t *trustCertChain, size_t chainSize, OicEncodingType_t encodingType, uint16_t *credId)
 * @see             OCStackResult OCGenerateKeyPair(char **publicKey, size_t *publicKeyLen, char **privateKey, size_t *privateKeyLen)
 * @see             OCStackResult OCGenerateRandomSerialNumber(char **serial, size_t *serialLen)
 * @see             OCStackResult OCGenerateIdentityCertificate(const OicUuid_t *subjectUuid, const char *subjectPublicKey, const char *issuerCert, const char *issuerPrivateKey, const char *serial, const char *notValidBefore, const char *notValidAfter, char **certificate, size_t *certificateLen)
 * @objective       Test OCSaveOwnCertChain positively with regular data
 * @target          OCStackResult OCSaveOwnCertChain(const char* cert, const char* key, uint16_t *credId)
 * @test_data       Regular data for the API
 * @pre_condition   Start two justworks simulators
 * @procedure       1. call OCRegisterPersistentStorageHandler
 *                  2. call OCInit
 *                  3. call OCInitPM
 *                  4. call OCDiscoverOwnedDevices
 *                  5. call OCGenerateKeyPair
 *                  6. call OCGenerateRandomSerialNumber
 *                  7. call OCGenerateRootCACertificate
 *                  8. call OCSaveTrustCertChain
 *                  9. call OCGenerateKeyPair
 *                 10. call OCGenerateRandomSerialNumber
 *                 11. call OCGenerateIdentityCertificate
 *                 12. call OCSaveOwnCertChain
 * @post_condition  None
 * @expected        OCSaveOwnCertChain will return OC_STACK_OK
 */
#if defined(__LINUX__) || defined(__TIZEN__) || defined(__WINDOWS__)
TEST_F(PMCsdkCertTest_stc, saveOwnCertChain_SRC_P)
{
    char* publicKey = NULL;
    size_t publicKeyLen = 0;
    size_t caKeyLen = 0;
    char* serial = NULL;
    size_t serialLen = 0;
    size_t caCertLen = 0;
    char* idPublicKey = NULL;
    char* idKey = NULL;
    char* idCert = NULL;
    size_t idCertLen = 0;
    size_t idKeyLen = 0;

    /* Create CA keypair, serial number and Create a CA certificate */

    if(!m_PMCertHelper.generateKeyPair(&publicKey, &publicKeyLen, &g_caKeyPem, &caKeyLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.generateRandomSerialNumber(&serial, &serialLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    if(!m_PMCertHelper.generateCACertificate(CA_SUBJECT, publicKey, NULL, g_caKeyPem, serial, TEST_CERT_NOT_BEFORE,
                    TEST_CERT_NOT_AFTER, &g_caCertPem, &caCertLen, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    uint16_t g_credId = 0;

    if (!m_PMCertHelper.saveTrustCertChain((uint8_t*) g_caCertPem, caCertLen, OIC_ENCODING_PEM, &g_caCredId, OC_STACK_OK))
    {
        SET_FAILURE(m_PMCertHelper.getFailureMessage());
        return;
    }

    /* Create identity certificate for use by the CA. */

    if(!m_PMCertHelper.generateKeyPair(&idPublicKey, &publicKeyLen, &idKey, &idKeyLen, OC_STACK_OK))
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
    memcpy(uuid.id, deviceId.id, sizeof(uuid.id));

    if(!m_PMCertHelper.generateIdentityCertificate(&uuid, idPublicKey, g_caCertPem, g_caKeyPem, serial, TEST_CERT_NOT_BEFORE,
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
