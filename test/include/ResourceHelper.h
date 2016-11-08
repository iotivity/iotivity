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
******************************************************************/

#ifndef __RESOURCE_HELPER_H__
#define __RESOURCE_HELPER_H__

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <algorithm>
#include "time.h"

#include "OCPlatform.h"

using namespace OC;
using namespace std;
namespace PH = std::placeholders;

#define TV_Device_URI "/TV-1"
#define TV_SWITCH_URI "/BinarySwitchResURI"
#define TV_AUDIO_URI "/AudioResURI"
#define TV_MEDIA_SOURCE_LIST_URI "/mediaSourceListResURI"

#define COLLECTION_RESOURCE_URI "/vendor/aircon/collection"

#define AC_Device_URI  "/AirCon-1"
#define AC_SWITCH_URI "/BinarySwitchResURI"
#define AC_TEMPERATURE_URI "/TemperatureResURI"
#define AC_AIR_FLOW_URI "/AirFlowResURI"
#define AC_TIMER_URI "/Vendor/AirConditioner/TimerClock"
#define AC_CHILD_LOCK_URI "/Vendor/AirConditioner/ChildLock"
#define AC_SWING_URI "/Vendor/AirConditioner/Swinger"

#define Device_TYPE_TV "oic.d.tv"
#define Device_TYPE_AC "oic.d.airconditioner"
#define SWITCH_RESOURCE_TYPE "oic.r.switch.binary"
#define AUDIO_RESOURCE_TYPE "oic.r.audio"
#define MEDIA_SOURCE_LIST_RESOURCE_TYPE "oic.r.mediasourcelist"
#define TEMPERATURE_RESOURCE_TYPE "oic.r.temperature"
#define AIR_FLOW_RESOURCE_TYPE "oic.r.airflow"
#define TIMER_RESOURCE_TYPE "x.com.vendor.timer"
#define CHILD_LOCK_RESOURCE_TYPE "x.com.vendor.child.lock"
#define SWING_RESOURCE_TYPE "x.com.vendor.swing"

#define TV_Device_INTERFACE "oic.if.a"
#define AC_Device_INTERFACE "oic.if.a"
#define SWITCH_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define AUDIO_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define MEDIA_SOURCE_LIST_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define TEMPERATURE_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define AIR_FLOW_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define TIMER_RESOURCE_INTERFACE "oic.if.a oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline oic.if.baseline"
#define CHILD_LOCK_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"
#define SWING_RESOURCE_INTERFACE "oic.if.a oic.if.baseline"

#define ACTUATOR_INTERFACE "oic.if.a"
#define LIGHT_1_URI "/device/light-1"
#define LIGHT_2_URI "/device/light-2"
#define LIGHT_3_URI "/device/light-3"
#define LIGHT_SECURED_URI "/device/light-1"
#define FAN_1_URI "/device/fan-1"
#define FAN_2_URI "/device/fan-2"
#define FAN_SECURED_URI "/device/fan-1"
#define ROOM_1_URI "/store/room-1"
#define ROOM_2_URI "/store/room-2"
#define LIGHT_INVISIBLE_URI "/device/light-invisible"
#define FAN_INVISIBLE_URI "/device/fan-invisible"
#define RESOURCE_TYPE_LIGHT "core.light core.brightlight"
#define RESOURCE_TYPE_FAN "core.fan core.table-fan"
#define GROUP_TYPE_ROOM "oic.wk.col"
#define RESOURCE_TYPE_ROOM "core.room"
#define SERVER_IP_V4 "0.0.0.0"
#define SERVER_IP_V6 ":::::"
#define SERVER_PORT 0
#define MAX_LIGHT_RESOURCE_COUNT 100
#define MAX_ATTRIBUTE_VALUE_LENGTH 2048

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
#define DEFAULT_MANUFACTURER "Vendor"
#define DEFAULT_REGION "Dhaka, Bangladesh"
#define DEFAULT_FACTORY_RESET_STATE "false"
#define DEFAULT_REBOOT_STATE "false"
#define DEFAULT_VERSION 1.0
#define DEFAULT_ACCURACY 0.85
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
#define GET_ACTIONSET_KEY "GetActionSet"
#define EXECUTE_ACTIONSET_KEY "DoAction"
#define EXECUTE_SCHEDULED_ACTIONSET_KEY "DoScheduledAction"
#define DELETE_ACTIONSET_KEY "DelActionSet"
#define VERSION_KEY "ver"
#define ACCURACY_KEY "accuracy"
#define CRUDN_SUPPORT_KEY "isCRUDN"
#define RESOURCE_TYPE_KEY "rt"
#define INTERFACE_KEY "if"

#define PLATFORM_ID "436f6e66-6f72-6d61-6e63-6553696d756c"
#define MANUFACTURER_NAME DEFAULT_MANUFACTURER
#define MANUFACTURER_URL "www.default-vendor.com"
#define MODEL_NUMBER "ABCDE00004"
#define DATE_OF_MANUFACTURE "2016-06-01"
#define PLATFORM_VERSION "0.0.1"
#define OPERATING_SYSTEM_VERSION "10"
#define HARDWARE_VERSION "1.1.0"
#define FIRMWARE_VERSION "1.1.1"
#define SUPPORT_URL "support.default-vendor.com"
#define SYSTEM_TIME "2016-06-20T10:10:10Z"
#define DEVICE_NAME "IotivitySmartRoom"

#define CORE_SPEC_VERSION "core.1.1.0"
#define RESOURCE_TYPE_SPEC_VERSION "res.1.1.0"
#define SMART_HOME_SPEC_VERSION "SH.1.1.0"

#define COAP_RESPONSE_CODE_SUCCESS 205
#define COAP_RESPONSE_CODE_CREATED 201
#define COAP_RESPONSE_CODE_DELETED 202
#define COAP_RESPONSE_CODE_UPDATED 204
#define COAP_RESPONSE_CODE_RETRIEVED 205
#define COAP_RESPONSE_CODE_ERROR 400
#define COAP_RESPONSE_CODE_RESOURCE_UNAUTHORIZED 401
#define COAP_RESPONSE_CODE_RESOURCE_NOT_FOUND 404
#define COAP_RESPONSE_CODE_METHOD_NOT_ALLOWED 405

class ResourceHelper
{
private:
    static ResourceHelper *s_resourceHelperInstance;
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

    ResourceHelper(void);
    ~ResourceHelper(void);

    void initializeResultMap(void);

    void onFoundCandidate(vector< shared_ptr< OCResource > > resourceList);
    void onFoundGroup(shared_ptr< OCResource > groupResource);
    void onConfigurationReceived(const HeaderOptions &headerOptions, const OCRepresentation &rep,
            const int eCode);
    void onPresenceReceived(std::string presence, OCStackResult result);

public:

    map< OCStackResult, string > m_resultMap;
    int m_actionSetCallbackCount;

    /**
     * @brief   Function is for getting singleton instance of ConformanceHelper
     * @return  singleton instance of ConformanceHelper
     */
    static ResourceHelper *getInstance(void);

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
     * API for freezing current thread from execute next code for a given time
     *
     * @param seconds no of seconds to wait for
     *
     * @return bool
     *
     * NOTE: This API will be support other platforms
     */
    static void waitInSecond(unsigned int seconds);

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
     * @brief Helper function to print representaion
     * @param[in]   rep - incoming representaion
     */
    void printRepresentation(OCRepresentation rep);

    /**
     * API for copying String to character array
     *
     * @param [out] targetString - character array for output
     * @param [in] sourceString - string to copy
     *
     */
    static void duplicateString(char ** targetString, string sourceString);

};

#endif // __RESOURCE_HELPER_H__
