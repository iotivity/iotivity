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

#ifndef INCLUDE_TESTCASE_SMHELPER_H_
#define INCLUDE_TESTCASE_SMHELPER_H_

#include <string>
#include <map>
#include <utility>
#include <vector>
#include <mutex>

#include "gtest/gtest.h"
#include "Scene.h"
#include "SceneAction.h"
#include "SceneCollection.h"
#include "SceneList.h"

#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"
#include "RCSResourceObject.h"
#include "RCSAddress.h"
#include "OCPlatform.h"

#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

#define MAX_SLEEP_TIME 5
#define INT_ZERO 0
#define INT_ONE 1
#define INT_TWO 2
#define EMPTY_STRING ""
#define NULL_PTR nullptr

#define FAN_SERVER "./fanserver"
#define LIGHT_SERVER "./lightserver"

constexpr char KEY_EMPTY[] { "" };
constexpr char VALUE_EMPTY[] { "" };
constexpr char KEY_POWER[] { "power" };
constexpr char VALUE_ON[] { "on" };
constexpr char VALUE_OFF[] { "off" };
constexpr char KEY_SPEED[] { "speed" };
constexpr char VALUE_SPEED[] { "20" };

using namespace OC;
using namespace OIC::Service;
namespace PH = std::placeholders;

class SMHelper {
private:
	static SMHelper* s_smHelperInstance;
	void onResourceDiscovered(
			std::shared_ptr<RCSRemoteResourceObject> foundResource);
	std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> discoveryTask;
	static std::mutex s_mutex;
	const std::string relativetUri;
	const std::vector<std::string> resourceTypes;

public:
	std::vector<RCSRemoteResourceObject::Ptr> g_foundResourceList;
	static bool s_checkExecuteCallback;

public:
	/**
	 * @brief 	constructor for SMHelper class
	 */
	SMHelper();

	/**
	 * @brief 	function for getting helper instance
	 * @return	helper instance
	 */
	static SMHelper* getInstance(void);

	/**
	 * @brief 	function for getting scene list instance
	 * @return  scene list instance
	 */
	SceneList* getSceneListInstance();

	/**
	 * @brief 	function for discovering resource
	 */
	void discoverResource();

	/**
	 * @brief 	callback function for executing scene
	 */
	static void onExecute(int /*Code*/);

	/**
	 * @brief 	function for stopping the discovery
	 */
	void stopDiscovery();

};
#endif
