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



#include "DeviceResource.h"
#include "xsd/Elements.h"

DeviceResource::DeviceResource(OC::Cm::IContext *context, std::string &protocol,
                               std::string &deviceAddress)
    : OC::Cm::Client::Resource::Resource(context, protocol, deviceAddress)
{
    p_respListener = NULL ;
    p_internalResponseListener = new DeviceResourceListenerInternal(this);
}

DeviceResource::~DeviceResource()
{
    if (p_internalResponseListener != NULL)
        delete p_internalResponseListener;
}

void
DeviceResource::addResponseListener (IDeviceResourceResponseListener &respListener)
{
    p_respListener = &respListener ;
}

IDeviceResourceResponseListener *
DeviceResource::getResponseListener ()
{
    return p_respListener;
}

bool
DeviceResource::getDevice( int &requestId )

{
    OC::Cm::CMError  error = this->get(requestId, NULL, *p_internalResponseListener);
    return (error.getErrorCode() == OC::Cm::CM_SUCCESS) ;
}

bool
DeviceResource::putDevice( int &requestId , ::Device &reqData)

{
    OC::Cm::CMError  error = this->put(requestId, NULL,
                                       (OC::Cm::Serialization::ISerializable *)&reqData, *p_internalResponseListener);
    return (error.getErrorCode() == OC::Cm::CM_SUCCESS) ;
}

bool
DeviceResource::deleteDevice( int &requestId )

{
    OC::Cm::CMError  error = this->deleteResource(requestId, NULL, *p_internalResponseListener);
    return (error.getErrorCode() == OC::Cm::CM_SUCCESS) ;
}

void
DeviceResource::buildPath( std::string deviceId)

{
    buildTemplatePath("/devices/{deviceId}" , deviceId.c_str());
}
