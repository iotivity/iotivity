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



#include "CapabilityResourceHandler.h"
#include "MyDevice.h"


CapabilityResourceHandler::CapabilityResourceHandler(OC::Cm::IContext *pContext)
    : OC::Cm::Server::ResourceHandler::SyncResourceHandler(pContext)
{
}

CapabilityResourceHandler::~CapabilityResourceHandler()
{
}

/******************************** NOTE: DO NOT modify below code ********************************/
bool
CapabilityResourceHandler::handleSyncResource( OC::Cm::Connector::Server::ServerSession &session,
        std::string methodId,
        int &statusCode,
        OC::Cm::Serialization::ISerializable *request,
        OC::Cm::Serialization::ISerializable *response)
{

    if ( 0 == methodId.compare("GET"))
    {
        if ( false == OC::Cm::Server::ResourceHandler::SyncResourceHandler::validateRepresentation(response,
                EN_CAPABILITY )) { statusCode = 404 ; return true ; }
        this->onGET(session, statusCode, (::Capability *)response);
    }
    else
    { statusCode = 405 ; session.getResponse().m_headers.setHeader( "Allow", "GET") ;}

    return true ;
}

bool
CapabilityResourceHandler::onGET( OC::Cm::Connector::Server::ServerSession &session,
                                  int &statusCode, ::Capability *respData)
{
    return MyDevice::getInstance()->getCapability(respData);
}
