//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

//******************************************************************
// File name:
//     OCPlatform.cpp
//
// Description: Implementation of the OCPlatform.
//
//
//
//*********************************************************************

#include <random>
#include <utility>
#include <functional>

#include "ocstack.h"

#include "OCPlatform.h"
#include "OCApi.h"
#include "OCException.h"
#include "OCUtilities.h"

#include "oc_logger.hpp"

namespace OC
{
void OCPlatform::init(const PlatformConfig& config)
{
    switch(config.mode)
     {
       case ModeType::Server:
                       m_server = m_WrapperInstance->CreateServerWrapper(*this, m_csdkLock, config);
                       break;

       case ModeType::Client:
                      m_client = m_WrapperInstance->CreateClientWrapper(*this, m_csdkLock, config);
                      break;

       case ModeType::Both:
                       m_server = m_WrapperInstance->CreateServerWrapper(*this, m_csdkLock, config);
                       m_client = m_WrapperInstance->CreateClientWrapper(*this, m_csdkLock, config);
                       break;
     }
}

OCPlatform::OCPlatform(const PlatformConfig& config)
 : m_log_stream      { std::move(oc_log_stream {oc_make_ostream_logger}) },
   m_cfg             { config },
   m_WrapperInstance { make_unique<WrapperFactory>() },
   m_csdkLock        { make_shared<std::recursive_mutex>() }
{
    init(m_cfg);
}

OCPlatform::OCPlatform(const PlatformConfig& config, OC::oc_log_stream& log_target)
 :  m_log_stream      { log_target },
    m_cfg             { config },
    m_WrapperInstance { make_unique<WrapperFactory>() },
    m_csdkLock        { make_shared<std::recursive_mutex>() }
{
    init(m_cfg);
}

OCPlatform::~OCPlatform(void)
{
}

OCStackResult OCPlatform::setDefaultDeviceEntityHandler(EntityHandler entityHandler)
{
    return checked_guard(m_server, &IServerWrapper::setDefaultDeviceEntityHandler,
                         entityHandler);
}

OCStackResult OCPlatform::notifyAllObservers(OCResourceHandle resourceHandle)
{
    return result_guard(OCNotifyAllObservers(resourceHandle));
}

OCStackResult OCPlatform::notifyListOfObservers(OCResourceHandle resourceHandle,
                                            ObservationIds& observationIds,
                                            const std::shared_ptr<OCResourceResponse> pResponse)
{
    if(!pResponse)
    {
     return result_guard(OC_STACK_ERROR);
    }

    std::string payload(pResponse->getPayload());

    return result_guard(
               OCNotifyListOfObservers(resourceHandle,
                                       &observationIds[0], observationIds.size(),
                                       reinterpret_cast<unsigned char *>(const_cast<char *>(payload.c_str()))));
}

OCResource::Ptr OCPlatform::constructResourceObject(const std::string& host, const std::string& uri,
                                                    bool isObservable, const std::vector<std::string>& resourceTypes,
                                                    const std::vector<std::string>& interfaces)
{
    if(!m_client)
    {
        return std::shared_ptr<OCResource>();
    }

    return std::shared_ptr<OCResource>(new OCResource(m_client, host, uri, isObservable, resourceTypes, interfaces));
}

OCStackResult OCPlatform::findResource(const std::string& host, const std::string& resourceName,
                                       FindCallback resourceHandler)
{
    return checked_guard(m_client, &IClientWrapper::ListenForResource,
                         host, resourceName, resourceHandler);
}


OCStackResult OCPlatform::registerResource(OCResourceHandle& resourceHandle,
                                           std::string& resourceURI,
                                           const std::string& resourceTypeName,
                                           const std::string& resourceInterface,
                                           EntityHandler entityHandler,
                                           uint8_t resourceProperty)
{
    return checked_guard(m_server, &IServerWrapper::registerResource,
                         ref(resourceHandle), resourceURI, resourceTypeName,
                                resourceInterface, entityHandler, resourceProperty);
}

OCStackResult OCPlatform::unregisterResource(const OCResourceHandle& resourceHandle) const
{
    return checked_guard(m_server, &IServerWrapper::unregisterResource,
                         resourceHandle);
}

OCStackResult OCPlatform::unbindResource(OCResourceHandle collectionHandle, OCResourceHandle resourceHandle)
{
    return result_guard(OCUnBindResource(ref(collectionHandle), ref(resourceHandle)));
}

OCStackResult OCPlatform::unbindResources(const OCResourceHandle collectionHandle, const std::vector<OCResourceHandle>& resourceHandles)
{
    for(const auto& h : resourceHandles)
    {
       OCStackResult r;

       if(OC_STACK_OK != (r = result_guard(OCUnBindResource(collectionHandle, h))))
       {
           return r;
       }
    }

    return OC_STACK_OK;
}

OCStackResult OCPlatform::bindResource(const OCResourceHandle collectionHandle, const OCResourceHandle resourceHandle)
{
    return result_guard(OCBindResource(collectionHandle, resourceHandle));
}

OCStackResult OCPlatform::bindResources(const OCResourceHandle collectionHandle, const std::vector<OCResourceHandle>& resourceHandles)
{
    for(const auto& h : resourceHandles)
    {
       OCStackResult r;

       if(OC_STACK_OK != (r = result_guard(OCBindResource(collectionHandle, h))))
       {
           return r;
       }
    }

    return OC_STACK_OK;
}

OCStackResult OCPlatform::bindTypeToResource(const OCResourceHandle& resourceHandle,
                                             const std::string& resourceTypeName) const
{
    return checked_guard(m_server, &IServerWrapper::bindTypeToResource,
                         resourceHandle, resourceTypeName);
}

OCStackResult OCPlatform::bindInterfaceToResource(const OCResourceHandle& resourceHandle,
                                                  const std::string& resourceInterfaceName) const
{
    return checked_guard(m_server, &IServerWrapper::bindInterfaceToResource,
                         resourceHandle, resourceInterfaceName);
}

OCStackResult OCPlatform::startPresence(const unsigned int announceDurationSeconds)
{
    return checked_guard(m_server, &IServerWrapper::startPresence,
                         announceDurationSeconds);
}

OCStackResult OCPlatform::stopPresence()
{
    return checked_guard(m_server, &IServerWrapper::stopPresence);
}

OCStackResult OCPlatform::subscribePresence(OCPresenceHandle& presenceHandle, const std::string& host,
                                            SubscribeCallback presenceHandler)
{
    return checked_guard(m_client, &IClientWrapper::SubscribePresence,
                         &presenceHandle, host, presenceHandler);
}

OCStackResult OCPlatform::unsubscribePresence(OCPresenceHandle presenceHandle)
{
    return checked_guard(m_client, &IClientWrapper::UnsubscribePresence,
                         ref(presenceHandle));
}

} //namespace OC
