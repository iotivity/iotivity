/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocprovisioningmanager.h"
#include "oxmjustworks.h"
#include "oxmrandompin.h"
#include "securevirtualresourcetypes.h"
#include "srmutility.h"
#include "pmtypes.h"

#include <pthread.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#define TAG "ClientOwnedBySubOwner"
static bool g_doneCB;

static pthread_t oc_process_thread;
static bool g_LoopFlag = true;

static void* oc_process_loop(void* ptr)
{
    struct timespec timeout;
    timeout.tv_sec  = 0;
    timeout.tv_nsec = 100000000L;

    while(g_LoopFlag)
    {
        OCProcess();
        nanosleep(&timeout, NULL);
    }
    pthread_join(&oc_process_thread, NULL);
    return NULL;
}

static void StartOCProcessThread()
{
    pthread_create(&oc_process_thread, NULL, oc_process_loop, NULL);
}

static void StopOCProcessThread()
{
    g_LoopFlag = false;
}

// declaration(s) for provisioning client using C-level provisioning API
// user input definition for main loop on provisioning client
#define _10_DISCOV_OWN_DEVS_    10
#define _20_GET_LED_RESOURCE_ 20
#define _21_PUT_LED_RESOURCE_ 21
#define _99_EXIT_PRVN_CLT_      99

#define DISCOVERY_TIMEOUT   5  // 10 sec
#define CALLBACK_TIMEOUT    60  // 1 min

static const char* SVR_DB_FILE_NAME = "oic_svr_db_client_owned_by_subowner.dat";
        // '_' for separaing from the same constant variable in |srmresourcestrings.c|
static const char* PRVN_DB_FILE_NAME = "oic_prvn_cobs.db";
// |g_ctx| means provision manager application context and
// the following, includes |un/own_list|, could be variables, which |g_ctx| has,
// for accessing all function(s) for these, they are declared on global domain
static const char* g_ctx = "Provision Manager Client Application Context";
static char* g_svr_fname;
static char* g_prvn_fname;
static OCProvisionDev_t* g_own_list;
static int g_own_cnt;

// function declaration(s) for calling them before implementing
static OCProvisionDev_t* getDevInst(const OCProvisionDev_t*, const int);
static int printDevList(const OCProvisionDev_t*);
static size_t printUuidList(const OCUuidList_t*);
static int printResultList(const OCProvisionResult_t*, const int);
static void printUuid(const OicUuid_t*);
static FILE* fopen_prvnMng(const char*, const char*);
static int waitCallbackRet(void);
static int selectTwoDiffNum(int*, int*, const int, const char*);

// callback function(s) for provisioning client using C-level provisioning API
static void ownershipTransferCB(void* ctx, int nOfRes, OCProvisionResult_t* arr, bool hasError)
{
    if(!hasError)
    {
        OIC_LOG_V(INFO, TAG, "Ownership Transfer SUCCEEDED - ctx: %s", (char*) ctx);
    }
    else
    {
        OIC_LOG_V(ERROR, TAG, "Ownership Transfer FAILED - ctx: %s", (char*) ctx);
        printResultList((const OCProvisionResult_t*) arr, nOfRes);
    }
    g_doneCB = true;
}

// response handler for LED requests.
static void LedCB(void *ctx, OCDoHandle UNUSED,
                                                    OCClientResponse *clientResponse)
{
    if(clientResponse)
    {
        if(clientResponse->result == OC_STACK_OK)
        {
            printf("Get OC_STACK_OK from server\n");
            if(clientResponse->payload)
            {
                printf("Response ===================> %s\n", clientResponse->payload);
            }
        }
        else
        {
            printf("Error in response : %d\n", clientResponse->result);
        }
    }
    else
    {
        printf("Hit the response callback but can not find response data\n");
    }

    g_doneCB = true;
}


// function(s) for provisioning client using C-level provisioning API
static int initProvisionClient(void)
{
    // initialize persistent storage for SVR DB
	static OCPersistentStorage ps = {fopen_prvnMng, fread, fwrite, fclose, unlink};
    if(OC_STACK_OK != OCRegisterPersistentStorageHandler(&ps))
    {
        OIC_LOG(ERROR, TAG, "OCRegisterPersistentStorageHandler error");
        return -1;
    }

    // initialize OC stack and provisioning manager
    if(OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        return -1;
    }

    if (access(PRVN_DB_FILE_NAME, F_OK) != -1)
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

    if(OC_STACK_OK != OCInitPM(PRVN_DB_FILE_NAME))
    {
        OIC_LOG(ERROR, TAG, "OC_PM init error");
        return -1;
    }

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

    // call |OCDiscoverOwnedDevices| API actually
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

static OCProvisionDev_t* getDevInst(const OCProvisionDev_t* dev_lst, const int dev_num)
{
    if(!dev_lst || 0>=dev_num)
    {
        printf("     Device List is Empty..\n");
        return NULL;
    }

    OCProvisionDev_t* lst = (OCProvisionDev_t*) dev_lst;
    for(int i=0; lst; )
    {
        if(dev_num == ++i)
        {
            return lst;
        }
        lst = lst->next;
    }

    return NULL;  // in here |lst| is always |NULL|
}

static int printDevList(const OCProvisionDev_t* dev_lst)
{
    if(!dev_lst)
    {
        printf("     Device List is Empty..\n\n");
        return 0;
    }

    OCProvisionDev_t* lst = (OCProvisionDev_t*) dev_lst;
    int lst_cnt = 0;
    for( ; lst; )
    {
        printf("     [%d] ", ++lst_cnt);
        printUuid((const OicUuid_t*) &lst->doxm->deviceID);
        printf("\n");
        lst = lst->next;
    }
    printf("\n");

    return lst_cnt;
}

static size_t printUuidList(const OCUuidList_t* uid_lst)
{
    if(!uid_lst)
    {
        printf("     Device List is Empty..\n\n");
        return 0;
    }

    OCUuidList_t* lst = (OCUuidList_t*) uid_lst;
    size_t lst_cnt = 0;
    for( ; lst; )
    {
        printf("     [%zu] ", ++lst_cnt);
        printUuid((const OicUuid_t*) &lst->dev);
        printf("\n");
        lst = lst->next;
    }
    printf("\n");

    return lst_cnt;
}

static int printResultList(const OCProvisionResult_t* rslt_lst, const int rslt_cnt)
{
    if(!rslt_lst || 0>=rslt_cnt)
    {
        printf("     Device List is Empty..\n\n");
        return 0;
    }

    int lst_cnt = 0;
    for( ; rslt_cnt>lst_cnt; ++lst_cnt)
    {
        printf("     [%d] ", lst_cnt+1);
        printUuid((const OicUuid_t*) &rslt_lst[lst_cnt].deviceId);
        printf(" - result: %d\n", rslt_lst[lst_cnt].res);
    }
    printf("\n");

    return lst_cnt;
}

static void printUuid(const OicUuid_t* uid)
{
    int i;
    for(i=0; i<UUID_LENGTH; )
    {
        printf("%02X", (*uid).id[i++]);
        if(i==4 || i==6 || i==8 || i==10)  // canonical format for UUID has '8-4-4-4-12'
        {
            printf("-");
        }
    }

    printf("\t(");
    for(i = 0; i < UUID_LENGTH; i++)
    {
        printf("%c", (*uid).id[i]);
    }
    printf(")");
}

static FILE* fopen_prvnMng(const char* path, const char* mode)
{
    (void)path;  // unused |path| parameter

    // input |g_svr_db_fname| internally by force, not using |path| parameter
    // because |OCPersistentStorage::open| is called |OCPersistentStorage| internally
    // with its own |SVR_DB_FILE_NAME|
    return fopen(SVR_DB_FILE_NAME, mode);
}

static int waitCallbackRet(void)
{
    for(int i=0; !g_doneCB && CALLBACK_TIMEOUT>i; ++i)
    {
        sleep(1);
    }

    return 0;
}

static void printMenu(void)
{
    printf("************************************************************\n");
    printf("****** OIC MOT Client with using C-level API ******\n");
    printf("************************************************************\n\n");

    printf("** [A] DISCOVER DEVICES ON NETWORK\n");
    printf("** 10. Discover Owned Devices on Network\n\n");

    printf("** [B] SEND GET/PUT Request\n");
    printf("** 20. Send GET request\n");
    printf("** 21. Send PUT request\n\n");

    printf("** [G] EXIT PROVISIONING CLIENT\n");
    printf("** 99. Exit Provisionong Client\n\n");

    printf("************************************************************\n\n");
}

#if 0 // Code for enabling path configuration for PDB and SVR DBf
static void printUsage(void)
{
    printf("\n");
    printf("OIC Provisioning Client with using C-level API\n");
    printf("Usage: provisioningclient [option]...\n");
    printf("\n");
    printf("  -h                           print help for this provisioning client\n");
    printf("  -p=[prvn_db_file_path/name]  input PRVN DB file path and name\n");
    printf("                               if not exists, will load default DB file\n");
    printf("                               (default: |oic_prvn_mng.db| on working dir)\n");
    printf("                               (ex. -p=oic_prvn_mng.db)\n");
    printf("  -s=[svr_db_file_path/name]   input SVR DB file path and name\n");
    printf("                               if not exists, will load default DB file\n");
    printf("                               (default: |oic_svr_db_client.json| on working dir)\n");
    printf("                               (ex. -s=oic_svr_db_client.json)\n");
    printf("\n");
}
#endif

// main function for provisioning client using C-level provisioning API
int main()
{
    OCProvisionDev_t* selDev = NULL;
    int selDevNum = 0;
    int mn_num = 0;

    // initialize provisioning client
    if(initProvisionClient())
    {
        OIC_LOG(ERROR, TAG, "ProvisionClient init error");
        goto PMCLT_ERROR;
    }

    StartOCProcessThread();
    printf("Client started...\n");
    // main loop for provisioning manager
    for( ; ; )
    {
        printf("\n");
        printMenu();
        printf(">> Enter Menu Number: ");
        for(int ret=0; 1!=ret; )
        {
            ret = scanf("%d", &mn_num);
            for( ; 0x20<=getchar(); );  // for removing overflow garbages
                                        // '0x20<=code' is character region
        }
        printf("\n");
        switch(mn_num)
        {
        case _10_DISCOV_OWN_DEVS_:
            if(discoverOwnedDevices())
            {
                OIC_LOG(ERROR, TAG, "_12_DISCOV_OWN_DEVS_: error");
            }
            break;
        case _20_GET_LED_RESOURCE_:
        {
            char query[256] = {0};
            OCCallbackData cbData;
            cbData.cb = &LedCB;
            cbData.context = NULL;
            cbData.cd = NULL;

            printDevList(g_own_list);

            for( ; ; )
            {
                printf("Select the device : ");
                for(int ret=0; 1!=ret; )
                {
                    scanf(" %d", &selDevNum);
                    for( ; 0x20<=getchar(); );  // for removing overflow garbages
                                                // '0x20<=code' is character region
                }
                if(0<selDevNum && g_own_cnt>=selDevNum)
                {
                    break;
                }
                printf("     Entered Wrong Number. Please Enter Again\n");
            }

            selDev = getDevInst(g_own_list, selDevNum);
            if(!selDev)
            {
                OIC_LOG(ERROR, TAG, "getDevInst: device instance empty");
                goto PMCLT_ERROR;
            }

            g_doneCB = false;
            snprintf(query, MAX_URI_LENGTH, "coaps://%s:%d/a/led", selDev->endpoint.addr, selDev->securePort);
            if(OC_STACK_OK != OCDoResource(NULL, OC_REST_GET, query, NULL, NULL, selDev->connType, OC_LOW_QOS, &cbData, NULL, 0))
            {
                printf("********************************\n");
                printf("Failed to send GET request to %s\n", query);
                printf("********************************\n");
                g_doneCB = true;
                break;
            }

            waitCallbackRet();

            break;
        }
        case _21_PUT_LED_RESOURCE_:
        {
            char query[256] = {0};
            OCCallbackData cbData;
            cbData.cb = &LedCB;
            cbData.context = NULL;
            cbData.cd = NULL;

            printDevList(g_own_list);

            for( ; ; )
            {
                printf("Select the device : ");
                for(int ret=0; 1!=ret; )
                {
                    scanf(" %d", &selDevNum);
                    for( ; 0x20<=getchar(); );  // for removing overflow garbages
                                                // '0x20<=code' is character region
                }
                if(0<selDevNum && g_own_cnt>=selDevNum)
                {
                    break;
                }
                printf("     Entered Wrong Number. Please Enter Again\n");
            }

            selDev = getDevInst(g_own_list, selDevNum);
            if(!selDev)
            {
                OIC_LOG(ERROR, TAG, "getDevInst: device instance empty");
                goto PMCLT_ERROR;
            }

            g_doneCB = false;
            snprintf(query, MAX_URI_LENGTH, "coaps://%s:%d/a/led", selDev->endpoint.addr, selDev->securePort);
            if(OC_STACK_OK != OCDoResource(NULL, OC_REST_PUT, query, NULL, NULL, selDev->connType, OC_LOW_QOS, &cbData, NULL, 0))
            {
                printf("********************************\n");
                printf("Failed to send PUT request to %s\n", query);
                printf("********************************\n");
                g_doneCB = true;
                break;
            }

            waitCallbackRet();

            break;
        }
        case _99_EXIT_PRVN_CLT_:
            goto PMCLT_ERROR;
        default:
            printf(">> Entered Wrong Number. Please Enter Again\n\n");
            break;
        }
    }

PMCLT_ERROR:
    if(OC_STACK_OK != OCStop())
    {
        OIC_LOG(ERROR, TAG, "OCStack stop error");
    }
    OCDeleteDiscoveredDevices(g_own_list);  // after here |g_own_list| points nothing

    if(g_svr_fname)
    {
        OICFree(g_svr_fname);  // after here |g_svr_fname| points nothing
    }
    if(g_prvn_fname)
    {
        OICFree(g_prvn_fname);  // after here |g_prvn_fname| points nothing
    }

    StopOCProcessThread();

    return 0;  // always return normal case
}

#ifdef __cplusplus
}
#endif //__cplusplus
