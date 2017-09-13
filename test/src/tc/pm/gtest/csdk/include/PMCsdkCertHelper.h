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
#ifndef PMCsdkCertHelper_H_
#define PMCsdkCertHelper_H_

#include "PMCsdkUtilityHelper.h"

#define g_ctx "Provision Manager Client Application Context"
#define ctxProvCreadential "ProvisionCredentials Context"
#define ctxProvPairwise "ctxProvPairwise"
#define ctxProvDirectPairing "ctxProvDirectPairing"
#define ctxUnlinkDevice "ctxUnlinkDevice"
#define ctxRemoveDevice "ctxRemoveDevice"
#define ctxRemoveDeviceWithUuid "ctxRemoveDeviceWithUuid"
#define ctxResetDevice "ctxResetDevice"
#define RANDOM_PIN_TEXT_FILE "server_pincode.txt"
#define PIN_MAX_SIZE 9
#define MAX_TIME_INPUT_PROMPT_FOR_PASSWORD 5

/*
 * Ownership Transfer Related Resource
 */
#define OTM_INVALID_LOBV -1
#define OTM_INVALID_UOBV 10
#define OTM_NONE 0
#define OTM_JUSTWORK 1
#define OTM_RANDOMPIN 2
#define OTM_ALL 3

/*
 * Credential Representative value
 */
#define SYM_PAIR_WISE_KEY 1
#define CRED_TYPE_UOBV 30
#define CRED_TYPE_LOBV -3

#define TEST_CERT_NOT_BEFORE "20170101000000" // Not before field for certificates, in format YYYYMMDDhhmmss
#define TEST_CERT_NOT_AFTER "20270101000000"
#define CA_SUBJECT "C=US, O=Open Connectivity Foundation, CN=IoTivity test code CA"

// function declaration(s) for calling them before implementing

/**
 * Helper Class for Provisioning Manager
 */
class PMCsdkCertHelper
{
private:
    std::string m_failureMessage;

public:
    static int g_OwnDevCount;
    static int g_UnownDevCount;
    static int g_motPMDevCount;
    static bool g_CBInvoked;
    static char* g_csrVal;

    static OCPersistentStorage pstStr;
    static ByteArray s_trustCertChainArray;

    PMCsdkCertHelper();

    static FILE* fopenProvManager(const char*, const char*);

    bool initProvisionClient(int clientOTMType, char* chDBPath);

    bool readTrustCertChain(uint16_t credId, uint8_t **trustCertChain, size_t *chainSize,
            OCStackResult expectedResult);

    bool provisionCertificate(void *ctx, const OCProvisionDev_t *pDev, const char* pemCert,
            OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    bool saveOwnCertChain(const char* cert, const char* key, uint16_t *credId,
            OCStackResult expectedResult);

    bool saveOwnRoleCert(const char* cert, uint16_t *credId, OCStackResult expectedResult);

    bool removeTrustCertChainNotifier();

    bool registerTrustCertChainNotifier(void *cb, TrustCertChainChangeCB CB,
            OCStackResult expectedResult);

    bool saveTrustCertChain(uint8_t *trustCertChain, size_t chainSize,
            OicEncodingType_t encodingType, uint16_t *credId, OCStackResult expectedResult);

    bool provisionTrustCertChain(void *ctx, OicSecCredType_t type, uint16_t credId,
            const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback,
            OCStackResult expectedResult);

    bool provisionCRL(void* ctx, const OCProvisionDev_t *selectedDeviceInfo, OicSecCrl_t *crl,
            OCProvisionResultCB resultCallback, OCStackResult expectedResult);

    bool generateKeyPair(char **publicKey, size_t *publicKeyLen, char **privateKey,
            size_t *privateKeyLen, OCStackResult expectedResult);

    bool generateRandomSerialNumber(char **serial, size_t *serialLen, OCStackResult expectedResult);

    bool generateCACertificate(const char *subject, const char *subjectPublicKey,
            const char *issuerCert, const char *issuerPrivateKey, const char *serial,
            const char *notValidBefore, const char *notValidAfter, char **certificate,
            size_t *certificateLen, OCStackResult expectedResult);

    bool getDeviceId(OCUUIdentity *deviceId, OCStackResult expectedResult);

    bool getUuidFromCSR(const char* csr, OicUuid_t* uuid, OCStackResult expectedResult);

    bool getPublicKeyFromCSR(const char* csr, char** publicKey, OCStackResult expectedResult);

    bool generateIdentityCertificate(const OicUuid_t *subjectUuid, const char *subjectPublicKey,
            const char *issuerCert, const char *issuerPrivateKey, const char *serial,
            const char *notValidBefore, const char *notValidAfter, char **certificate,
            size_t *certificateLen, OCStackResult expectedResult);

    bool getCSRResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
            OCGetCSRResultCB resultCallback, OCStackResult expectedResult);

    bool verifyCSRSignature(const char* csr, OCStackResult expectedResult);

    /**
     * All Callback Methods for Provision Manager
     */
    static void trustCertChainChangeCB(void *ctx, uint16_t credId, uint8_t *trustCertChain,
            size_t chainSize);

    static void getCsrForCertProvCB(void* ctx, size_t nOfRes, OCPMGetCsrResult_t* arr,
    bool hasError);

    static ByteArray getTrustCertChainArray();bool readFile(const char *name, OCByteString *out);
    /**
     * All Utility Methods for Provision Manager
     */
    static int waitCallbackRet(void);

    std::string getFailureMessage();
};
#endif
