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

#include "SMLocalOperations.h"

SMLocalOperations* SMLocalOperations::s_smLocalOperationsInstance = NULL;
std::mutex SMLocalOperations::s_mutex;
bool SMLocalOperations::s_checkExecuteCallback = false;

SMLocalOperations::SMLocalOperations()
{
    m_resourceTypes =
    {   "core.light", "core.fan"};
    m_relativetUri = OC_RSRVD_WELL_KNOWN_URI;
    m_pSceneList = nullptr;
    m_pSceneCollection = nullptr;
    m_pScene1 = nullptr;
    m_pSceneAction1 = nullptr;
    m_pSceneAction2 = nullptr;
    m_pResource1 = nullptr;
    m_pResource2 = nullptr;
}

SMLocalOperations* SMLocalOperations::getInstance(void)
{
    s_smLocalOperationsInstance = new SMLocalOperations();
    return s_smLocalOperationsInstance;
}

SceneList* SMLocalOperations::getSceneListInstance()
{
    SceneList* sceneListInstance = nullptr;
    sceneListInstance = SceneList::getInstance();
    return sceneListInstance;
}

void SMLocalOperations::onResourceDiscovered(
        std::shared_ptr< RCSRemoteResourceObject > foundResource)
{
    s_mutex.lock();

    cout << "onResourceDiscovered callback" << endl;

    string resourceURI = foundResource->getUri();
    string hostAddress = foundResource->getAddress();

    cout << "Resource URI : " << resourceURI << endl;
    cout << "Resource URI : " << hostAddress << endl;

    m_foundResourceList.push_back(foundResource);

    s_mutex.unlock();
}

void SMLocalOperations::onExecute(int /*Code*/)
{
    SMLocalOperations::s_checkExecuteCallback = true;
    cout << __func__ << " is called" << endl;
}

void SMLocalOperations::discoverResource()
{
    string exceptionMsg = "";
    try
    {
        m_discoveryTask = RCSDiscoveryManager::getInstance()->discoverResourceByTypes(
                RCSAddress::multicast(), m_relativetUri, m_resourceTypes,
                std::bind(&SMLocalOperations::onResourceDiscovered, this, PH::_1));
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

void SMLocalOperations::stopDiscovery()
{
    m_discoveryTask->cancel();
}

void SMLocalOperations::createLocalScene()
{
    cout << "Creating Local Scene" << endl;
    string localSceneErrorMsg = "Local Scene can not be created. Reason: ";

    cout << "Trying to Discover Resources" << endl;
    discoverResource();
    cout << "Wait 5 seconds until discovered." << endl;
    sleep(MAX_SLEEP_TIME);
    stopDiscovery();
    cout << "Size of resource list : " << m_foundResourceList.size() << endl;

    if (m_foundResourceList.size() != INT_TWO)
    {
        cout << localSceneErrorMsg << "Needed Two Resources. Found: " << m_foundResourceList.size()
                << endl;
        return;
    }

    m_pSceneList = getSceneListInstance();
    if (m_pSceneList == NULL_PTR)
    {
        cout << localSceneErrorMsg << "SceneList pointer is not found" << endl;
        return;
    }

    m_pSceneCollection = m_pSceneList->addNewSceneCollection();
    if (m_pSceneCollection == NULL_PTR)
    {
        cout << localSceneErrorMsg << "Scene Collection pointer is not found" << endl;
        return;
    }

    string sceneName1 = "Scene1";
    m_pScene1 = m_pSceneCollection->addNewScene(sceneName1);
    if (m_pScene1 == NULL_PTR)
    {
        cout << localSceneErrorMsg << "Scene pointer is not found" << endl;
        return;
    }

    m_pResource1 = m_foundResourceList.at(INT_ZERO);
    m_pResource2 = m_foundResourceList.at(INT_ONE);

    m_pSceneAction1 = m_pScene1->addNewSceneAction(m_pResource1, KEY_POWER,
            RCSResourceAttributes::Value(VALUE_ON));
    if (m_pSceneAction1 == NULL_PTR)
    {
        cout << "Local Scene is created but first Scene Action is not created" << endl;
        return;
    }

    m_pSceneAction2 = m_pScene1->addNewSceneAction(m_pResource2, KEY_SPEED,
            RCSResourceAttributes::Value(SPEED_TWENTY));
    if (m_pSceneAction2 == NULL_PTR)
    {
        cout << "Local Scene is created but second Scene Action is not created" << endl;
        return;
    }

    cout << "Local Scene has been created successfully" << endl;
}

void SMLocalOperations::executeLocalScene()
{
    cout << "Executing Local Scene" << endl;
    if (m_pScene1 == NULL_PTR)
    {
        cout << "Local Scene was not created. First create Local Scene" << endl;
        return;
    }
    m_pScene1->execute(&SMLocalOperations::onExecute);
    sleep(MAX_SLEEP_TIME);
    if (s_checkExecuteCallback != true)
    {
        cout << "Local Scene is not executed. Reason: ExecuteCallback is not called" << endl;
        return;
    }
    cout << "Local Scene is executed successfully" << endl;
}
