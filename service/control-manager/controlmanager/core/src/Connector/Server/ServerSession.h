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



#if !defined(_SERVERSESSION_H)
#define _SERVERSESSION_H

#include "string"
#include "CMConfig.h"
#include "CMConnector.h"
#include "../Session.h"

// Forward Declarations...
namespace OC
{
    namespace Cm
    {
        class Device;
    }
}

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            namespace Server
            {

                class ServerSessionData ;

                /**
                 * @class       ServerSession
                 * @brief       This class represents a server session.
                 *
                 * @since       1.0
                 *
                 * This class holds the data related to a server session.
                 * In server session's scope, server processes a request
                 *
                 */
                class CMAPI ServerSession : public OC::Cm::Connector::Session
                {

                    public:
                        /**
                         * Constructor of this class
                         *
                         * @param serverId          id of the server
                         * @param clientProtocol    protocol of client device
                         * @param clientAddress     Address of client device
                         */
                        ServerSession(int serverId, std::string clientProtocol, std::string clientAddress);

                        /**
                         * Default destructor of this class
                         */
                        virtual ~ServerSession();

                        /**
                         * returns the id of the server to which this session belongs
                         *
                         * @return server id
                         */
                        virtual int getServerId() const;

                        /**
                         * returns the client protocol of the device from which request processed by this session received
                         *
                         * @return client protocol of the device
                         */
                        virtual std::string getClientProtocol() const;

                        /**
                         * returns the address of the device from which request processed by this session received
                         *
                         * @return address of the device
                         */
                        virtual std::string getClientAddress() const;

                        /**
                         * returns a device object from which request processed by this session received
                         *
                         * @return device object
                         */
                        virtual OC::Cm::Device *getClientDevice() const;

                        /**
                         *
                         * returns object which contains session related data which is maintained by framework
                         *
                         * @return response object
                         *
                         */
                        ServerSessionData &getSessionData() ;

                        OC::OCResourceRequest::Ptr m_pOCRequest; // Represents the OC request format (OIC)
                        OC::OCResourceResponse::Ptr m_pOCResponse; // Represents the OC response format  (OIC)

                    private:

                        std::string m_clientProtocol;

                        std::string m_clientAddress;

                        ServerSessionData *m_pServerSessionData ;
                };
            }
        }
    }
}

#endif  //_SERVERSESSION_H
