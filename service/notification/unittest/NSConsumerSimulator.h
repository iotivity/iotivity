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

#ifndef _NS_CONSUMER_SIMULATOR_H_
#define _NS_CONSUMER_SIMULATOR_H_

#include <iostream>

#include "OCPlatform.h"
#include "OCApi.h"

class NSConsumerSimulator
{
private:
    std::function<void(const std::string&, const std::string&, const std::string&)> m_messageFunc;
    std::function<void(int, const std::string&)> m_syncFunc;

    std::shared_ptr<OC::OCResource> m_syncResource;


public:
    NSConsumerSimulator()
    : m_messageFunc(), m_syncFunc(),
      m_syncResource() { };
    ~NSConsumerSimulator() = default;

    NSConsumerSimulator(const NSConsumerSimulator &) = delete;
    NSConsumerSimulator & operator = (const NSConsumerSimulator &) = delete;

    NSConsumerSimulator(NSConsumerSimulator &&) = delete;
    NSConsumerSimulator & operator = (NSConsumerSimulator &&) = delete;

    void findProvider()
    {
        OC::OCPlatform::findResource("", std::string("/oic/res?rt=oic.r.notification"),
                OCConnectivityType::CT_DEFAULT,
                std::bind(&NSConsumerSimulator::findResultCallback, this, std::placeholders::_1),
                OC::QualityOfService::LowQos);
    }

    void syncToProvider(int & type, const std::string & id)
    {
        if (m_syncResource == nullptr)
        {
            std::cout << "m_syncResource is null" << std::endl;
            return;
        }

        OC::OCRepresentation rep;
        rep.setValue("ID", id);
        rep.setValue("STATE", type);

        m_syncResource->post(rep, OC::QueryParamsMap(), &onPost, OC::QualityOfService::LowQos);
    }

    void setCallback(const std::function<void(const std::string&, const std::string&, const std::string&)> & messageFunc,
            const std::function<void(int, const std::string&)> & syncFunc)
    {
        m_messageFunc = messageFunc;
        m_syncFunc = syncFunc;
    }

private:
    static void onPost(const OC::HeaderOptions &/*headerOption*/,
                const OC::OCRepresentation & /*rep*/ , const int eCode)
    {
        std::cout << __func__ << " result : " << eCode << std::endl;
    }
    void findResultCallback(std::shared_ptr<OC::OCResource> resource)
    {
        std::cout << __func__ << " " << resource->host() << std::endl;
        resource->get(OC::QueryParamsMap(),
                std::bind(&NSConsumerSimulator::onGet, this,
                        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, resource),
                OC::QualityOfService::LowQos);
    }
    void onGet(const OC::HeaderOptions &/*headerOption*/,
            const OC::OCRepresentation & rep , const int eCode,
            std::shared_ptr<OC::OCResource> resource)
    {
        std::cout << __func__ << " " << rep.getHost() << " result : " << eCode << std::endl;

        std::shared_ptr<OC::OCResource> msgResource
            = OC::OCPlatform::constructResourceObject(resource->host(), resource->uri() + "/message",
                    resource->connectivityType(), false, resource->getResourceTypes(),
                    resource->getResourceInterfaces());
        m_syncResource
            = OC::OCPlatform::constructResourceObject(resource->host(), resource->uri() + "/sync",
                    resource->connectivityType(), false, resource->getResourceTypes(),
                    resource->getResourceInterfaces());

        msgResource->observe(OC::ObserveType::Observe, OC::QueryParamsMap(),
                std::bind(&NSConsumerSimulator::onObserve, this,
                        std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4, resource),
                OC::QualityOfService::LowQos);
        m_syncResource->observe(OC::ObserveType::Observe, OC::QueryParamsMap(),
                std::bind(&NSConsumerSimulator::onObserve, this,
                        std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4, resource),
                OC::QualityOfService::LowQos);

    }
    void onObserve(const OC::HeaderOptions &/*headerOption*/,
            const OC::OCRepresentation &rep , const int &eCode, const int &,
            std::shared_ptr<OC::OCResource> )
    {
        std::cout << __func__ << " " << rep.getHost() << " result : " << eCode;
        std::cout << " uri : " << rep.getUri() << std::endl;

        if (rep.getUri() == "/notification/message" && rep.hasAttribute("ID")
                && rep.getValueToString("ID") != "0000-0000-0000-0000")
        {
            std::cout << "ID : " << rep.getValueToString("ID") << std::endl;
            std::cout << "TITLE : " << rep.getValueToString("TITLE") << std::endl;
            std::cout << "CONTENT : " << rep.getValueToString("CONTENT") << std::endl;
            m_messageFunc(std::string(rep.getValueToString("ID")),
                          std::string(rep.getValueToString("TITLE")),
                          std::string(rep.getValueToString("CONTENT")));
        }
        else if (rep.getUri() == "/notification/sync")
        {
            m_syncFunc(int(rep.getValue<int>("STATE")),
                       std::string(rep.getValueToString("ID")));
        }
    }
};


#endif //_NS_CONSUMER_SIMULATOR_H_
