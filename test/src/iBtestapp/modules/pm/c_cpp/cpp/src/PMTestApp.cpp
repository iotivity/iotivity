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
#include "../include/PMCppAppHelper.h"
#include "../include/PMCppDPAppHelper.h"

static int g_userInput = 0;
static DeviceList_t s_unOwnedDevList, s_ownedDevList;
static OCPersistentStorage s_pmPs;
static OCPersistentStorage s_dpPs;
static PlatformConfig s_pmCfg;
static PlatformConfig s_dpCfg;
static OTMCallbackData_t s_justWorksCBDataDp, s_pinBasedCBDataDp;

/*
 * UI Related
 */
typedef enum
{
    EXIT,
    START_JUSTWORKS_SERVER = 11,
    START_JUSTWORKS_SERVER2,
    START_RANDOMPIN_SERVER,

    START_PM_CLIENT = 20,
    JUSTWORKS_OWNERSHIP_TRANSFER,
    RANDOMPIN_OWNERSHIP_TRANSFER,
    RANDOMPIN_OWNERSHIP_TRANSFER_WITH_WRONG_PIN,
    PROVISION_ACL,
    PROVISION_CREDENTIAL,
    PROVISIONING_PAIRWISE_DEVICES,
    PROVISION_DIRECT_PAIRING,
    GET_LINKED_DEVICES,
    UNLINK_DEVICE,
    REMOVE_DEVICES,

    DO_DIRECT_PAIRING = 31,
    GET_DIRECT_PAIRING_DEV

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

    printf("\n====================PM CLIENT=========================\n");
    printf("%d. Start Provisioning Client\n", START_PM_CLIENT);
    printf("%d. Justworks Ownership Transfer\n", JUSTWORKS_OWNERSHIP_TRANSFER);
    printf("%d. RandomPin Ownership Transfer\n", RANDOMPIN_OWNERSHIP_TRANSFER);
    printf("%d. Provision Access Control List (ACL)\n", PROVISION_ACL);
    printf("%d. Provision Credential\n", PROVISION_CREDENTIAL);
    printf("%d. Provision Pairwise Devices\n", PROVISIONING_PAIRWISE_DEVICES);
    printf("%d. Provision Direct Pairing\n", PROVISION_DIRECT_PAIRING);
    printf("%d. Get Linked Devices\n", GET_LINKED_DEVICES);
    printf("%d. Unlink Devices\n", UNLINK_DEVICE);
    printf("%d. Remove Devices\n\n", REMOVE_DEVICES);

    printf("====================DP CLIENT=========================\n");
    printf("%d. Do Direct Pairing\n", DO_DIRECT_PAIRING);
    printf("%d. Get Direct Pairing Devices\n\n", GET_DIRECT_PAIRING_DEV);
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

        case START_PM_CLIENT:

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

            std::cout << "setup succeeded" << std::endl;

            break;

        case JUSTWORKS_OWNERSHIP_TRANSFER:
            s_unOwnedDevList.clear();

            discoverUnownedDevices(DISCOVERY_TIMEOUT, s_unOwnedDevList, OC_STACK_OK);

            s_justWorksCBDataDp.loadSecretCB = LoadSecretJustWorksCallback;
            s_justWorksCBDataDp.createSecureSessionCB = CreateSecureSessionJustWorksCallback;
            s_justWorksCBDataDp.createSelectOxmPayloadCB = CreateJustWorksSelectOxmPayload;
            s_justWorksCBDataDp.createOwnerTransferPayloadCB = CreateJustWorksOwnerTransferPayload;

            OCSecure::setOwnerTransferCallbackData(OIC_JUST_WORKS, &s_justWorksCBDataDp, NULL);

            doOwnershipTransfer(s_unOwnedDevList, ownershipTransferCB, OC_STACK_OK);

            break;

            case RANDOMPIN_OWNERSHIP_TRANSFER:
            s_unOwnedDevList.clear();
            discoverUnownedDevices(DISCOVERY_TIMEOUT, s_unOwnedDevList, OC_STACK_OK);

            s_pinBasedCBDataDp.loadSecretCB = InputPinCodeCallback;
            s_pinBasedCBDataDp.createSecureSessionCB =
            CreateSecureSessionRandomPinCallback;
            s_pinBasedCBDataDp.createSelectOxmPayloadCB =
            CreatePinBasedSelectOxmPayload;
            s_pinBasedCBDataDp.createOwnerTransferPayloadCB =
            CreatePinBasedOwnerTransferPayload;
            OCSecure::setOwnerTransferCallbackData(OIC_RANDOM_DEVICE_PIN,
                    &s_pinBasedCBDataDp, InputPinCB);
            SetInputPinCB(InputPinCB);
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

        case PROVISION_DIRECT_PAIRING:
            s_ownedDevList.clear();
            discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);

            OicSecPconf_t p_conf;
            provisionDirectPairing(s_ownedDevList, p_conf, NULL, OC_STACK_OK);
            break;

        case GET_LINKED_DEVICES:
            s_ownedDevList.clear();
            discoverOwnedDevices(DISCOVERY_TIMEOUT, s_ownedDevList, OC_STACK_OK);
            {
                UuidList_t uuidList1;
                getLinkedDevices(s_ownedDevList, uuidList1, OC_STACK_OK);
            }
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

        case DO_DIRECT_PAIRING:
            s_dpPs =
            {   clientDpCppOpen, fread, fwrite, fclose, unlink};

            s_dpCfg =
            {   OC::ServiceType::InProc, OC::ModeType::Both, "0.0.0.0", 0,
                OC::QualityOfService::LowQos, &s_dpPs};

            OCPlatform::Configure(s_dpCfg);

            findDirectPairingDevices(DP_DISCOVERY_TIMEOUT,
                    findCallback, OC_STACK_OK);

            doDirectPairing(DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_PRECONFIG_PIN, resultCallback, OC_STACK_OK);
            break;

        case GET_DIRECT_PAIRING_DEV:
            getDirectPairedDevices(pairedDevListCB, OC_STACK_OK);
            break;
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
        scanf("%d", &g_userInput);
        if (!g_userInput)
        {
            break;
        }
        doAction(g_userInput);
    }
}
