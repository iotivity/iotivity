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

#ifndef PMCsdkAutoProvisionHelper_H_
#define PMCsdkAutoProvisionHelper_H_

#include "PMCsdkCallbackHelper.h"
#include "PMCsdkUtilityHelper.h"

/**
 * Helper Class for Provisioning Manager
 */
class PMCsdkAutoProvisionHelper
{
private:
    std::string m_failureMessage;

public:

    static OCPersistentStorage pstStr;
    static bool g_CBInvoked;
    static const char* g_caCertPem;
    static const char* g_caKeyPem;
    static char* g_csr;

    PMCsdkAutoProvisionHelper();

    static void getCsrForCertProvCB(void* ctx, size_t nOfRes, OCPMGetCsrResult_t* arr,
    bool hasError);

    static FILE* fopenProvManager(const char*, const char*);

    bool initProvisionClient();

    bool getCSRResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
            OCGetCSRResultCB resultCallback, OCStackResult expectedResult);

    bool getRolesResource(void* ctx, const OCProvisionDev_t *selectedDeviceInfo,
            OCGetRolesResultCB resultCallback, OCStackResult expectedResult);

    bool provisionSymmetricRoleCredentials(void *ctx, OicSecCredType_t type, size_t keySize,
            const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2, const OicSecRole_t *role1,
            const OicSecRole_t *role2, OCProvisionResultCB resultCallback,
            OCStackResult expectedResult);

    bool generateRoleCertificate(const OicUuid_t *subjectUuid, const char *subjectPublicKey,
            const char *issuerCert, const char *issuerPrivateKey, const char *serial,
            const char *notValidBefore, const char *notValidAfter, const char *role,
            const char *authority, char **certificate, size_t *certificateLen,
            OCStackResult expectedResult);

    bool saveOwnRoleCert(const char* cert, uint16_t *credId, OCStackResult expectedResult);

    bool removeCredential(const OicUuid_t* subjectUuid, OCStackResult expectedResult);

    bool deleteRoleCertificateByCredId(void *ctx, const OCProvisionDev_t *selectedDeviceInfo,
                OCProvisionResultCB resultCallback, uint32_t credId, OCStackResult expectedResult);

    static int waitCallbackRet(void);

    std::string getFailureMessage();
};
#endif
