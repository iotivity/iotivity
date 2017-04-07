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
 *      http://www.apache.org/licenses/LICENSE-2.0
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

const string HOST_ADDRESS = "coap+tcp://192.168.43.158:5683";
string g_authProvider = "";
string g_authCode = "";
string g_groupId = "";
int g_userInput = -1;
int g_apptype = NONE;
bool g_value = false;
bool g_controleeOn = true;
bool g_controllerOn = true;
string g_devIDcontrolee = "";
string g_cmd = "";
string g_AccTokcontrolee = "";
string g_authTokenControlee = "";
string g_refreshAuthTokenControlee = "";
string g_devIDcontroller = "";
string g_authTokenController = "";
string g_refreshAuthTokenController = "";
OCAccountManager::Ptr g_accountMgrControlee = nullptr;
OCAccountManager::Ptr g_accountMgrController = nullptr;
OCPlatform::OCPresenceHandle g_presenceHandle = nullptr;
OC::OCResource::Ptr g_mqBrokerResource = nullptr;
OC::OCResource::Ptr g_mqSelectedTopicResource = nullptr;
QualityOfService g_qos = QualityOfService::LowQos;
std::vector< std::string > g_deviceIdAddded;
std::vector< std::string > g_deviceIdRemoving;
QueryParamsMap g_Query;
string g_Key;
string g_Value;
string g_AcceptInvitaition;
OCRepresentation g_PropertyValue;
vector< string > g_Values;
string g_URI;
string g_TopicType;
bool g_IsControlleeSignUp = false;
bool g_IsControllerSignUp = false;

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
    SEARCH_USER_UUID,
    SEARCH_USER_PHONE,
    SEARCH_USER_EMAIL,
    DELETE_DEVICE,
    CREATE_GROUP,
    CREATE_GROUP_WITH_QUERY,
    DELETE_GROUP,
    GET_GROUP_INFO_ALL,
    GET_GROUP_INFO,

    OBSERVE_GROUP,
    CANCEL_OBSERVE_GROUP,
    OBSERVE_INVITATION,
    CANCEL_OBSERVE_INVITATION,
    SEND_INVITATION,
    CANCEL_INVITATION,
    DELETE_INVITATION,

    PUBLISH_RESOURCE_TO_RD,
    TOGGLE_RESOURCE_ATTRIBUTE,

    START_CONTROLLER = 201,
    SIGN_UP_CONTROLLER,
    SIGN_IN_CONTROLLER,
    SIGN_OUT_CONTROLLER,
    FIND_RESOURCE_WITH_QUERY,
    FIND_RESOURCE,
    TOGGLE_RESOURCE_CONTROLLER,
    ADD_PROPERTY_VALUE,
    DELETE_PROPERTY_VALUE,
    UPDATE_PROPERTY_VALUE,
    DELETE_RESOURCE,
    OBSERVE_RESOURCE,
    CANCEL_OBSERVE_RESOURCE,

    DISCOVER_ALL_TOPIC = 301,
    DISCOVER_TOPIC_TYPE,
    SELECT_TOPIC_INDEX,
    PUBLISH_TOPIC_DATA,
    CREATE_TOPIC,
    CREATE_TYPED_TOPIC,
    SELECT_TOPIC,
    SUBSCRIBE_TOPIC,
    UNSUBSCRIBE_TOPIC

} menuCloud;

static void printMenu(void)
{
    cout << "====================Controlee===========================" << endl;
    cout << START_CONTROLEE << ". Start Controlee" << endl;
    cout << SIGN_UP_CONTROLEE << ". Sign Up With Controlee" << endl;
    cout << SIGN_IN_CONTROLEE << ". Sign In to Cloud" << endl;
    cout << SIGN_OUT_CONTROLEE << ". Sign Out from Cloud" << endl;
    cout << SEARCH_USER_UUID << ". Search User Information With DEV UUID" << endl;
    cout << SEARCH_USER_PHONE << ". Search User Information With Phone number" << endl;
    cout << SEARCH_USER_EMAIL << ". Search User Information With Email Address" << endl;
    cout << DELETE_DEVICE << ". Delete Device" << endl;

    cout << CREATE_GROUP << ". Create Group" << endl;
    cout << CREATE_GROUP_WITH_QUERY << ". Create Group with query" << endl;

    cout << DELETE_GROUP << ". Delete Group" << endl;
    cout << GET_GROUP_INFO_ALL << ". Get Group Info ALL" << endl;
    cout << GET_GROUP_INFO << ". Get Group Info" << endl;

    cout << OBSERVE_GROUP << ". Observe Group" << endl;
    cout << CANCEL_OBSERVE_GROUP << ". Cancel Observe Group" << endl;
    cout << OBSERVE_INVITATION << ". Observe Invitation" << endl;
    cout << CANCEL_OBSERVE_INVITATION << ". Cancel Observe Invitation" << endl;
    cout << SEND_INVITATION << ". Send Invitation" << endl;
    cout << CANCEL_INVITATION << ". Cancel Invitation" << endl;
    cout << DELETE_INVITATION << ". Delete Invitation" << endl;

    cout << PUBLISH_RESOURCE_TO_RD << ". Publish Resource" << endl;
    cout << TOGGLE_RESOURCE_ATTRIBUTE << ". Toggle Resource Attribute" << endl; //feature not implemented

    cout << "====================Controller===========================" << endl;
    cout << START_CONTROLLER << ". Start Controller" << endl;
    cout << SIGN_UP_CONTROLLER << ". Sign Up With Controller" << endl;
    cout << SIGN_IN_CONTROLLER << ". Sign In With Controller" << endl;
    cout << SIGN_OUT_CONTROLLER << ". Sign Out from Cloud" << endl;
    cout << FIND_RESOURCE_WITH_QUERY << ". Find  Resource with query" << endl;
    cout << FIND_RESOURCE << ". Find  Resource without query" << endl;
    cout << TOGGLE_RESOURCE_CONTROLLER << ". Toggle Resource Attribute from controller" << endl;
    cout << ADD_PROPERTY_VALUE << ". Add property value to group" << endl;
    cout << DELETE_PROPERTY_VALUE << ". Delete property value to group " << endl;
    cout << UPDATE_PROPERTY_VALUE << ". Update property value to group " << endl;
    cout << DELETE_RESOURCE << ". Send DELETE Request to resource" << endl;
    cout << OBSERVE_RESOURCE << ". Send OBSERVE Request to resource" << endl;
    cout << CANCEL_OBSERVE_RESOURCE << ". OBSERVE Cancel Request to resource" << endl;

    cout << "====================Subscribe Features===========================" << endl;
    cout << DISCOVER_ALL_TOPIC << ". Discover all topics" << endl;
    cout << DISCOVER_TOPIC_TYPE << ". Discover type based topics" << endl;
    cout << SELECT_TOPIC_INDEX << ". Select topic index for publishing data" << endl;
    cout << PUBLISH_TOPIC_DATA << ". Publish data to selected topic" << endl;
    cout << CREATE_TOPIC << ". create topic" << endl;
    cout << CREATE_TYPED_TOPIC << ". create type based topic" << endl;
    cout << SELECT_TOPIC << ". Select topic for subscribing or unsubscribing" << endl;
    cout << SUBSCRIBE_TOPIC << ". Subscribe to selected topic" << endl;
    cout << UNSUBSCRIBE_TOPIC << ". Unsubscribe to selected topic" << endl;

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
    OCRepresentation rep;
    if (g_IsControlleeSignUp || g_IsControllerSignUp || userInput == START_CONTROLEE
            || userInput == SIGN_UP_CONTROLEE || userInput == SIGN_IN_CONTROLEE
            || userInput == START_CONTROLLER || userInput == SIGN_UP_CONTROLLER
            || userInput == SIGN_IN_CONTROLLER)
    {
        switch (userInput)
        {
            case START_CONTROLEE:
                OCPersistentStorage psControlee
                { controleeOpen, fread, fwrite, fclose, unlink };
                PlatformConfig cfg
                { ServiceType::InProc, ModeType::Both, "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                        0, // Uses randomly available port
                        QualityOfService::LowQos, &psControlee };

                OCPlatform::Configure(cfg);
                g_accountMgrControlee = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
                        CT_ADAPTER_TCP);
                break;
            case SIGN_UP_CONTROLEE:
                CloudCommonUtil::signUp(g_accountMgrControlee);
                g_IsControlleeSignUp = true;
                break;
            case SIGN_IN_CONTROLEE:
                CloudCommonUtil::signIn(g_accountMgrControlee);
                g_IsControlleeSignUp = true;
                break;
            case SIGN_OUT_CONTROLEE:
                CloudCommonUtil::signOut(g_accountMgrControlee);
                g_IsControlleeSignUp = false;
                break;
            case SEARCH_USER_UUID:
                cout << "Enter UserUUID to search";
                cin >> g_cmd;
                g_Query["uid"] = g_cmd;
                searchUser(g_accountMgrControlee, g_Query, onSearchUser);
                break;
            case SEARCH_USER_PHONE:
                cout << "Enter phone number to search";
                cin >> g_cmd;
                g_Query["phone"] = g_cmd;
                searchUser(g_accountMgrControlee, g_Query, onSearchUser);
                break;
            case SEARCH_USER_EMAIL:
                cout << "Enter Email ID to search";
                cin >> g_cmd;
                g_Query["email"] = g_cmd;
                searchUser(g_accountMgrControlee, g_Query, onSearchUser);
                break;
            case DELETE_DEVICE:
                cout << "Enter Device ID : ";
                cin >> g_devIDcontrolee;
                cout << "Enter AccessToken : ";
                cin >> g_AccTokcontrolee;
                deleteDevice(g_accountMgrControlee, g_devIDcontrolee, g_AccTokcontrolee,
                        onDeleteDevice);
                break;
            case CREATE_GROUP:
                //query.insert(pair< string, string >(NULL, NULL));
                createGroup(g_accountMgrControlee, g_Query, onCreateGroup);
                break;
            case CREATE_GROUP_WITH_QUERY:
                cout << "query key(ex: gname/parent): ";
                cin >> g_Key;
                cout << "query value: ";
                cin >> g_Value;
                g_Query.insert(pair< string, string >(g_Key, g_Value));
                createGroup(g_accountMgrControlee, g_Query, onCreateGroup);
                break;
            case GET_GROUP_INFO_ALL:
                g_groupId = "";
                getGroupInfo(g_accountMgrControlee, g_groupId, onGetGroupInfo);
                break;
            case GET_GROUP_INFO:
                cout << "Enter Group ID : ";
                cin >> g_groupId;
                getGroupInfo(g_accountMgrControlee, g_groupId, onGetGroupInfo);
                break;
            case DELETE_GROUP:
                cout << "Enter Group ID : ";
                cin >> g_groupId;
                deleteGroup(g_accountMgrControlee, g_groupId, onDeleteGroup);
                break;
            case OBSERVE_GROUP:
                cout << "Enter Group ID : ";
                cin >> g_groupId;
                observeGroup(g_accountMgrControlee, onObserveGroup);
                break;
            case CANCEL_OBSERVE_GROUP:
                cout << "Enter Group ID : ";
                cin >> g_groupId;
                cancelObserveGroup(g_accountMgrControlee);
                break;
            case OBSERVE_INVITATION:
                observeInvitation(g_accountMgrControlee, onObserveInvitation);
                break;
            case CANCEL_OBSERVE_INVITATION:
                cancelObserveInvitation(g_accountMgrControlee);
                break;
            case SEND_INVITATION:
                cout << "Enter Group ID to invite : ";
                cin >> g_groupId;
                cout << "Enter UUID to invite : ";
                cin >> g_devIDcontrolee;
                sendInvitation(g_accountMgrControlee, g_groupId, g_devIDcontrolee,
                        onSendInvitation);
                break;
            case CANCEL_INVITATION:
                cout << "Enter Group ID to cancel invitation : ";
                cin >> g_groupId;
                cout << "Enter Device ID to cancel invitation : ";
                cin >> g_devIDcontrolee;
                cancelInvitation(g_accountMgrControlee, g_groupId, g_devIDcontrolee,
                        onCancelInvitation);
                break;
            case DELETE_INVITATION:
                cout << "Enter Group ID to reply and then delete invitation: ";
                cin >> g_groupId;
                cout << "accept to invitation? (Enter 1 for accept invitation)";
                cin >> g_AcceptInvitaition;
                replyToInvitation(g_accountMgrControlee, g_groupId, g_AcceptInvitaition,
                        onDeleteInvitation);
                break;
            case PUBLISH_RESOURCE_TO_RD:
                BinarySwitchResource binarySwitch(SWITCH_RES_URI,
                { SWITCH_RES_TYPE },
                { DEFAULT_INTERFACE });
                g_URI = binarySwitch.getResourceUri();
                string rt = binarySwitch.getResourceType()[0];
                string itf = binarySwitch.getInterfaces()[0];
                int result = OCPlatform::registerResource(binarySwitch.m_handle, g_URI, rt, itf,
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
                publishResourceToRD(HOST_ADDRESS, OCConnectivityType::CT_ADAPTER_TCP,
                        resourceHandles, &onPublish);
                break;
            case TOGGLE_RESOURCE_ATTRIBUTE:
                OCRepresentation rep;
                if (g_controleeOn)
                {
                    rep.setValue(string("value"), false);
                    BinarySwitchResource binarySwitch("/power/0",
                    { "oic.r.switch.binary" },
                    { DEFAULT_INTERFACE });
                    binarySwitch.setBinarySwitchRepresentation(rep);
                }
                else
                {
                    rep.setValue(string("value"), true);
                    BinarySwitchResource binarySwitch("/power/0",
                    { "oic.r.switch.binary" },
                    { DEFAULT_INTERFACE });
                    binarySwitch.setBinarySwitchRepresentation(rep);
                }
                break;
            case START_CONTROLLER:
                OCPersistentStorage psController
                { controllerOpen, fread, fwrite, fclose, unlink };

                PlatformConfig cfgController
                { ServiceType::InProc, ModeType::Both, "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
                        0, // Uses randomly available port
                        QualityOfService::LowQos, &psController };

                OCPlatform::Configure(cfgController);
                g_accountMgrController = OCPlatform::constructAccountManagerObject(HOST_ADDRESS,
                        CT_ADAPTER_TCP);
                break;
            case SIGN_UP_CONTROLLER:
                CloudCommonUtil::signUp(g_accountMgrController);
                g_IsControllerSignUp = true;
                break;
            case SIGN_IN_CONTROLLER:
                CloudCommonUtil::signIn(g_accountMgrController);
                g_IsControllerSignUp = true;
                break;
            case SIGN_OUT_CONTROLLER:
                CloudCommonUtil::signOut(g_accountMgrController);
                g_IsControllerSignUp = false;
                break;
            case FIND_RESOURCE_WITH_QUERY:
                g_URI = QUERY_FOR_SWITCH;
                cout << "insert Query : ";
                cin >> g_cmd;
                g_URI += "?";
                g_URI += g_cmd;
                findResource(HOST_ADDRESS, g_URI,
                        static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4),
                        foundDevice);
                break;
            case FIND_RESOURCE:
                findResource(HOST_ADDRESS, QUERY_FOR_SWITCH,
                        static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4),
                        foundDevice);
                break;
            case TOGGLE_RESOURCE_CONTROLLER:
                if (g_controllerOn)
                {
                    turnOnOffSwitch(false);
                }
                else
                {
                    turnOnOffSwitch(true);
                }
                break;
            case UPDATE_PROPERTY_VALUE:
                cout << "Enter group ID: ";
                cin >> g_groupId;
                cout << "property(ex: members/devices): ";
                cin >> g_Key;
                cout << "\tvalue: ";
                cin >> g_Value;
                g_Values.push_back(g_Value);
                g_PropertyValue.setValue< vector< string > >(g_Key, g_Values);
                updatePropertyValueOnGroup(g_accountMgrController, g_groupId, g_PropertyValue,
                        &onPost);
                break;
            case ADD_PROPERTY_VALUE:
                cout << "Enter group ID: ";
                cin >> g_groupId;
                cout << "property(ex: members/devices): ";
                cin >> g_Key;
                cout << "\tvalue: ";
                cin >> g_Value;
                g_Values.push_back(g_Value);
                g_PropertyValue.setValue< vector< string > >(g_Key, g_Values);
                addPropertyValueToGroup(g_accountMgrController, g_groupId, g_PropertyValue,
                        &onPost);
                break;
            case DELETE_PROPERTY_VALUE:
                cout << "Enter group ID: ";
                cin >> g_groupId;
                cout << "property(ex: members/devices): ";
                cin >> g_Key;
                cout << "\tvalue: ";
                cin >> g_Value;
                g_Values.push_back(g_Value);
                g_PropertyValue.setValue< vector< string > >(g_Key, g_Values);
                deletePropertyValueFromGroup(g_accountMgrController, g_groupId, g_PropertyValue,
                        &onPost);
                break;
            case DELETE_RESOURCE:
                if (getFoundResource())
                {
                    getFoundResource()->deleteResource(onDelete, g_qos);
                }
                else
                {
                    cout << "No Resource found yet" << endl;
                }
                break;
            case OBSERVE_RESOURCE:
                if (getFoundResource())
                {
                    getFoundResource()->observe(ObserveType::Observe, QueryParamsMap(), &onObserve,
                            g_qos);
                }
                else
                {
                    cout << "No Resource found yet" << endl;
                }
                break;
            case CANCEL_OBSERVE_RESOURCE:
                if (getFoundResource())
                {
                    getFoundResource()->cancelObserve(g_qos);
                }
                else
                {
                    cout << "No Resource found yet" << endl;
                }
                break;
            case DISCOVER_ALL_TOPIC:
                g_topicList.clear();
                g_Query.clear();
                cout << "Discovering topics" << endl;
                discoveryMQTopics(g_mqBrokerResource, g_Query, &discoverTopicCB, g_qos);
                break;
            case DISCOVER_TOPIC_TYPE:
                g_topicList.clear();
                cout << "Put topic type to discover: ";
                cin >> g_cmd;
                g_Query["rt"] = g_cmd;
                discoveryMQTopics(g_mqBrokerResource, g_Query, &discoverTopicCB, g_qos);
                break;
            case SELECT_TOPIC_INDEX:
                cout << "Put discovered topic index to select: ";
                cin >> g_cmd;
                {
                    int index = atoi(g_cmd.c_str());
                    if (index < 0 || (unsigned int) index >= g_topicList.size())
                    {
                        cout << "invalid topic index selected" << endl;
                    }
                    g_mqSelectedTopicResource = g_topicList[index];
                    cout << g_mqSelectedTopicResource->uri() << " selected" << endl;
                }
                break;
            case PUBLISH_TOPIC_DATA:
                if (g_mqSelectedTopicResource == nullptr)
                {
                    cout << "Topic is not selected." << endl;
                }
                cout << "Put message to selected topic: ";
                cin >> g_cmd;
                rep["samplemessage"] = g_cmd;
                publishMQTopic(g_mqBrokerResource, rep, g_Query, &publishMessageCB, g_qos);
                break;
            case CREATE_TOPIC:
                cout << "Put topic uri to create topic: ";
                cin >> g_cmd;
                createMQTopic(g_mqBrokerResource, rep, g_cmd, g_Query, &createTopicCB, g_qos);
                break;
            case CREATE_TYPED_TOPIC:
                cout << "Put topic uri to create topic: ";
                cin >> g_cmd;
                cout << "Put topic type: ";
                cin >> g_TopicType;
                g_Query["rt"] = g_TopicType;
                createMQTopic(g_mqBrokerResource, rep, g_cmd, g_Query, &createTopicCB, g_qos);
                break;
            case SELECT_TOPIC:
                cout << "Put discovered topic index to select: ";
                cin >> g_cmd;
                {
                    int index = atoi(g_cmd.c_str());
                    if (index < 0 || (unsigned int) index >= g_topicList.size())
                    {
                        cout << "invalid topic index selected" << endl;
                    }

                    g_mqSelectedTopicResource = g_topicList[index];
                    cout << g_mqSelectedTopicResource->uri() << " selected" << endl;
                }
                break;
            case SUBSCRIBE_TOPIC:
                if (g_mqSelectedTopicResource == nullptr)
                {
                    cout << "Topic is not selected." << endl;
                }
                cout << "Subscribe to selected topic" << endl;
                subscribeMQTopic(g_mqBrokerResource, ObserveType::Observe, g_Query, &subscribeCB,
                        g_qos);
                break;
            case UNSUBSCRIBE_TOPIC:
                if (g_mqSelectedTopicResource == nullptr)
                {
                    cout << "Topic is not selected." << endl;
                }
                cout << "Unsubscribe to selected topic" << endl;
                unsubscribeMQTopic(g_mqBrokerResource, g_qos);
                break;

            default:
                cout << "Wrong Input, Please provide Input Again" << endl;
                return;
        }
    }
    else
    {
        cout
                << "\033[1;31m======================Error=========================================\033[0m\n";
        cout
                << "\033[1;31mYour expected feature not work without SignUp/SignIn \nPlease signUp/signIn first.\033[0m\n";
        cout
                << "\033[1;31m==================================================================\033[0m\n";
    }

}

int main()
{
    g_mqBrokerResource = OCPlatform::constructResourceObject(HOST_ADDRESS, DEFAULT_MQ_BROKER_URI,
            static_cast< OCConnectivityType >(CT_ADAPTER_TCP | CT_IP_USE_V4), false,
            { string("oic.wk.ps") },
            { string(DEFAULT_INTERFACE) });
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
