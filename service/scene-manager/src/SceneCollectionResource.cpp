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

#include "SceneCollectionResource.h"

#include <atomic>
#include "OCApi.h"
#include "RCSRequest.h"

namespace OIC
{
    namespace Service
    {
        namespace
        {
            std::atomic_int numOfSceneCollection(0);
        }

        SceneCollectionResource::Ptr
        SceneCollectionResource::createSceneCollectionObject()
        {
            auto instance = new SceneCollectionResource();
            SceneCollectionResource::Ptr newSceneCollectionObject;
            newSceneCollectionObject.reset(instance);

            newSceneCollectionObject->m_Uri
                = PREFIX_SCENE_COLLECTION_URI + "/" + std::to_string(numOfSceneCollection++);

            newSceneCollectionObject->m_SceneCollectionResourceObj
                = RCSResourceObject::Builder(
                        newSceneCollectionObject->m_Uri,
                        SCENE_COLLECTION_RT, OC_RSRVD_INTERFACE_DEFAULT).
                        addInterface(OC::BATCH_INTERFACE).
                        setDiscoverable(true).setObservable(false).build();

            auto collectionObj = newSceneCollectionObject->m_SceneCollectionResourceObj;
            {
                RCSResourceObject::LockGuard guard(collectionObj);
                collectionObj->setAttribute(SCENE_KEY_LAST_SCENE, std::string());
                collectionObj->setAttribute(SCENE_KEY_NAME, std::string());
                collectionObj->setAttribute(SCENE_KEY_ID, SceneUtils::OICGenerateUUIDStr());
                collectionObj->setAttribute(SCENE_KEY_RTS, SCENE_MEMBER_RT);
                collectionObj->setAttribute(SCENE_KEY_SCENEVALUES, std::vector<std::string>());
            }

            newSceneCollectionObject->m_RequestHandler.m_Owner
                = std::weak_ptr<SceneCollectionResource>(newSceneCollectionObject);

            collectionObj->setSetRequestHandler(std::bind(
                    &SceneCollectionResource::SceneCollectionRequestHandler::onSetRequest,
                    newSceneCollectionObject->m_RequestHandler,
                    std::placeholders::_1, std::placeholders::_2));

            newSceneCollectionObject->m_Address = SceneUtils::getNetAddress();

            return newSceneCollectionObject;
        }

        void SceneCollectionResource::addScene(const std::string & newScene)
        {
            addScene(std::string(newScene));
        }

        void SceneCollectionResource::addScene(std::string && newScene)
        {
            auto sceneValues = m_SceneCollectionResourceObj->getAttributeValue(
                    SCENE_KEY_SCENEVALUES).get< std::vector< std::string > >();

            auto foundScene
                = std::find(sceneValues.begin(), sceneValues.end(), newScene);
            if (foundScene == sceneValues.end())
            {
                sceneValues.push_back(std::move(newScene));

                RCSResourceObject::LockGuard guard(m_SceneCollectionResourceObj);
                m_SceneCollectionResourceObj->setAttribute(
                        SCENE_KEY_SCENEVALUES, sceneValues);
            }
        }

        void SceneCollectionResource::addSceneMember(
                SceneMemberResource::Ptr newMember)
        {
            std::unique_lock<std::mutex> memberlock(m_SceneMemberLock);

            struct FindMember
            {
                bool operator()(SceneMemberResource::Ptr ptr) const
                {
                    return ptr->getFullUri() == name;
                }
                std::string name;
            };
            FindMember fMember;
            fMember.name = newMember->getFullUri();
            auto foundmember = std::find_if(
                    m_SceneMembers.begin(), m_SceneMembers.end(), fMember);
            if (foundmember != m_SceneMembers.end())
            {
                // TODO will change to Scene Exception.
                throw std::exception();
            }

            m_SceneMembers.push_back(newMember);
            m_SceneCollectionResourceObj->bindResource(newMember->getRCSResourceObject());
        }

        void SceneCollectionResource::execute(std::string && sceneName)
        {
            execute(std::move(sceneName), nullptr);
        }

        void SceneCollectionResource::execute(const std::string & sceneName)
        {
            execute(std::string(sceneName));
        }

        void SceneCollectionResource::execute(
                const std::string & sceneName, SceneExecuteCallback executeCB)
        {
            execute(std::string(sceneName), std::move(executeCB));
        }

        void SceneCollectionResource::execute(
                std::string && sceneName, SceneExecuteCallback executeCB)
        {
            auto sceneValues = m_SceneCollectionResourceObj->getAttributeValue(
                    SCENE_KEY_SCENEVALUES).get< std::vector< std::string > >();
            auto foundSceneValue
                = std::find(sceneValues.begin(), sceneValues.end(), sceneName);

            if (foundSceneValue == sceneValues.end())
            {
                if (executeCB)
                {
                    executeCB(SCENE_CLIENT_BADREQUEST);
                }
                return;
            }

            auto executeHandler
                = SceneExecuteResponseHandler::createExecuteHandler(
                        shared_from_this(), std::move(executeCB));

            std::unique_lock<std::mutex> handlerlock(m_ExecuteHandlerLock);
            m_ExecuteHandlers.push_back(executeHandler);
            handlerlock.unlock();

            std::unique_lock<std::mutex> memberlock(m_SceneMemberLock);
            for (unsigned int it = 0; it < m_SceneMembers.size(); ++it)
            {
                m_SceneMembers[it]->execute(sceneName, std::bind(
                        &SceneExecuteResponseHandler::onResponse, executeHandler,
                        std::placeholders::_1, std::placeholders::_2));
            }
            memberlock.unlock();

            RCSResourceObject::LockGuard guard(m_SceneCollectionResourceObj);
            m_SceneCollectionResourceObj->setAttribute(SCENE_KEY_LAST_SCENE, sceneName);
        }

        void SceneCollectionResource::onExecute(
                int errorCode, SceneExecuteCallback cb, SceneExecuteResponseHandler::Ptr ptr)
        {
            std::unique_lock<std::mutex> handlerlock(m_ExecuteHandlerLock);
            m_ExecuteHandlers.remove(ptr);
            handlerlock.unlock();

            cb(errorCode);
        }

        std::string SceneCollectionResource::getId() const
        {
            return m_SceneCollectionResourceObj->
                    getAttributeValue(SCENE_KEY_ID).get<std::string>();
        }

        std::string SceneCollectionResource::getUri() const
        {
            return m_Uri;
        }

        std::string SceneCollectionResource::getAddress() const
        {
            return m_Address;
        }

        const std::vector<SceneMemberResource::Ptr>
        SceneCollectionResource::getSceneMembers()
        {
            std::unique_lock<std::mutex> memberlock(m_SceneMemberLock);
            std::vector<SceneMemberResource::Ptr> retMembers(m_SceneMembers);
            return retMembers;
        }

        RCSResourceObject::Ptr SceneCollectionResource::getRCSResourceObject() const
        {
            return m_SceneCollectionResourceObj;
        }

        void SceneCollectionResource::setName(std::string && sceneCollectionName)
        {
            RCSResourceObject::LockGuard guard(m_SceneCollectionResourceObj);
            m_SceneCollectionResourceObj->setAttribute(
                    SCENE_KEY_NAME, std::move(sceneCollectionName));
        }

        void SceneCollectionResource::setName(const std::string & sceneCollectionName)
        {
            setName(std::string(sceneCollectionName));
        }

        std::string SceneCollectionResource::getName() const
        {
            return m_SceneCollectionResourceObj->getAttributeValue(
                    SCENE_KEY_NAME).get<std::string>();
        }

        RCSSetResponse SceneCollectionResource::SceneCollectionRequestHandler::
        onSetRequest(const RCSRequest & request, RCSResourceAttributes & attributes)
        {
            if (request.getInterface() == OC::BATCH_INTERFACE)
            {
                return createSceneMemberRequest(request, attributes);
            }

            if (attributes.contains(SCENE_KEY_SCENEVALUES))
            {
                return addSceneRequest(request, attributes);
            }

            if (attributes.contains(SCENE_KEY_LAST_SCENE))
            {
                return executeSceneRequest(request, attributes);
            }

            return RCSSetResponse::create(attributes, (int)SCENE_CLIENT_BADREQUEST)
            .setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        RCSSetResponse SceneCollectionResource::SceneCollectionRequestHandler::
        addSceneRequest(const RCSRequest & /*request*/, RCSResourceAttributes & attributes)
        {
            SceneCollectionResource::Ptr ptr = m_Owner.lock();
            if (ptr == nullptr)
            {
                return RCSSetResponse::create(
                        RCSResourceAttributes(attributes), SCENE_CLIENT_BADREQUEST).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            auto scenes = attributes.at(SCENE_KEY_SCENEVALUES).get<std::vector<std::string>>();

            for(unsigned int it = 0; it < scenes.size(); ++it)
            {
                ptr->addScene(scenes[it]);
            }

            return RCSSetResponse::create(RCSResourceAttributes(attributes)).
                    setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        RCSSetResponse SceneCollectionResource::SceneCollectionRequestHandler::
        executeSceneRequest(const RCSRequest & /*request*/, RCSResourceAttributes & attributes)
        {
            SceneCollectionResource::Ptr ptr = m_Owner.lock();
            if (ptr == nullptr)
            {
                return RCSSetResponse::create(
                        RCSResourceAttributes(attributes), SCENE_CLIENT_BADREQUEST).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            auto request_key = attributes.at(SCENE_KEY_LAST_SCENE).get<std::string>();
            ptr->execute(std::string(request_key), std::bind(
                    &SceneCollectionResource::SceneCollectionRequestHandler::onExecute, this,
                    std::placeholders::_1,
                    attributes));

            // TODO slow response
            return RCSSetResponse::create(RCSResourceAttributes(attributes))
            .setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        RCSSetResponse SceneCollectionResource::SceneCollectionRequestHandler::
        createSceneMemberRequest(const RCSRequest & /*request*/, RCSResourceAttributes & attributes)
        {
            SceneCollectionResource::Ptr ptr = m_Owner.lock();
            if (ptr == nullptr || !attributes.contains(SCENE_KEY_PAYLOAD_LINK))
            {
                return RCSSetResponse::create(
                        RCSResourceAttributes(attributes), SCENE_CLIENT_BADREQUEST).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            auto linkAtt = attributes.at(SCENE_KEY_PAYLOAD_LINK).get<RCSResourceAttributes>();

            auto memberObj
                = SceneMemberResource::createSceneMemberResource(linkAtt);

            try
            {
                ptr->addSceneMember(memberObj);
            } catch (...)
            {
                memberObj.reset();
                return RCSSetResponse::create(
                        RCSResourceAttributes(attributes), SCENE_CLIENT_BADREQUEST).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            if (attributes.contains(SCENE_KEY_SCENEMAPPINGS))
            {
                auto sceneMappings
                    = attributes.at(SCENE_KEY_SCENEMAPPINGS).get<std::vector<RCSResourceAttributes>>();
                for (unsigned int it = 0; it < sceneMappings.size(); ++it)
                {
                    memberObj->addMappingInfo(SceneMemberResource::MappingInfo(
                            sceneMappings[it].at(SCENE_KEY_SCENE).get<std::string>(),
                            sceneMappings[it].at(SCENE_KEY_MEMBERPROPERTY).get<std::string>(),
                            sceneMappings[it].at(SCENE_KEY_MEMBERVALUE)));
                }
            }

            RCSResourceAttributes responseAtt(attributes);
            responseAtt[SCENE_KEY_ID] = RCSResourceAttributes::Value(memberObj->getId());
            responseAtt[SCENE_KEY_CREATEDLINK]
                        = RCSResourceAttributes::Value(memberObj->getFullUri());

            return RCSSetResponse::create(responseAtt)
            .setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        void SceneCollectionResource::SceneCollectionRequestHandler::
        onExecute(int /*errorCode*/, /*const RCSRequest & request,*/ RCSResourceAttributes & /*att*/)
        {
            // TODO slow response
        }

        void SceneCollectionResource::SceneExecuteResponseHandler::
        onResponse(const RCSResourceAttributes & /*attributes*/, int errorCode)
        {
            m_responseMembers++;
            if (errorCode != SCENE_RESPONSE_SUCCESS && m_errorCode != errorCode)
            {
                m_errorCode = errorCode;
            }
            if (m_responseMembers == m_numOfMembers)
            {
                m_Cb(m_errorCode);
            }
        }

        SceneCollectionResource::SceneExecuteResponseHandler::Ptr
        SceneCollectionResource::SceneExecuteResponseHandler::createExecuteHandler(
                const SceneCollectionResource::Ptr ptr, SceneExecuteCallback executeCB)
        {
            auto executeHandler = std::make_shared<SceneExecuteResponseHandler>();

            executeHandler->m_numOfMembers = ptr->m_SceneMembers.size();
            executeHandler->m_responseMembers = 0;

            executeHandler->m_Cb = std::bind(
                    &SceneCollectionResource::onExecute, ptr,
                    std::placeholders::_1, std::move(executeCB), executeHandler);

            executeHandler->m_Owner
                = std::weak_ptr<SceneCollectionResource>(ptr);
            executeHandler->m_errorCode  = SCENE_RESPONSE_SUCCESS;

            return executeHandler;
        }
    }
}
