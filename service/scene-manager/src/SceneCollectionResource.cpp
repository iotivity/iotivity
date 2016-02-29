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
#include "RCSSeparateResponse.h"

namespace OIC
{
    namespace Service
    {
        namespace
        {
            std::atomic_int numOfSceneCollection(0);
        }

        SceneCollectionResource::Ptr SceneCollectionResource::createSceneCollectionObject()
        {
            SceneCollectionResource::Ptr sceneCollectionResource(new SceneCollectionResource());

            sceneCollectionResource->m_uri
                = PREFIX_SCENE_COLLECTION_URI + "/" + std::to_string(numOfSceneCollection++);

            sceneCollectionResource->createResourceObject();

            sceneCollectionResource->setDefaultAttributes();

            sceneCollectionResource->initSetRequestHandler();

            sceneCollectionResource->m_address = SceneUtils::getNetAddress();

            return sceneCollectionResource;
        }

        SceneCollectionResource::Ptr SceneCollectionResource::createSceneCollectionObject(
                const RCSResourceAttributes & inputAttr)
        {
            auto sceneCollectionResource = SceneCollectionResource::createSceneCollectionObject();
            if (inputAttr.contains(SCENE_KEY_NAME))
            {
                sceneCollectionResource->setName(inputAttr.at(SCENE_KEY_NAME).get<std::string>());
            }

            if (inputAttr.contains(SCENE_KEY_SCENEVALUES))
            {
                auto sceneValues = inputAttr.at(SCENE_KEY_SCENEVALUES).
                        get<std::vector<std::string>>();
                sceneCollectionResource->getRCSResourceObject()->setAttribute(
                        SCENE_KEY_SCENEVALUES, sceneValues);
            }

            if (inputAttr.contains(SCENE_KEY_LAST_SCENE))
            {
                auto sceneValues = inputAttr.at(SCENE_KEY_LAST_SCENE).get<std::string>();
                sceneCollectionResource->getRCSResourceObject()->setAttribute(
                        SCENE_KEY_LAST_SCENE, sceneValues);
            }

            return sceneCollectionResource;
        }

        void SceneCollectionResource::createResourceObject()
        {
            m_sceneCollectionResourceObject
                = RCSResourceObject::Builder(
                        m_uri, SCENE_COLLECTION_RT, OC_RSRVD_INTERFACE_DEFAULT).
                        addInterface(OC::BATCH_INTERFACE).
                        setDiscoverable(true).setObservable(false).build();
        }

        void SceneCollectionResource::setDefaultAttributes()
        {
            m_sceneCollectionResourceObject->setAttribute(SCENE_KEY_LAST_SCENE, std::string());
            m_sceneCollectionResourceObject->setAttribute(SCENE_KEY_NAME, std::string());
            m_sceneCollectionResourceObject->setAttribute(
                    SCENE_KEY_ID, SceneUtils::OICGenerateUUIDStr());
            m_sceneCollectionResourceObject->setAttribute(SCENE_KEY_RTS, SCENE_MEMBER_RT);
            m_sceneCollectionResourceObject->setAttribute(
                    SCENE_KEY_SCENEVALUES, std::vector<std::string>());
            m_sceneCollectionResourceObject->setAttribute(SCENE_KEY_URI, m_uri);
        }

        void SceneCollectionResource::initSetRequestHandler()
        {
            m_requestHandler.m_owner
                = std::weak_ptr<SceneCollectionResource>(shared_from_this());

            m_sceneCollectionResourceObject->setSetRequestHandler(std::bind(
                    &SceneCollectionResource::SceneCollectionRequestHandler::onSetRequest,
                    m_requestHandler, std::placeholders::_1, std::placeholders::_2));
        }

        void SceneCollectionResource::addScene(const std::string & newScene)
        {
            addScene(std::string(newScene));
        }

        void SceneCollectionResource::addScene(std::string && newScene)
        {
            auto sceneValues = m_sceneCollectionResourceObject->getAttributeValue(
                    SCENE_KEY_SCENEVALUES).get< std::vector< std::string > >();

            auto foundScene
                = std::find(sceneValues.begin(), sceneValues.end(), newScene);
            if (foundScene == sceneValues.end())
            {
                sceneValues.push_back(std::move(newScene));

                m_sceneCollectionResourceObject->setAttribute(SCENE_KEY_SCENEVALUES, sceneValues);
            }
            else
            {
                throw std::exception();
            }
        }

        void SceneCollectionResource::addSceneMember(
                SceneMemberResource::Ptr newMember)
        {
            std::lock_guard<std::mutex> memberlock(m_sceneMemberLock);

            auto foundmember = std::find_if(m_sceneMembers.begin(), m_sceneMembers.end(),
                    [& newMember](const SceneMemberResource::Ptr & ptr) -> bool
                    {
                        return ptr->getTargetUri() == newMember->getTargetUri();
                    });

            if (foundmember != m_sceneMembers.end())
            {
                throw std::exception();
            }

            m_sceneMembers.push_back(newMember);
            m_sceneCollectionResourceObject->bindResource(newMember->getRCSResourceObject());
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
            auto sceneValues = m_sceneCollectionResourceObject->getAttributeValue(
                    SCENE_KEY_SCENEVALUES).get< std::vector< std::string > >();

            auto foundSceneValue
                = std::find(sceneValues.begin(), sceneValues.end(), sceneName);
            if (foundSceneValue == sceneValues.end() && executeCB && !m_sceneMembers.size())
            {
                std::thread(executeCB, SCENE_CLIENT_BADREQUEST).detach();
                return;
            }

            auto executeHandler
                = SceneExecuteResponseHandler::createExecuteHandler(
                        shared_from_this(), std::move(executeCB));

            {
                std::lock_guard<std::mutex> handlerlock(m_executeHandlerLock);
                m_executeHandlers.push_back(executeHandler);
            }

            {
                std::lock_guard<std::mutex> memberlock(m_sceneMemberLock);
                for (auto & it : m_sceneMembers)
                {
                    it->execute(sceneName, std::bind(
                            &SceneExecuteResponseHandler::onResponse, executeHandler,
                            std::placeholders::_1, std::placeholders::_2));
                }
            }

            m_sceneCollectionResourceObject->setAttribute(SCENE_KEY_LAST_SCENE, sceneName);
        }

        void SceneCollectionResource::onExecute(
                int errorCode, SceneExecuteCallback cb, SceneExecuteResponseHandler::Ptr ptr)
        {
            {
                std::lock_guard<std::mutex> handlerlock(m_executeHandlerLock);
                m_executeHandlers.remove(ptr);
            }

            std::thread(cb, errorCode).detach();
        }

        std::string SceneCollectionResource::getId() const
        {
            return m_sceneCollectionResourceObject->getAttributeValue(
                    SCENE_KEY_ID).get<std::string>();
        }

        std::string SceneCollectionResource::getUri() const
        {
            return m_uri;
        }

        std::string SceneCollectionResource::getAddress() const
        {
            return m_address;
        }

        std::vector<std::string> SceneCollectionResource::getSceneValues() const
        {
            return m_sceneCollectionResourceObject->getAttributeValue(
                    SCENE_KEY_SCENEVALUES).get<std::vector<std::string>>();
        }

        std::vector<SceneMemberResource::Ptr> SceneCollectionResource::getSceneMembers() const
        {
            return m_sceneMembers;
        }

        RCSResourceObject::Ptr SceneCollectionResource::getRCSResourceObject() const
        {
            return m_sceneCollectionResourceObject;
        }

        void SceneCollectionResource::setName(std::string && sceneCollectionName)
        {
            m_sceneCollectionResourceObject->setAttribute(
                    SCENE_KEY_NAME, std::move(sceneCollectionName));
        }

        void SceneCollectionResource::setName(const std::string & sceneCollectionName)
        {
            setName(std::string(sceneCollectionName));
        }

        std::string SceneCollectionResource::getName() const
        {
            return m_sceneCollectionResourceObject->getAttributeValue(
                    SCENE_KEY_NAME).get<std::string>();
        }

        RCSSetResponse SceneCollectionResource::SceneCollectionRequestHandler::
        onSetRequest(const RCSRequest & request, RCSResourceAttributes & attributes)
        {
            if (request.getInterface() == LINK_BATCH)
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

            if (attributes.contains(SCENE_KEY_NAME))
            {
                return setSceneCollectionName(request, attributes);
            }

            return RCSSetResponse::create(attributes, (int)SCENE_CLIENT_BADREQUEST).
                    setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        RCSSetResponse SceneCollectionResource::SceneCollectionRequestHandler::
        addSceneRequest(const RCSRequest & /*request*/, RCSResourceAttributes & attributes)
        {
            SceneCollectionResource::Ptr ptr = m_owner.lock();
            if (ptr == nullptr)
            {
                return RCSSetResponse::create(attributes, SCENE_CLIENT_BADREQUEST).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            auto values = attributes.at(SCENE_KEY_SCENEVALUES).get<std::vector<std::string>>();

            auto sizeofValues = values.size();
            unsigned int sameSize = 0;
            std::for_each (values.begin(), values.end(),
                    [& ptr, & sizeofValues, & sameSize](const std::string & value)
                    {
                        try
                        {
                            ptr->addScene(value);
                        } catch (...)
                        {
                            sameSize++;
                        }
                    });

            int eCode = SCENE_RESPONSE_SUCCESS;
            if (sameSize == sizeofValues)
            {
                eCode = SCENE_CLIENT_BADREQUEST;
            }

            return RCSSetResponse::create(attributes, eCode).
                    setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        RCSSetResponse SceneCollectionResource::SceneCollectionRequestHandler::
        executeSceneRequest(const RCSRequest & request, RCSResourceAttributes & attributes)
        {
            SceneCollectionResource::Ptr ptr = m_owner.lock();
            if (ptr == nullptr)
            {
                return RCSSetResponse::create(attributes, SCENE_CLIENT_BADREQUEST).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            auto requestKey = attributes.at(SCENE_KEY_LAST_SCENE).get<std::string>();

            RCSRequest req(request.getResourceObject().lock(), request.getOCRequest());
            ptr->execute(std::string(requestKey), std::bind(
                    &SceneCollectionResource::SceneCollectionRequestHandler::onExecute, this,
                    std::placeholders::_1, req, attributes));

            return RCSSetResponse::separate();
        }

        RCSSetResponse SceneCollectionResource::SceneCollectionRequestHandler::
        createSceneMemberRequest(const RCSRequest & /*request*/, RCSResourceAttributes & attributes)
        {
            SceneCollectionResource::Ptr ptr = m_owner.lock();
            if (ptr == nullptr || !attributes.contains(SCENE_KEY_PAYLOAD_LINK))
            {
                return RCSSetResponse::create(attributes, SCENE_CLIENT_BADREQUEST).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            auto linkAtt = attributes.at(SCENE_KEY_PAYLOAD_LINK).get<RCSResourceAttributes>();
            if (!linkAtt.contains(SCENE_KEY_HREF) ||
                    !linkAtt.contains(SCENE_KEY_RT) || !linkAtt.contains(SCENE_KEY_IF))
            {
                return RCSSetResponse::create(attributes, SCENE_CLIENT_BADREQUEST).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            auto memberObj = createSceneMemberFromRemote(linkAtt);
            if (memberObj == nullptr)
            {
                return RCSSetResponse::create(attributes, SCENE_CLIENT_BADREQUEST).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            if (attributes.contains(SCENE_KEY_SCENEMAPPINGS))
            {
                addMemberInfoFromRemote(memberObj, attributes.at(
                        SCENE_KEY_SCENEMAPPINGS).get<std::vector<RCSResourceAttributes>>());
            }

            RCSResourceAttributes responseAtt(attributes);
            responseAtt[SCENE_KEY_ID] = RCSResourceAttributes::Value(memberObj->getId());
            responseAtt[SCENE_KEY_CREATEDLINK]
                        = RCSResourceAttributes::Value(memberObj->getFullUri());

            return RCSSetResponse::create(responseAtt).
                    setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        RCSSetResponse
        SceneCollectionResource::SceneCollectionRequestHandler::setSceneCollectionName(
                const RCSRequest & /*request*/, RCSResourceAttributes & attr)
        {
            SceneCollectionResource::Ptr ptr = m_owner.lock();
            if (ptr == nullptr)
            {
                return RCSSetResponse::create(attr, SCENE_CLIENT_BADREQUEST).
                        setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
            }

            ptr->setName(attr.at(SCENE_KEY_NAME).get<std::string>());

            return RCSSetResponse::create(attr).
                    setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }

        SceneMemberResource::Ptr
        SceneCollectionResource::SceneCollectionRequestHandler::createSceneMemberFromRemote(
                RCSResourceAttributes & linkAtt)
        {
            auto memberObj = SceneMemberResource::createSceneMemberResource(linkAtt);

            try
            {
                SceneCollectionResource::Ptr ptr = m_owner.lock();
                ptr->addSceneMember(memberObj);
            } catch (...)
            {
                return nullptr;
            }

            return memberObj;
        }

        void SceneCollectionResource::SceneCollectionRequestHandler::addMemberInfoFromRemote(
                SceneMemberResource::Ptr memberObj, std::vector<RCSResourceAttributes> mInfo)
        {
            std::for_each (mInfo.begin(), mInfo.end(),
                    [& memberObj](const RCSResourceAttributes & att)
                    {
                        memberObj->addMappingInfo(SceneMemberResource::MappingInfo(
                                att.at(SCENE_KEY_SCENE).get<std::string>(),
                                att.at(SCENE_KEY_MEMBERPROPERTY).get<std::string>(),
                                att.at(SCENE_KEY_MEMBERVALUE)));
                    });
        }

        void SceneCollectionResource::SceneCollectionRequestHandler::
        onExecute(int /*errorCode*/, const RCSRequest & request, RCSResourceAttributes & /*att*/)
        {
            // TODO need to set error code.
            // and need to set specific attr' but this attr not to be apply to RCSResourceObject.
            RCSSeparateResponse(request).set();
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
                m_cb(m_errorCode);
            }
        }

        SceneCollectionResource::SceneExecuteResponseHandler::Ptr
        SceneCollectionResource::SceneExecuteResponseHandler::createExecuteHandler(
                const SceneCollectionResource::Ptr ptr, SceneExecuteCallback executeCB)
        {
            auto executeHandler = std::make_shared<SceneExecuteResponseHandler>();

            executeHandler->m_numOfMembers = ptr->m_sceneMembers.size();
            executeHandler->m_responseMembers = 0;

            executeHandler->m_cb = std::bind(
                    &SceneCollectionResource::onExecute, ptr,
                    std::placeholders::_1, std::move(executeCB), executeHandler);

            executeHandler->m_owner
                = std::weak_ptr<SceneCollectionResource>(ptr);
            executeHandler->m_errorCode  = SCENE_RESPONSE_SUCCESS;

            return executeHandler;
        }
    }
}
