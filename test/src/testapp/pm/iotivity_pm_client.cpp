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
#include "CommonUtil.h"
#include "IotivityTest_Logger.h"
#include "SecurityCommonUtil.h"

#ifdef __WINDOWS__
std::string g_appName;
#endif

#define CTX_OWNERSHIP "Ownership Transfer"
#define CTX_PAIRWISE_DEVICE "Pairwise Provisioning"

#define KILL_SERVERS "iotivity_pm_ser"
#define JUSTWORKS_SERVER1 "./iotivity_pm_server oic_svr_db_server.dat 1"
#define JUSTWORKS_SERVER2 "./iotivity_pm_server oic_svr_db_server_justworks.dat 1"

const string UNOWNED_DIR = "unowned";
const string OWNED_DIR = "owned";
const string PAIRWISE_DIR = "pairwise";
const string DEVICE_PROP_FILE = "device_properties.dat";
const string PM_CLIENT_DB_FILE = "pmclient.db";
const string PM_CLIENT_DAT_FILE = "oic_svr_db_client.dat";
const string JUST_WORK_SERVER_01 = "oic_svr_db_server.dat";
const string JUST_WORK_SERVER_02 = "oic_svr_db_server_justworks.dat";

const string DEFAULT_DIR = "pm_default";

void createBackup(string dirPath)
{
    CommonUtil::mkDir(dirPath);
    CommonUtil::copyFile(DEVICE_PROP_FILE, dirPath + "/" + DEVICE_PROP_FILE);
    CommonUtil::copyFile(PM_CLIENT_DAT_FILE, dirPath + "/" + PM_CLIENT_DAT_FILE);
    CommonUtil::copyFile(PM_CLIENT_DB_FILE, dirPath + "/" + PM_CLIENT_DB_FILE);
    CommonUtil::copyFile(JUST_WORK_SERVER_01, dirPath + "/" + JUST_WORK_SERVER_01);
    CommonUtil::copyFile(JUST_WORK_SERVER_02, dirPath + "/" + JUST_WORK_SERVER_02);
}

/**
 * Procedure:
 * 1. Kill All Server
 * 2. Remove all client dat and device.properties file
 * 3. Copy .dat file from Unowned directory
 * 4. Launch Two Justworks Server
 * 5. Call OCRegisterPersistentStorageHandler
 * 6. Call OCInit
 * 7. Call OCInitPM
 * 8. Call OCDiscoverUnownedDevices
 * 9. Filter discovered devices
 * 10. Call OCDoOwnershipTransfer
 * 11. Create backup .dat files into "Owned" directory
 * 12. Call OCDiscoverOwnedDevices
 * 13. Call OCProvisionPairwiseDevices
 * 14. Create backup .dat files into "pairwise" directory
 *
 * Expected: PM initialization Successfull.
 */
int main(int argc, char **argv)
{
    bool isSuccessful = true;
    bool isDeviceExist = true;

    SecurityCommonUtil m_SecurityCommonUtil;
    OCProvisionDev_t *m_UnownList = NULL;
    OCProvisionDev_t *m_OwnList = NULL;
    OicSecAcl_t *m_Acl = NULL;
    OicSecAcl_t *m_Acl1 = NULL;
    OicSecAcl_t *m_Acl2 = NULL;

    // Reset Cbor Resources
    CommonUtil::rmFile(PM_CLIENT_DAT_FILE);
    CommonUtil::rmFile(PM_CLIENT_DB_FILE);
    CommonUtil::rmFile(DEVICE_PROP_FILE);
    CommonUtil::copyFile("./" + UNOWNED_DIR + "/" + PM_CLIENT_DAT_FILE, PM_CLIENT_DAT_FILE);
    CommonUtil::copyFile("./" + UNOWNED_DIR + "/" + JUST_WORK_SERVER_01, JUST_WORK_SERVER_01);
    CommonUtil::copyFile("./" + UNOWNED_DIR + "/" + JUST_WORK_SERVER_02, JUST_WORK_SERVER_02);
    CommonUtil::waitInSecond(DELAY_MEDIUM);
    // Intilize Provisioning Manager
    m_SecurityCommonUtil.initProvisionClient();

    //Getting Info if there is any unwanted device
    IOTIVITYTEST_LOG(INFO, "Getting Existing Devices of the Current Network");
    IOTIVITYTEST_LOG(INFO, "================================================");
    IOTIVITYTEST_LOG(INFO, "================================================");
    IOTIVITYTEST_LOG(INFO, "================================================");

    m_SecurityCommonUtil.discoverAllDevices(DISCOVERY_TIMEOUT, &m_OwnList,
            &m_UnownList, OC_STACK_OK);

    while (m_UnownList)
    {
        int port = m_UnownList->endpoint.port;
        string processId = CommonUtil::getPIDFromPort(std::to_string(port));
        CommonUtil::killPID(processId);
        IOTIVITYTEST_LOG(INFO, "Killing Unowned Dev Process Id : %s from Port : %d", processId.c_str(), port);
        m_UnownList = m_UnownList->next;
    }

    while (m_OwnList)
    {
        int port = m_OwnList->endpoint.port;
        string processId = CommonUtil::getPIDFromPort(std::to_string(port));
        CommonUtil::killPID(processId);
        IOTIVITYTEST_LOG(INFO, "Killing Owned Dev Process Id : %s from Port : %d", processId.c_str(), port);
        m_OwnList = m_OwnList->next;
    }

    IOTIVITYTEST_LOG(INFO, "================================================");
    IOTIVITYTEST_LOG(INFO, "================================================");
    IOTIVITYTEST_LOG(INFO, "================================================");

    // Kill all unwanted devices
    CommonUtil::killApp(KILL_SERVERS);
    CommonUtil::waitInSecond(DELAY_MEDIUM);

    // Start Justworks Servers
    CommonUtil::launchApp(JUSTWORKS_SERVER1);
    CommonUtil::launchApp(JUSTWORKS_SERVER2);
    CommonUtil::waitInSecond(DELAY_MEDIUM);
    // Provisiong Manager Secure Resource Management
    m_UnownList = NULL;
    m_OwnList = NULL;

    isSuccessful = m_SecurityCommonUtil.discoverUnownedDevices(DISCOVERY_TIMEOUT, &m_UnownList,
            OC_STACK_OK);

    // Filtering Unowned Devices
    m_UnownList = SecurityCommonUtil::filterDevices(m_UnownList);
    IOTIVITYTEST_LOG(INFO, "Filtered Unowned Device List");
    SecurityCommonUtil::printDevList(m_UnownList);

    isDeviceExist = m_UnownList && m_UnownList->next;

    if (isSuccessful && isDeviceExist)
    {

        // Ownership Transfer
        isSuccessful = m_SecurityCommonUtil.doOwnerShipTransfer((void*) CTX_OWNERSHIP, &m_UnownList,
                SecurityCommonUtil::ownershipTransferCB, OC_STACK_OK);

        createBackup(OWNED_DIR);

        if (isSuccessful)
        {
            isSuccessful = m_SecurityCommonUtil.discoverOwnedDevices(DISCOVERY_TIMEOUT, &m_OwnList,
                    OC_STACK_OK);

            isDeviceExist = m_OwnList && m_OwnList->next;

            if (isSuccessful && isDeviceExist)
            {
                // Filtering Owned Devices
                m_OwnList = SecurityCommonUtil::filterDevices(m_OwnList);
                IOTIVITYTEST_LOG(INFO, "Filtered Owned Device List");
                SecurityCommonUtil::printDevList(m_OwnList);

                // Provisioning Pairwise Devices
                OCProvisionDev_t *device1 = m_OwnList;
                OCProvisionDev_t *device2 = m_OwnList->next;
                m_Acl1 = m_SecurityCommonUtil.createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION,
                        &m_OwnList);
                m_Acl2 = m_SecurityCommonUtil.createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION,
                        &m_OwnList);
                OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
                size_t keySize = OWNER_PSK_LENGTH_128;

                isSuccessful = m_SecurityCommonUtil.provisionPairwiseDevices(
                        (void*) CTX_PAIRWISE_DEVICE, type, keySize, device1, m_Acl1, device2,
                        m_Acl2, SecurityCommonUtil::provisionCB, OC_STACK_OK);

                createBackup(PAIRWISE_DIR);
            }
        }

    }

    CommonUtil::killApp(KILL_SERVERS);
    IOTIVITYTEST_LOG(INFO, "PM Initialization %s",
            isSuccessful && isDeviceExist ? "Successful" : "Failed");

    return 0;
}
