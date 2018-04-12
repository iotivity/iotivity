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
#include "securevirtualresourcetypes.h"
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
static OCPersistentStorage s_pstStr;
static string g_hostAddress = "";
static OCAccountManager::Ptr g_accountMgr = nullptr;

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

    s_pstStr.open = controllerOpen;
    s_pstStr.read = fread;
    s_pstStr.write = fwrite;
    s_pstStr.close = fclose;
    s_pstStr.unlink = unlink;

    if (OC_STACK_OK != OCRegisterPersistentStorageHandler(&s_pstStr))
    {
        IOTIVITYTEST_LOG(ERROR, "[IC Server] OCRegisterPersistentStorageHandler error");
        return;
    }

    // initialize OC stack and provisioning manager
    if (OC_STACK_OK != OCInit(NULL, 0, OC_CLIENT_SERVER))
    {
        IOTIVITYTEST_LOG(ERROR, "[IC Server] OCStack init error");
        return;
    }

    IOTIVITYTEST_LOG(INFO, "[IC Server] Host Address : %s", g_hostAddress.c_str());

    g_accountMgr = OCPlatform::constructAccountManagerObject(g_hostAddress, CT_ADAPTER_TCP);
    __FUNC_OUT__
}

/**
 * @pre_condition   1. Start Kafka Server
 *                  2. Start ZooKeeper Server
 *                  3. Start Mongo DB Server
 *                  4. Start RD, MQ, Account and CI Server respectively
 * @procedure       1. Call OCRegisterPersistentStorageHandler
 *                  2. Call OCInit
 *                  6. Call signUp and save uid and accessToken in cloud.properties file
 *                  7. Call signIn using uid and accessToken
 *                  10. Call signOut
 * @post_condition  None
 * @expected        Cloud Initialization Successful.
 */
int main()
{
    bool isInitialized = true;
#ifdef __SECURED__
    IOTIVITYTEST_LOG(INFO, "[IC Server] SECURED MODE : On");
#else
    IOTIVITYTEST_LOG(INFO, "[IC Server] SECURED MODE : Off");
#endif

#ifdef __TLS_ON__
    IOTIVITYTEST_LOG(INFO, "TLS MODE : On");
#else
    IOTIVITYTEST_LOG(INFO, "TLS MODE : Off");
#endif

    g_hostAddress = CloudCommonUtil::getDefaultHostAddess();

    initServer();

    isInitialized = CloudCommonUtil::signUp(g_accountMgr);

    if (isInitialized)
    {
        isInitialized = CloudCommonUtil::signIn(g_accountMgr);

        if (isInitialized)
        {
            CloudCommonUtil::signOut(g_accountMgr);
        }
    }

    IOTIVITYTEST_LOG(INFO, "Cloud Initialization %s", isInitialized ? "Successful" : "Failed");
    return 0;
}
