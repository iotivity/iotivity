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

#include "../include/PMCsdkAppHelper.h"
#include "../include/PMCsdkAppDpHelper.h"

static int g_userInput = 0;
OCProvisionDev_t *g_OwnList, *g_unownList;
OicSecAcl_t *g_Acl, *g_Acl1, *g_Acl2;

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

    printf("====================SERVERS===========================\n");
    printf("%d. Start Justworks Server [1]\n", START_JUSTWORKS_SERVER);
    printf("%d. Start Justworks Server Without DP Support\n", START_JUSTWORKS_SERVER2);
    printf("%d. Start RandomPin Server\n\n", START_RANDOMPIN_SERVER);

    printf("====================PM CLIENT=========================\n");
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
            initProvisionClient();
            break;

        case JUSTWORKS_OWNERSHIP_TRANSFER:
            g_unownList = NULL;
            discoverUnownedDevices(DISCOVERY_TIMEOUT, &g_unownList, OC_STACK_OK);
            doOwnerShipTransfer((void*) g_ctx, &g_unownList, ownershipTransferCB, OC_STACK_OK);
            break;

        case RANDOMPIN_OWNERSHIP_TRANSFER:
            g_unownList = NULL;
            discoverUnownedDevices(DISCOVERY_TIMEOUT, &g_unownList, OC_STACK_OK);
            doOwnerShipTransfer((void*) g_ctx, &g_unownList, ownershipTransferCB, OC_STACK_OK);
            break;

        case PROVISION_ACL:
            g_OwnList= NULL;
            discoverOwnedDevices(DISCOVERY_TIMEOUT, &g_OwnList, OC_STACK_OK);
            g_Acl = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &g_OwnList);
            provisionACL((void*) g_ctx, g_OwnList, g_Acl, provisionAclCB, OC_STACK_OK);
            break;

        case PROVISION_CREDENTIAL:
            g_OwnList= NULL;
            discoverOwnedDevices(DISCOVERY_TIMEOUT, &g_OwnList, OC_STACK_OK);
            {
                OCProvisionDev_t *device1 = g_OwnList;
                OCProvisionDev_t *device2 = g_OwnList->next;
                OicSecCredType_t type = (OicSecCredType_t) SYMMETRIC_PAIR_WISE_KEY;
                size_t keySize = OWNER_PSK_LENGTH_128;

                provisionCredentials((void*) ctxProvCreadential, type, keySize, device1, device2,
                        provisionCredCB, OC_STACK_INVALID_PARAM);
            }
            break;

        case PROVISIONING_PAIRWISE_DEVICES:
            g_OwnList= NULL;
            discoverOwnedDevices(DISCOVERY_TIMEOUT, &g_OwnList, OC_STACK_OK);
            {
                OCProvisionDev_t *device1 = g_OwnList;
                OCProvisionDev_t *device2 = g_OwnList->next;
                g_Acl1 = createAcl(DEVICE_INDEX_ONE, FULL_PERMISSION, &g_OwnList);
                g_Acl2 = createAcl(DEVICE_INDEX_TWO, FULL_PERMISSION, &g_OwnList);
                OicSecCredType_t type = (OicSecCredType_t) SYMMETRIC_PAIR_WISE_KEY;
                size_t keySize = OWNER_PSK_LENGTH_128;

                provisionPairwiseDevices((void*) ctxProvPairwise, type, keySize, device1, g_Acl1,
                        device2, g_Acl2, provisionPairwiseCB, OC_STACK_OK);
            }
            break;

        case PROVISION_DIRECT_PAIRING:
            g_OwnList= NULL;
            discoverOwnedDevices(DISCOVERY_TIMEOUT, &g_OwnList, OC_STACK_OK);
            {
                OCProvisionDev_t *device1 = g_OwnList;
                OicSecPconf_t pconf;
                memset(&pconf, 0, sizeof(OicSecPconf_t));
                pconf.edp = true;

                // set default supported PRM types
                pconf.prmLen = sizeof(SUPPORTED_PRMS) / sizeof(OicSecPrm_t);
                pconf.prm = (OicSecPrm_t *) OICCalloc(pconf.prmLen, sizeof(OicSecPrm_t));

                if (pconf.prm)
                {
                    for (size_t i = 0; i < pconf.prmLen; i++)
                    {
                        pconf.prm[i] = SUPPORTED_PRMS[i];
                    }
                }

                memcpy(pconf.pin.val, DEFAULT_DP_PROVSIONING_PIN, DP_PIN_LENGTH);

                // set default pdacl
                pconf.pdacls = createPdAcl(FULL_PERMISSION);

                proivisioningDirectPairing((void*) ctxProvDirectPairing, device1, &pconf,
                        provisionDPCB, OC_STACK_OK);
            }
            break;

        case GET_LINKED_DEVICES:
            g_OwnList= NULL;
            discoverOwnedDevices(DISCOVERY_TIMEOUT, &g_OwnList, OC_STACK_OK);
            {
                OCProvisionDev_t *device1 = g_OwnList;
                OCUuidList_t* uuidList = NULL;
                size_t numOfDevices = 0;
                getLinkedStatus(&device1->doxm->deviceID, &uuidList, &numOfDevices, OC_STACK_OK);

            }
            break;

        case UNLINK_DEVICE:
            g_OwnList= NULL;
            discoverOwnedDevices(DISCOVERY_TIMEOUT, &g_OwnList, OC_STACK_OK);
            {
                OCProvisionDev_t *device1 = g_OwnList;
                OCProvisionDev_t *device2 = g_OwnList->next;
                unlinkDevices((void*) ctxUnlinkDevice, device1, device2, unlinkDevicesCB,
                        OC_STACK_OK);
            }
            break;

        case REMOVE_DEVICES:
            g_OwnList= NULL;
            discoverOwnedDevices(DISCOVERY_TIMEOUT, &g_OwnList, OC_STACK_OK);
            {
                OCProvisionDev_t *device1 = g_OwnList;
                removeDevice((void*) ctxRemoveDevice, DISCOVERY_TIMEOUT, device1, removeDeviceCB,
                        OC_STACK_OK);

            }
            break;
        case DO_DIRECT_PAIRING:
            OCPersistentStorage ps;
            ps=
            {   clientFopenDP, fread, fwrite, fclose, unlink};
            OCRegisterPersistentStorageHandler(&ps);

            OCInit(NULL, 0, OC_CLIENT_SERVER);
            discoverDirectPairingDevices(DP_DISCOVERY_TIMEOUT);
            doDirectPairing(NULL, DP_DEV_INSTANCE_ONE, DP_PRE_CONFIGURED, DP_DEFAULT_PIN,
                    pairingReqCBF, OC_STACK_OK, OC_STACK_OK);
            break;
         case GET_DIRECT_PAIRING_DEV:
            getDirectPairedDevices();
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
