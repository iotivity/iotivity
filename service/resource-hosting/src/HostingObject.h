//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef RH_HOSTINGOBJECT_H_
#define RH_HOSTINGOBJECT_H_

#include "logger.h"

#include "RCSRemoteResourceObject.h"
#include "RCSResourceObject.h"

#define OIC_HOSTING_LOG(level, fmt, args...) OIC_LOG_V((level), PCF("Hosting"), fmt, ##args)

namespace OIC
{
    namespace Service
    {

        class HostingObject
        {
        private:
            typedef RCSResourceObject::Ptr ResourceObjectPtr;
            typedef RCSRemoteResourceObject::Ptr RemoteObjectPtr;

        public:
            typedef std::shared_ptr<HostingObject> Ptr;
            typedef std::weak_ptr<HostingObject> wPtr;

            typedef std::function<void()> DestroyedCallback;
            typedef RCSRemoteResourceObject::StateChangedCallback BrokerCallback;
            typedef RCSRemoteResourceObject::CacheUpdatedCallback CacheCallback;
            typedef RCSResourceObject::SetRequestHandler SetRequestHandler;

        public:
            HostingObject();
            ~HostingObject();

            HostingObject(const HostingObject &) = delete;
            HostingObject & operator = (const HostingObject &) = delete;

            HostingObject(HostingObject &&) = delete;
            HostingObject & operator = (HostingObject &&) = delete;

            static HostingObject::Ptr createHostingObject(const RemoteObjectPtr & rResource,
                    DestroyedCallback destroyCB);

            RemoteObjectPtr getRemoteResource() const;

        private:
            RemoteObjectPtr remoteObject;
            ResourceObjectPtr mirroredServer;

            CacheCallback pDataUpdateCB;
            DestroyedCallback pDestroyCB;

            std::mutex mutexForCB;

            ResourceObjectPtr createMirroredServer(RemoteObjectPtr rObject);

            RCSSetResponse setRequestHandler(
                    const RCSRequest & request, RCSResourceAttributes & attributes);

            void destroyHostingObject();

        public:
            void stateChangedCB(ResourceState state);
            void dataChangedCB(const RCSResourceAttributes & attributes);
        };

    } /* namespace Service */
} /* namespace OIC */

#endif /* RH_HOSTINGOBJECT_H_ */
