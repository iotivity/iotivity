//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include <iostream>
#include <vector>
#include <algorithm>

#include "OCPlatform.h"
#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSAddress.h"
#include "RemoteSceneList.h"

using namespace OC;
using namespace OIC::Service;


typedef std::function< void() > Run;

enum Menu
{
    CREATE_REMOTE_SCENE_LIST = 1, CREATE_REMOTE_SCENE_COLLECTION,
    CREATE_REMOTE_SCENE, CREATE_REMOTE_SCENE_ACTION, EXECUTE_REMOTE_SCENE
};

constexpr int SCENE_RESULT_OK = 200;
constexpr int numMenu = 1;

const std::string scene_name = "Going Out";
const std::string relativetUri = OC_RSRVD_WELL_KNOWN_URI;
const std::vector<std::string> resourceTypes{ "oic.wk.scenelist", "core.light", "core.fan" };

std::mutex g_mtx;
std::mutex g_discoverymtx;
std::condition_variable g_cond;

std::unique_ptr<RCSDiscoveryManager::DiscoveryTask> g_discoveryTask;

RCSRemoteResourceObject::Ptr g_foundListResource;
RCSRemoteResourceObject::Ptr g_foundLightResource;
RCSRemoteResourceObject::Ptr g_foundFanResource;

RemoteSceneList::Ptr g_sceneList;
RemoteSceneCollection::Ptr g_sceneCollection;
RemoteScene::Ptr g_scene;

void displaySceneList();
void displaySceneCollection();
void displayScene();
void displaySceneAction();

void runMenu(Menu);


// Scene Manager Remote API sample ---
void onRemoteSceneListCreated(RemoteSceneList::Ptr remoteSceneList, int eCode)
{
    std::cout << __func__ << " - error code : " << eCode << std::endl;

    if (eCode == SCENE_RESULT_OK)
    {
        g_sceneList = std::move(remoteSceneList);

        displaySceneList();
        runMenu(CREATE_REMOTE_SCENE_COLLECTION);
    }
    else
    {
        std::cout << "Create Remote scene list failed." << std::endl;
        runMenu(CREATE_REMOTE_SCENE_LIST);
    }
}

void onRemoteSceneCollectionCreated(RemoteSceneCollection::Ptr remoteSceneCol, int eCode)
{
    std::cout << __func__ << " - error code : " << eCode << std::endl;

    if (eCode == SCENE_RESULT_OK)
    {
        g_sceneCollection = remoteSceneCol;

        displaySceneList();
        displaySceneCollection();
        runMenu(CREATE_REMOTE_SCENE);
    }
    else
    {
        std::cout << "Create Remote scene collection failed." << std::endl;
        runMenu(CREATE_REMOTE_SCENE_COLLECTION);
    }
}

void onRemoteSceneCreated(RemoteScene::Ptr remoteScene, int eCode)
{
    std::cout << __func__ << " - error code : " << eCode << std::endl;

    if (eCode == SCENE_RESULT_OK)
    {
        g_scene = remoteScene;

        displaySceneList();
        displaySceneCollection();
        displayScene();
        runMenu(CREATE_REMOTE_SCENE_ACTION);
    }
    else
    {
        std::cout << "Create Remote scene failed." << std::endl;
        runMenu(CREATE_REMOTE_SCENE);
    }
}

void onRemoteSceneActionCreated(RemoteSceneAction::Ptr, int eCode)
{
    std::cout << __func__ << " - error code : " << eCode << std::endl;

    if (eCode == SCENE_RESULT_OK)
    {
        displaySceneList();
        displaySceneCollection();
        displaySceneAction();
        runMenu(EXECUTE_REMOTE_SCENE);
    }
    else
    {
        std::cout << "Create Remote scene action failed." << std::endl;
        runMenu(CREATE_REMOTE_SCENE_ACTION);
    }
}

void onRemoteSceneExecuted(const std::string &sceneName, int eCode)
{
    std::cout << __func__ << " - scene name : " << sceneName
              <<  ", error code : " << eCode << std::endl;

    if (eCode != SCENE_RESULT_OK)
    {
        std::cout << "Execute scene failed." << std::endl;
    }

    runMenu(EXECUTE_REMOTE_SCENE);
}

void createRemoteSceneList()
{
    if (g_foundListResource)
    {
        RemoteSceneList::createInstance(g_foundListResource, onRemoteSceneListCreated);
    }
    else
    {
        std::cout << "Scene List Resource is not discovered." << std::endl;
        runMenu(CREATE_REMOTE_SCENE_LIST);
    }
}

void createRemoteSceneCollection()
{
    if (!g_sceneList) return;

    g_sceneList->addNewSceneCollection(onRemoteSceneCollectionCreated);
}

void createRemoteScene()
{
    if (!g_sceneCollection) return;

    g_sceneCollection->addNewScene(scene_name, onRemoteSceneCreated);
}

void createRemoteSceneAction(
    RemoteScene::Ptr scene, RCSRemoteResourceObject::Ptr member,
    const std::string &key, const std::string &value)
{
    if (scene && member)
    {
        g_scene->addNewSceneAction(member, key, RCSResourceAttributes::Value(value),
            onRemoteSceneActionCreated);
    }
}

void createRemoteSceneActions()
{
    createRemoteSceneAction(g_scene, g_foundLightResource, "power", "off");
    createRemoteSceneAction(g_scene, g_foundFanResource, "speed", "0");
}

void executeScene()
{
    displaySceneList();
    displaySceneCollection();
    displaySceneAction();

    if (g_scene)
    {
        g_scene->execute(onRemoteSceneExecuted);
        std::cout << "\n\t'" << g_scene->getName() << "' is executed!\n" << std::endl;
    }
}
// --- Scene Manager Remote API sample


void configurePlatform()
{
    PlatformConfig config
    {
        OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos
    };
    OCPlatform::Configure(config);
}

void processUserInput(int min, int max)
{
    assert(min <= max);

    int input;

    std::cin >> input;

    if (!std::cin.fail())
    {
        if (input == max + 1)  exit(0);
        if (min <= input && input <= max) return;
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    throw std::runtime_error("Invalid Input, please try again");
}

void excecuteCommand(std::string str, Run runFunc)
{
    std::cout << "\n========================================================\n";
    std::cout << "1. " << str << "\n";
    std::cout << "2. Quit         \n";
    std::cout << "========================================================  \n";

    try
    {
        processUserInput(1, numMenu);
        runFunc();
    }
    catch(std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
}

void runMenu(Menu menu)
{
    std::string strMenu;
    Run runFunc;

    switch (menu)
    {
    case CREATE_REMOTE_SCENE_LIST:
        strMenu = "Create a RemoteSceneList";
        runFunc = createRemoteSceneList;
        break;
    case CREATE_REMOTE_SCENE_COLLECTION:
        strMenu = "Create a RemoteSceneCollection";
        runFunc = createRemoteSceneCollection;
        break;
    case CREATE_REMOTE_SCENE:
        strMenu = "Create a RemoteScene";
        runFunc = createRemoteScene;
        break;
    case CREATE_REMOTE_SCENE_ACTION:
        strMenu = "Create RemoteSceneActions";
        runFunc = createRemoteSceneActions;
        break;
    case EXECUTE_REMOTE_SCENE:
        strMenu = "Execute RemoteScene";
        runFunc = executeScene;
        break;
    default:
        return;
    }

    excecuteCommand(strMenu, runFunc);
}

void displaySceneList()
{
    if (g_sceneList)
    {
        std::cout << "\tSceneList" << "\n\t   |_ _ _ ";
        std::cout << g_sceneList->getName() << std::endl;
    }
}

void displaySceneCollection()
{
    if (g_sceneCollection)
    {
        std::cout << "\t\t   |_ _ _ " << g_sceneCollection->getId() 
            << " (SceneCollection)" << std::endl;
    }
}

void displayScene()
{
    if (g_scene)
    {
        std::cout << "\t\t\t   |_ _ _ " << g_scene->getName() << " (Scene)" << std::endl;
    }
}

void displaySceneAction()
{
    if (!g_scene) return;

    displayScene();

    auto sceneActionList = g_scene->getRemoteSceneActions();
    for (const auto &it : sceneActionList)
    {
        auto attr = it->getExecutionParameter();
        for (const auto &att : attr)
        {
            std::cout << "\t\t\t      \t\t|_ _ _ ";
            std::cout << it->getRemoteResourceObject()->getUri() << " : ";
            std::cout << att.key() << " - " << att.value().toString() << std::endl;
        }
    }
}

void onResourceDiscovered(std::shared_ptr<RCSRemoteResourceObject> foundResource)
{
    std::lock_guard< std::mutex > lock(g_discoverymtx);
    std::cout << "onResourceDiscovered callback" << std::endl;

    std::string resourceURI = foundResource->getUri();
    std::string hostAddress = foundResource->getAddress();
    std::vector< std::string > vecRTs = foundResource->getTypes();

    std::cout << "\t\tResource URI : " << resourceURI << std::endl;
    std::cout << "\t\tResource Host : " << hostAddress << std::endl;

    // if the found resource is a scene list resource
    if (std::find(vecRTs.begin(), vecRTs.end(), "oic.wk.scenelist") != vecRTs.end())
        g_foundListResource = foundResource;

    // if the found resource is a light resource
    else if (std::find(vecRTs.begin(), vecRTs.end(), "core.light") != vecRTs.end())
    {
        g_foundLightResource = foundResource;
    }

    // if the found resource is a fan resource
    else if (std::find(vecRTs.begin(), vecRTs.end(), "core.fan") != vecRTs.end())
    {
        g_foundFanResource = foundResource;
    }

    if (g_foundListResource && g_foundLightResource && g_foundFanResource)
    {
        g_discoveryTask->cancel();
        return;
    }

    g_cond.notify_all();
}

void discoverResource()
{
    std::cout << "Wait 2 seconds until discovered." << std::endl;

    try
    {
        g_discoveryTask
            = RCSDiscoveryManager::getInstance()->discoverResourceByTypes(RCSAddress::multicast(),
                    relativetUri, resourceTypes, &onResourceDiscovered);
    }
    catch (const RCSPlatformException &e)
    {
        std::cout << e.what() << std::endl;
    }
    std::unique_lock<std::mutex> lck(g_mtx);

    g_cond.wait_for(lck, std::chrono::seconds(4));
    return;
}

int main()
{
    configurePlatform();

    try
    {
        discoverResource();
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return 0;
    }

    try
    {
        runMenu(CREATE_REMOTE_SCENE_LIST);
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return 0;
    }

    while (true) { }

    std::cout << "Stopping the scene client" << std::endl;

    return 0;
}
