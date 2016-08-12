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

#ifndef INCLUDE_TESTCASE_SMCOMMON_H_
#define INCLUDE_TESTCASE_SMCOMMON_H_

#include <string>
#include <map>
#include <utility>
#include <vector>
#include <mutex>

#include "gtest/gtest.h"
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

#define RESOURCE_TYPE_LIGHT "core.light"
#define RESOURCE_TYPE_FAN "core.fan"

constexpr char KEY_EMPTY[] { "" };
constexpr char VALUE_EMPTY[] { "" };
constexpr char KEY_POWER[] { "power" };
constexpr char VALUE_ON[] { "on" };
constexpr char VALUE_OFF[] { "off" };
constexpr char KEY_SPEED[] { "speed" };
constexpr char VALUE_SPEED[] { "20" };

constexpr char SCENE_COLLECTION_NAME[] { "Kitchen" };
constexpr char SCENE_NAME_1[] { "Scene1" };
constexpr char SCENE_NAME_2[] { "Scene2" };

#endif
