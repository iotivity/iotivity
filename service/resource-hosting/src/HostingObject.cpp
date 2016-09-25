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

#include "HostingObject.h"

#include "RCSSeparateResponse.h"
#include "RequestObject.h"

namespace OIC
{
    namespace Service
    {

        namespace
        {
            const auto sizeofHostingTag = strlen("/hosting");
        }

        HostingObject::HostingObject()
        : remoteObject(nullptr), mirroredServer(nullptr),
          pDataUpdateCB(nullptr), pDestroyCB(nullptr)
        {
        }

        HostingObject::~HostingObject()
        {
            pDataUpdateCB = {};

            if (remoteObject)
            {
                remoteObject->stopMonitoring();
                remoteObject->stopCaching();
            }
        }

        auto HostingObject::getRemoteResource() const -> RemoteObjectPtr
        {
            return remoteObject;
        }

        auto HostingObject::createHostingObject(const RemoteObjectPtr & rResource,
                DestroyedCallback destroyCB) -> Ptr
        {
            auto newObject = std::make_shared<HostingObject>();

            newObject->remoteObject = rResource;
            newObject->pDestroyCB = destroyCB;

            newObject->pDataUpdateCB = std::bind(&HostingObject::dataChangedCB, newObject,
                    std::placeholders::_1);

            newObject->remoteObject->startMonitoring(
                    std::bind(&HostingObject::stateChangedCB, newObject,
                            std::placeholders::_1));
            newObject->remoteObject->startCaching(newObject->pDataUpdateCB);

            return newObject;
        }

        void HostingObject::destroyHostingObject()
        {
            if (pDestroyCB) pDestroyCB();
        }

        void HostingObject::stateChangedCB(ResourceState state)
        {
            switch (state)
            {
            case ResourceState::ALIVE:
            {
                if (!this->remoteObject->isCaching())
                {
                    try
                    {
                        this->remoteObject->startCaching(pDataUpdateCB);
                    } catch (const RCSException & e)
                    {
                        OIC_HOSTING_LOG(DEBUG,
                                "[HostingObject::stateChangedCB]startCaching InvalidParameterException:%s",
                                e.what());
                    }
                }
                break;
            }
            case ResourceState::LOST_SIGNAL:
            case ResourceState::DESTROYED:
            {
                try
                {
                    this->remoteObject->stopCaching();
                    this->remoteObject->stopMonitoring();
                } catch (const RCSException & e)
                {
                    OIC_HOSTING_LOG(DEBUG,
                            "[HostingObject::stateChangedCB]stopWatching InvalidParameterException:%s",
                            e.what());
                }
                mirroredServer.reset();
                destroyHostingObject();
                break;
            }
            default:
                // not support of state
                break;
            }
        }

        void HostingObject::dataChangedCB(const RCSResourceAttributes & attributes)
        {
            if (attributes.empty())
            {
                return;
            }

            std::unique_lock<std::mutex> lock(mutexForCB);
            if (mirroredServer == nullptr)
            {
                try
                {
                    mirroredServer = createMirroredServer(this->remoteObject);
                } catch (const RCSException & e)
                {
                    OIC_HOSTING_LOG(DEBUG,
                                "[HostingObject::dataChangedCB]createMirroredServer Exception:%s",
                                e.what());
                    return;
                }
            }
            lock.unlock();

            RCSResourceObject::LockGuard guard(mirroredServer);
            mirroredServer->getAttributes() = std::move(attributes);
        }

        auto HostingObject::createMirroredServer(RemoteObjectPtr rObject) -> ResourceObjectPtr
        {
            if (rObject == nullptr)
            {
                throw RCSPlatformException(OC_STACK_ERROR);
            }

            std::string fulluri = rObject->getUri();
            std::string uri = fulluri.substr(0, fulluri.size() - sizeofHostingTag);
            std::vector<std::string> types = rObject->getTypes();
            std::vector<std::string> interfaces = rObject->getInterfaces();
            try
            {
                auto resourceBuild = RCSResourceObject::Builder(uri, types[0], interfaces[0]);
                for (unsigned int i = 1; i < types.size(); ++i)
                {
                    resourceBuild.addType(types[i]);
                }
                for (unsigned int i = 1; i < interfaces.size(); ++i)
                {
                    resourceBuild.addInterface(interfaces[i]);
                }
                auto retResource = resourceBuild.build();

                retResource->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy::UPDATED);
                retResource->setSetRequestHandler(
                        std::bind(&HostingObject::setRequestHandler, this,
                                std::placeholders::_1, std::placeholders::_2));
                return retResource;
            } catch (...)
            {
                OIC_HOSTING_LOG(DEBUG, "[HostingObject::createMirroredServer] %s", "Exception");
                throw;
            }
        }

        RCSSetResponse HostingObject::setRequestHandler(const RCSRequest & primitiveRequest,
                    RCSResourceAttributes & resourceAttibutes)
        {
            try
            {
                RequestObject::invokeRequest(getRemoteResource(),
                        primitiveRequest, RequestObject::RequestMethod::Set, resourceAttibutes);

            } catch (const RCSPlatformException & e)
            {
                OIC_HOSTING_LOG(DEBUG,
                        "[HostingObject::setRequestHandler] PlatformException:%s",
                        e.what());
                throw;
            }

            return RCSSetResponse::separate();
        }

    } /* namespace Service */
} /* namespace OIC */
