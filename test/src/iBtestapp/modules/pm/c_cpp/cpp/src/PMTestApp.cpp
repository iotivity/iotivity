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
#include "PMCppAppHelper.h"

static int g_userInput = 0;
static DeviceList_t s_unOwnedDevList, s_ownedDevList, s_motEnableDevList, s_motOwnedDevList;
static OCPersistentStorage s_pmPs, s_pmMotPs;
static PlatformConfig s_pmCfg, s_pmMotCfg;
static OTMCallbackData_t s_justWorksCBDataDp, s_pinBasedCBDataDp;
static std::shared_ptr< OCSecureResource > s_TargetList;
static OicUuid_t rev;
static InputPinCallbackHandle callbackHandle = nullptr;

/*
 * UI Related
 */
typedef enum
{
    EXIT,
    START_JUSTWORKS_SERVER = 11,
    START_JUSTWORKS_SERVER2,
    START_RANDOMPIN_SERVER,
    START_PRECONFIG_SERVER,
    START_MVJUSTWORK_SERVER,

    START_PM_CLIENT = 20,
    DISCOVER_SINGLE_DEVICE,
    JUSTWORKS_OWNERSHIP_TRANSFER,
    RANDOMPIN_OWNERSHIP_TRANSFER,
    RANDOMPIN_OWNERSHIP_TRANSFER_WITH_WRONG_PIN,
    PROVISION_ACL,
    PROVISION_CREDENTIAL,
    PROVISIONING_PAIRWISE_DEVICES,
    GET_LINKED_DEVICES,
    SAVE_ACL,
    UNLINK_DEVICE,
    REMOVE_DEVICES,
    REMOVE_DEVICES_WITH_UUID,
    ENABLE_MOT_DEV = 35,
    GET_DEVICE_STATUS,

#if defined(__MOT__)
START_MOT_CLIENT = 61,
DISCOVER_MOT_ENABLED_DEVICE,
DO_MULTIPLE_OWNERSHIP_TRANSFER,
DISCOVER_MOT_OWNED_DEVICE,
GET_LED_RESOURCE,
PUT_LED_RESOURCE,
#endif
} menu;

void clearScreen()
{
const char* CLEAR_SCREE_ANSI = "\e[1;1H\e[2J";
write(STDOUT_FILENO, CLEAR_SCREE_ANSI, 12);
}

static void printMenu(void)
{
clearScreen();

printf("\n======================SERVER==========================\n");
printf("%d. Start Justworks Server [1]\n", START_JUSTWORKS_SERVER);
printf("%d. Start Justworks Server Without DP Support\n", START_JUSTWORKS_SERVER2);
printf("%d. Start RandomPin Server\n", START_RANDOMPIN_SERVER);
printf("%d. Start Preconfig Server\n", START_PRECONFIG_SERVER);
printf("%d. Start MV JustWork Server\n", START_MVJUSTWORK_SERVER);

printf("\n====================PM CLIENT=========================\n");
printf("%d. Start Provisioning Client\n", START_PM_CLIENT);
printf("%d. Single Device discovery\n", DISCOVER_SINGLE_DEVICE);
printf("%d. Justworks Ownership Transfer\n", JUSTWORKS_OWNERSHIP_TRANSFER);
printf("%d. RandomPin Ownership Transfer\n", RANDOMPIN_OWNERSHIP_TRANSFER);
printf("%d. Provision Access Control List (ACL)\n", PROVISION_ACL);
printf("%d. Provision Credential\n", PROVISION_CREDENTIAL);
printf("%d. Provision Pairwise Devices\n", PROVISIONING_PAIRWISE_DEVICES);
printf("%d. Get Linked Devices\n", GET_LINKED_DEVICES);
printf("%d. Save Access Control List\n", SAVE_ACL);
printf("%d. Unlink Devices\n", UNLINK_DEVICE);
printf("%d. Remove Devices\n", REMOVE_DEVICES);
printf("%d. Remove Devices using UUID\n", REMOVE_DEVICES_WITH_UUID);
printf("%d. Enable Mot Devices\n", ENABLE_MOT_DEV);
printf("%d. Get Device Status\n", GET_DEVICE_STATUS);

#if defined(__MOT__)
printf("\n====================MOT CLIENT=========================\n");
printf("%d. Start MOT CLIENT\n", START_MOT_CLIENT);
printf("%d. Discover MOT Enable Devices\n", DISCOVER_MOT_ENABLED_DEVICE);
printf("%d. Do Multiple Ownership Transfer\n", DO_MULTIPLE_OWNERSHIP_TRANSFER);
printf("%d. Discover MOT Owned Devices\n", DISCOVER_MOT_OWNED_DEVICE);
printf("%d. Get LED resource\n", GET_LED_RESOURCE);
printf("%d. Put LED resource\n\n", PUT_LED_RESOURCE);
#endif

printf("%d. Exit\n", EXIT);
printf("Enter Input: ");

}

void doAction(int userInput)
{
switch (userInput)
{
    case START_JUSTWORKS_SERVER:
        startServer(1);
        break;

    case START_JUSTWORKS_SERVER2:
        startServer(2);
        break;

    case START_RANDOMPIN_SERVER:
        startServer(3);
        break;

    case START_PRECONFIG_SERVER:
        startServer(4);
        break;

    case START_MVJUSTWORK_SERVER:
        startServer(5);
        break;

    case START_PM_CLIENT:
        //setup
        removeAllResFile (CLIENT);
        CommonUtil::waitInSecond(DELAY_LONG);
        copyAllResFile(CLIENT);
        CommonUtil::waitInSecond(DELAY_LONG);

        s_pmPs =
        {   clientCppAppOpen, fread, fwrite, fclose, unlink};

        s_pmCfg =
        {   OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0,
            OC::QualityOfService::LowQos, &s_pmPs};

        OCPlatform::Configure(s_pmCfg);

        if (OCSecure::provisionInit(EMPTY_STRING) != OC_STACK_OK)
        {
            std::cout << "PM Init failed" << std::endl;
        }

        if (OCSecure::registerInputPinCallback(OnInputPinCB, &callbackHandle) != OC_STACK_OK)
        {
            std::cout << "!!Error - registerInputPinCallback failed." << std::endl;
        }

        if(OCSecure::registerDisplayNumCallback(displayMutualVerifNumCB) != OC_STACK_OK)
        {
            std::cout << "!!Error - registerDisplayNumCallback failed." << std::endl;
        }

        if(OCSecure::registerUserConfirmCallback(confirmMutualVerifNumCB) != OC_STACK_OK)
        {
            std::cout << "!!Error - registerUserConfirmCallback failed." << std::endl;
        }

        std::cout << "setup succeeded" << std::endl;

        break;

        case DISCOVER_SINGLE_DEVICE:
        //s_unOwnedDevList.clear();

        discoverUnownedDevices(DISCOVERY_TIMEOUT, s_unOwnedDevList, OC_STACK_OK);
        {
            convertStrToUuid(s_unOwnedDevList[0]->getDeviceID(), &rev,OC_STACK_OK);
            discoverSingleDevice(DISCOVERY_TIMEOUT,&rev,s_TargetList,OC_STACK_OK);
        }

        break;

        case JUSTWORKS_OWNERSHIP_TRANSFER:
        s_unOwnedDevList.clear();

        discoverUnownedDevices(DISCOVERY_TIMEOUT, s_unOwnedDevList, OC_STACK_OK);
        doOwnershipTransfer(s_unOwnedDevList, ownershipTransferCB, OC_STACK_OK);

        break;

        case RANDOMPIN_OWNERSHIP_TRANSFER:
        s_unOwnedDevList.clear();
        discoverUnownedDevices(DISCOVERY_TIMEOUT, s_unOwnedDevList, OC_STACK_OK);
        doOwnershipTransfer(s_unOwnedDevList, ownershipTransferCB, OC_STACK_OK);

        break;

        case PROVISION_ACL:
        s_ownedDevList.clear();
        discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);

        {
            OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, s_ownedDevList);
            provisionACL(s_ownedDevList, acl1, provisionCB, OC_STACK_OK);
        }

        break;

        case SAVE_ACL:
        s_ownedDevList.clear();
        discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);

        {
            OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, s_ownedDevList);
            saveACL(acl1,OC_STACK_OK);
        }

        break;

        case PROVISION_CREDENTIAL:
        s_ownedDevList.clear();
        discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);

        {
            OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
            size_t keySize = OWNER_PSK_LENGTH_128;
            Credential cred(type, keySize);

            provisionCredentials(s_ownedDevList, cred, *s_ownedDevList[1].get(), provisionCB,
                    OC_STACK_OK);
        }
        break;

        case PROVISIONING_PAIRWISE_DEVICES:
        s_ownedDevList.clear();
        discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);
        {
            OicSecAcl_t *acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, s_ownedDevList);
            OicSecAcl_t *acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, s_ownedDevList);
            OicSecCredType_t type = SYMMETRIC_PAIR_WISE_KEY;
            size_t keySize = OWNER_PSK_LENGTH_128;
            Credential cred(type, keySize);

            provisionPairwiseDevices(s_ownedDevList, cred, acl1, *s_ownedDevList[1].get(), acl2,
                    provisionCB, OC_STACK_OK);

        }

        break;

        case GET_LINKED_DEVICES:
        s_ownedDevList.clear();
        discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);
        {
            UuidList_t uuidList1;
            getLinkedDevices(s_ownedDevList, uuidList1, OC_STACK_OK);
        }
        break;

        case GET_DEVICE_STATUS:
        s_ownedDevList.clear();
        s_unOwnedDevList.clear();
        getDevInfoFromNetwork(DISCOVERY_TIMEOUT,s_ownedDevList,s_unOwnedDevList, OC_STACK_OK);

        break;

        case UNLINK_DEVICE:
        s_ownedDevList.clear();
        discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);
        unlinkDevices(s_ownedDevList, *s_ownedDevList[1].get(), provisionCB, OC_STACK_OK);
        break;

        case REMOVE_DEVICES:
        s_ownedDevList.clear();
        discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);
        removeDevice(s_ownedDevList, DISCOVERY_TIMEOUT, provisionCB, OC_STACK_OK);
        break;

        case REMOVE_DEVICES_WITH_UUID:
        s_ownedDevList.clear();
        discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);
        removeDeviceWithUuid(DISCOVERY_TIMEOUT, s_ownedDevList[0]->getDeviceID(), provisionCB, OC_STACK_OK);
        break;

#if defined(__MOT__)
        case ENABLE_MOT_DEV:
        s_ownedDevList.clear();
        s_motEnableDevList.clear();
        discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);
        {
            changeMOTMode(s_ownedDevList, OIC_MULTIPLE_OWNER_ENABLE, changeMOTModeCB, OC_STACK_OK);
            discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT, s_motEnableDevList, OC_STACK_OK);
            provisionPreconfPin(s_motEnableDevList, MOT_DEFAULT_PRE_CONFIG_PIN, strlen(MOT_DEFAULT_PRE_CONFIG_PIN),preConfigPinCB,OC_STACK_OK);
            selectMOTMethod(s_motEnableDevList, OIC_PRECONFIG_PIN, selectMOTMethodCB, OC_STACK_OK);
        }
        break;

        case START_MOT_CLIENT:

        //setup
        removeAllResFile(MOTCLIENT);
        CommonUtil::waitInSecond(DELAY_LONG);
        copyAllResFile(MOTCLIENT);
        CommonUtil::waitInSecond(DELAY_LONG);

        s_pmMotPs =
        {   clientCppMOTAppOpen, fread, fwrite, fclose, unlink};

        s_pmMotCfg =
        {   OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0,
            OC::QualityOfService::LowQos, &s_pmMotPs};

        OCPlatform::Configure(s_pmMotCfg);

        if (OCSecure::registerInputPinCallback(OnInputPinCB, &callbackHandle) != OC_STACK_OK)
        {
            std::cout << "!!Error - registerInputPinCallback failed." << std::endl;
        }

        if (OCSecure::provisionInit(MOT_DB_FILE_NAME) != OC_STACK_OK)
        {
            std::cout << "PM Init failed" << std::endl;
        }

        std::cout << "MOT Client Setup Succeeded" << std::endl;
        break;

        case DISCOVER_MOT_ENABLED_DEVICE:
        s_motEnableDevList.clear();
        discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT, s_motEnableDevList, OC_STACK_OK);
        break;

        case DO_MULTIPLE_OWNERSHIP_TRANSFER:
        s_motEnableDevList.clear();
        discoverMultipleOwnerEnabledDevices(DISCOVERY_TIMEOUT, s_motEnableDevList, OC_STACK_OK);
        addPreconfigPIN(s_motEnableDevList, MOT_DEFAULT_PRE_CONFIG_PIN, strlen(MOT_DEFAULT_PRE_CONFIG_PIN), OC_STACK_OK);
        doMultipleOwnershipTransfer(s_motEnableDevList, multipleOwnershipTransferCB,OC_STACK_OK);
        break;

        case DISCOVER_MOT_OWNED_DEVICE:
        s_motOwnedDevList.clear();
        discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT,s_motOwnedDevList, OC_STACK_OK);
        break;

        case GET_LED_RESOURCE:
        s_motOwnedDevList.clear();
        discoverMultipleOwnedDevices(DISCOVERY_TIMEOUT,s_motOwnedDevList, OC_STACK_OK);
        getLedResource(s_motOwnedDevList, getCallback, OC_STACK_OK);

        break;
#endif
        default:
        printf("Wrong Input, Please provide Input Again");
        return;

    }
}

int main()
{
    for (;;)
    {
        printMenu();

        if (!scanf("%d", &g_userInput))
        {
            printf("Wrong Input, Please provide Input Again");
        }

        if (!g_userInput)
        {
            break;
        }

        doAction(g_userInput);
    }
}
