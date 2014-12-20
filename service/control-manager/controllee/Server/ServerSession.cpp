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

#include "ServerSession.h"
#include "Device.h"
#include <string>

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            namespace Server
            {
                ServerSession::ServerSession(int serverId, std::string clientProtocol,
                                             std::string clientAddress) :
                    Session(serverId)
                {
                    m_clientProtocol = clientProtocol;
                    m_clientAddress = clientAddress;
                    //m_pServerSessionData = new ServerSessionData() ;
                }

                ServerSession::~ServerSession()
                {
                    /**
                     if ( m_pServerSessionData )
                     {
                     delete m_pServerSessionData ;
                     }
                     **/
                }

                int ServerSession::getServerId() const
                {
                    return m_Id;
                }

                std::string ServerSession::getClientProtocol() const
                {
                    return m_clientProtocol;
                }

                std::string ServerSession::getClientAddress() const
                {
                    return m_clientAddress;
                }

                OC::Cm::Device *ServerSession::getClientDevice() const
                {
                    return NULL;
                }

                /**
                 ServerSessionData&
                 ServerSession::getSessionData()
                 {
                 return *m_pServerSessionData ;
                 }
                 **/
            }
        }
    }
}
