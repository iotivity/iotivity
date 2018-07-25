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
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef SM_SCENECOLLECTION_RESOURCE_REQUESTOR_H_
#define SM_SCENECOLLECTION_RESOURCE_REQUESTOR_H_

#include <map>
#include <mutex>

#include "SceneCommons.h"
#include "RCSRemoteResourceObject.h"
#include "RCSRepresentation.h"
#include "SceneMemberResourceRequestor.h"

/** OIC namespace */
namespace OIC
{
    /** service namespace */
    namespace Service
    {

        class SceneCollectionResourceRequestor
            : public std::enable_shared_from_this< SceneCollectionResourceRequestor >
        {
            public:
                typedef std::shared_ptr< SceneCollectionResourceRequestor > Ptr;
                typedef std::weak_ptr< SceneCollectionResourceRequestor > wPtr;

                enum REQUEST_TYPE
                {
                    ADD_SCENE, REMOVE_SCENE, EXECUTE_SCENE
                };

                /** internal scene request callback */
                typedef std::function< 
                    void(REQUEST_TYPE, const std::string &name, int eCode) >
                        InternalSceneRequestCallback;

                /** internal add member callback */
                typedef std::function < void(int eCode) > InternalAddMemberCallback;

                /** internal set name callback */
                typedef std::function < void(int eCode) > InternalSetNameCallback;

            public:
                SceneCollectionResourceRequestor(RCSRemoteResourceObject::Ptr collectionResource);
                ~SceneCollectionResourceRequestor() = default;

                /** This method is for new scene creation request */
                void requestSceneCreation(const std::string &name, InternalSceneRequestCallback);
                /** This method is for scene removal request */
                void requestSceneRemoval(const std::string &name, InternalSceneRequestCallback);
                /** This method is for scene execution request */
                void requestSceneExecution(const std::string &name, InternalSceneRequestCallback);
                /** This method is for add scene member request */
                void requestAddSceneMember(RCSRemoteResourceObject::Ptr targetResource,
                                           const std::string &sceneName,
                                           const RCSResourceAttributes &attr,
                                           InternalAddMemberCallback);

                /** This method is for set name request */
                void requestSetName(const std::string &, InternalSetNameCallback);
                /** This method is for get request */
                void requestGet(const std::string &, RCSRemoteResourceObject::GetCallback);
                /** This method is for get remote resource object */
                RCSRemoteResourceObject::Ptr getRemoteResourceObject() const;
                /** This method is for create scene member resource requestor */
                SceneMemberResourceRequestor::Ptr createSceneMemberResourceRequestor(
                    const std::string &memHref, const std::string &id,
                    RCSRemoteResourceObject::Ptr target);
                /** This method is for get scene member resource requestor */
                SceneMemberResourceRequestor::Ptr getSceneMemberResourceRequestor(
                    const std::string &targetHref) const;

            private:
                static void onSetResponseForScene(
                    const RCSResourceAttributes &attrs, int eCode,
                    const std::string &name, const InternalSceneRequestCallback &,
                    REQUEST_TYPE, SceneCollectionResourceRequestor::wPtr);

                void onSetResponseForScene_impl(
                    const RCSResourceAttributes &attrs, int eCode,
                    const std::string &name, const InternalSceneRequestCallback &,
                    REQUEST_TYPE);

                static void onSceneMemberAdded(
                    const RCSRepresentation &, int eCode,
                    RCSRemoteResourceObject::Ptr, const InternalAddMemberCallback &,
                    SceneCollectionResourceRequestor::wPtr);

                void onSceneMemberAdded_impl(
                    const RCSRepresentation &, int eCode,
                    RCSRemoteResourceObject::Ptr, const InternalAddMemberCallback &);

                static void onNameSet(const RCSRepresentation &, int eCode, const std::string &,
                    const InternalSetNameCallback &, SceneCollectionResourceRequestor::wPtr);

                void onNameSet_impl(const RCSRepresentation &, int eCode, const std::string &,
                    const InternalSetNameCallback &);

            private:
                RCSRemoteResourceObject::Ptr m_sceneCollectionResource;
                mutable std::mutex m_memberRequestorLock;
                std::map< std::string, SceneMemberResourceRequestor::Ptr > m_memberRequestors;
        };
        
    }
}

#endif /* SM_SCENECOLLECTION_RESOURCE_REQUESTOR_H_ */

