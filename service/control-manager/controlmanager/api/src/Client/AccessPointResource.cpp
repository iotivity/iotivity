//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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



#include "AccessPointResource.h"
#include "xsd/Elements.h"



AccessPointResource::AccessPointResource(OC::Cm::IContext *context, std::string &protocol,
        std::string &deviceAddress)
    : OC::Cm::Client::Resource::Resource(context, protocol, deviceAddress)
{
    p_respListener = NULL ;
    p_internalResponseListener = new AccessPointResourceListenerInternal(this);
}

AccessPointResource::~AccessPointResource()
{
    if (p_internalResponseListener != NULL)
        delete p_internalResponseListener;
}

void
AccessPointResource::addResponseListener (IAccessPointResourceResponseListener &respListener)
{
    p_respListener = &respListener ;
}

IAccessPointResourceResponseListener *
AccessPointResource::getResponseListener ()
{
    return p_respListener;
}

bool
AccessPointResource::getAccessPoint( int &requestId )

{
    OC::Cm::CMError  error = this->get(requestId, NULL, *p_internalResponseListener);
    return (error.getErrorCode() == OC::Cm::CM_SUCCESS) ;
}

void
AccessPointResource::buildPath( std::string deviceId, std::string networkId,
                                std::string accesspointId)

{
    buildTemplatePath("/devices/{deviceId}/configuration/networks/{networkId}/wifi/accesspoints/{accesspointId}"
                      , deviceId.c_str(), networkId.c_str(), accesspointId.c_str());
}
