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

#ifndef SCENE_COLLECTION_RESOURCE_OBJECT_H
#define SCENE_COLLECTION_RESOURCE_OBJECT_H

#include <list>

#include "RCSResourceObject.h"
#include "SceneCommons.h"
#include "SceneMemberResource.h"

/** OIC namespace */
namespace OIC
{
    namespace Service
    {
        class SceneCollectionResource
                : public std::enable_shared_from_this<SceneCollectionResource>
        {
        public:
            typedef std::shared_ptr< SceneCollectionResource > Ptr;
            typedef std::function< void(int) > SceneExecuteCallback;

            ~SceneCollectionResource() = default;

            static SceneCollectionResource::Ptr create();
            static SceneCollectionResource::Ptr create(const RCSResourceAttributes &);

            /**
             * add new scene resource and it is already exists then throw the exception
             * @param new scene to add
             */
            void addScene(std::string &&);
            void addScene(const std::string &);

            /**
             * add new scene member and if it is already registered then throw exception
             * @param new scene member to add
             */
            void addSceneMember(SceneMemberResource::Ptr);

             /**
              * execute the given scene name
              * @param name of the scene to execute
              * @param scene execute callback
              */
            void execute(std::string &&);
            void execute(const std::string &);
            void execute(std::string &&, SceneExecuteCallback);
            void execute(const std::string &, SceneExecuteCallback);

            /**
             * set the scene name
             * @param string name to set
             */
            void setName(std::string &&);
            void setName(const std::string &);

            /** get scene values from resource collection */
            std::vector<std::string> getSceneValues() const;

             /** get the scene name from collections */
            std::string getName() const;

            /** get Id of scene resource */
            std::string getId() const;
             /** get uri details */
            std::string getUri() const;
            /** get address details */
            std::string getAddress() const;

            /** get scene member information */
            std::vector<SceneMemberResource::Ptr> getSceneMembers() const;

            /** find scene member details with the given scene name */
            std::vector<SceneMemberResource::Ptr> findSceneMembers(
                    const std::string & sceneName) const;

           /** get the resource object */
            RCSResourceObject::Ptr getRCSResourceObject() const;

        private:
            class SceneExecuteResponseHandler
            {
            public:
                typedef std::shared_ptr<SceneExecuteResponseHandler> Ptr;

                SceneExecuteResponseHandler()
                : m_numOfMembers(0), m_responseMembers(0), m_errorCode(0)
                {
                }
                ~SceneExecuteResponseHandler() = default;

                int m_numOfMembers;
                int m_responseMembers;
                int m_errorCode;
                std::weak_ptr<SceneCollectionResource> m_owner;
                SceneExecuteCallback m_cb;
                std::mutex m_responseMutex;

                static SceneExecuteResponseHandler::Ptr createExecuteHandler(
                        const SceneCollectionResource::Ptr, SceneExecuteCallback);
                void onResponse(const RCSResourceAttributes &, int);
            };

            class SceneCollectionRequestHandler
            {
            public:
                SceneCollectionRequestHandler() = default;
                ~SceneCollectionRequestHandler() = default;

                std::weak_ptr<SceneCollectionResource> m_owner;

                RCSSetResponse onSetRequest(
                        const RCSRequest &, RCSResourceAttributes &);

            private:
                RCSSetResponse addSceneRequest(
                        const RCSRequest &, RCSResourceAttributes &);
                RCSSetResponse executeSceneRequest(
                        const RCSRequest &, RCSResourceAttributes &);
                RCSSetResponse createSceneMemberRequest(
                        const RCSRequest &, RCSResourceAttributes &);
                RCSSetResponse setSceneCollectionName(
                        const RCSRequest &, RCSResourceAttributes &);

                void addMemberInfoFromRemote(SceneMemberResource::Ptr,
                        std::vector<RCSResourceAttributes>);
            };

            std::string m_uri;
            std::string m_address;

            RCSResourceObject::Ptr m_sceneCollectionResourceObject;
            mutable std::mutex m_sceneMemberLock;
            std::vector<SceneMemberResource::Ptr> m_sceneMembers;

            SceneCollectionRequestHandler m_requestHandler;

            SceneCollectionResource();

            SceneCollectionResource(const SceneCollectionResource &) = delete;
            SceneCollectionResource & operator = (
                    const SceneCollectionResource &) = delete;

            SceneCollectionResource(SceneCollectionResource &&) = delete;
            SceneCollectionResource & operator = (
                    SceneCollectionResource &&) = delete;

            /** create the new resource object */
            RCSResourceObject::Ptr createResourceObject();
            /** set the default attributes value */
            void setDefaultAttributes();
            /** intialize the request handler */
            void initSetRequestHandler();
        };
    }
}

#endif // SCENE_COLLECTION_RESOURCE_OBJECT_H

