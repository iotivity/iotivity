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

namespace OIC
{
namespace Service
{

void OIC_HOSTING_LOG(LogLevel level, const char * format, ...)
{
    if (!format) return;
    char buffer[MAX_LOG_V_BUFFER_SIZE] = {};
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof buffer - 1, format, args);
    va_end(args);
    OCLog(level, PCF("Hosting"), buffer);
}

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
    if(pDestroyCB) pDestroyCB();
}

void HostingObject::stateChangedCB(ResourceState state)
{
    switch (state)
    {
    case ResourceState::ALIVE:
    {
        if(!this->remoteObject->isCaching())
        {
            try
            {
                this->remoteObject->startCaching(pDataUpdateCB);
            }catch(const RCSException &e)
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
        }
        catch(const RCSException &e)
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
    if(attributes.empty())
    {
        return;
    }

    {
        std::unique_lock<std::mutex> lock(mutexForCB);
        if(mirroredServer == nullptr)
        {
            try
            {
                mirroredServer = createMirroredServer(this->remoteObject);
            }catch(const RCSException &e)
            {
                OIC_HOSTING_LOG(DEBUG,
                            "[HostingObject::dataChangedCB]createMirroredServer Exception:%s",
                            e.what());
                return;
            }
        }
    }

    RCSResourceObject::LockGuard guard(mirroredServer);
    mirroredServer->getAttributes() = std::move(attributes);
}

auto HostingObject::createMirroredServer(RemoteObjectPtr rObject) -> ResourceObjectPtr
{
    if(rObject == nullptr)
    {
        throw RCSPlatformException(OC_STACK_ERROR);
    }

    std::string fulluri = rObject->getUri();
    std::string uri = fulluri.substr(0, fulluri.size() - sizeofHostingTag);
    std::vector<std::string> types = rObject->getTypes();
    std::vector<std::string> interfaces = rObject->getInterfaces();
    try
    {
        auto retResource = RCSResourceObject::Builder(uri, types[0], interfaces[0]).build();

        // TODO need to bind types and interfaces
        retResource->setAutoNotifyPolicy(RCSResourceObject::AutoNotifyPolicy::UPDATED);
        retResource->setSetRequestHandler(
                std::bind(&HostingObject::setRequestHandler, this,
                        std::placeholders::_1, std::placeholders::_2));
        return retResource;
    }catch(...)
    {
        OIC_HOSTING_LOG(DEBUG, "[HostingObject::createMirroredServer] %s", "Exception");
        throw;
    }
}

RCSSetResponse HostingObject::setRequestHandler(const RCSRequest & primitiveRequest,
            RCSResourceAttributes & resourceAttibutes)
{
    (void)primitiveRequest;
    try
    {
        RequestObject newRequest = { };
        newRequest.invokeRequest(remoteObject, RequestObject::RequestMethod::Set,
                resourceAttibutes);
    }catch(const RCSPlatformException &e)
    {
        OIC_HOSTING_LOG(DEBUG,
                "[HostingObject::setRequestHandler] PlatformException:%s",
                e.what());
        throw;
    }

    return RCSSetResponse::defaultAction();
}

} /* namespace Service */
} /* namespace OIC */
