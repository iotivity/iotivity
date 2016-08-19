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

#ifndef INCLUDE_SM_LOCAL_OPERATIONS
#define INCLUDE_SM_LOCAL_OPERATIONS

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <mutex>

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

#define MAX_SLEEP_TIME 5
#define INT_ZERO 0
#define INT_ONE 1
#define INT_TWO 2
#define EMPTY_STRING ""
#define NULL_PTR nullptr

constexpr char KEY_EMPTY[]
{ "" };
constexpr char VALUE_EMPTY[]
{ "" };
constexpr char KEY_POWER[]
{ "power" };
constexpr char VALUE_ON[]
{ "on" };
constexpr char VALUE_OFF[]
{ "off" };
constexpr char KEY_SPEED[]
{ "speed" };
constexpr char SPEED_ZERO[]
{ "0" };
constexpr char SPEED_TWENTY[]
{ "20" };

using namespace std;
using namespace OC;
using namespace OIC::Service;
namespace PH = std::placeholders;

class SMLocalOperations
{
public:
    static SMLocalOperations* s_smLocalOperationsInstance;
    void onResourceDiscovered(std::shared_ptr< RCSRemoteResourceObject > foundResource);
    std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > m_discoveryTask;
    static std::mutex s_mutex;
    const std::string m_relativetUri;
    const std::vector< std::string > m_resourceTypes;
    std::vector< RCSRemoteResourceObject::Ptr > m_foundResourceList;
    static bool s_checkExecuteCallback;

    SceneList* m_pSceneList;
    std::shared_ptr< SceneCollection > m_pSceneCollection;
    std::shared_ptr< Scene > m_pScene1;
    std::shared_ptr< SceneAction > m_pSceneAction1;
    std::shared_ptr< SceneAction > m_pSceneAction2;
    std::shared_ptr< RCSRemoteResourceObject > m_pResource1;
    std::shared_ptr< RCSRemoteResourceObject > m_pResource2;

    SMLocalOperations();
    static SMLocalOperations* getInstance(void);
    SceneList* getSceneListInstance();
    void discoverResource();
    static void onExecute(int /*Code*/);
    void stopDiscovery();
    void createLocalScene();
    void executeLocalScene();
};
#endif
