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



#include "CMServerResourceHandler.h"
#include "CMServerConnector.h"
#include "CMContext.h"

namespace OC
{
    namespace Cm
    {
        namespace Server
        {
            namespace ResourceHandler
            {


                SyncResourceHandler::SyncResourceHandler(IContext *context)
                {
                    this->p_context = context;
                }

                SyncResourceHandler::~SyncResourceHandler()
                {

                }

                bool
                SyncResourceHandler::handleResource( OC::Cm::Connector::Server::ServerSession &session,
                                                     std::string methodId,
                                                     OC::Cm::Serialization::ISerializable  *request,
                                                     OC::Cm::Serialization::ISerializable  *response)
                {
                    int status_code = 200 ;
                    bool bRet = this->handleSyncResource(session, methodId, status_code, request, response);

                    if (bRet)
                    {
                        OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *) p_context ;

                        pContext->m_engine.sendResponse(status_code, response, session);
                        return true ;
                    }
                    else
                    {
                        return false ;
                    }
                }

                bool
                SyncResourceHandler::onInit()
                {
                    return true ;
                }

                bool
                SyncResourceHandler::onDeinit()
                {
                    return true ;
                }

                bool
                SyncResourceHandler::validateRepresentation( OC::Cm::Serialization::ISerializable  *data,
                        const std::string elementName )
                {
                    if ( NULL == data )
                    {
                        return true;
                    }

                    std::string element = data->getElementName() ;

                    if ( 0 == elementName.compare( element ))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }


            }
        }
    }
}
