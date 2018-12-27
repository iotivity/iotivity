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

#ifndef INCLUDE_SM_REMOTE_OPERATIONS
#define INCLUDE_SM_REMOTE_OPERATIONS

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include <mutex>

#include "RemoteScene.h"
#include "RemoteSceneAction.h"
#include "RemoteSceneCollection.h"
#include "RemoteSceneList.h"

#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"
#include "RCSResourceObject.h"
#include "RCSAddress.h"
#include "OCPlatform.h"
#include "SMLocalOperations.h"

#define MAX_SLEEP_TIME 5
#define CALLBACK_WAIT_MAX 10
#define CALLBACK_WAIT_DEFAULT 5
#define CALLBACK_WAIT_MIN 1
#define INT_ZERO 0
#define INT_ONE 1
#define INT_TWO 2
#define EMPTY_STRING ""
#define NULL_PTR nullptr

constexpr char SPEED_THIRTY[]
{ "30" };

using namespace std;
using namespace OIC::Service;
using namespace OC;
namespace PH = std::placeholders;

class SMRemoteOperations
{
private:
    static SMRemoteOperations* s_smRemoteOperationsInstance;
    static std::mutex s_mutex;

public:
    RCSRemoteResourceObject::Ptr m_pFoundSceneList;
    RCSRemoteResourceObject::Ptr m_pFoundLightResource;
    RCSRemoteResourceObject::Ptr m_pFoundFanResource;
    std::unique_ptr< RCSDiscoveryManager::DiscoveryTask > m_discoveryTask;
    const std::string m_relativetUri;
    const std::vector< std::string > m_resourceTypes;
    std::vector< RCSRemoteResourceObject::Ptr > m_foundResourceList;
    std::shared_ptr< RemoteSceneList > m_pRemoteSceneList;
    std::shared_ptr< RemoteSceneCollection > m_pRemoteSceneCollection;
    RemoteScene::Ptr m_pRemoteScene;
    RemoteSceneAction::Ptr m_pRemoteSceneAction;
    std::shared_ptr< RCSRemoteResourceObject > m_pRemoteResource1;
    std::shared_ptr< RCSRemoteResourceObject > m_pRemoteResource2;bool m_isCallback;bool m_isRemoteSceneCreated;

    SMRemoteOperations();
    static SMRemoteOperations* getInstance(void);
    void discoverResource();
    void onRemoteResourceDiscovered(std::shared_ptr< RCSRemoteResourceObject > foundResource);
    void stopDiscovery();
    std::shared_ptr< RemoteSceneList > createRemoteSceneListInstance();
    void onRemoteSceneListCreated(RemoteSceneList::Ptr remoteSceneList, int eCode);
    void onRemoteSceneCollectionCreated(RemoteSceneCollection::Ptr remoteSceneCol, int);
    void onRemoteSceneCreated(RemoteScene::Ptr remoteScene, int);
    void onRemoteSceneActionCreated(RemoteSceneAction::Ptr remoteSceneAction, int);
    void onRemoteSceneExecuted(string, int);bool initializeRemote();
    void executeCreatedLocalSceneFromRemoteEnd();
    void createRemoteScene();
    void executeRemoteScene();
};

#endif
