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

#include <CloudHelper.h>

const string HOST_ADDRESS = "coap+tcp://127.0.0.1:5683";
string g_authProvider = "";
string g_authCode = "";
string g_groupId = "";
int g_userInput = -1;
int g_apptype = NONE;
string g_devIDcontrolee = "";
string g_authTokenControlee = "";
string g_refreshAuthTokenControlee = "";
string g_devIDcontroller = "";
string g_authTokenController = "";
string g_refreshAuthTokenController = "";
OCAccountManager::Ptr g_accountMgrControlee = nullptr;
OCAccountManager::Ptr g_accountMgrController = nullptr;
OCPlatform::OCPresenceHandle g_presenceHandle = nullptr;
std::vector< std::string > deviceIdAddded;
std::vector< std::string > deviceIdRemoving;

/*
 * UI Related
 */
typedef enum
{
    EXIT,
    START_CONTROLEE = 101,
    SIGN_UP_CONTROLEE,
    SIGN_IN_CONTROLEE,
    SIGN_OUT_CONTROLEE,
    REFRESH_CONTROLEE_ACCESS_TOKEN,
    SEARCH_USER_CONTROLEE,
    DELETE_DEVICE,
    CREATE_GROUP,
    JOIN_GROUP,
    LEAVE_GROUP,
    DELETE_GROUP,
    GET_GROUP_INFO,
    GET_GROUP_LIST,
    ADD_DEVICE_TO_GROUP,
    DELETE_DEVICE_FROM_GROUP,

    PUBLISH_RESOURCE_TO_RD,
    DELETE_RESOURCE_FROM_RD,
    TOGGLE_RESOURCE_ATTRIBUTE,

    START_CONTROLLER = 201,
    SIGN_UP_CONTROLLER,
    SIGN_IN_CONTROLLER,
    SIGN_OUT_CONTROLLER,
    FIND_RESOURCE,

    OBSERVE_GROUP,
    CANCEL_OBSERVE_GROUP,
    OBSERVE_INVITATION,
    CANCEL_OBSERVE_INVITATION,
    SEND_INVITATION,
    CANCEL_INVITAION,
    DELETE_INVITATION,

    SUBCRIBE_DEVICE_PRESENCE = 301,
    UNSUBCRIBE_DEVICE_PRESENCE

} menuCloud;

static void printMenu(void)
{
    cout << "====================Controlee===========================" << endl;
    cout << START_CONTROLEE << ". Start Controlee" << endl;
    cout << SIGN_UP_CONTROLEE << ". Sign Up With Controlee" << endl;
    cout << SIGN_IN_CONTROLEE << ". Sign In to Cloud" << endl;
    cout << SIGN_OUT_CONTROLEE << ". Sign Out from Cloud" << endl;
    cout << REFRESH_CONTROLEE_ACCESS_TOKEN << ". Refresh Controlee Access Token" << endl;
    cout << SEARCH_USER_CONTROLEE << ". Search User Information With DEV ID" << endl;
    cout << DELETE_DEVICE << ". Delete Device" << endl;

    cout << CREATE_GROUP << ". Create Group" << endl;
    cout << JOIN_GROUP << ". Join Group" << endl;
    cout << LEAVE_GROUP << ". Leave Group" << endl;
    cout << DELETE_GROUP << ". Delete Group" << endl;
    cout << GET_GROUP_INFO << ". Get Group Info" << endl;
    cout << GET_GROUP_LIST << ". Get Group List" << endl;

    cout << ADD_DEVICE_TO_GROUP << ". Add Device to Group" << endl;
    cout << DELETE_DEVICE_FROM_GROUP << ". Delete Device From Group" << endl;

    cout << PUBLISH_RESOURCE_TO_RD << ". Publish Resource" << endl;
    cout << DELETE_RESOURCE_FROM_RD << ". Delete Resource" << endl;
    cout << TOGGLE_RESOURCE_ATTRIBUTE << ". Toggle Resource Attribute" << endl;

    cout << "====================Controller===========================" << endl;
    cout << START_CONTROLLER << ". Start Controller" << endl;
    cout << SIGN_UP_CONTROLLER << ". Sign Up With Controller" << endl;
    cout << SIGN_IN_CONTROLLER << ". Sign In With Controller" << endl;
    cout << SIGN_OUT_CONTROLLER << ". Sign Out from Cloud" << endl;
    cout << FIND_RESOURCE << ". Find  Resource" << endl;

    cout << OBSERVE_GROUP << ". Observe Group" << endl;
    cout << CANCEL_OBSERVE_GROUP << ". Cancel Observe Group" << endl;
    cout << OBSERVE_INVITATION << ". Observe Invitation" << endl;
    cout << CANCEL_OBSERVE_INVITATION << ". Cancel Observe Invitation" << endl;
    cout << SEND_INVITATION << ". Send Invitation" << endl;
    cout << CANCEL_INVITAION << ". Cancel Invitation" << endl;
    cout << DELETE_INVITATION << ". Delete Invitation" << endl;

    cout << "====================Subscribe Features===========================" << endl;
    cout << SUBCRIBE_DEVICE_PRESENCE << ". Subscribe Device Presence" << endl;
    cout << UNSUBCRIBE_DEVICE_PRESENCE << ". Unsubscribe Device Presence" << endl;

    cout << EXIT << ". Exit" << endl;
    printf("Enter Input: ");
}

int checkAppStatus(int expectedType)
{
    if (expectedType != g_apptype && expectedType == CONTROLEE)
    {
        cout << "App is not initialized as Controlee" << endl;
        return 1;
    }
    if (expectedType != g_apptype && expectedType == CONTROLLER)
    {
        cout << "App is not initialized as Controller" << endl;
        return 1;
    }
    return 0;
}

void doAction(int userInput)
{
    switch (userInput)
    {
        case START_CONTROLEE:
        {
            OCPersistentStorage psControlee
            { controleeOpen, fread, fwrite, fclose, unlink };

            PlatformConfig cfg
            { ServiceType::InProc, ModeType::Both, "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                    0, // Uses randomly available port
                    QualityOfService::LowQos, &psControlee };

            OCPlatform::Configure(cfg);
            g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
                    CT_ADAPTER_TCP);
        }
            break;
        case SIGN_UP_CONTROLEE:
            cout << "Provide Auth Provide (github): ";
            cin >> g_authProvider;
            cout << "Provide Auth Code: ";
            cin >> g_authCode;
            signUp(g_accountMgrControlee, g_authProvider, g_authCode, handleLoginoutCB,
                    g_devIDcontrolee, g_authTokenControlee, g_refreshAuthTokenControlee);
            break;
        case SIGN_IN_CONTROLEE:
            signIn(g_accountMgrControlee, g_devIDcontrolee, g_authTokenControlee, handleLoginoutCB);
            break;
        case SIGN_OUT_CONTROLEE:
            signOut(g_accountMgrControlee, handleLoginoutCB);
            break;
        case REFRESH_CONTROLEE_ACCESS_TOKEN:
            refreshAccessToken(g_accountMgrControlee, g_devIDcontrolee, g_refreshAuthTokenControlee,
                    onRefreshTokenCB);
            break;
        case SEARCH_USER_CONTROLEE:
            searchUser(g_accountMgrControlee, g_devIDcontrolee, onSearchUser);
            break;
        case DELETE_DEVICE:
            cout << "Enter Device ID : ";
            cin >> g_devIDcontrolee;
            deleteDevice(g_accountMgrControlee, g_devIDcontrolee, onDeleteDevice);
            break;

        case CREATE_GROUP:
            createGroup(g_accountMgrControlee, AclGroupType::PUBLIC, onCreateGroup, g_groupId);
            break;

        case JOIN_GROUP:
            cout << "Enter Group ID : ";
            cin >> g_groupId;
            joinGroup(g_accountMgrControlee, g_groupId, onJoinGroup);
            break;

        case GET_GROUP_INFO:
            cout << "Enter Group ID : ";
            cin >> g_groupId;
            getGroupInfo(g_accountMgrControlee, g_groupId, onGetGroupInfo);
            break;

        case LEAVE_GROUP:
            cout << "Enter Group ID : ";
            cin >> g_groupId;
            leaveGroup(g_accountMgrControlee, g_groupId, onLeaveGroup);
            break;

        case DELETE_GROUP:
            cout << "Enter Group ID : ";
            cin >> g_groupId;
            deleteGroup(g_accountMgrControlee, g_groupId, onDeleteGroup);
            break;

        case GET_GROUP_LIST:
            getGroupList(g_accountMgrControlee, onGetGroupList);
            break;

        case ADD_DEVICE_TO_GROUP:
            cout << "Enter Device ID : ";
            cin >> g_devIDcontrolee;
            cout << "Enter Group ID : ";
            cin >> g_groupId;
            deviceIdAddded.push_back(g_devIDcontrolee);
            addDeviceToGroup(g_accountMgrControlee, g_groupId, deviceIdAddded, onAddDeviceToGroup);
            break;
        case DELETE_DEVICE_FROM_GROUP:
            cout << "Enter Device ID : ";
            cin >> g_devIDcontrolee;
            cout << "Enter Group ID : ";
            cin >> g_groupId;
            deviceIdRemoving.push_back(g_devIDcontrolee);
            deleteDeviceFromGroup(g_accountMgrControlee, g_groupId, deviceIdRemoving,
                    onDeleteDeviceFromGroup);
            break;

        case PUBLISH_RESOURCE_TO_RD:
        {
            BinarySwitchResource binarySwitch(SWITCH_RES_URI,
            { SWITCH_RES_TYPE },
            { DEFAULT_INTERFACE });
            string uri = binarySwitch.getResourceUri();
            string rt = binarySwitch.getResourceType()[0];
            string itf = binarySwitch.getInterfaces()[0];
            int result = OCPlatform::registerResource(binarySwitch.m_handle, uri, rt, itf,
                    std::bind(&BinarySwitchResource::entityHandler, &binarySwitch,
                            std::placeholders::_1), OC_DISCOVERABLE);
            if (result != OC_STACK_OK)
            {
                cout << "Resource registration was unsuccessful" << endl;
            }

            ResourceHandles resourceHandles;
            OCDeviceInfo devInfoAirConditioner;
            OCStringLL deviceType;

            deviceType.value = DEVICE_TYPE_VALUE;
            deviceType.next = NULL;
            devInfoAirConditioner.deviceName = RES_DEVICE_NAME;
            devInfoAirConditioner.types = &deviceType;
            devInfoAirConditioner.specVersion = NULL;
            devInfoAirConditioner.dataModelVersions = NULL;

            OCPlatform::registerDeviceInfo(devInfoAirConditioner);

            resourceHandles.push_back(binarySwitch.m_handle);

            publishResourceToRD(HOST_ADDRESS, OCConnectivityType::CT_ADAPTER_TCP, onPublish);

        }

            break;
        case DELETE_RESOURCE_FROM_RD:
            cout << "Feature Not Implemented" << endl;
            break;
        case TOGGLE_RESOURCE_ATTRIBUTE:
            cout << "Feature Not Implemented" << endl;
            break;
        case START_CONTROLLER:
        {
            OCPersistentStorage psController
            { controllerOpen, fread, fwrite, fclose, unlink };

            PlatformConfig cfgController
            { ServiceType::InProc, ModeType::Both, "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                    0, // Uses randomly available port
                    QualityOfService::LowQos, &psController };

            OCPlatform::Configure(cfgController);
            g_accountMgrController = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
                    CT_ADAPTER_TCP);
        }

            break;
        case SIGN_UP_CONTROLLER:
            cout << "Provide Auth Provide (github): ";
            cin >> g_authProvider;
            cout << "Provide Auth Code: ";
            cin >> g_authCode;
            signUp(g_accountMgrController, g_authProvider, g_authCode, handleLoginoutCB,
                    g_devIDcontroller, g_authTokenController, g_refreshAuthTokenController);

            cout << "Collected Data for Debug:" << endl;
            cout << g_devIDcontroller << " " << g_authTokenController << endl;

            break;
        case SIGN_IN_CONTROLLER:
            signIn(g_accountMgrController, g_devIDcontroller, g_authTokenController,
                    handleLoginoutCB);
            break;
        case SIGN_OUT_CONTROLLER:
            signOut(g_accountMgrController, handleLoginoutCB);
            break;
        case FIND_RESOURCE:

            findResource(HOST_ADDRESS, QUERY_FOR_SWITCH,
                    static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4), foundDevice);
            break;
        case OBSERVE_GROUP:
            cout << "Feature Not Implemented" << endl;
            break;
        case CANCEL_OBSERVE_GROUP:
            cout << "Feature Not Implemented" << endl;
            break;
        case OBSERVE_INVITATION:
            cout << "Feature Not Implemented" << endl;
            break;
        case CANCEL_OBSERVE_INVITATION:
            cout << "Feature Not Implemented" << endl;
            break;
        case SEND_INVITATION:
            cout << "Feature Not Implemented" << endl;
            break;
        case CANCEL_INVITAION:
            cout << "Feature Not Implemented" << endl;
            break;
        case DELETE_INVITATION:
            cout << "Feature Not Implemented" << endl;
            break;
        case SUBCRIBE_DEVICE_PRESENCE:
            cout << "Feature Not Implemented" << endl;
            break;
        case UNSUBCRIBE_DEVICE_PRESENCE:
            cout << "Feature Not Implemented" << endl;
            break;
        default:
            cout << "Wrong Input, Please provide Input Again" << endl;
            return;
    }

}

int main()
{
    for (;;)
    {
        printMenu();
        cin >> g_userInput;
        if (!g_userInput)
        {
            break;
        }
        doAction(g_userInput);
    }
}
