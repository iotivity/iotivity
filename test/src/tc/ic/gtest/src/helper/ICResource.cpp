/******************************************************************
*
* Copyright 2017 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "ICHelper.h"
#include "ICResource.h"
using namespace OC;
using namespace std;

ICResource::ICResource(string uri, vector<string> rt, vector<string> itf)
{
    m_representation.setUri(uri);
    m_representation.setResourceTypes(rt);
    m_representation.setResourceInterfaces(itf);
}

string ICResource::getResourceUri()
{
    return m_representation.getUri();
}

vector<string> ICResource::getResourceType()
{
    return m_representation.getResourceTypes();
}

vector<string> ICResource::getInterfaces()
{
    return m_representation.getResourceInterfaces();
}

OCRepresentation ICResource::getRepresentation(void)
{
    m_representation.clearChildren();
    for (auto it = m_childResources.begin(); it != m_childResources.end(); it++)
    {
      m_representation.addChild((*it)->getRepresentation());
    }
    return m_representation;
}

OCStackResult ICResource::addChildResource(ICResource  *childResource)
{
    m_childResources.push_back(childResource);
    return OCPlatform::bindResource(m_handle, childResource->m_handle);
}

OCStackResult ICResource::sendRepresentation(shared_ptr<OCResourceRequest> pRequest)
{
    auto pResponse = make_shared<OC::OCResourceResponse>();
    pResponse->setRequestHandle(pRequest->getRequestHandle());
    pResponse->setResourceHandle(pRequest->getResourceHandle());

    QueryParamsMap queryParamsMap = pRequest->getQueryParameters();

    cout << "\t\t\tquery params: \n";
    for (auto it = queryParamsMap.begin(); it != queryParamsMap.end(); it++)
    {
      cout << "\t\t\t\t" << it->first << ":" << it->second << endl;
    }

    auto findRes = queryParamsMap.find("if");

    if (findRes != queryParamsMap.end())
    {
      pResponse->setResourceRepresentation(getRepresentation(), findRes->second);
    }
    else
    {
      pResponse->setResourceRepresentation(getRepresentation(), DEFAULT_INTERFACE);
    }

    pResponse->setResponseResult(OC_EH_OK);

    return OCPlatform::sendResponse(pResponse);
}

OCStackResult ICResource::propagate()
{
    if (m_interestedObservers.size() > 0)
    {
     shared_ptr<OCResourceResponse> resourceResponse =
     {
       make_shared<OCResourceResponse>()
     };
     resourceResponse->setResourceRepresentation(getRepresentation(), DEFAULT_INTERFACE);

     return OCPlatform::notifyListOfObservers(m_handle,
     m_interestedObservers,
     resourceResponse);
    }

    return OC_STACK_OK;
}
