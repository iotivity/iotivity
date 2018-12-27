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

#include "Scene.h"
#include "SceneAction.h"
#include "SceneCollection.h"
#include "SceneList.h"
#include "SMCommon.h"

constexpr char SCENE_LIST_NAME[] { "Home" };

using namespace std;
using namespace OC;
using namespace OIC::Service;
namespace PH = std::placeholders;

class SMHelper {
private:
    std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> m_pDiscoveryTask;
    static std::mutex s_mutex;
    const std::string m_relativetUri;
    const std::vector<std::string> m_resourceTypes;

public:
    std::vector<RCSRemoteResourceObject::Ptr> g_foundResourceList;
    static bool s_checkExecuteCallback;

private:
    /**
     * @brief     callback function for resource discovery
     */
    void onResourceDiscovered(
                std::shared_ptr<RCSRemoteResourceObject> foundResource);
public:
    /**
     * @brief     constructor for SMHelper class
     */
    SMHelper();

    /**
     * @brief     function for getting scene list instance
     * @return  scene list instance
     */
    SceneList* getSceneListInstance();

    /**
     * @brief     function for discovering resource
     */
    void discoverResource();

    /**
     * @brief     callback function for executing scene
     */
    static void onExecute(int /*Code*/);

    /**
     * @brief     function for stopping the discovery
     */
    void stopDiscovery();

};
#endif
