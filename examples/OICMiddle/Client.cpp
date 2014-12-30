//******************************************************************
//
// Copyright 2014 Intel Corporation.
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

#include <map>

#include "WrapResource.h"
#include "Client.h"

MiddleClient::MiddleClient()
{
    m_findCB = bind(&MiddleClient::foundOCResource, this, placeholders::_1);
}

bool MiddleClient::init()
{
    findResources();
    return true;
}

void MiddleClient::findResources()
{
    m_resourceMap.clear();

    OC::OCPlatform::findResource("", OC_WELL_KNOWN_QUERY, m_findCB);
}

void MiddleClient::foundOCResource(shared_ptr<OCResource> resource)
{
    WrapResource *wres;
    string resourceID = formatResourceID(resource);

    m_mutexFoundCB.lock();

    try {
        wres = m_resourceMap.at(resourceID);
    } catch (const std::out_of_range) {
        wres = new WrapResource(resourceID, resource);
        m_resourceMap[resourceID] = wres;
    }

    m_mutexFoundCB.unlock();

    wres->findTypes();
}

/*
 *  I need a unique ID, so I concatenate the host string and resource uri
 *  It's arbitrary and sufficient.
 */
string MiddleClient::formatResourceID(std::shared_ptr<OCResource> resource)
{
    string host = resource->host();
    if (host.compare(0, 7, "coap://") == 0)
        host = host.erase(0, 7);
    return host + resource->uri();
}

void MiddleClient::addResource(WrapResource *wres)
{
    string resourceID = wres->getResourceID();
    try {
        m_resourceMap[resourceID];
    } catch (const std::out_of_range) {
        m_resourceMap[resourceID] = wres;
    }
}
