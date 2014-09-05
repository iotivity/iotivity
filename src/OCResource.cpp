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

#include "OCResource.h"
#include "OCReflect.h"

namespace OC {
    OCResource::OCResource(std::weak_ptr<IClientWrapper> clientWrapper, const std::string& host, const std::string& uri, bool observable, const std::vector<std::string>& resourceTypes, const std::vector<std::string>& interfaces) : 
        m_clientWrapper(clientWrapper), m_uri(uri), m_host(host), m_isObservable(observable), m_isCollection(false), m_resourceTypes(resourceTypes), m_interfaces(interfaces), m_observeHandle(nullptr)
    {
        m_isCollection = std::find(m_interfaces.begin(), m_interfaces.end(), LINK_INTERFACE) != m_interfaces.end();

        if (m_uri.empty() || resourceTypes.empty() || interfaces.empty()|| m_clientWrapper.expired())
        {
            throw ResourceInitException(m_uri.empty(), resourceTypes.empty(), interfaces.empty(), m_clientWrapper.expired());
        }
    }

    OCResource::~OCResource()
    {
    }

    OCStackResult OCResource::get(const QueryParamsMap& queryParametersMap, GetCallback attributeHandler)
    {
        auto cw = m_clientWrapper.lock();

        if(cw)
        {
            return cw->GetResourceAttributes(m_host, m_uri, queryParametersMap, attributeHandler);
        }
        else
        {
            return OC_STACK_ERROR;
        }
    }

    OCStackResult OCResource::get(const std::string& resourceType, const std::string& resourceInterface,
        const QueryParamsMap& queryParametersMap, GetCallback attributeHandler)
    {
        auto cw = m_clientWrapper.lock();

        if(cw)
        {
            QueryParamsMap mapCpy(queryParametersMap);

            if(!resourceType.empty())
            {
                mapCpy["rt"]=resourceType;
            }
            if(!resourceInterface.empty())
            {
                mapCpy["if"]= resourceInterface;
            }

            return get(mapCpy, attributeHandler);
        }
        else
        {
            return OC_STACK_ERROR;
        }
    }

    OCStackResult OCResource::put(const OCRepresentation& rep, const QueryParamsMap& queryParametersMap, 
        PutCallback attributeHandler)
    {
        auto cw = m_clientWrapper.lock();

        if(cw)
        {
            return cw->SetResourceAttributes(m_host, m_uri, rep, queryParametersMap, attributeHandler);
        }
        else
        {
            return OC_STACK_ERROR;
        }
    }

    OCStackResult OCResource::put(const std::string& resourceType, const std::string& resourceInterface,
        const OCRepresentation& rep, const QueryParamsMap& queryParametersMap,
        PutCallback attributeHandler)
    {
        auto cw = m_clientWrapper.lock();

        if(cw)
        {
            QueryParamsMap mapCpy(queryParametersMap);

            if(!resourceType.empty())
            {
                mapCpy["rt"]=resourceType;
            }
            if(!resourceInterface.empty())
            {
                mapCpy["if"]=resourceInterface;
            }

            return put(rep, mapCpy, attributeHandler);
        }
        else
        {
            return OC_STACK_ERROR;
        }
    }

    OCStackResult OCResource::observe(ObserveType observeType, const QueryParamsMap& queryParametersMap, 
        ObserveCallback observeHandler)
    {
        if(m_observeHandle != nullptr)
        {
            return OC_STACK_INVALID_PARAM;
        }
        else
        {
            auto cw = m_clientWrapper.lock();

            if(cw)
            {
                return cw->ObserveResource(observeType, &m_observeHandle, m_host, m_uri, queryParametersMap, observeHandler);
            }
            else
            {
                return OC_STACK_ERROR;
            }
        }
    }

    OCStackResult OCResource::cancelObserve()
    {
        if(m_observeHandle == nullptr)
        {
            return OC_STACK_INVALID_PARAM;
        }
        else
        {
            auto cw = m_clientWrapper.lock();

            if(cw)
            {
                OCStackResult ret = cw->CancelObserveResource(m_observeHandle, m_host, m_uri);
                m_observeHandle = nullptr;
                return ret;
            }
            else
            {
                return OC_STACK_ERROR;
            }
        }
    }

    std::string OCResource::host() const 
    {
        return m_host;
    }
    std::string OCResource::uri() const 
    {
        return m_uri;
    }
    bool OCResource::isObservable() const 
    {
        return m_isObservable;
    }
} // namespace OC
