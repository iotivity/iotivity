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

#include <PrimitiveServerResource.h>

#include <string>
#include <functional>
#include <vector>

#include "OCPlatform.h"

using namespace OC;

PrimitiveServerResource::PrimitiveServerResource(OCResourceHandle &baseResourceHandle)
{
    m_resourceHandle = baseResourceHandle;
}

PrimitiveServerResource::Builder::Builder(const std::string &uri, const std::string &type,
        const std::string &interface)
{
    m_uri = uri;
    m_type = type;
    m_interface = interface;
    m_property = 0;
}

PrimitiveServerResource::Builder &PrimitiveServerResource::Builder::setDiscoverable(
    bool discoverable)
{
    //set flag
    if (discoverable)
        m_property |= OC_DISCOVERABLE;
    else
        m_property ^ = OC_DISCOVERABLE;
    return *this;
}

PrimitiveServerResource::Builder &PrimitiveServerResource::Builder::setObservable(bool observable)
{
    //set flag
    if (observable)
        m_property |= OC_OBSERVABLE;
    else
        m_property ^ = OC_OBSERVABLE;
    return *this;
}

PrimitiveServerResource::Builder &PrimitiveServerResource::Builder::setAttributes(
    const ResourceAttributes &attrs)
{
    //set Attributemap
    return *this;
}

PrimitiveServerResource PrimitiveServerResource::Builder::create() const
{
    //TODO: EntityHandler param change
    OCResourceHandle handle = NULL;
    EntityHandler cb = std::bind(&PrimitiveServerResource::entityHandler, this, std::placeholders::_1);
    OCStackResult result = OCPlatform::registerResource( handle, m_uri,
                           m_type, m_interface, cb, m_property);

    if (OC_STACK_OK != result)
    {
        //TODO: Throw error exception.
    }
    return PrimitiveServerResource(handle);
}

OCEntityHandlerResult PrimitiveServerResource::entityHandler(std::shared_ptr<OCResourceRequest>
        request)
{
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if (request)
    {
        //TODO: simplify Code.

        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if (requestFlag & RequestHandlerFlag::RequestFlag)
        {
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            // If the request type is GET
            if (requestType == "GET")
            {
                cout << "\t\t\trequestType : GET\n";
                //TODO: implementation sevral method for "default" or "custom"
//              pResponse->setErrorCode(200);
//              pResponse->setResponseResult(OC_EH_OK);
//              pResponse->setResourceRepresentation(get());
//              if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
//              {
//                  ehResult = OC_EH_OK;
//              }
            }
            else if (requestType == "PUT")
            {
                cout << "\t\t\trequestType : PUT\n";
                //TODO: implementation sevral method for "default" or "custom"

//                OCRepresentation rep = request->getResourceRepresentation();
//
//                // Do related operations related to PUT request
//                // Update the lightResource
//                put(rep);
//                pResponse->setErrorCode(200);
//                pResponse->setResponseResult(OC_EH_OK);
//                pResponse->setResourceRepresentation(get());
//                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
//                {
//                    ehResult = OC_EH_OK;
//                }
            }
        }

        if (requestFlag & RequestHandlerFlag::ObserverFlag)
        {
            ObservationInfo observationInfo = request->getObservationInfo();
            if (ObserveAction::ObserveRegister == observationInfo.action)
            {
                m_interestedObservers.push_back(observationInfo.obsId);
            }
            else if (ObserveAction::ObserveUnregister == observationInfo.action)
            {
                m_interestedObservers.erase(std::remove(
                                                m_interestedObservers.begin(),
                                                m_interestedObservers.end(),
                                                observationInfo.obsId),
                                            m_interestedObservers.end());
            }

            pthread_t threadId;

            cout << "\t\trequestFlag : Observer\n";
            gObservation = 1;
            static int startedThread = 0;

            // Observation happens on a different thread in ChangeLightRepresentation function.
            // If we have not created the thread already, we will create one here.
            if (!startedThread)
            {
                pthread_create (&threadId, NULL, ChangeLightRepresentation, (void *)this);
                startedThread = 1;
            }
            ehResult = OC_EH_OK;
        }
    }
    else
    {
        std::cout << "Request invalid" << std::endl;
    }

    return ehResult;
}


template<typename T>
void PrimitiveServerResource::setAttribute(const std::string &key, const T &value)
{

}

template<typename T>
T PrimitiveServerResource::getAttribute(T &key) const
{

}

bool PrimitiveServerResource::hasAttribute(const std::string &key) const
{

}

const ResourceAttributes &PrimitiveServerResource::getAttributes() const
{

}
ResourceAttributes &PrimitiveServerResource::getAttributes()
{

}

bool PrimitiveServerResource:: isObservable() const
{

}

bool PrimitiveServerResource::isDiscoverable() const
{

}

void PrimitiveServerResource::setGetRequestHandler(GetRequestHandler)
{

}

void PrimitiveServerResource::setSetRequestHandler(SetRequestHandler)
{

}

void PrimitiveServerResource::notify()
{

}

std::string PrimitiveServerResource::getUri() const
{

}

std::vector<std::string> PrimitiveServerResource::getTypes() const
{

}

std::vector<std::string> PrimitiveServerResource::getInterfaces() const
{

}


