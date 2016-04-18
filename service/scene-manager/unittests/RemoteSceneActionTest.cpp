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

#include <mutex>
#include <condition_variable>

#include "RemoteSceneList.h"

#include "UnitTestHelper.h"
#include "SceneCommons.h"
#include "SceneList.h"
#include "RCSResourceObject.h"
#include "RCSRemoteResourceObject.h"
#include "OCPlatform.h"

using namespace std;
using namespace OIC::Service;
using namespace OC;

constexpr int DEFAULT_WAITTIME = 2000;

constexpr char RESOURCE_URI[]{ "/a/light" };
constexpr char RESOURCE_TYPE[]{ "core.light" };
constexpr char KEY[]{ "power" };
constexpr char VALUE[]{ "off" };

static int lightNum = 0;

RCSRemoteResourceObject::Ptr pListResource;
RemoteSceneList::Ptr pSceneList;
RemoteSceneCollection::Ptr pSceneCollection;
RemoteScene::Ptr pScene;
RemoteSceneAction::Ptr pSceneAction;
RCSRemoteResourceObject::Ptr pLightResource;
std::condition_variable cond;
std::mutex g_mutex;

void onRemoteSceneListCreated(RemoteSceneList::Ptr remoteSceneList, int)
{
    pSceneList = std::move(remoteSceneList);
    cond.notify_all();
}

void onRemoteSceneCollectionCreated(RemoteSceneCollection::Ptr remoteSceneCol, int)
{
    pSceneCollection = remoteSceneCol;
    cond.notify_all();
}

void onRemoteSceneCreated(RemoteScene::Ptr remoteScene, int)
{
    pScene = remoteScene;
    cond.notify_all();
}

void onRemoteSceneActionCreated(RemoteSceneAction::Ptr remoteSceneAction, int)
{
    pSceneAction = remoteSceneAction;
    cond.notify_all();
}

void onActionUpdated(int)
{
    cond.notify_all();
}
void createListServer()
{
    std::vector< std::string > vecRT{ SCENE_LIST_RT };
    std::vector< std::string > vecIF{ OC_RSRVD_INTERFACE_DEFAULT, OC::BATCH_INTERFACE };

    pListResource = SceneUtils::createRCSResourceObject(
        "coap://" + SceneUtils::getNetAddress() + SCENE_LIST_URI,
        SCENE_CONNECTIVITY, vecRT, vecIF);
}

void waitForCallback(int waitingTime = DEFAULT_WAITTIME)
{
    std::unique_lock< std::mutex > lock{ g_mutex };
    cond.wait_for(lock, std::chrono::milliseconds{ waitingTime });
}

void setup()
{
    SceneList::getInstance()->getName();
    createListServer();

    RemoteSceneList::createInstance(pListResource, onRemoteSceneListCreated);

    waitForCallback();

    pSceneList->addNewSceneCollection(onRemoteSceneCollectionCreated);

    waitForCallback();

    pSceneCollection->addNewScene("Test Scene", onRemoteSceneCreated);

    waitForCallback();
}


class RemoteSceneActionTest : public TestWithMock
{
protected:
    void SetUp()
    {
        TestWithMock::SetUp();
    }

    void createLightServer()
    {
        RCSResourceObject::Ptr pResource = RCSResourceObject::Builder(
            RESOURCE_URI, RESOURCE_TYPE, DEFAULT_INTERFACE).build();
        pResource->setAttribute(KEY, RCSResourceAttributes::Value(VALUE));

        pLightResource
            = SceneUtils::createRCSResourceObject(
            "coap://" + SceneUtils::getNetAddress() + RESOURCE_URI
            + "/" + std::to_string(lightNum++),
            SCENE_CONNECTIVITY, pResource->getTypes(), pResource->getInterfaces());
    }
};

TEST_F(RemoteSceneActionTest, createSceneAction)
{
    setup();
    createLightServer();

    pScene->addNewSceneAction(pLightResource, KEY, RCSResourceAttributes::Value(VALUE),
        onRemoteSceneActionCreated);

    waitForCallback();

    ASSERT_NE(nullptr, pSceneAction);
}

TEST_F(RemoteSceneActionTest, createSceneActionWithEmptyRCSRemoteResourceObjectPtr)
{
    ASSERT_THROW(pScene->addNewSceneAction(nullptr, KEY, RCSResourceAttributes::Value(VALUE),
        onRemoteSceneActionCreated), RCSInvalidParameterException);
}

TEST_F(RemoteSceneActionTest, getAllRemoteSceneActions)
{
    setup();
    createLightServer();

    pScene->addNewSceneAction(pLightResource, KEY, RCSResourceAttributes::Value(VALUE),
        onRemoteSceneActionCreated);

    waitForCallback();

    vector< RemoteSceneAction::Ptr > actions
        = pScene->getRemoteSceneActions();

    ASSERT_EQ((unsigned int)1, actions.size());
    ASSERT_TRUE(actions.at(0)->getExecutionParameter().contains(KEY));
    ASSERT_EQ(VALUE, actions.at(0)->getExecutionParameter().at(KEY).get< string >());
}

TEST_F(RemoteSceneActionTest, getRemoteSceneAction)
{
    createLightServer();

    pScene->addNewSceneAction(pLightResource, KEY, RCSResourceAttributes::Value(VALUE),
        onRemoteSceneActionCreated);

    waitForCallback();

    RemoteSceneAction::Ptr action = pScene->getRemoteSceneAction(pLightResource);

    ASSERT_TRUE(action->getExecutionParameter().contains(KEY));
    ASSERT_EQ(VALUE, action->getExecutionParameter().at(KEY).get< string >());
}

TEST_F(RemoteSceneActionTest, updateSceneAction)
{
    createLightServer();

    pScene->addNewSceneAction(pLightResource, KEY, RCSResourceAttributes::Value(VALUE),
        onRemoteSceneActionCreated);

    waitForCallback();

    pSceneAction->resetExecutionParameter(
        KEY, RCSResourceAttributes::Value("on"), onActionUpdated);

    waitForCallback();

    ASSERT_EQ("on", pSceneAction->getExecutionParameter().at(KEY).get< string >());
}
