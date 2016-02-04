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

#include "OCPlatform.h"
#include "RCSDiscoveryManager.h"
#include "RCSRemoteResourceObject.h"
#include "RCSResourceAttributes.h"
#include "RCSResourceObject.h"
#include "RCSAddress.h"
#include "SceneList.h"

using namespace OC;
using namespace OIC::Service;

constexpr int CREATE_SCENE_LIST = 1;
constexpr int CREATE_SCENE_COLLECTION = 2;
constexpr int GET_SCENE_COLLECTIONS = 3;
constexpr int CREATE_SCENE = 4;
constexpr int GET_SCENE = 5;
constexpr int EXECUTE_SCENE = 6;
constexpr int GET_SCENE_ACTION = 7;
constexpr int UPDATE_SCENE_ACTION = 8;

struct CloseApp {};

const std::vector<std::string> resourceTypes{"core.light", "core.fan"};
const std::string relativetUri = OC_RSRVD_WELL_KNOWN_URI;

std::mutex mtx;
std::condition_variable cond;

RCSRemoteResourceObject::Ptr g_selectedResource;
std::shared_ptr<RCSRemoteResourceObject> g_discoveredResources;

std::vector<RCSRemoteResourceObject::Ptr> g_foundResourceList;
std::vector<RCSResourceObject::Ptr> g_memberResourceList;

SceneCollection::Ptr g_sceneColObj;
Scene::Ptr g_scene;
SceneAction::Ptr g_sceneAction;

typedef std::function<void(std::shared_ptr<RCSRemoteResourceObject>)> DiscoveryCallback;
typedef std::function<void (int)> ExecuteCallback;

void onExecute(int /*Code*/)
{
    std::cout << __func__ << std::endl;
}

void configurePlatform()
{
    PlatformConfig config
    {
        OC::ServiceType::InProc, ModeType::Both, "0.0.0.0", 0, OC::QualityOfService::LowQos
    };
    OCPlatform::Configure(config);
}

int processUserInput(int min, int max)
{
    assert(min <= max);

    int input;

    std::cin >> input;

    if (!std::cin.fail())
    {
        if(input == max + 1) throw CloseApp();
        if(min <= input && input <= max) return input;
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    throw std::runtime_error("Invalid Input, please try again");
}

void onResourceDiscovered(std::shared_ptr<RCSRemoteResourceObject> foundResource)
{
    std::cout << "onResourceDiscovered callback" << std::endl;

    std::string resourceURI = foundResource->getUri();
    std::string hostAddress = foundResource->getAddress();

    std::cout << "\t\tResource URI : " << resourceURI << std::endl;
    std::cout << "\t\tResource Host : " << hostAddress << std::endl;

    g_foundResourceList.push_back(foundResource);

    cond.notify_all();
}

bool discoverResource()
{
    std::cout << "Wait 2 seconds until discovered." << std::endl;


    try
    {
        RCSDiscoveryManager::getInstance()->discoverResourceByTypes(RCSAddress::multicast(),
                relativetUri, resourceTypes, &onResourceDiscovered);
    }
    catch(const RCSPlatformException& e)
    {
         std::cout << e.what() << std::endl;
    }
    std::unique_lock<std::mutex> lck(mtx);

    cond.wait_for(lck, std::chrono::seconds(4));
    return g_discoveredResources != nullptr;
}

void createSceneList()
{
    SceneList::getInstance()->setName("sceneList Name");
    std::cout << SceneList::getInstance()->getName() << std::endl;
}

void createSceneCollection()
{
    g_sceneColObj = SceneList::getInstance()->addNewSceneCollection();
    g_sceneColObj->setName("Living Room");
    std::cout << "Created CollectionName : " << g_sceneColObj->getName() << std::endl;
    std::cout << "Id : " << g_sceneColObj->getId() << std::endl;

    g_sceneColObj = SceneList::getInstance()->addNewSceneCollection();
    g_sceneColObj->setName("Kitchen");
    std::cout << "Created CollectionName : " << g_sceneColObj->getName() << std::endl;
    std::cout << "Id : " << g_sceneColObj->getId() << std::endl;
}

void getSceneCollection()
{
    auto sceneCollectionList = SceneList::getInstance()->getSceneCollections();
    int i = 1;

    for(const auto& it : sceneCollectionList)
    {
        std::cout << i++ << "." << it->getName() << std::endl;
    }
}

void createScene()
{
    g_scene = g_sceneColObj->addNewScene("Going Out");
    g_scene->addNewSceneAction(g_foundResourceList.at(0), "power", "off");
    g_scene->addNewSceneAction(g_foundResourceList.at(1), "speed", "10");

    g_sceneColObj->addNewScene("Cooking");

    auto sceneList = g_sceneColObj->getScenes();
    for(const auto &it : sceneList)
    {
        std::cout << it.first << std::endl;
    }
}

void getScene()
{
    auto sceneList = g_sceneColObj->getScenes();
    for(const auto &it : sceneList)
    {
        std::cout << it.first << std::endl;
    }

    g_scene = g_sceneColObj->getScene("Going Out");
}

void executeScene()
{
    g_scene->execute(onExecute);
    std::cout << "execute scene!" << std::endl;
}

void getSceneAction()
{
    auto sceneActionList = g_scene->getSceneActions();
    std::cout << g_scene->getName() << std::endl;
    for(const auto &it : sceneActionList)
    {
        std::cout << it.use_count() << std::endl;
        std::cout << it->getRemoteResourceObject()->getUri() << std::endl;
        auto attr = it->getAction();
        for(const auto &att : attr)
        {
            std::cout << att.key() << " : "  << att.value().toString() << std::endl;
        }
    }
    auto sceneActions = g_scene->getSceneAction(g_foundResourceList.at(0));
    g_sceneAction = sceneActions.at(0);
}

void updateSceneAction()
{
    g_sceneAction->update("power", "on");
    executeScene();
}

int main()
{
    configurePlatform();

    discoverResource();

    int command;

    while (true)
    {
        try
        {
            std::cout << "========================================================\n";
            std::cout << CREATE_SCENE_LIST  << ". Create a SceneList                       \n";
            std::cout << CREATE_SCENE_COLLECTION << ". Create a SceneCollection                     \n";
            std::cout << GET_SCENE_COLLECTIONS << ". Get SceneCollections                      \n";
            std::cout << CREATE_SCENE << ". Create a Scene                      \n";
            std::cout << GET_SCENE << ". Get a Scene                      \n";
            std::cout << EXECUTE_SCENE << ". Execute Scene                      \n";
            std::cout << GET_SCENE_ACTION << ". Get a SceneAction                      \n";
            std::cout << UPDATE_SCENE_ACTION << ". Update SceneAction                      \n";
            std::cout << "========================================================\n";

            command = processUserInput(CREATE_SCENE_LIST, UPDATE_SCENE_ACTION);

            switch(command)
            {
                case CREATE_SCENE_LIST:
                    createSceneList();
                    break;
                case CREATE_SCENE_COLLECTION:
                    createSceneCollection();
                    break;
                case GET_SCENE_COLLECTIONS:
                    getSceneCollection();
                    break;
                case CREATE_SCENE:
                    createScene();
                    break;
                case GET_SCENE:
                    getScene();
                    break;
                case EXECUTE_SCENE:
                    executeScene();
                    break;
                case GET_SCENE_ACTION:
                    getSceneAction();
                    break;
                case UPDATE_SCENE_ACTION:
                    updateSceneAction();
                    break;
            }
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        catch (const CloseApp&)
        {
            break;
        }
    }

    std::cout << "Stopping the client" << std::endl;

    return 0;
}
