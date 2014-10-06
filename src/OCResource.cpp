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
#include "OCUtilities.h"

namespace OC {

using OC::nil_guard;
using OC::result_guard;
using OC::checked_guard;

OCResource::OCResource(std::weak_ptr<IClientWrapper> clientWrapper, const std::string& host,
                       const std::string& uri, bool observable, const std::vector<std::string>& resourceTypes,
                       const std::vector<std::string>& interfaces) 
 :  m_clientWrapper(clientWrapper), m_uri(uri), m_host(host), m_isObservable(observable),
    m_isCollection(false), m_resourceTypes(resourceTypes), m_interfaces(interfaces),
    m_observeHandle(nullptr)
{
    m_isCollection = std::find(m_interfaces.begin(), m_interfaces.end(), LINK_INTERFACE)
                        != m_interfaces.end();

    if (m_uri.empty() ||
        resourceTypes.empty() ||
        interfaces.empty()||
        m_clientWrapper.expired())
    {
        throw ResourceInitException(m_uri.empty(), resourceTypes.empty(),
                interfaces.empty(), m_clientWrapper.expired());
    }
}

OCResource::~OCResource()
{
}

OCStackResult OCResource::get(const QueryParamsMap& queryParametersMap,
                              GetCallback attributeHandler)
{
    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::GetResourceRepresentation,
                         m_host, m_uri, queryParametersMap, attributeHandler);
}

OCStackResult OCResource::get(const std::string& resourceType,
                              const std::string& resourceInterface, const QueryParamsMap& queryParametersMap,
                              GetCallback attributeHandler)
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

    return result_guard(get(mapCpy, attributeHandler));
}

OCStackResult OCResource::put(const OCRepresentation& rep,
                              const QueryParamsMap& queryParametersMap, PutCallback attributeHandler)
{
    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PutResourceRepresentation,
                         m_host, m_uri, rep, queryParametersMap, attributeHandler);
}

OCStackResult OCResource::put(const std::string& resourceType,
                              const std::string& resourceInterface, const OCRepresentation& rep,
                              const QueryParamsMap& queryParametersMap,
                              PutCallback attributeHandler)
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

    return result_guard(put(rep, mapCpy, attributeHandler));
}

OCStackResult OCResource::post(const OCRepresentation& rep,
                               const QueryParamsMap& queryParametersMap, PostCallback attributeHandler)
{
    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         m_host, m_uri, rep, queryParametersMap, attributeHandler);
}

OCStackResult OCResource::post(const std::string& resourceType,
                               const std::string& resourceInterface, const OCRepresentation& rep,
                               const QueryParamsMap& queryParametersMap,
                               PostCallback attributeHandler)
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

    return result_guard(post(rep, mapCpy, attributeHandler));
}

OCStackResult OCResource::observe(ObserveType observeType,
                                  const QueryParamsMap& queryParametersMap, ObserveCallback observeHandler)
{
    if(m_observeHandle != nullptr)
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::ObserveResource,
                         observeType, &m_observeHandle, m_host,
                         m_uri, queryParametersMap, observeHandler);
}

OCStackResult OCResource::cancelObserve()
{
    if(m_observeHandle == nullptr)
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::CancelObserveResource,
                         m_observeHandle, m_host, m_uri);
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
