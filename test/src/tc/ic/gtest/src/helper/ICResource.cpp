#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>
#include <unistd.h>

#include "ocstack.h"
#include "ocpayload.h"
#include "rd_client.h"
#include "rd_server.h"
#include "rd_database.h"

#include <OCApi.h>
#include <OCPlatform.h>
#include "ICHelper.h"

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

    pResponse->setErrorCode(200);
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

     resourceResponse->setErrorCode(200);
     resourceResponse->setResourceRepresentation(getRepresentation(), DEFAULT_INTERFACE);

     return OCPlatform::notifyListOfObservers(m_handle,
     m_interestedObservers,
     resourceResponse);
    }

    return OC_STACK_OK;
}
