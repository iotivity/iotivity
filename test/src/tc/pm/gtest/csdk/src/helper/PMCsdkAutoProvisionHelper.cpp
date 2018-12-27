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

#include "PMCsdkAutoProvisionHelper.h"
#include "PMCsdkUtilityHelper.h"

char* PMCsdkAutoProvisionHelper::g_csr = NULL;
const char* PMCsdkAutoProvisionHelper::g_caCertPem = "-----BEGIN CERTIFICATE-----\n"
        "MIIBfjCCASSgAwIBAgIJAPQXoGTceaW5MAoGCCqGSM49BAMCMBkxFzAVBgNVBAoM\n"
        "DklvVGl2aXR5VGVzdENBMB4XDTE3MDMxNTAwNTExOVoXDTMwMTEyMjAwNTExOVow\n"
        "GTEXMBUGA1UECgwOSW9UaXZpdHlUZXN0Q0EwWTATBgcqhkjOPQIBBggqhkjOPQMB\n"
        "BwNCAARvYPdt+LjqASlHoc2zrjo3hHGjZsI31c+bg9AwINW5TuRZsE03w/Ejotza\n"
        "y4VDLImMlDhGP+K/f6OmKD3FNHhKo1UwUzAhBgNVHSUEGjAYBgorBgEEAYLefAEG\n"
        "BgorBgEEAYLefAEHMA8GA1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFNw+hm69Rxb5\n"
        "UpclERf5r85g1nwmMAoGCCqGSM49BAMCA0gAMEUCIQDbvNLA3ZkwEzuoH6XUR6JS\n"
        "UzZTVgsDgnJcOqtqOg0qEAIgUJR2g8XlMxqiuXP7JdwALdtnvCQTlJQbuD1gu+Jy\n"
        "AdQ=\n"
        "-----END CERTIFICATE-----\n";
const char* PMCsdkAutoProvisionHelper::g_caKeyPem = "-----BEGIN EC PRIVATE KEY-----\n"
        "MHcCAQEEILx9VOHDrMYuan6SXN4CQIHHXNq6SjzanaDFDgIaOaXloAoGCCqGSM49\n"
        "AwEHoUQDQgAEb2D3bfi46gEpR6HNs646N4Rxo2bCN9XPm4PQMCDVuU7kWbBNN8Px\n"
        "I6Lc2suFQyyJjJQ4Rj/iv3+jpig9xTR4Sg==\n"
        "-----END EC PRIVATE KEY-----\n";

OCPersistentStorage PMCsdkAutoProvisionHelper::pstStr =
{ 0, 0, 0, 0, 0 };

FILE* PMCsdkAutoProvisionHelper::fopenProvManager(const char* path, const char* mode)
{
    //OC_UNUSED(path);
    if (0 == strncmp(path, OC_SECURITY_DB_DAT_FILE_NAME, strlen(OC_SECURITY_DB_DAT_FILE_NAME)))
    {
        return fopen(SVR_DB_FILE_NAME, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}

int PMCsdkAutoProvisionHelper::waitCallbackRet(void)
{
    __FUNC_IN__

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        IOTIVITYTEST_LOG(DEBUG, "waitCallbackRet Loop = %d", i);

        if (true == PMCsdkCallbackHelper::s_CBInvoked)
        {
            return CALLBACK_INVOKED;
        }

        CommonUtil::waitInSecond(DELAY_SHORT);

        if (OC_STACK_OK != OCProcess())
        {
            IOTIVITYTEST_LOG(ERROR, "OCStack process error");
            return CALLBACK_NOT_INVOKED;
        }
    }

    __FUNC_OUT__
    return CALLBACK_NOT_INVOKED;
}

void PMCsdkAutoProvisionHelper::getCsrForCertProvCB(void* ctx, size_t nOfRes,
        OCPMGetCsrResult_t* arr, bool hasError)
{
    if (!hasError)
    {
        if (nOfRes != 1)
        {
            IOTIVITYTEST_LOG(ERROR, "getCsrForCertProvCB FAILED - ctx: %s", (char* )ctx);
        }

        if (arr[0].encoding == OIC_ENCODING_DER)
        {
            OCStackResult res = OCConvertDerCSRToPem((char*) arr[0].csr, arr[0].csrLen, &g_csr);
            if (res != OC_STACK_OK)
            {
                IOTIVITYTEST_LOG(ERROR,
                        "getCsrForCertProvCB FAILED (CSR re-encoding failed) - error: %d, ctx: %s",
                        res, (char* )ctx);
            }
            PMCsdkCallbackHelper::s_CBInvoked = true;
        }
        else if (arr[0].encoding == OIC_ENCODING_PEM)
        {
            g_csr = (char*) OICCalloc(1, arr[0].csrLen);
            if (g_csr == NULL)
            {
                IOTIVITYTEST_LOG(ERROR, "getCsrForCertProvCB FAILED (memory allocation) - ctx: %s",
                        (char* )ctx);
            }

            memcpy(g_csr, arr[0].csr, arr[0].csrLen);

            IOTIVITYTEST_LOG(INFO, "getCsrForCertProvCB success");
            PMCsdkCallbackHelper::s_CBInvoked = true;
        }
        else
        {
            IOTIVITYTEST_LOG(ERROR, "getCsrForCertProvCB FAILED (unknown encoding) - ctx: %s",
                    (char* )ctx);
        }
    }
    else
    {
        IOTIVITYTEST_LOG(ERROR, "getCsrForCertProvCB FAILED - ctx: %s", (char* )ctx);
    }

}

PMCsdkAutoProvisionHelper::PMCsdkAutoProvisionHelper()
{
    IOTIVITYTEST_LOG(DEBUG, "[PMHelper] Constructor");
}

bool PMCsdkAutoProvisionHelper::initProvisionClient()
{
    __FUNC_IN__

    pstStr.open = PMCsdkAutoProvisionHelper::fopenProvManager;
    pstStr.read = fread;
    pstStr.write = fwrite;
    pstStr.close = fclose;
    pstStr.unlink = unlink;

    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&pstStr))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCRegisterPersistentStorageHandler error");
        return false;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCStack init error");
        return false;
    }

    if (access(PRVN_DB_FILE_NAME, F_OK) != -1)
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMHelper] Provisioning DB file already exists.");
    }
    else
    {
        IOTIVITYTEST_LOG(DEBUG, "[PMHelper] No provisioning DB file, creating new ");
    }

    IOTIVITYTEST_LOG(INFO, "************OCInitPM In************");

    if (OC_STACK_OK != OCInitPM(AUTO_PROVISION_SVR_DB_FILE_NAME))
    {
        IOTIVITYTEST_LOG(ERROR, "[PMHelper] OCInitPM Error");
        return false;
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCGetCSRResource
 */
bool PMCsdkAutoProvisionHelper::getCSRResource(void* ctx,
        const OCProvisionDev_t *selectedDeviceInfo, OCGetCSRResultCB resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCallbackHelper::s_CBInvoked = false;

    OCStackResult res = OCGetCSRResource(ctx, selectedDeviceInfo, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCGetCSRResource API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkAutoProvisionHelper::waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCGetRolesResource
 */
bool PMCsdkAutoProvisionHelper::getRolesResource(void* ctx,
        const OCProvisionDev_t *selectedDeviceInfo, OCGetRolesResultCB resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCallbackHelper::s_CBInvoked = false;

    OCStackResult res = OCGetRolesResource(ctx, selectedDeviceInfo, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCGetRolesResource API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkAutoProvisionHelper::waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCProvisionSymmetricRoleCredentials
 */
bool PMCsdkAutoProvisionHelper::provisionSymmetricRoleCredentials(void *ctx, OicSecCredType_t type,
        size_t keySize, const OCProvisionDev_t *pDev1, const OCProvisionDev_t *pDev2,
        const OicSecRole_t *role1, const OicSecRole_t *role2, OCProvisionResultCB resultCallback,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCallbackHelper::s_CBInvoked = false;

    OCStackResult res = OCProvisionSymmetricRoleCredentials(ctx, type, keySize, pDev1, pDev2, role1,
            role2, resultCallback);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCProvisionSymmetricRoleCredentials API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    if (OC_STACK_OK == res)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkAutoProvisionHelper::waitCallbackRet())
        {
            m_failureMessage = PMCsdkUtilityHelper::setFailureMessage("CALLBACK Not invoked");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCGenerateRoleCertificate
 */
bool PMCsdkAutoProvisionHelper::generateRoleCertificate(const OicUuid_t *subjectUuid,
        const char *subjectPublicKey, const char *issuerCert, const char *issuerPrivateKey,
        const char *serial, const char *notValidBefore, const char *notValidAfter, const char *role,
        const char *authority, char **certificate, size_t *certificateLen,
        OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCallbackHelper::s_CBInvoked = false;

    OCStackResult res = OCGenerateRoleCertificate(subjectUuid, subjectPublicKey, issuerCert,
            issuerPrivateKey, serial, notValidBefore, notValidAfter, role, authority, certificate,
            certificateLen);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCGenerateRoleCertificate API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

/**
 * Helper Method for OCSaveOwnRoleCert
 */
bool PMCsdkAutoProvisionHelper::saveOwnRoleCert(const char* cert, uint16_t *credId, OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCallbackHelper::s_CBInvoked = false;

    OCStackResult res = OCSaveOwnRoleCert(cert, credId);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCSaveOwnRoleCert API returns: %s",
            CommonUtil::getOCStackResult(res));

    IOTIVITYTEST_LOG(INFO, "CredId  : %d", *credId);

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}


/**
 * Helper Method for OCSaveOwnRoleCert
 */
bool PMCsdkAutoProvisionHelper::removeCredential(const OicUuid_t* subjectUuid, OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCallbackHelper::s_CBInvoked = false;

    OCStackResult res = OCRemoveCredential(subjectUuid);
    IOTIVITYTEST_LOG(INFO, "[PMHelper]  OCRemoveCredential API returns: %s",
            CommonUtil::getOCStackResult(res));

    if (expectedResult != res)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(res, expectedResult);
        return false;
    }

    __FUNC_OUT__
    return true;
}

bool PMCsdkAutoProvisionHelper::deleteRoleCertificateByCredId(void *ctx,
        const OCProvisionDev_t *selectedDeviceInfo, OCProvisionResultCB resultCallback,
        uint32_t credId, OCStackResult expectedResult)
{
    __FUNC_IN__

    PMCsdkCallbackHelper::s_CBInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCDeleteRoleCertificateByCredId(ctx, selectedDeviceInfo, resultCallback,
            credId);
    IOTIVITYTEST_LOG(INFO, "[Cloud Acl] OCDeleteRoleCertificateByCredId returns %s",
            CommonUtil::getOCStackResult(result));

    if (expectedResult != result)
    {
        m_failureMessage = PMCsdkUtilityHelper::setFailureMessage(result, expectedResult);
        return false;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == PMCsdkAutoProvisionHelper::waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[Cloud] CALLBACK_NOT_INVOKED");
            return false;
        }
    }

    __FUNC_OUT__
    return true;
}

