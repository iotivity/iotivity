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



#include "UsageLimitResource.h"
#include "xsd/Elements.h"

UsageLimitResource::UsageLimitResource(OC::Cm::IContext *context, std::string &protocol,
                                       std::string &deviceAddress)
    : OC::Cm::Client::Resource::Resource(context, protocol, deviceAddress)
{
    p_respListener = NULL ;
    p_internalResponseListener = new UsageLimitResourceListenerInternal(this);
}

UsageLimitResource::~UsageLimitResource()
{
    if (p_internalResponseListener != NULL)
        delete p_internalResponseListener;
}

void
UsageLimitResource::addResponseListener (IUsageLimitResourceResponseListener &respListener)
{
    p_respListener = &respListener ;
}

IUsageLimitResourceResponseListener *
UsageLimitResource::getResponseListener ()
{
    return p_respListener;
}

bool
UsageLimitResource::getUsageLimit( int &requestId )

{
    OC::Cm::CMError  error = this->get(requestId, NULL, *p_internalResponseListener);
    return (error.getErrorCode() == OC::Cm::CM_SUCCESS) ;
}

bool
UsageLimitResource::putUsageLimit( int &requestId , ::UsageLimit &reqData)

{
    OC::Cm::CMError  error = this->put(requestId, NULL,
                                       (OC::Cm::Serialization::ISerializable *)&reqData, *p_internalResponseListener);
    return (error.getErrorCode() == OC::Cm::CM_SUCCESS) ;
}

bool
UsageLimitResource::deleteUsageLimit( int &requestId )

{
    OC::Cm::CMError  error = this->deleteResource(requestId, NULL, *p_internalResponseListener);
    return (error.getErrorCode() == OC::Cm::CM_SUCCESS) ;
}

void
UsageLimitResource::buildPath( std::string usagelimitId)

{
    buildTemplatePath("/energy/usagelimits/{usagelimitId}" , usagelimitId.c_str());
}
