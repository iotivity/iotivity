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

#ifndef __THINGS_MANAGER_HELPER_H__
#define __THINGS_MANAGER_HELPER_H__

#include "SampleResource.h"

#include <iostream>
#include <stdlib.h>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <mutex>
#include <algorithm>
#include "time.h"
#include "ocstack.h"
#include "gtest/gtest.h"
#include "gtest_custom.h"
#include "SampleResource.h"
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

#define RESOURCE_TYPE_LIGHT "core.light"
#define RESOURCE_TYPE_FAN "core.fan"
#define RESOURCE_TYPE_DUMMY "core.dummy"
#define RESOURCE_TYPE_DUMMY_1 "core.dummy-1"
#define RESOURCE_TYPE_DUMMY_2 "core.dummy-2"
#define RESOURCE_TYPE_DUMMY_3 "core.dummy-3"

#define COLLECTION_TYPE_ROOM_SMALL "core.room-small"
#define COLLECTION_TYPE_ROOM_MEDIUM "core.room-medium"
#define COLLECTION_TYPE_ROOM_LARGE "core.room-large"

#define RESOURCE_APP "./testserver"
#define UNSUPPORTIVE_RESOURCE_APP "./unsupportivetestserver"
#define COLLECTION_APP "./testcollection"
#define BOOTSTRAP_APP "./bootstrapserver"

#define INT_MINUS_ONE -1
#define INT_ZERO 0
#define INT_ONE 1
#define INT_TWO 2
#define INT_THREE 3
#define EMPTY_STRING ""

const int RESOURCE_COUNT_LIGHT = 3;
const int RESOURCE_COUNT_FAN = 2;
const int DEFAULT_GROUP_COUNT = 3;
const int MAX_RESOURCE_COUNT = 10;

const int CALLBACK_WAIT_DEFAULT = 5;
const int APP_WAIT = 2;
const int CALLBACK_WAIT_MAX = 10;
const int CALLBACK_WAIT_MIN = 1;
const int CALLBACK_WAIT_NONE = 0;
const int SUCCESS_RESPONSE = 0;

class TMHelper {
private:
	static TMHelper* s_tmHelperInstance;
	static std::mutex s_mutex;

	HeaderOptions m_headerOption;
	OCRepresentation m_representation;
	OCStackResult m_presenceCallbackResult;
	vector<shared_ptr<OCResource> > m_resourceList;
	vector<shared_ptr<OCResource> > m_groupList;
	int m_resourceFoundCallbackCount;
	int m_groupFoundCallbackCount;
	int m_configReceivedCallbackCount;
	int m_presenceReceivedCallbackCount;
	int m_isResourceEnlisted;
	int m_callbackErrorCode;

	std::string m_defaultRegionValue;
	std::string m_defaultTimeValue;
	std::string m_defaultCurrentTimeValue;
	std::string m_defaultNetworkValue;
	std::string m_defaultIPAddressValue;
	std::string m_defaultSecurityValue;
	std::string m_defaultModeValue;
	std::string m_defaultConfigurationValue;
	std::string m_defaultFactorySetValue;

	TMHelper(void);
	~TMHelper(void);

	void initializeResultMap(void);

	void onFoundCandidate(vector<shared_ptr<OCResource> > resourceList);
	void onConfigurationReceived(const HeaderOptions& headerOptions,
			const OCRepresentation& rep, const int eCode);
	void onPresenceReceived(std::string presence, OCStackResult result);

public:
	map<OCStackResult, string> m_resultMap;
	GroupManager m_groupManager;
	ThingsConfiguration* m_pThingsConfiguration;
	ThingsMaintenance* m_pThingsMaintenance;
	int m_actionSetCallbackCount;

	/**
	 * @brief   Function is for getting singleton instance of TMHelper
	 * @return  singleton instance of TMHelper
	 */
	static TMHelper* getInstance(void);

	/**
	 * @brief   Function to Find Candidate Resource and Verify
	 * @param   resourceTypeList - A vector of resourceType
	 * @param   expectedResult - Result that is expected from the findCandidateResource API
	 * @param   expectedResourceCount - Expected found resource count
	 * @param   errorMsg - A reference of Error Message
	 * @return  bool - If verified returns true otherwise false
	 */
	bool findCandidateResourcesAndVerify(vector<string> resourceTypeList,
			OCStackResult expectedResult, int expectedResourceCount,
			string& errorMsg);

	/**
	 * @brief   Function to Join Group and Verify
	 * @param[in]   groupType - Type of the Group
	 * @param[in]   groupType - Type of the resource to join
	 * @param[out]  resourceHandle - OCResourceHandle& instance
	 * @param[in]   expectedResult - Result that is expected from the joinGroup API
	 * @param[out]  errorMsg - A reference of Error Message
	 * @return  bool - If verified returns true otherwise false
	 */
	bool bindResourceToGroupAndVerify(string resourceType,
			OCResourceHandle& childHandle, OCStackResult expectedResult,
			string& errorMsg);

	/**
	 * @brief   Function to doBootstrap And Verify
	 * @param 	expectedResult - Result that is expected from the doBootstrap API
	 * @param   expectedErrorCode - expected error code from callback
	 * @param   errorMsg - A reference of Error Message
	 * @return  bool - If verified returns true otherwise false
	 */
	bool doBootstrapAndVerify(OCStackResult expectedResult,
			int expectedErrorCode, string& errorMsg);

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
	bool getConfigurationAndVerify(const std::shared_ptr<OCResource> resource,
			OCStackResult expectedResult, bool isCallbackExpected,
			int expectedErrorCode, string expectedValue, string& errorMsg);

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
	bool updateConfigurationAndVerify(
			const std::shared_ptr<OCResource> resource,
			OCStackResult expectedResult, bool isCallbackExpected,
			int expectedErrorCode, string updatedValue, string& errorMsg);

	/**
	 * @brief   Function to reboot and Verify
	 * @param   resource - Resource Pointer
	 * @param   expectedResult - Result that is expected from the reboot API
	 * @param   expectedErrorCode - Error code that is expected from callback
	 * @param   errorMsg - A reference of Error Message
	 * @return  bool - If verified returns true otherwise false
	 */
	bool rebootAndVerify(const std::shared_ptr<OCResource> resource,
			OCStackResult expectedResult, int expectedErrorCode,
			string expectedlValue, string& errorMsg);

	/**
	 * @brief   Function to factoryReset and Verify
	 * @param   resource - Resource Pointer
	 * @param   expectedResult - Result that is expected from the factoryReset API
	 * @param   expectedErrorCode - Error code that is expected from callback
	 * @param   errorMsg - A reference of Error Message
	 * @return  bool - If verified returns true otherwise false
	 */
	bool factoryResetAndVerify(const std::shared_ptr<OCResource> resource,
			OCStackResult expectedResult, int expectedErrorCode,
			bool supportsReset, string& errorMsg);

	/**
	 * @brief   Function to subscribeCollectionPresence and Verify
	 * @param   resource - Resource Pointer
	 * @param   expectedResult - Result that is expected from the subscribeCollectionPresence API
	 * @param   errorMsg - A reference of Error Message
	 * @param   expectedErrorCode - Error code that is expected from callback
	 * @return  bool - If verified returns true otherwise false
	 */
	bool subscribeCollectionPresenceAndVerify(
			const std::shared_ptr<OCResource> resource,
			OCStackResult expectedResult, int expectedErrorCode,
			string& errorMsg, bool isCallbackExpected = true);

	/**
	 * @brief   Function to get ResourceList
	 * @return  vector< shared_ptr< OCResource > > - returns resourceList that is used in the Helper
	 *          Class
	 */
	vector<shared_ptr<OCResource> > getResourceList(void);

	/**
	 * @brief   Function to Wait until resource Found
	 * @param   foundCollectionResourceList - Resource Pointer List
	 * @param   timeOut - Time limit for waiting
	 * @return  bool - Returns true if the Resource is not found within timeOut otherwise false
	 */
	bool waitForResourceFound(
			std::vector<std::shared_ptr<OCResource> > &foundResourceList,
			int timeOut);

	/**
	 * @brief 	Call back to manage and check system response after bootstrapping.
	 * @param   headerOptions - header options of the response
	 * @param   rep - OCRepresentation state after bootstrapping
	 * @param   eCode - Error code based from the callback
	 */
	void onBootstrap(const HeaderOptions& headerOptions,
			const OCRepresentation& rep, const int eCode);

	/**
	 * brief 	Helper function to start an app
	 * @param   app - name of the app to kill
	 */
	void launchApp(std::string app);

	/**
	 * @brief 	Helper function to stop an app
	 * @param   app - name of the app to stop
	 */
	void killApp(std::string app);

	/**
	 * @brief 		Helper function to find resource type in resource list
	 * @param[in]   resourceList - list containing the resources to find
	 * @param[in]   resourceTypeList - resource type list to find from
	 * @param[out]  errorMessage - String to return error message if there are any
	 * @return   	if all resources are found , true is returned, else false is returned
	 */
	bool containsResource(vector<shared_ptr<OCResource> > resourceList,
			vector<string> resourceTypeList, string& errorMessage);
};

#endif //__THINGS_MANAGER_HELPER_H__
