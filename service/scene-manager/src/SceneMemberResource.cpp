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

#include "SceneMemberResource.h"

#include <atomic>
#include "OCPlatform.h"

namespace OIC
{
    namespace Service
    {
        namespace
        {
            std::atomic_int numOfSceneMember(0);
        }

        SceneMemberResource::Ptr
        SceneMemberResource::createSceneMemberResource(
                RCSRemoteResourceObject::Ptr remoteObject)
        {
            auto instance = new SceneMemberResource();
            SceneMemberResource::Ptr newSceneMemberObject;
            newSceneMemberObject.reset(instance);

            newSceneMemberObject->m_Uri = PREFIX_SCENE_MEMBER_URI + "/" +
                std::to_string(numOfSceneMember++);
            newSceneMemberObject->m_SceneMemberResourceObj
                = RCSResourceObject::Builder(
                        newSceneMemberObject->m_Uri,
                        SCENE_MEMBER_RT, OC_RSRVD_INTERFACE_DEFAULT).
                        setDiscoverable(true).setObservable(false).build();
            newSceneMemberObject->m_RequestHandler.m_Owner
                = std::weak_ptr<SceneMemberResource>(newSceneMemberObject);

            newSceneMemberObject->m_RemoteMemberObj = remoteObject;

            auto resourceObj = newSceneMemberObject->m_SceneMemberResourceObj;

            RCSResourceObject::LockGuard guard(resourceObj);
            resourceObj->setAttribute(SCENE_KEY_ID, SceneUtils::OICGenerateUUIDStr());
            resourceObj->setAttribute(SCENE_KEY_NAME, std::string());

            RCSResourceAttributes subAtt;
            subAtt[SCENE_KEY_HREF]
                    = RCSResourceAttributes::Value(
                            remoteObject->getAddress() + remoteObject->getUri());
            subAtt[SCENE_KEY_IF] = RCSResourceAttributes::Value(remoteObject->getInterfaces());
            subAtt[SCENE_KEY_RT] = RCSResourceAttributes::Value(remoteObject->getTypes());
            resourceObj->setAttribute(SCENE_KEY_PAYLOAD_LINK, subAtt);

            resourceObj->setAttribute(
                    SCENE_KEY_SCENEMAPPINGS, std::vector<RCSResourceAttributes>());

            resourceObj->setSetRequestHandler(std::bind(
                    &SceneMemberResource::SceneMemberRequestHandler::onSetRequest,
                    newSceneMemberObject->m_RequestHandler,
                    std::placeholders::_1, std::placeholders::_2));

            return newSceneMemberObject;
        }

        SceneMemberResource::Ptr
        SceneMemberResource::createSceneMemberResource(const RCSResourceAttributes & link)
        {
            return createSceneMemberResource(RCSResourceAttributes(link));
        }

        SceneMemberResource::Ptr
        SceneMemberResource::createSceneMemberResource(RCSResourceAttributes && link)
        {
            auto href = link.at(SCENE_KEY_HREF).get<std::string>();

            std::string address;
            std::string uri;

            SceneUtils::getHostUriString(href, &address, &uri);

            auto ocResourcePtr
                = OC::OCPlatform::constructResourceObject(
                    address, uri, OCConnectivityType::CT_ADAPTER_IP, false,
                    link.at(SCENE_KEY_RT).get<std::vector<std::string>>(),
                    link.at(SCENE_KEY_IF).get<std::vector<std::string>>());

            return createSceneMemberResource(RCSRemoteResourceObject::fromOCResource(ocResourcePtr));
        }

        void SceneMemberResource::addMappingInfo(MappingInfo && mInfo)
        {
            RCSResourceAttributes newAtt;
            {
                RCSResourceObject::LockGuard guard(m_SceneMemberResourceObj);
                newAtt = m_SceneMemberResourceObj->getAttributes();
            }

            auto mappingInfo = newAtt.at(SCENE_KEY_SCENEMAPPINGS).
                    get<std::vector<RCSResourceAttributes>>();

            struct FindSceneName
            {
                bool operator()(RCSResourceAttributes att) const
                {
                    return att.at(SCENE_KEY_SCENE).get<std::string>() == name;
                }
                std::string name;
            };

            FindSceneName fScene;
            fScene.name = mInfo.sceneName;
            auto foundMInfo = std::find_if(mappingInfo.begin(), mappingInfo.end(), fScene);

            if (foundMInfo != mappingInfo.end() &&
                    (* foundMInfo).at(SCENE_KEY_MEMBERPROPERTY).get<std::string>() == mInfo.key)
            {
                mappingInfo.erase(foundMInfo);
            }
            RCSResourceAttributes newMapInfo;
            newMapInfo[SCENE_KEY_SCENE] = RCSResourceAttributes::Value(mInfo.sceneName);
            newMapInfo[SCENE_KEY_MEMBERPROPERTY] = RCSResourceAttributes::Value(mInfo.key);
            newMapInfo[SCENE_KEY_MEMBERVALUE] = mInfo.value;
            mappingInfo.push_back(newMapInfo);

            RCSResourceObject::LockGuard guard(m_SceneMemberResourceObj);
            m_SceneMemberResourceObj->setAttribute(SCENE_KEY_SCENEMAPPINGS, mappingInfo);
        }

        void SceneMemberResource::addMappingInfo(const MappingInfo & mInfo)
        {
            addMappingInfo(MappingInfo(mInfo));
        }

        std::vector<SceneMemberResource::MappingInfo>
        SceneMemberResource::getMappingInfo()
        {
            auto mappingInfo
                = m_SceneMemberResourceObj->getAttributeValue(SCENE_KEY_SCENEMAPPINGS).
                  get<std::vector<RCSResourceAttributes>>();

            std::vector<MappingInfo> retMInfo;
            for(unsigned int it = 0; it < mappingInfo.size(); ++it)
            {
                MappingInfo info(mappingInfo[it].at(
                                         SCENE_KEY_SCENE).get<std::string>(),
                                 mappingInfo[it].at(
                                         SCENE_KEY_MEMBERPROPERTY).get<std::string>(),
                                 mappingInfo[it].at(
                                         SCENE_KEY_MEMBERVALUE));
                retMInfo.push_back(info);
            }

            return retMInfo;
        }

        std::string SceneMemberResource::getId() const
        {
            return m_SceneMemberResourceObj->getAttributeValue(SCENE_KEY_ID).get<std::string>();
        }

        std::string SceneMemberResource::getFullUri() const
        {
            return std::string(COAP_TAG + SceneUtils::getNetAddress() + m_Uri);
        }

        RCSRemoteResourceObject::Ptr SceneMemberResource::getRemoteResourceObject() const
        {
            return m_RemoteMemberObj;
        }

        RCSResourceObject::Ptr SceneMemberResource::getRCSResourceObject() const
        {
            return m_SceneMemberResourceObj;
        }

        void SceneMemberResource::execute(std::string && sceneName)
        {
            execute(std::move(sceneName), nullptr);
        }

        void SceneMemberResource::execute(const std::string & sceneName)
        {
            execute(std::string(sceneName));
        }

        void SceneMemberResource::execute(
                std::string && sceneName, executeCallback executeCB)
        {
            bool hasScene = false;
            auto mInfo = getMappingInfo();

            for (unsigned int it = 0; it < mInfo.size(); ++it)
            {
                if (mInfo[it].sceneName == sceneName)
                {
                    hasScene = true;

                    RCSResourceAttributes setAtt;
                    setAtt[mInfo[it].key] = mInfo[it].value;
                    m_RemoteMemberObj->setRemoteAttributes(setAtt, executeCB);
                }
            }
            if (!hasScene && executeCB != nullptr)
            {
                executeCB(RCSResourceAttributes(), SCENE_RESPONSE_SUCCESS);
            }
        }

        void SceneMemberResource::execute(
                const std::string & sceneName, executeCallback executeCB)
        {
            execute(std::string(sceneName), std::move(executeCB));
        }

        RCSSetResponse SceneMemberResource::SceneMemberRequestHandler::
        onSetRequest(const RCSRequest & /*request*/, RCSResourceAttributes & attributes)
        {
            auto ptr = m_Owner.lock();
            if (!ptr)
            {
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
                    ptr->addMappingInfo(SceneMemberResource::MappingInfo(
                            sceneMappings[it].at(SCENE_KEY_SCENE).get<std::string>(),
                            sceneMappings[it].at(SCENE_KEY_MEMBERPROPERTY).get<std::string>(),
                            sceneMappings[it].at(SCENE_KEY_MEMBERVALUE)));
                }
            }

            return RCSSetResponse::create(RCSResourceAttributes(attributes)).
                    setAcceptanceMethod(RCSSetResponse::AcceptanceMethod::IGNORE);
        }
    }
}
