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

#ifndef _NS_PROVIDER_SERVICE_SIMULATOR_H_
#define _NS_PROVIDER_SERVICE_SIMULATOR_H_

#include <iostream>

#include "OCPlatform.h"
#include "OCApi.h"
#include "OCResourceResponse.h"

namespace
{
    enum class requestType
    {
        NS_NOTIFICATION,
        NS_MESSAGE,
        NS_SYNC,
    };
}

class NSProviderSimulator
{
private:
    OCResourceHandle m_notificationHandle;
    OCResourceHandle m_messageHandle;
    OCResourceHandle m_syncHandle;
    OC::OCRepresentation m_syncRep;
    OC::OCRepresentation m_messageRep;
    int m_accepter;

    std::string m_notificationUri;
    std::string m_messageUri;
    std::string m_syncUri;

    OC::ObservationIds m_syncObservers;

public:
    NSProviderSimulator()
    : m_notificationHandle(), m_messageHandle(), m_syncHandle(),
      m_syncRep(), m_messageRep(), m_accepter(0),
      m_notificationUri(std::string("/notification")),
      m_messageUri(std::string("/message")),
      m_syncUri(std::string("/sync")),
      m_syncObservers()
    {

    };

    ~NSProviderSimulator() = default;

    NSProviderSimulator(const NSProviderSimulator &) = delete;
    NSProviderSimulator & operator = (const NSProviderSimulator &) = delete;

    NSProviderSimulator(NSProviderSimulator &&) = delete;
    NSProviderSimulator & operator = (NSProviderSimulator &&) = delete;

private:
    std::shared_ptr<OC::OCResourceResponse> getResponse(
            std::shared_ptr< OC::OCResourceRequest > requests, requestType type)
    {
        auto response = std::make_shared<OC::OCResourceResponse>();
        response->setRequestHandle(requests->getRequestHandle());
        response->setResourceHandle(requests->getResourceHandle());

        int requestFlag = requests->getRequestHandlerFlag();
        if (requestFlag == OC::RequestHandlerFlag::RequestFlag)
        {
            std::string request = requests->getRequestType();

            response->setErrorCode(200);
            response->setResponseResult(OC_EH_OK);

            if (request == "GET")
            {
                OC::OCRepresentation rep;

                if (type == requestType::NS_NOTIFICATION)
                {
                    std::string msgUri = m_notificationUri + m_messageUri;
                    std::string syncUri = m_notificationUri + m_syncUri;
                    std::string providerId = "123456789012345678901234567890123456";
                    rep.setValue("ACCEPTER", m_accepter);
                    rep.setValue("MESSAGE_URI", msgUri);
                    rep.setValue("SYNC_URI", syncUri);
                    rep.setValue("PROVIDER_ID", providerId);
                }
                else if (type == requestType::NS_SYNC)
                {
                    rep = m_syncRep;
                }
                else if (type == requestType::NS_MESSAGE)
                {
                    rep = m_messageRep;
                }
                else
                {
                    return NULL;
                }

                response->setResourceRepresentation(rep);
                return response;
            }

            else if (request == "POST" && type == requestType::NS_SYNC)
            {
                m_syncRep = requests->getResourceRepresentation();

                std::cout << "Receive POST at Sync" << std::endl;
                std::cout << "Sync Id : " << m_syncRep.getValueToString("ID") << std::endl;
                std::cout << "Sync State : " << m_syncRep.getValueToString("STATE") << std::endl;

                response->setResourceRepresentation(m_syncRep);

                OC::OCPlatform::notifyListOfObservers(m_syncHandle, m_syncObservers, response);

                return response;
            }
        }

        return NULL;
    }

    void setObserver(std::shared_ptr< OC::OCResourceRequest > requests, requestType type)
    {
        if (type == requestType::NS_SYNC)
        {
            OC::ObservationInfo observationInfo = requests->getObservationInfo();
            if (OC::ObserveAction::ObserveRegister == observationInfo.action)
            {
                m_syncObservers.push_back(observationInfo.obsId);
            }
            else if (OC::ObserveAction::ObserveUnregister == observationInfo.action)
            {
                m_syncObservers.erase(std::remove(
                        m_syncObservers.begin(), m_syncObservers.end(),
                        observationInfo.obsId), m_syncObservers.end());
            }
        }
        else if (type == requestType::NS_MESSAGE)
        {
            OC::OCRepresentation rep;
            std::string providerId = "123456789012345678901234567890123456";
            rep.setValue<int>("MESSAGE_ID", (int)1);
            rep.setValue("PROVIDER_ID", providerId);

            auto response = std::make_shared<OC::OCResourceResponse>();
            response->setRequestHandle(requests->getRequestHandle());
            response->setResourceHandle(requests->getResourceHandle());
            response->setErrorCode(200);
            response->setResponseResult(OC_EH_OK);
            response->setResourceRepresentation(rep);

            OC::ObservationIds ids;
            ids.push_back(requests->getObservationInfo().obsId);

            OC::OCPlatform::notifyListOfObservers(m_messageHandle, ids, response);
        }
    }

    OCEntityHandlerResult entityHandler(
            std::shared_ptr< OC::OCResourceRequest > request, requestType type)
    {
        if (!request)
        {
            return OC_EH_ERROR;
        }

        std::cout << "Provider : Income request : " << request->getRequestHandlerFlag() << std::endl;
        if ((request->getRequestHandlerFlag() & OC::RequestHandlerFlag::ObserverFlag))
        {
            std::cout << "Provider : Income Observe : " << std::endl;
            setObserver(request, type);
            return OC_EH_OK;
        }

        auto pResponse = getResponse(request, type);
        if (pResponse == nullptr)
        {
            return OC_EH_ERROR;
        }

        try
        {
            OC::OCPlatform::sendResponse(pResponse);
        }
        catch (std::exception & e)
        {
            return OC_EH_ERROR;
        }

        return OC_EH_OK;
    }

public:

    void setAccepter(int accepter)
    {
        m_accepter = accepter;
    }

    void notifyMessage()
    {
        std::cout << "Provider : notify~" << std::endl;
        OC::OCPlatform::notifyAllObservers(m_messageHandle);
    }

    void notifyMessage(const uint64_t & id, const std::string & title, const std::string & content)
    {
        setMessage(id, title, content);
        notifyMessage();
    }

    void sendRead(const uint64_t & id)
    {
        std::string providerId = "123456789012345678901234567890123456";
        m_syncRep.setValue<int>("MESSAGE_ID", id);
        m_syncRep.setValue("STATE", (int)1);
        m_syncRep.setValue("PROVIDER_ID", providerId);
        OC::OCPlatform::notifyAllObservers(m_syncHandle);
    }
    void sendDismiss(const uint64_t & id)
    {
        std::string providerId = "123456789012345678901234567890123456";
        m_syncRep.setValue<int>("MESSAGE_ID", id);
        m_syncRep.setValue("STATE", (int)2);
        m_syncRep.setValue("PROVIDER_ID", providerId);
        OC::OCPlatform::notifyAllObservers(m_syncHandle);
    }

    void setMessage(const uint64_t & id, const std::string & title, const std::string & content)
    {
        std::string providerId = "123456789012345678901234567890123456";
        m_messageRep.setValue<int>("MESSAGE_ID", id);
        m_messageRep.setValue("TITLE", title);
        m_messageRep.setValue("CONTENTTEXT", content);
        m_messageRep.setValue("PROVIDER_ID", providerId);
    }

    void deleteNotificationResource()
    {
        OC::OCPlatform::unregisterResource(m_notificationHandle);
        OC::OCPlatform::unregisterResource(m_messageHandle);
        OC::OCPlatform::unregisterResource(m_syncHandle);
    }

    void createNotificationResource()
    {
        createNotificationResource(m_notificationUri);
    }

    void createNotificationResource(const std::string & uri)
    {
        if (m_notificationUri != uri)
        {
            m_notificationUri = uri;
        }

        OC::OCPlatform::startPresence(30);

        std::string notificationUri = m_notificationUri;
        std::string resourceTypeName = "oic.r.message.notification";
        std::string resourceInterface = OC::DEFAULT_INTERFACE;

        uint8_t resourceProperty = OC_OBSERVABLE;
        std::string childUri = uri + m_messageUri;
        try
        {
            OC::OCPlatform::registerResource(
                    m_messageHandle, childUri,
                    resourceTypeName, resourceInterface,
                    std::bind(& NSProviderSimulator::entityHandler, this,
                            std::placeholders::_1, requestType::NS_MESSAGE),
                            resourceProperty);
        }
        catch (std::exception & e)
        {
            std::cout << e.what() << std::endl;
        }

        resourceTypeName = "oic.r.sync.notification";
        childUri = uri + m_syncUri;
        try
        {
            OC::OCPlatform::registerResource(
                    m_syncHandle, childUri,
                    resourceTypeName, resourceInterface,
                    std::bind(& NSProviderSimulator::entityHandler, this,
                            std::placeholders::_1, requestType::NS_SYNC),
                            resourceProperty);
        }
        catch (std::exception & e)
        {
            std::cout << e.what() << std::endl;
        }

        resourceProperty = OC_DISCOVERABLE;
        resourceTypeName = "oic.r.notification";
        try
        {
            OC::OCPlatform::registerResource(
                    m_notificationHandle, notificationUri,
                    resourceTypeName, resourceInterface,
                    std::bind(& NSProviderSimulator::entityHandler, this,
                            std::placeholders::_1, requestType::NS_NOTIFICATION),
                            resourceProperty);
        }
        catch (std::exception & e)
        {
            std::cout << e.what() << std::endl;
        }
    }
};

#endif /* _NS_PROVIDER_SERVICE_SIMULATOR_H_ */
