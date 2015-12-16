//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef __CONFORMANCE_HELPER_H__
#define __CONFORMANCE_HELPER_H__

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <algorithm>
#include "time.h"
#include "ocstack.h"
#include "GroupManager.h"
#include "ThingsConfiguration.h"
#include "ThingsMaintenance.h"
#include "ActionSet.h"
#include "OCPlatform.h"
#include "CommonUtil.h"

using namespace OC;
using namespace OIC;
using namespace std;
namespace PH = std::placeholders;

#define LIGHT_1_URI "/device/light-1"
#define LIGHT_2_URI "/device/light-2"
#define LIGHT_3_URI "/device/light-3"
#define FAN_1_URI "/device/fan-1"
#define FAN_2_URI "/device/fan-2"
#define ROOM_1_URI "/store/room-1"
#define ROOM_2_URI "/store/room-2"

#define RESOURCE_TYPE_LIGHT "core.light"
#define RESOURCE_TYPE_FAN "core.fan"
#define RESOURCE_TYPE_ROOM "core.room"
#define RESOURCE_TYPE_TV "core.tv"
#define RESOURCE_TYPE_FRIDGE "core.fridge"
#define THERMOMETER_URI "/sensor/thermometer"
#define BAROMETER_URI "/sensor/barometer"
#define RESOURCE_TYPE_THERMOMETER "core.thermometer"
#define RESOURCE_TYPE_BAROMETER "core.barometer"
#define RESOURCE_TYPE_DUMMY "core.dummy"
#define RESOURCE_TYPE_DUMMY_1 "core.dummy-1"
#define RESOURCE_TYPE_DUMMY_2 "core.dummy-2"
#define RESOURCE_TYPE_DUMMY_3 "core.dummy-3"

#define COLLECTION_TYPE_ROOM_SMALL "core.room-small"
#define COLLECTION_TYPE_ROOM_MEDIUM "core.room-medium"
#define COLLECTION_TYPE_ROOM_LARGE "core.room-large"
#define COLLECTION_TYPE_HOUSE_MEDIUM "core.house-medium"
#define COLLECTION_TYPE_HOUSE_LARGE "core.house-large"
#define NON_DISCOVERABLE_RESOURCE_TYPE "core.nondis"

#define RESOURCE_APP "./testserver"
#define UNSUPPORTIVE_RESOURCE_APP "./unsupportivetestserver"
#define COLLECTION_APP "./testcollection"
#define BOOTSTRAP_APP "./bootstrapserver"

const int RESOURCE_COUNT_LIGHT = 3;
const int RESOURCE_COUNT_FAN = 2;
const int DEFAULT_GROUP_COUNT = 3;
const int MAX_RESOURCE_COUNT = 10;

const int CALLBACK_WAIT_DEFAULT = 5;
const int CALLBACK_WAIT_MAX = 10;
const int CALLBACK_WAIT_MIN = 1;
const int CALLBACK_WAIT_NONE = 0;
const int SUCCESS_RESPONSE = 0;

#define OIC_SUCCESS_RESPONSE 200
#define OIC_SUCCESS_PUT_RESPONSE 203
#define OIC_SUCCESS_POST_RESPONSE 204
#define OIC_SUCCESS_GET_RESPONSE 205
#define OIC_ERROR_RESPONSE 400
#define OIC_RESOURCE_NOT_FOUND_RESPONSE 404

#define DEFAULT_POWER_STATE "off"
#define DEFAULT_INTENSITY 10
#define DEFAULT_MANUFACTURER "samsung"
#define DEFAULT_REGION "Dhaka, Bangladesh"
#define DEFAULT_FACTORY_RESET_STATE "false"
#define DEFAULT_REBOOT_STATE "false"
#define DEFAULT_VERSION 1.0
#define DEFAULT_CRUDN_SUPPORT true

#define URI_KEY "href"
#define POWER_KEY "power"
#define INTENSITY_KEY "intensity"
#define MANUFACTURER_KEY "manufacturer"
#define REGION_KEY "region"
#define FACTORY_RESET_KEY "value"
#define REBOOT_KEY "rb"
#define ACTIONSET_KEY "ActionSet"
#define CANCEL_ACTIONSET_KEY "CancelAction"
#define GET_ACTIONSET_KEY "GetAction"
#define EXECUTE_ACTIONSET_KEY "DoAction"
#define EXECUTE_SCHEDULED_ACTIONSET_KEY "DoScheduledAction"
#define DELETE_ACTIONSET_KEY "DelActionSet"
#define VERSION_KEY "ver"
#define CRUDN_SUPPORT_KEY "isCRUDN"

#define PLATFORM_ID "ConformanceTool"
#define MANUFACTURER_NAME DEFAULT_MANUFACTURER
#define MANUFACTURER_URL "www.samsung.com"
#define MODEL_NUMBER "ABCDE00004"
#define DATE_OF_MANUFACTURE "02-10-2015"
#define PLATFORM_VERSION "0.0.1"
#define OPERATING_SYSTEM_VERSION "10"
#define HARDWARE_VERSION "1.0.0"
#define FIRMWARE_VERSION "1.0.1"
#define SUPPORT_URL "support.samsung.com"
#define SYSTEM_TIME "00:00:00"
#define DEVICE_NAME "IotivitySmartRoom"

#define COAP_RESPONSE_CODE_SUCCESS 205
#define COAP_RESPONSE_CODE_CREATED 201
#define COAP_RESPONSE_CODE_DELETED 202
#define COAP_RESPONSE_CODE_UPDATED 204
#define COAP_RESPONSE_CODE_RETRIEVED 205
#define COAP_RESPONSE_CODE_ERROR 400
#define COAP_RESPONSE_CODE_RESOURCE_UNAUTHORIZED 401
#define COAP_RESPONSE_CODE_RESOURCE_NOT_FOUND 404
#define COAP_RESPONSE_CODE_METHOD_NOT_ALLOWED 405

class ConformanceHelper
{
private:
    static ConformanceHelper *s_conformanceHelperInstance;
    static std::mutex s_mutex;

    HeaderOptions m_headerOption;
    OCRepresentation m_representation;
    OCStackResult m_presenceCallbackResult;
    vector< shared_ptr< OCResource > > m_resourceList;
    vector< shared_ptr< OCResource > > m_groupList;
    int m_resourceFoundCallbackCount;
    int m_groupFoundCallbackCount;
    int m_configReceivedCallbackCount;
    int m_presenceReceivedCallbackCount;
    int m_isResourceEnlisted;
    int m_callbackErrorCode;

    // for bootstrap
    std::string m_defaultRegionValue;
    std::string m_defaultTimeValue;
    std::string m_defaultCurrentTimeValue;
    std::string m_defaultNetworkValue;
    std::string m_defaultIPAddressValue;
    std::string m_defaultSecurityValue;
    std::string m_defaultModeValue;
    std::string m_defaultConfigurationValue;
    std::string m_defaultFactorySetValue;

    ConformanceHelper(void);
    ~ConformanceHelper(void);

    void initializeResultMap(void);

    void onFoundCandidate(vector< shared_ptr< OCResource > > resourceList);
    void onFoundGroup(shared_ptr< OCResource > groupResource);
    void onConfigurationReceived(const HeaderOptions &headerOptions, const OCRepresentation &rep,
            const int eCode);
    void onPresenceReceived(std::string presence, OCStackResult result);

public:

    map< OCStackResult, string > m_resultMap;
    GroupManager m_groupManager;
    ThingsConfiguration* m_pThingsConfiguration;
    ThingsMaintenance* m_pThingsMaintenance;
    int m_actionSetCallbackCount;

    /**
     * @brief   Function is for getting singleton instance of ConformanceHelper
     * @return  singleton instance of ConformanceHelper
     */
    static ConformanceHelper *getInstance(void);

    /**
     * @brief   Function to Find Candidate Resource and Verify
     * @param   resourceTypeList - A vector of resourceType
     * @param   expectedResult - Result that is expected from the findCandidateResource API
     * @param   expectedResourceCount - Expected found resource count
     * @param   errorMsg - A reference of Error Message
     * @return  bool - If verified returns true otherwise false
     */
    bool findCandidateResourcesAndVerify(vector< string > resourceTypeList,
            OCStackResult expectedResult, int expectedResourceCount, string &errorMsg);

    /**
     * @brief   Function to Join Group and Verify
     * @param[in]   groupType - Type of the Group
     * @param[in]   groupType - Type of the resource to join
     * @param[out]  resourceHandle - OCResourceHandle& instance
     * @param[in]   expectedResult - Result that is expected from the joinGroup API
     * @param[out]  errorMsg - A reference of Error Message
     * @return  bool - If verified returns true otherwise false
     */
    bool bindResourceToGroupAndVerify(string groupType, string resourceType,
            OCResourceHandle &childHandle, OCStackResult expectedResult, string &errorMsg);

    /**
     * @brief   Function to doBootstrap And Verify
     * @param   expectedResult - Result that is expected from the doBootstrap API
     * @param   expectedErrorCode - expected error code from callback
     * @param   errorMsg - A reference of Error Message
     * @return  bool - If verified returns true otherwise false
     */
    bool doBootstrapAndVerify(OCStackResult expectedResult, int expectedErrorCode,
            string &errorMsg);

    /**
     * @brief   Function to getConfiguration and Verify
     * @param   resource - Resource Pointer
     * @param   expectedResult - Result that is expected from the getConfiguration API
     * @param   isCallbackExpected - If callback call is expected, then true, else false
     * @param   expectedErrorCode - expected error code from callback
     * @param   expectedValue - expected value of configuration
     * @param   errorMsg - A reference of Error Message
     * @return  bool - If verified returns true otherwise false
     */
    bool getConfigurationAndVerify(const std::shared_ptr< OCResource > resource,
            OCStackResult expectedResult, bool isCallbackExpected, int expectedErrorCode,
            string expectedValue, string &errorMsg);

    /**
     * @brief   Function to updateConfiguration and Verify
     * @param   resource - Resource Pointer
     * @param   expectedResult - Result that is expected from the updateConfiguration API
     * @param   errorMsg - A reference of Error Message
     * @param   isCallbackExpected - If callback call is expected, then true, else false
     * @param   expectedErrorCode - expected error code from callback
     * @param   updatedValue - value of the configuration after updateConfiguration is called
     * @return  bool - If verified returns true otherwise false
     */
    bool updateConfigurationAndVerify(const std::shared_ptr< OCResource > resource,
            OCStackResult expectedResult, bool isCallbackExpected, int expectedErrorCode,
            string updatedValue, string &errorMsg);

    /**
     * @brief   Function to update a grop's Configuration and Verify
     * @param   resource - Resource Pointer
     * @param   expectedResult - Result that is expected from the updateConfiguration API
     * @param   errorMsg - A reference of Error Message
     * @param   isCallbackExpected - If callback call is expected, then true, else false
     * @param   expectedErrorCode - expected error code from callback
     * @param   updatedValue - value of the configuration after updateConfiguration is called
     * @return  bool - If verified returns true otherwise false
     */
    bool updateGroupConfigurationAndVerify(const std::shared_ptr< OCResource > resource,
            OCStackResult expectedResult, bool isCallbackExpected, int expectedErrorCode,
            string updatedValue, string &errorMsg);
    /**
     * @brief   Function to reboot and Verify
     * @param   resource - Resource Pointer
     * @param   expectedResult - Result that is expected from the reboot API
     * @param   expectedErrorCode Error code that is expected from callback
     * @param   errorMsg - A reference of Error Message
     * @return  bool - If verified returns true otherwise false
     */
    bool rebootAndVerify(const std::shared_ptr< OCResource > resource, OCStackResult expectedResult,
            int expectedErrorCode, string expectedlValue, string &errorMsg);

    /**
     * @brief   Function to factoryReset and Verify
     * @param   resource - Resource Pointer
     * @param   expectedResult - Result that is expected from the factoryReset API
     * @param   expectedErrorCode Error code that is expected from callback
     * @param   errorMsg - A reference of Error Message
     * @return  bool - If verified returns true otherwise false
     */
    bool factoryResetAndVerify(const std::shared_ptr< OCResource > resource,
            OCStackResult expectedResult, int expectedErrorCode, bool supportsReset,
            string &errorMsg);

    /**
     * @brief   Function to subscribeCollectionPresence and Verify
     * @param   resource - Resource Pointer
     * @param   expectedResult - Result that is expected from the subscribeCollectionPresence API
     * @param   errorMsg - A reference of Error Message
     * @param   expectedErrorCode Error code that is expected from callback
     * @return  bool - If verified returns true otherwise false
     */
    bool subscribeCollectionPresenceAndVerify(const std::shared_ptr< OCResource > resource,
            OCStackResult expectedResult, int expectedErrorCode, string &errorMsg,
            bool isCallbackExpected = true);

    /**
     * @brief   Function to get ResourceList
     * @return  vector< shared_ptr< OCResource > > - returns resourceList that is used in the Helper
     *          Class
     */
    vector< shared_ptr< OCResource > > getResourceList(void);

    /**
     * @brief   Function to get GroupList
     * @return  vector< shared_ptr< OCResource > > - returns groupList that is used in the Helper
     *          Class
     */
    vector< shared_ptr< OCResource > > getGroupList(void);

    /**
     * @brief   Function to Create Action
     * @param   resource - Type of the Resource
     * @param   capability - Capabilities of the Action
     * @param   status - Status of the Action
     * @return  Action - A pointer of the newly creatd Action Instance
     */
    Action *createAction(shared_ptr< OCResource > resource, string capability, string status);

    /**
     * @brief   Function to Wait until resource Found
     * @param   foundCollectionResourceList - Resource Pointer List
     * @param   timeOut - Time limit for waiting
     * @return  bool - Returns true if the Resource is not found within timeOut otherwise false
     */
    bool waitForResourceFound(std::vector< std::shared_ptr< OCResource > > &foundResourceList,
            int timeOut);

    /**
     * @brief   Function to Wait until the Callback is called within TimeOut
     * @param   callbackMonitor - Reference to an int variable that will will increase
     *          when the callback is called
     * @param   timeOut - Time limit for waiting
     * @return  bool - Returns true if the Callback is not called within timeOut otherwise false
     */
    bool IsCallbackTimeoutOccured(int &callbackMonitor, int timeOut);

    /**
     * @brief   Function to Initialize Group by Finding and Then Adding ActionSet and Verify
     * @param   actionSet - Pointer to ActionSet Instance
     * @param   errorMessage - A reference of Error Message
     * @return  bool - If verified returns true otherwise false
     */
    bool initializeAndAddActionSet(ActionSet *actionSet, string &errorMessage,
            OCStackResult expectedResult = OC_STACK_OK, ACTIONSET_TYPE actionSetType =
                    ACTIONSET_TYPE::NONE, bool isPrevious = false);

    /**
     * @brief   Call back to manage and check system response after getActionSet() API call.
     * @param   headerOptions - header options of the response
     * @param   rep - OCRepresentation state after getting actionset
     * @param   eCode - Error code based from the callback
     *
     */
    void onGet(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);

    /**
     * @brief   Call back to manage and check system response after deleteActionSet() and
     *          executeActionSet() API is called
     * @param   headerOptions - header options of the response
     * @param   rep - OCRepresentation state after deleting and executing actionset
     * @param   eCode - Error code based from the callback
     */
    void onPost(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);

    /**
     * @brief Call back to manage and check system response after addActionSet() API call.
     * @param   headerOptions - header options of the response
     * @param   rep - OCRepresentation state after adding actionset
     * @param   eCode - Error code based from the callback
     */
    void onPut(const HeaderOptions &headerOptions, const OCRepresentation &rep, const int eCode);

    /**
     * @brief Call back to manage and check system response after bootstrapping.
     * @param   headerOptions - header options of the response
     * @param   rep - OCRepresentation state after bootstrappig
     * @param   eCode - Error code based from the callback
     */
    void onBootstrap(const HeaderOptions &headerOptions, const OCRepresentation &rep,
            const int eCode);

    /**
     * brief Helper function to start an app
     * @param   app - name of the app to kill
     */
    void launchApp(std::string app);

    /**
     * @brief Helper function to stop an app
     * @param   app - name of the app to stop
     */
    void killApp(std::string app);

    /**
     * @brief Helper function to find resource typein resource list
     * @param[in]   resourceList - list containing the resources to find
     * @param[in]   resourceTypeList   resource type list to find from
     * @param[out]   errorMessage   String to return error message if there are any
     * @return   if all resources are found , true is returned, else false is returned
     */
    bool containsResource(vector< shared_ptr< OCResource > > resourceList,
            vector< string > resourceTypeList, string &errorMessage);

    /**
     * @brief Helper function to get String form ActionSet and Verify
     * @param[in]   actionSet - Pointer to the ActionSet
     * @param[in]   resourceList - list containing the resources found
     * @param[in]   actionCapability -  Action Capability like "power", "intensity" etc.
     * @param[in]   capabilityStatus -  Capability Status like "on", "off" etc.
     * @param[out]   errorMessage   String to return error message if there are any
     * @return   bool - If String form ActionSet is valid, returns true otherwise false
     */
    bool getStringFromActionSetAndVerify(ActionSet *actionSet,
            vector< shared_ptr< OCResource > > resourceList, string actionCapability,
            string capabilityStatus, string &errorMessage);

    /**
     * @brief Helper function to get ActionSet from string and Verify
     * @param[in]   actionsetName - Action Set Name
     * @param[in]   resourceUriList - List of Resource URIs
     * @param[in]   capability - Action Capability
     * @param[in]   capabilityStatus - Action Capability Status
     * @param[in]   isNullTest - true if the getActionSetfromString returns NULL otherwise
     *              false
     * @param[out]   errorMessage   String to return error message if there are any
     * @return   bool - If ActionSet form String is valid, returns true otherwise false
     */
    bool getActionSetfromStringAndVerify(string actionSetName, vector< string > resourceUriList,
            string capability, string capabilityStatus, bool isNullTest, string &errorMessage);

    /**
     * @brief Helper function to add ActionSet to resource and verify
     * @param[in]   resource - Resource Pointer to add ActionSet
     * @param[in]   actionSet - Pointer to the ActionSet
     * @param[out]   errorMessage   String to return error message if there are any
     * @return   bool - If successfully adds ActionSet, returns true otherwise false
     */
    bool addActionSetAndVerify(shared_ptr< OCResource > resource, ActionSet *actionSet,
            string &errorMessage, OCStackResult expectedResult = OC_STACK_OK);

    /**
     * @brief Helper function to add ActionSet to resource and verify
     * @param[in]   resource - Resource Pointer to add ActionSet
     * @param[in]   actionSetToCompare - Pointer to the ActionSet which was originally
     *              added to the resource
     * @param[out]   errorMessage   String to return error message if there are any
     * @return   bool - If successfully adds ActionSet, returns true otherwise false
     */
    bool getActionSetAndVerify(shared_ptr< OCResource > resource, ActionSet *actionSetToCompare,
            string &errorMessage);

    /**
     * @brief Helper function to Delete ActionSet to resource and verify
     * @param[in]   actionSet - Pointer to the ActionSet
     * @param[out]   errorMessage   String to return error message if there are any
     * @return   bool - If successfully Deletes ActionSet, returns true otherwise false
     */
    bool deleteActionSetAndVerify(ActionSet *actionSet, string &errorMessage);

    /**
     * @brief Helper function to Execute ActionSet and verify
     * @param[in]   actionSet - Pointer to the ActionSet
     * @param[out]   errorMessage   String to return error message if there are any
     * @param[in]   isDelayed - if execute will be called with delay, true, default is false
     * @param[in]   delaySec - time in seconds to delay, default is 0 second
     * @return   bool - If successfully Executes ActionSet, returns true otherwise false
     */
    bool executeActionSetAndVerify(ActionSet *actionSet, string &errorMessage,
            OCStackResult expectedResult = OC_STACK_OK, bool isDelayed = false, int delaySec = 0,
            bool isCancelDue = false);

    /**
     * @brief Helper function to Cancel ActionSet and verify
     * @param[in]   actionSet - Pointer to the ActionSet
     * @param[out]   errorMessage   String to return error message if there are any
     * @param[in]   actionSetType
     * @return   bool - If successfully Executes ActionSet, returns true otherwise false
     */
    bool cancelActionSetAndVerify(ActionSet *actionSet, string &errorMessage,
            ACTIONSET_TYPE actionSetType);

    /**
     * @brief Helper function to print representaion
     * @param[in]   rep - incoming representaion
     */
    void printRepresentation(OCRepresentation rep);
};

#endif //__THINGS_MANAGER_HELPER_H__
