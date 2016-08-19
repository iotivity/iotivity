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

#include "SMRemoteOperations.h"

SMRemoteOperations* SMRemoteOperations::s_smRemoteOperationsInstance = NULL;
std::mutex SMRemoteOperations::s_mutex;

SMRemoteOperations::SMRemoteOperations()
{
    PlatformConfig config
    { OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos };
    OCPlatform::Configure(config);
    m_resourceTypes =
    {   "oic.wk.scenelist", "core.light", "core.fan"};
    m_relativetUri = OC_RSRVD_WELL_KNOWN_URI;
    m_pRemoteSceneList = nullptr;
    m_pRemoteSceneCollection = nullptr;
    m_pRemoteScene = nullptr;
    m_pRemoteSceneAction = nullptr;
    m_pRemoteResource1 = nullptr;
    m_pRemoteResource2 = nullptr;
    m_isCallback = false;
    m_isRemoteSceneCreated = false;
}

SMRemoteOperations* SMRemoteOperations::getInstance(void)
{
    s_smRemoteOperationsInstance = new SMRemoteOperations();
    return s_smRemoteOperationsInstance;
}

void SMRemoteOperations::discoverResource()
{
    string exceptionMsg = "";
    try
    {
        m_discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByTypes(
                RCSAddress::multicast(), m_relativetUri, m_resourceTypes,
                std::bind(&SMRemoteOperations::onRemoteResourceDiscovered, this, PH::_1));
    }
    catch (const RCSPlatformException& e)
    {
        exceptionMsg = e.what();
        cout << "Exception occured: " << exceptionMsg << endl;
    }
    catch (const RCSException& e)
    {
        exceptionMsg = e.what();
        cout << "Exception occured: " << exceptionMsg << endl;
    }
}

void SMRemoteOperations::stopDiscovery()
{
    m_discoveryTask->cancel();
}

void SMRemoteOperations::onRemoteResourceDiscovered(
        std::shared_ptr< RCSRemoteResourceObject > foundResource)
{
    s_mutex.lock();
    cout << "onResourceDiscovered callback" << endl;

    string resourceURI = foundResource->getUri();
    string hostAddress = foundResource->getAddress();
    std::vector< std::string > vecRTs = foundResource->getTypes();

    cout << "Resource URI : " << resourceURI << endl;
    cout << "Resource URI : " << hostAddress << endl;

    // if the found resource is a scene list resource
    if (std::find(vecRTs.begin(), vecRTs.end(), "oic.wk.scenelist") != vecRTs.end())
        m_pFoundSceneList = foundResource;

    // if the found resource is a light resource
    else if (std::find(vecRTs.begin(), vecRTs.end(), "core.light") != vecRTs.end())
    {
        m_pFoundLightResource = foundResource;
        m_foundResourceList.push_back(foundResource);
    }

    // if the found resource is a fan resource
    else if (std::find(vecRTs.begin(), vecRTs.end(), "core.fan") != vecRTs.end())
    {
        m_pFoundFanResource = foundResource;
        m_foundResourceList.push_back(foundResource);
    }

    s_mutex.unlock();
}

void SMRemoteOperations::onRemoteSceneListCreated(RemoteSceneList::Ptr remoteSceneList, int)
{
    cout << __func__ << " is called" << endl;

    if (remoteSceneList)
    {
        m_pRemoteSceneList = std::move(remoteSceneList);
    }
    else
    {
        cout << "Create remote scene list failed" << endl;
    }
}

std::shared_ptr< RemoteSceneList > SMRemoteOperations::createRemoteSceneListInstance()
{
    RemoteSceneList::createInstance(m_pFoundSceneList,
            std::bind(&SMRemoteOperations::onRemoteSceneListCreated, this, placeholders::_1,
                    placeholders::_2));
    sleep(CALLBACK_WAIT_DEFAULT);

    return m_pRemoteSceneList;
}

void SMRemoteOperations::onRemoteSceneCollectionCreated(RemoteSceneCollection::Ptr remoteSceneCol,
        int)
{
    cout << __func__ << " is called" << endl;

    if (remoteSceneCol)
    {
        m_pRemoteSceneCollection = remoteSceneCol;
    }
    else
    {
        cout << __func__ << " returns null value" << endl;
    }
}

void SMRemoteOperations::onRemoteSceneCreated(RemoteScene::Ptr remoteScene, int)
{
    cout << __func__ << " is called" << endl;

    if (remoteScene)
    {
        m_pRemoteScene = remoteScene;
    }
    else
    {
        cout << __func__ << " returns null value" << endl;
    }
}

void SMRemoteOperations::onRemoteSceneActionCreated(RemoteSceneAction::Ptr remoteSceneAction, int)
{
    cout << __func__ << " is called" << endl;

    if (remoteSceneAction)
    {
        m_pRemoteSceneAction = remoteSceneAction;
    }
    else
    {
        cout << __func__ << " returns null value" << endl;
    }
}

void SMRemoteOperations::onRemoteSceneExecuted(string, int)
{
    cout << __func__ << " is called" << endl;
    m_isCallback = true;
}

bool SMRemoteOperations::initializeRemote()
{
    cout << "Initializing remote operations" << endl;
    m_pFoundSceneList = nullptr;
    m_foundResourceList.clear();

    cout << "Trying to Discover Resources" << endl;
    discoverResource();
    cout << "Wait 5 seconds until discovered." << endl;
    sleep(MAX_SLEEP_TIME);
    stopDiscovery();
    cout << "Size of resource list : " << m_foundResourceList.size() << endl;

    if (m_pFoundSceneList == NULL_PTR)
    {
        cout
                << "Local SceneList resource is not found. First create Local Scene before executing remote operations"
                << endl;
        return false;
    }

    m_pRemoteSceneList = createRemoteSceneListInstance();
    if (m_pRemoteSceneList == NULL_PTR)
    {
        cout << "Remote operations can not be executed. Remote SceneList pointer is not found"
                << endl;
        return false;
    }

    return true;
}

void SMRemoteOperations::executeCreatedLocalSceneFromRemoteEnd()
{
    m_pRemoteSceneList = nullptr;
    m_pRemoteSceneCollection = nullptr;
    m_pRemoteScene = nullptr;

    if (initializeRemote() != true)
    {
        return;
    }

    cout << "Executing Local Scene from Remote End" << endl;
    if (m_pRemoteSceneList->getRemoteSceneCollections().size() == INT_ZERO)
    {
        cout << "Local Scene Collection is not found from remote end" << endl;
        return;
    }

    if (m_pRemoteSceneList->getRemoteSceneCollections().size() == INT_TWO)
    {
        cout << "You can not execute step 6 before step 5. Try again after restaring the app"
                << endl;
        return;
    }
    m_pRemoteSceneCollection = m_pRemoteSceneList->getRemoteSceneCollections().at(INT_ZERO);
    if (m_pRemoteSceneCollection == NULL_PTR)
    {
        cout << "Found remote scene collection pointer is null" << endl;
        return;
    }

    if (m_pRemoteSceneCollection->getRemoteScenes().size() == INT_ZERO)
    {
        cout << "Local Scene is not found from remote end" << endl;
        return;
    }
    m_pRemoteScene = m_pRemoteSceneCollection->getRemoteScenes().begin()->second;
    if (m_pRemoteScene == NULL_PTR)
    {
        cout << "Found remote scene pointer is null" << endl;
        return;
    }

    m_isCallback = false;
    m_pRemoteScene->execute(
            std::bind(&SMRemoteOperations::onRemoteSceneExecuted, this, placeholders::_1,
                    placeholders::_2));
    sleep(CALLBACK_WAIT_MIN);
    if (m_isCallback == false)
    {
        cout
                << "Local Scene is not executed from remote end. Reason: RemoteSceneExecuteCallback is not called"
                << endl;
        return;
    }
    cout << "Local Scene is executed successfully from remote end" << endl;
}

void SMRemoteOperations::createRemoteScene()
{
    m_pRemoteSceneList = nullptr;
    m_pRemoteSceneCollection = nullptr;
    m_pRemoteScene = nullptr;

    if (initializeRemote() != true)
    {
        return;
    }

    cout << "Creating Remote Scene and executing" << endl;

    string remoteSceneCreationErrorMsg = "Remote Scene can not be created. Reason: ";

    if (m_foundResourceList.size() != INT_TWO)
    {
        cout << remoteSceneCreationErrorMsg << "Needed Two Resources. Found: "
                << m_foundResourceList.size() << endl;
        return;
    }

    m_pRemoteSceneList->addNewSceneCollection(
            std::bind(&SMRemoteOperations::onRemoteSceneCollectionCreated, this, placeholders::_1,
                    placeholders::_2));
    sleep(CALLBACK_WAIT_MAX);
    if (m_pRemoteSceneCollection == NULL_PTR)
    {
        cout << remoteSceneCreationErrorMsg << "Remote Scene Collection pointer is not found"
                << endl;
        return;
    }

    string sceneName1 = "Scene1";

    m_pRemoteSceneCollection->addNewScene(sceneName1,
            std::bind(&SMRemoteOperations::onRemoteSceneCreated, this, placeholders::_1,
                    placeholders::_2));
    sleep(CALLBACK_WAIT_MAX);
    if (m_pRemoteScene == NULL_PTR)
    {
        cout << remoteSceneCreationErrorMsg << "Remote Scene pointer is not found" << endl;
        return;
    }

    m_pRemoteResource1 = m_foundResourceList.at(
    INT_ZERO);
    m_pRemoteResource2 = m_foundResourceList.at(
    INT_ONE);

    m_pRemoteScene->addNewSceneAction(m_pRemoteResource1, KEY_POWER,
            RCSResourceAttributes::Value(VALUE_OFF),
            std::bind(&SMRemoteOperations::onRemoteSceneActionCreated, this, placeholders::_1,
                    placeholders::_2));
    sleep(CALLBACK_WAIT_MIN);
    if (m_pRemoteSceneAction == NULL_PTR)
    {
        cout << "Remote Scene is created but first Remote Scene Action is not created" << endl;
        return;
    }

    m_pRemoteSceneAction = nullptr;
    m_pRemoteScene->addNewSceneAction(m_pRemoteResource2, KEY_SPEED,
            RCSResourceAttributes::Value(SPEED_THIRTY),
            std::bind(&SMRemoteOperations::onRemoteSceneActionCreated, this, placeholders::_1,
                    placeholders::_2));
    sleep(CALLBACK_WAIT_MIN);
    if (m_pRemoteSceneAction == NULL_PTR)
    {
        cout << "Remote Scene is created but second Remote Scene Action is not created" << endl;
        return;
    }

    m_isRemoteSceneCreated = true;
    cout << "Remote Scene is created successfully" << endl;
}

void SMRemoteOperations::executeRemoteScene()
{
    if (!m_isRemoteSceneCreated)
    {
        cout << "First Create Remote Scene" << endl;
        return;
    }

    m_pRemoteScene->execute(
            std::bind(&SMRemoteOperations::onRemoteSceneExecuted, this, placeholders::_1,
                    placeholders::_2));
    sleep(CALLBACK_WAIT_MIN);
    if (m_isCallback == false)
    {
        cout << "Remote Scene is not executed. Reason: RemoteSceneExecuteCallback is not called"
                << endl;
        return;
    }
    cout << "Remote Scene is executed successfully" << endl;
}

