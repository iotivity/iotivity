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



#if !defined(_CLIENTSESSION_H)
#define _CLIENTSESSION_H

#include "CMConfig.h"
#include "CMConnector.h"
#include "../Session.h"

// Forward Declarations...
namespace OC
{
    namespace Cm
    {
        namespace Client
        {
            namespace Resource
            {
                class IResourceResponseListener;
            }
        }
    }
}

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            namespace Client
            {

                class ClientSessionData ;

                /**
                 * @class       ClientSession
                 * @brief       This class represents a client session.
                 *
                 * @since       1.0
                 *
                 * This class holds the data related to a client session.
                 * In client session's scope, a request will be sent to a controllable
                 *
                 */
                class CMAPI ClientSession : public OC::Cm::Connector::Session
                {

                    public:

                        /**
                         * Constructor of this class
                         *
                         * @param[in] clientId  id of the client to which this session belongs
                         */
                        ClientSession(int clientId);

                        /**
                         * Default destructor of this class
                         */
                        ~ClientSession();

                        /**
                         * This method returns id of client to which this session belongs
                         *
                         * @return  id of the client.
                         */
                        int getClientId() const;

                        /**
                         *
                         * returns object which contains the session related data which framework maintains
                         *
                         * @return response object
                         *
                         */
                        OC::Cm::Connector::Client::ClientSessionData &getSessionData() ;

                        /**
                         *  forces framework to use xsd classes used by application for Control Manager requests/responses which are handled by framework. (Ex: Subscription)
                         */
                        bool forceApplicationSerializables ;

                    private:

                        OC::Cm::Connector::Client::ClientSessionData *m_pClientSessionData ;
                };
            }
        }
    }
}

#endif  //_CLIENTSESSION_H
