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



#include "AccessPointsResource.h"
#include "xsd/Elements.h"

AccessPointsResource::AccessPointsResource(OC::Cm::IContext *context, std::string &protocol,
        std::string &deviceAddress)
    : OC::Cm::Client::Resource::Resource(context, protocol, deviceAddress)
{
    p_respListener = NULL ;
    p_internalResponseListener = new AccessPointsResourceListenerInternal(this);
}

AccessPointsResource::~AccessPointsResource()
{
    if (p_internalResponseListener != NULL)
        delete p_internalResponseListener;
}

void
AccessPointsResource::addResponseListener (IAccessPointsResourceResponseListener &respListener)
{
    p_respListener = &respListener ;
}

IAccessPointsResourceResponseListener *
AccessPointsResource::getResponseListener ()
{
    return p_respListener;
}

bool
AccessPointsResource::getAccessPoints( int &requestId )

{
    OC::Cm::CMError  error = this->get(requestId, NULL, *p_internalResponseListener);
    return (error.getErrorCode() == OC::Cm::CM_SUCCESS) ;
}

void
AccessPointsResource::buildPath( std::string deviceId, std::string networkId)

{
    buildTemplatePath("/devices/{deviceId}/configuration/networks/{networkId}/wifi/accesspoints" ,
                      deviceId.c_str(), networkId.c_str());
}
