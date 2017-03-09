/******************************************************************
 *
 * Copyright 2017 Microsoft. All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *****************************************************************/

/*
 * This file is derived from iotivity\resource\csdk\security\provisioning\sample\provisioningclient.c
 * That program is interactive, and requires user input to perform provisioning
 * steps, while this version takes input on the command line and executes a
 * sequence of provisioning operations automatically, then exists, reporting
 * whether the sequence succeeded.  This is so that we can script provisioning
 * for testing.
 */

#include "iotivity_config.h"

#include <stdio.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "payload_logging.h"
#include "utlist.h"
#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocprovisioningmanager.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "securevirtualresourcetypes.h"
#include "srmutility.h"
#include "pmtypes.h"
#include "oxmverifycommon.h"
#include "mbedtls/config.h"
#include "mbedtls/pem.h"
#include "mbedtls/x509_csr.h"
#include "certhelpers.h"

#ifdef _MSC_VER
#include <io.h>

#define F_OK 0
#define access _access_s
#endif

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#define DISCOVERY_TIMEOUT   3  // 3 sec
#define CALLBACK_TIMEOUT    60  // 1 min
#define TAG "provisioningclient"

static const char* SVR_DB_FILE_NAME = "oic_svr_db_client.dat";
        // '_' for separaing from the same constant variable in |srmresourcestrings.c|
static const char* PRVN_DB_FILE_NAME = "oic_autoprvn_mng.db";

// |g_ctx| means provision manager application context and
// the following, includes |un/own_list|, could be variables, which |g_ctx| has,
// for accessing all function(s) for these, they are declared on global domain
static char* g_ctx = "Provision Manager Client Application Context";
static char* g_svr_fname;
static char* g_prvn_fname;
static OCProvisionDev_t* g_own_list;
static OCProvisionDev_t* g_unown_list;
static int g_own_cnt;
static int g_unown_cnt;
mbedtls_x509_csr g_csr;

static volatile bool g_doneCB;    /* Set to true by the callback to indicate it completed. */
static bool g_successCB; /* Set to true by the callback to indicate success. */

// function declaration(s) for calling them before implementing

static OCProvisionDev_t* getDevInst(const OCProvisionDev_t*, const int);
static int printDevList(const OCProvisionDev_t*);
static size_t printUuidList(const OCUuidList_t*);
static size_t printResultList(const OCProvisionResult_t*, const size_t);
static void printUuid(const OicUuid_t*);
static FILE* fopen_prvnMng(const char*, const char*);
static int waitCallbackRet(void);

/* At a few places in this file, warning 4028 is incorrectly produced, disable it for the whole file. */
#ifdef _MSC_VER
#pragma warning( disable : 4028)
#endif

// callback function(s) for provisioning client using C-level provisioning API
static void ownershipTransferCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if(!hasError)
    {
        OIC_LOG_V(INFO, TAG, "Ownership Transfer SUCCEEDED - ctx: %s", (char*) ctx);
        g_successCB = true;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "Ownership Transfer FAILED - ctx: %s", (char*) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
        g_successCB = false;
    }
    g_doneCB = true;
}

static void provisionCredCB(void* ctx, size_t nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if(!hasError)
    {
        OIC_LOG_V(INFO, TAG, "Provision Credential SUCCEEDED - ctx: %s", (char*) ctx);
        g_successCB = true;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "Provision Credential FAILED - ctx: %s", (char*) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
        g_successCB = false;
    }
    g_doneCB = true;
}

/* Function of type OCProvisionResultCB from \resource\csdk\security\provisioning\include\pmtypes.h */
void provisionTrustChainCB(void* ctx, int nOfRes, OCProvisionResult_t *arr, bool hasError)
{
    if(!hasError)
    {
        OIC_LOG_V(INFO, TAG, "Provision Credential SUCCEEDED - ctx: %s", (char*)ctx);
        g_successCB = true;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "Provision Credential FAILED - ctx: %s", (char*)ctx);
        printResultList((const OCProvisionResult_t*)arr, nOfRes);
        g_successCB = false;
    }
    g_doneCB = true;
}

static void getCsrForCertProvCB(void* ctx, size_t nOfRes, OCPMGetCsrResult_t* arr, bool hasError)
{
    g_successCB = false;

    if(!hasError)
    {
        if(nOfRes != 1)
        {
            OIC_LOG_V(ERROR, TAG, "getCsrForCertProvCB FAILED - ctx: %s", (char*)ctx);
            goto exit;
        }

        mbedtls_x509_csr_init(&g_csr);
        int ret = mbedtls_x509_csr_parse(&g_csr, arr[0].csr, arr[0].csrLen);
        if(ret < 0)
        {
            OIC_LOG_V(ERROR, TAG, "Couldn't parse CSR: %d", ret);
            mbedtls_x509_csr_free(&g_csr);

            goto exit;
        }

        OIC_LOG(INFO, TAG, "getCsrForCertProvCB success");
        g_successCB = true;
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "getCsrForCertProvCB FAILED - ctx: %s", (char*)ctx);
    }

exit:
    g_doneCB = true;
}

// function(s) for provisioning client using C-level provisioning API
static int initProvisionClient(void)
{
    // initialize persistent storage for SVR DB
    static OCPersistentStorage pstStr =
    {
        .open = fopen_prvnMng,
        .read = fread,
        .write = fwrite,
        .close = fclose,
        .unlink = remove
    };
    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&pstStr))
    {
        OIC_LOG(ERROR, TAG, "OCRegisterPersistentStorageHandler error");
        return -1;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return -1;
    }

    if (access(PRVN_DB_FILE_NAME, F_OK) == 0)
    {
        printf("************************************************************\n");
        printf("************Provisioning DB file already exists.************\n");
        printf("************************************************************\n");
    }
    else
    {
        printf("*************************************************************\n");
        printf("************No provisioning DB file, creating new************\n");
        printf("*************************************************************\n");
    }

    if (OC_STACK_OK != OCInitPM(PRVN_DB_FILE_NAME))
    {
        OIC_LOG(ERROR, TAG, "OC_PM init error");
        return -1;
    }

    return 0;
}

static int discoverAllDevices(void)
{
    // delete un/owned device lists before updating them
    if(g_own_list)
    {
        OCDeleteDiscoveredDevices(g_own_list);
        g_own_list = NULL;
    }
    if(g_unown_list)
    {
        OCDeleteDiscoveredDevices(g_unown_list);
        g_unown_list = NULL;
    }

    // call |OCGetDevInfoFromNetwork| API
    printf("   Discovering All Un/Owned Devices on Network..\n");
    if(OC_STACK_OK != OCGetDevInfoFromNetwork(DISCOVERY_TIMEOUT, &g_own_list, &g_unown_list))
    {
        OIC_LOG(ERROR, TAG, "OCGetDevInfoFromNetwork API error");
        return -1;
    }

    // display the discovered un/owned lists
    printf("   > Discovered Owned Devices\n");
    g_own_cnt = printDevList(g_own_list);
    printf("   > Discovered Unowned Devices\n");
    g_unown_cnt = printDevList(g_unown_list);

    return 0;
}


static int discoverUnownedDevices(void)
{
    // delete unowned device list before updating it
    if(g_unown_list)
    {
        OCDeleteDiscoveredDevices(g_unown_list);
        g_unown_list = NULL;
    }

    // call |OCDiscoverUnownedDevices| API
    printf("   Discovering Only Unowned Devices on Network..\n");
    if(OC_STACK_OK != OCDiscoverUnownedDevices(DISCOVERY_TIMEOUT, &g_unown_list))
    {
        OIC_LOG(ERROR, TAG, "OCDiscoverUnownedDevices API error");
        return -1;
    }

    // display the discovered unowned list
    printf("   > Discovered Unowned Devices\n");
    g_unown_cnt = printDevList(g_unown_list);

    return 0;
}

static int discoverOwnedDevices(void)
{
    // delete owned device list before updating it
    if(g_own_list)
    {
        OCDeleteDiscoveredDevices(g_own_list);
        g_own_list = NULL;
    }

    // call |OCDiscoverOwnedDevices| API
    printf("   Discovering Only Owned Devices on Network..\n");
    if(OC_STACK_OK != OCDiscoverOwnedDevices(DISCOVERY_TIMEOUT, &g_own_list))
    {
        OIC_LOG(ERROR, TAG, "OCDiscoverOwnedDevices API error");
        return -1;
    }

    // display the discovered owned list
    printf("   > Discovered Owned Devices\n");
    g_own_cnt = printDevList(g_own_list);

    return 0;
}

static int registerDevices(void)
{
    // check |unown_list| for registering devices
    if(!g_unown_list || 0>=g_unown_cnt)
    {
        OIC_LOG(ERROR, TAG, "Unowned device list empty, must discover unowned devices first");
        return -1;  // Error, we should have registered unowned devices already
    }

    // call |OCDoOwnershipTransfer| API
    // calling this API with callback actually acts like blocking
    // for error checking, the return value saved and printed
    g_doneCB = false;
    printf("   Registering All Discovered Unowned Devices..\n");
    OCStackResult rst = OCDoOwnershipTransfer((void*) g_ctx, g_unown_list, ownershipTransferCB);
    if(OC_STACK_OK != rst)
    {
        OIC_LOG_V(ERROR, TAG, "OCDoOwnershipTransfer API error: %d", rst);
        return -1;
    }
    if(waitCallbackRet())  // input |g_doneCB| flag implicitly
    {
        OIC_LOG(ERROR, TAG, "OCDoOwnershipTransfer callback error");
        return -1;
    }

    if(!g_successCB)
    {
        OIC_LOG(ERROR, TAG, "OCDoOwnershipTransfer callback failed");
        return -1;
    }

    // display the registered result
    printf("   > Registered Discovered Unowned Devices\n");
    printf("   > Please Discover Owned Devices for the Registered Result, with [10|12] Menu\n");

    return 0;
}

/*
* Test CA key and certificate created with
* iotivity/resource/csdk/security/scripts/test_cert_generation.sh
*/
const char* caCertPem = "-----BEGIN CERTIFICATE-----\n"
"MIIBfzCCASSgAwIBAgIJAOMHgsOiVyXpMAoGCCqGSM49BAMCMBkxFzAVBgNVBAoM\n"
"DklvVGl2aXR5VGVzdENBMB4XDTE3MDEwNjAwNDA1M1oXDTMwMDkxNTAwNDA1M1ow\n"
"GTEXMBUGA1UECgwOSW9UaXZpdHlUZXN0Q0EwWTATBgcqhkjOPQIBBggqhkjOPQMB\n"
"BwNCAAQMaZlQqz2C2DFRh3xcATSAyEIp9RIjYMU8/x6cZ0OTKA0ihZCDXmMksLk8\n"
"zWhYI/Sr7wpYxBaQh5yvaiP5zFl2o1UwUzAhBgNVHSUEGjAYBgorBgEEAYLefAEG\n"
"BgorBgEEAYLefAEHMA8GA1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFHnMVojc5HNr\n"
"fuLOz3iNV3u3vG3kMAoGCCqGSM49BAMCA0kAMEYCIQC7+/pCT8hEupk3nhStU3pF\n"
"QmJg6VCbcFKMY8MmqyBx2QIhAIwM63/OPXh9hpbRoVp5Z90KxUvbjZ/XstqULmYl\n"
"LzEE\n"
"-----END CERTIFICATE-----\n";

const char* caKeyPem = "-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEID8CoB1N6UCnGYZPAKWu376GtJxW7KlI2BXzS3ISQrEnoAoGCCqGSM49\n"
"AwEHoUQDQgAEUjKmL4wS485e3FIl3s07qvN+Auw0ra6+/y2phVu/qA/D92rd+HJK\n"
"Tnzf9FpTBRvWkiUh8rAWVuwoQxVkLmr9WA==\n"
"-----END EC PRIVATE KEY-----\n";

static int provisionTrustAnchor(int dev_num)
{
    // make sure we own at least one device to provision
    if(!g_own_list || g_own_cnt == 0)
    {
        OIC_LOG(ERROR, TAG, "Unowned device list empty, must discover unowned devices first");
        return -1;  // Error, we should have registered unowned devices already
    }

    OCProvisionDev_t* targetDevice = getDevInst((const OCProvisionDev_t*)g_own_list, dev_num);
    if(targetDevice == NULL)
    {
        OIC_LOG(ERROR, TAG, "Error, invalid device %d");
        return -1;
    }

    // Install the CA trust anchor
    uint16_t caCredId = 0;
    OCStackResult rst = OCSaveTrustCertChain((uint8_t*)caCertPem, strlen(caCertPem) + 1,
        OIC_ENCODING_PEM, &caCredId);
    if (OC_STACK_OK != rst)
    {
        OIC_LOG_V(ERROR, TAG, "OCSaveTrustCertChain error: %d", rst);
        return -1;
    }

    // Provision the CA root cert to the target device
    printf("   > Saving root certificate (trust anchor) to selected device..\n");
    g_doneCB = false;
    OicSecCredType_t type = SIGNED_ASYMMETRIC_KEY;

    rst = OCProvisionTrustCertChain((void*)g_ctx, type, caCredId, targetDevice, &provisionTrustChainCB);
    if(OC_STACK_OK != rst)
    {
        OIC_LOG_V(ERROR, TAG, "OCProvisionTrustCertChain returned error: %d", rst);
        return -1;
    }

    if(waitCallbackRet())  // input |g_doneCB| flag implicitly
    {
        OIC_LOG(ERROR, TAG, "OCProvisionTrustCertChain callback error");
        return -1;
    }
    if(!g_successCB)
    {
        return -1;
    }

    printf("   > Provisioned certificate trust anchor\n");

    return 0;

}

static int getCsr(int dev_num)
{
    // check |own_list| for retrieving CSR
    if(!g_own_list || 1 > g_own_cnt)
    {
        OIC_LOG(ERROR, TAG, "Unowned device list empty, must discover unowned devices first");
        return -1;  // Error, we should have registered unowned devices already
    }

    // call |getDevInst| API
    // calling this API with callback actually acts like blocking
    // for error checking, the return value saved and printed
    g_doneCB = false;
    OCProvisionDev_t* dev = getDevInst((const OCProvisionDev_t*) g_own_list, dev_num);
    if(!dev)
    {
        OIC_LOG(ERROR, TAG, "getDevInst: device instance empty");
        goto GETCSR_ERROR;
    }
    OCStackResult rst = OCGetCSRResource((void*) g_ctx, dev, getCsrForCertProvCB);
    if(OC_STACK_OK != rst)
    {
        OIC_LOG_V(ERROR, TAG, "OCGetCSRResource API error: %d", rst);

        goto GETCSR_ERROR;
    }
    if(waitCallbackRet())  // input |g_doneCB| flag implicitly
    {
        OIC_LOG(ERROR, TAG, "OCGetCSRResource callback error");
        goto GETCSR_ERROR;
    }
    if(!g_successCB)
    {
        return -1;
    }

    /* @todo: once change https://gerrit.iotivity.org/gerrit/#/c/17509 merges, we'll have this function */
#if 0
    int ret = OCInternalVerifyCSRSignature(&g_csr);
    if(ret != 0)
    {
        OIC_LOG(ERROR, TAG, "Failed to validate CSR signature");
        mbedtls_x509_csr_free(&g_csr);
        return -1;
    }
#endif
    mbedtls_x509_csr_free(&g_csr);

    printf("   > Get CSR SUCCEEDED\n");

    return 0;

GETCSR_ERROR:
    printf("   > Get CSR FAILED\n");
    return -1;
}

static OCProvisionDev_t* getDevInst(const OCProvisionDev_t* dev_lst, const int dev_num)
{
    if (!dev_lst || 0 >= dev_num)
    {
        printf("     Device List is Empty..\n");
        return NULL;
    }

    OCProvisionDev_t* lst = (OCProvisionDev_t*)dev_lst;
    for (int i = 0; lst; )
    {
        if (dev_num == ++i)
        {
            return lst;
        }
        lst = lst->next;
    }

    return NULL;  // in here |lst| is always |NULL|
}

static int printDevList(const OCProvisionDev_t* dev_lst)
{
    if (!dev_lst)
    {
        printf("     Device List is Empty..\n\n");
        return 0;
    }

    OCProvisionDev_t* lst = (OCProvisionDev_t*)dev_lst;
    int lst_cnt = 0;
    for (; lst; )
    {
        printf("     [%d] ", ++lst_cnt);
        printUuid((const OicUuid_t*)&lst->doxm->deviceID);
        printf("\n");
        lst = lst->next;
    }
    printf("\n");

    return lst_cnt;
}

static size_t printUuidList(const OCUuidList_t* uid_lst)
{
    if (!uid_lst)
    {
        printf("     Device List is Empty..\n\n");
        return 0;
    }

    OCUuidList_t* lst = (OCUuidList_t*)uid_lst;
    size_t lst_cnt = 0;
    for (; lst; )
    {
        printf("     [%zu] ", ++lst_cnt);
        printUuid((const OicUuid_t*)&lst->dev);
        printf("\n");
        lst = lst->next;
    }
    printf("\n");

    return lst_cnt;
}

static size_t printResultList(const OCProvisionResult_t* rslt_lst, const size_t rslt_cnt)
{
    if (!rslt_lst || (0 == rslt_cnt))
    {
        printf("     Device List is Empty..\n\n");
        return 0;
    }

    size_t lst_cnt = 0;
    for (; rslt_cnt > lst_cnt; ++lst_cnt)
    {
        printf("     [%" PRIuPTR "] ", lst_cnt + 1);
        printUuid((const OicUuid_t*)&rslt_lst[lst_cnt].deviceId);
        printf(" - result: %d\n", rslt_lst[lst_cnt].res);
    }
    printf("\n");

    return lst_cnt;
}

static void printUuid(const OicUuid_t* uid)
{
    for (int i = 0; i<UUID_LENGTH; )
    {
        printf("%02X", (*uid).id[i++]);
        if (i == 4 || i == 6 || i == 8 || i == 10)  // canonical format for UUID has '8-4-4-4-12'
        {
            printf("-");
        }
    }
}

static FILE* fopen_prvnMng(const char* path, const char* mode)
{
    if (0 == strncmp(path, OC_SECURITY_DB_DAT_FILE_NAME, strlen(OC_SECURITY_DB_DAT_FILE_NAME)))
    {
        // input |g_svr_db_fname| internally by force, not using |path| parameter
        // because |OCPersistentStorage::open| is called |OCPersistentStorage| internally
        // with its own |SVR_DB_FILE_NAME|
        return fopen(SVR_DB_FILE_NAME, mode);
    }
    else
    {
        return fopen(path, mode);
    }
}

static int waitCallbackRet(void)
{
    for(int i = 0; !g_doneCB; ++i)
    {
        if(OC_STACK_OK != OCProcess())
        {
            OIC_LOG(ERROR, TAG, "OCStack process error");
            return -1;
        }

        if (i == CALLBACK_TIMEOUT)
        {
            return -1;
        }

        sleep(1);
    }

    return 0;
}

void shutdownProvisionClient()
{
    if(OC_STACK_OK != OCStop())
    {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
    }
    OCDeleteDiscoveredDevices(g_own_list);  // after here |g_own_list| points to nothing
    OCDeleteDiscoveredDevices(g_unown_list);  // after here |g_unown_list| points to nothing

    OICFreeAndSetToNull(&g_svr_fname);
    OICFreeAndSetToNull(&g_prvn_fname);
}

static int initDiscoverRegisterAllDevices()
{
    if(initProvisionClient())
    {
        OIC_LOG_V(ERROR, TAG, "%s: ProvisionClient init error", __func__);
        return -1;
    }

    if(discoverAllDevices())
    {
        OIC_LOG_V(ERROR, TAG, "%s: Discovery failed", __func__);
        return -1;
    }

    if(registerDevices())
    {
        OIC_LOG_V(ERROR, TAG, "%s: Failed to onboard devices", __func__);
        return -1;
    }

    if(discoverAllDevices())
    {
        OIC_LOG_V(ERROR, TAG, "%s: Re-discovery failed after registerDevices", __func__);
        return -1;
    }

    return 0;

}

int TestTrustAnchorProvisioning()
{
    int ret = -1;

    OIC_LOG_V(ERROR, TAG, "Running %s", __func__);

    if(initDiscoverRegisterAllDevices())
    {
        OIC_LOG_V(ERROR, TAG, "%s: Failed discover and provision devices", __func__);
        goto exit;
    }

    /* There should be one owned device with number 1. */
    if(provisionTrustAnchor(1))
    {
        OIC_LOG_V(ERROR, TAG, "%s: Failed to provision trust anchor to device", __func__);
        goto exit;
    }

    ret = 0;

exit:

    shutdownProvisionClient();

    return ret;
}

int TestCSRResource()
{
    int ret = -1;

    OIC_LOG_V(ERROR, TAG, "Running %s", __func__);

    if(initDiscoverRegisterAllDevices())
    {
        OIC_LOG_V(ERROR, TAG, "%s: Failed discover and provision devices", __func__);
        goto exit;
    }

    /* There should be one owned device with number 1. */
    if(getCsr(1))
    {
        OIC_LOG(ERROR, TAG, "Test1: Failed to get CSR from device");
        goto exit;
    }

    ret = 0;

exit:

    shutdownProvisionClient();

    return ret;
}

// main function for provisioning client using C-level provisioning API
int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("%s: No test number provided (argc = %d)\n", argv[0], argc);
        return 1;
    }

    unsigned int testNumber = strtoul(argv[1], NULL, 10);
    switch (testNumber)
    {
        case 1:
            return TestTrustAnchorProvisioning();
        case 2:
            return TestCSRResource();
        default:
            printf("%s: Invalid test number\n", argv[0]);
            return 1;
    }

}

#ifdef __cplusplus
}
#endif //__cplusplus
