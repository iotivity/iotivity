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

#include <stddef.h>
#include <string>

#include "logger.h"
#include "ocpayload.h"
#include "ocstack.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "OCAccountManager.h"
#include "OCApi.h"
#include "OCPlatform.h"
#include "OCPlatform_impl.h"
#include "payload_logging.h"
#include "rd_client.h"

#ifdef __SECURED__
#include "cathreadpool.h"
#include "utlist.h"
#include "casecurityinterface.h"
#include "occloudprovisioning.h"
#include "ocprovisioningmanager.h"
#include "srmutility.h"
#include "ssl_ciphersuites.h"
#include "utils.h"
#include "pmtypes.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "ca_adapter_net_ssl.h"
#include "CAManager.h"
#endif

#include "CommonUtil.h"
#include "Configuration.h"
#include "CloudCommonUtil.h"
#include "CommonProperties.h"
#include "IotivityTest_Logger.h"

using namespace std;
using namespace OC;

#define ROOT_CERT "rootca.crt"
const char* CLOUD_SVR_CBOR_DAT = "cloud.dat";
static OCPersistentStorage g_pstStr;
static string g_authprovider = "github";
static string g_authCode = "";
static string g_uid = "";
static string g_accesstoken = "";
static string g_hostAddress = "";
static OCAccountManager::Ptr g_accountMgr = nullptr;

static char* g_ctx = (char*)"cloud";
static OCDevAddr g_endPoint;
OicCloud_t* g_pCloud = NULL;
static int g_isCbInvoked = CALLBACK_NOT_INVOKED;

#ifdef __SECURED__
void createAcl();
void getAclID();
#endif

#ifdef __SECURED__

#define TAG "CLOUD SIMULATOR"

static bool readFile(const char *name, OCByteString *out)
{
    FILE *file = NULL;
    int length = 0;
    uint8_t *buffer = NULL;
    bool result = false;

    //Open file
    file = fopen(name, "rb");
    if (!file)
    {
        OIC_LOG_V(ERROR, TAG, "Unable to open file %s", name);
        return result;
    }

    //Get file length
    if (fseek(file, 0, SEEK_END))
    {
        OIC_LOG(ERROR, TAG, "Failed to SEEK_END");
    }

    length = ftell(file);
    if (length < 0)
    {
        OIC_LOG(ERROR, TAG, "Failed to ftell");
        // goto exit;
    }

    if (fseek(file, 0, SEEK_SET))
    {
        OIC_LOG(ERROR, TAG, "Failed to SEEK_SET");
        //goto exit;
    }

    //Allocate memory
    buffer = (uint8_t *)malloc(length);
    if (!buffer)
    {
        OIC_LOG(ERROR, TAG, "Failed to allocate buffer");
        //goto exit;
    }

    //Read file contents into buffer
    size_t count = 1;
    size_t realCount = fread(buffer, length, count, file);
    if (realCount != count)
    {
        OIC_LOG_V(ERROR, TAG, "Read %d bytes %" PRIuPTR " times instead of %" PRIuPTR, length, realCount, count);
        //goto exit;
    }

    out->bytes = buffer;
    out->len = length;

    result = true;

    fclose(file);
    return result;
}

int saveTrustCert(void)
{
    __FUNC_IN__
    OCStackResult res = OC_STACK_ERROR;
    uint16_t g_credId = 0;
    OIC_LOG(INFO, TAG, "Save Trust Cert. Chain into Cred of SVR");

    ByteArray_t trustCertChainArray =
    {   0, 0};
    if (!readFile(ROOT_CERT, (OCByteString *)&trustCertChainArray))
    {
        OIC_LOG_V(ERROR, TAG, "Can't read %s file", ROOT_CERT);
        return OC_STACK_ERROR;
    }

    OIC_LOG_BUFFER(DEBUG, TAG, trustCertChainArray.data, trustCertChainArray.len);

    res = OCSaveTrustCertChain(trustCertChainArray.data, trustCertChainArray.len, OIC_ENCODING_PEM, &g_credId);

    if (OC_STACK_OK != res)
    {
        OIC_LOG(ERROR, TAG, "OCSaveTrustCertChain API error");
    }
    else
    {
        OIC_LOG_V(INFO, TAG, "CredId of Saved Trust Cert. Chain into Cred of SVR : %d.\n", g_credId);
    }
    OICFree(trustCertChainArray.data);

    __FUNC_OUT__
    return res;
}

#ifdef __TLS_ON__
void enableTlsMode()
{
    setCoapPrefix(true);
    if (CA_STATUS_OK != saveTrustCert())
    {
        IOTIVITYTEST_LOG(ERROR, "saveTrustCert returned an error");
    }

    uint16_t cipher = MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256;
    if (CA_STATUS_OK != CASelectCipherSuite(cipher, CA_ADAPTER_TCP))
    {
        IOTIVITYTEST_LOG(ERROR, "CASelectCipherSuite returned an error");
    }
}
#endif

#endif

static int waitCallbackRet()
{
    IOTIVITYTEST_LOG(DEBUG, "Waiting for Callback to be invoked");

    for (int i = 0; CALLBACK_TIMEOUT > i; ++i)
    {
        if (CALLBACK_INVOKED == g_isCbInvoked)
        {
            return CALLBACK_INVOKED;
        }

        sleep(DELAY_SHORT);
        printf("Second Elapsed : %d seconds\n", i);

        if (OC_STACK_OK != OCProcess())
        {
            printf("OCStack process error\n");
            return CALLBACK_NOT_INVOKED;
        }
    }

    return CALLBACK_NOT_INVOKED;
}

FILE* controllerOpen(const char *path, const char *mode)
{
    if (0 == strncmp(path, OC_SECURITY_DB_DAT_FILE_NAME, strlen(OC_SECURITY_DB_DAT_FILE_NAME)))
    {
        return fopen(path, mode);
    }
    else
    {
        return fopen(CLOUD_SVR_CBOR_DAT, mode);
    }
}

void initServer()
{
    __FUNC_IN__

    g_pstStr.open = controllerOpen;
    g_pstStr.read = fread;
    g_pstStr.write = fwrite;
    g_pstStr.close = fclose;
    g_pstStr.unlink = unlink;

    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&g_pstStr))
    {
        IOTIVITYTEST_LOG(ERROR, "[CS Server] OCRegisterPersistentStorageHandler error");
        return;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        IOTIVITYTEST_LOG(ERROR, "[CS Server] OCStack init error");
        return;
    }

    IOTIVITYTEST_LOG(INFO, "[CS SIMULATOR] Host Address : %s", g_hostAddress.c_str());

    g_accountMgr = OCPlatform::constructAccountManagerObject(g_hostAddress, CT_ADAPTER_TCP);
    __FUNC_OUT__
}

#ifdef __SECURED__
static void aclResponseCB(void* ctx, OCClientResponse* response, void* data)
{
    __FUNC_IN__

    OC_UNUSED(data);

    IOTIVITYTEST_LOG(INFO, "%s: Received result = %d for ctx : %s", __func__, response->result,
            (char* )ctx);

    if (OC_STACK_OK == response->result || OC_STACK_RESOURCE_CHANGED == response->result
            || OC_STACK_DUPLICATE_REQUEST == response->result)
    {
        CloudCommonUtil::printRepresentation(CloudCommonUtil::parseOCClientResponse(response));
    }

    if (OC_STACK_RESOURCE_CHANGED >= response->result
            || OC_STACK_DUPLICATE_REQUEST == response->result)
    {
        g_isCbInvoked = true;
    }

    __FUNC_OUT__
}

void createAcl()
{
    __FUNC_IN__
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCCloudAclIdCreate(g_ctx, DEFAULT_OWNER_ID, DEFAULT_DEV_ID, g_pCloud->cis,
            aclResponseCB);
    IOTIVITYTEST_LOG(INFO, "[CS Server] OCCloudAclIdCreate returns %s", CommonUtil::getOCStackResult(result));

    if (OC_STACK_OK != result)
    {
        IOTIVITYTEST_LOG(ERROR, "[CS Server] OCCloudAclIdCreate returns %s",
                CommonUtil::getOCStackResult(result));
        return;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[CS Server] Callback Not Invoked");
            return;
        }
    }
    __FUNC_OUT__
}

void getAclId()
{
    __FUNC_IN__
    g_isCbInvoked = CALLBACK_NOT_INVOKED;
    OCStackResult result = OCCloudGetAclIdByDevice(g_ctx, DEFAULT_DEV_ID, g_pCloud->cis, aclResponseCB);
    IOTIVITYTEST_LOG(INFO, "[CS Server] OCCloudGetAclIdByDevice returns %s",
            CommonUtil::getOCStackResult(result));

    if (OC_STACK_OK != result)
    {
        IOTIVITYTEST_LOG(ERROR, "[CS Server] OCCloudAclIdCreate returns %s",
                CommonUtil::getOCStackResult(result));
        return;
    }

    if (OC_STACK_OK == result)
    {
        if (CALLBACK_NOT_INVOKED == waitCallbackRet())
        {
            IOTIVITYTEST_LOG(ERROR, "[CS Server] ERROR");
            return;
        }
    }
    __FUNC_OUT__
}

#endif

/**
 * @pre_condition   1. Start Kafka Server
 *                  2. Start ZooKeeper Server
 *                  3. Start Mongo DB Server
 *                  4. Start RD, MQ, Account and CI Server respectively
 * @procedure       1. Call OCRegisterPersistentStorageHandler
 *                  2. Call OCInit
 *                  3. Call setCoapPrefix for TLS MODE enabled
 *                  4. Call CASelectCipherSuite(true)for TLS MODE enabled
 *                  5. Call OCSaveTrustCertChain for TLS MODE enabled
 *                  6. Call signUp and save uid and accessToken in cloud.properties file
 *                  7. Call signIn using uid and accessToken
 *                  8. Call OCCloudAclIdCreate
 *                  9. Call OCCloudGetAclIdByDevice
 *                  10. Call signOut
 * @post_condition  None
 * @expected        Cloud Initialization Successful.
 */
int main()
{
    bool isInitialized = true;
#ifdef __SECURED__
    IOTIVITYTEST_LOG(INFO, "SECURED MODE : On");
#else
    IOTIVITYTEST_LOG(INFO, "SECURED MODE : Off");
#endif

#ifdef __TLS_ON__
    IOTIVITYTEST_LOG(INFO, "TLS MODE : On");
#else
    IOTIVITYTEST_LOG(INFO, "TLS MODE : Off");
#endif

    g_hostAddress = CloudCommonUtil::getDefaultHostAddess();

    initServer();
#ifdef __SECURED__
    g_endPoint = CloudCommonUtil::getDefaultEndPoint();
    g_pCloud = CloudCommonUtil::getCloudServer();
#ifdef __TLS_ON__
    enableTlsMode();
#endif
#endif

    isInitialized = CloudCommonUtil::signUp(g_accountMgr);

    if (isInitialized)
    {

        isInitialized = CloudCommonUtil::signIn(g_accountMgr);

#ifdef __SECURED__
        if (isInitialized)
        {
            createAcl();
            getAclId();
            CloudCommonUtil::signOut(g_accountMgr);
        }
#endif
    }

    IOTIVITYTEST_LOG(INFO, "Cloud Initialization %s", isInitialized ? "Successful" : "Failed");

    return 0;
}
