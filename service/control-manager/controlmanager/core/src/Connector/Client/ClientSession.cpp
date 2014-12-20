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



#include "CMClientConnector.h"
#include "CMClientResource.h"
#include "Thread/Mutex.h"
#include "Connector/Client/ClientSessionData.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            namespace Client
            {

                ClientSession::ClientSession(int clientId)
                    : Session(clientId)
                {
                    m_pClientSessionData = new ClientSessionData() ;
                    forceApplicationSerializables = false ;
                }

                ClientSession::~ClientSession()
                {
                    if (NULL != m_pClientSessionData)
                    {
                        delete m_pClientSessionData ;
                        m_pClientSessionData = NULL;
                    }
                }

                ClientSessionData &
                ClientSession::getSessionData()
                {
                    return *m_pClientSessionData ;
                }

                int
                ClientSession::getClientId() const
                {
                    return m_Id ;
                }

            }
        }
    }
}
