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



#include "NotificationsResource.h"
#include "xsd/Elements.h"

NotificationsResource::NotificationsResource(OC::Cm::IContext *context, std::string &protocol,
        std::string &deviceAddress)
    : OC::Cm::Client::Resource::Resource(context, protocol, deviceAddress)
{
    p_respListener = NULL ;
    p_internalResponseListener = new NotificationsResourceListenerInternal(this);
}

NotificationsResource::~NotificationsResource()
{
    if (p_internalResponseListener != NULL)
        delete p_internalResponseListener;
}

void
NotificationsResource::addResponseListener (INotificationsResourceResponseListener &respListener)
{
    p_respListener = &respListener ;
}

INotificationsResourceResponseListener *
NotificationsResource::getResponseListener ()
{
    return p_respListener;
}

bool
NotificationsResource::postNotifications( int &requestId , ::Notification &reqData)

{
    OC::Cm::CMError  error = this->post(requestId, NULL,
                                        (OC::Cm::Serialization::ISerializable *)&reqData, *p_internalResponseListener);
    return (error.getErrorCode() == OC::Cm::CM_SUCCESS) ;
}

void
NotificationsResource::buildPath(  )

{
    buildTemplatePath("/notifications");
}
