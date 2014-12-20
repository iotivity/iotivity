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



#if !defined(_ISERVERLISTENER_H)
#define _ISERVERLISTENER_H

#include "string"
#include "CMConfig.h"
#include "CMErrorCode.h"


//using namespace std;

// Forward declarations
namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            class ControlRequest;

            class ControlResponse;

            namespace Server
            {
                class ServerSession;
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
            namespace Server
            {
                /**
                 * @interface   IServerListener
                 * @brief       Status Listener of a Server Connector
                 *
                 * @since       1.0
                 *
                 * This interface provides listener methods to receive status and requests from a server connector.
                 * Control Manager framework will register a IServerListener with server connector.
                 * Server connector is supposed to invoke this registered IServerListener on receiving the requests.

                 */
                class CMAPI IServerListener
                {
                    public:

                        /**
                         * Default destructor of this class
                         */
                        virtual ~IServerListener() {}

                        /**
                         * listener method which will be invoked when server starts
                         *
                         * @param[in] serverId  id of the server
                         * @param[in] address   address of the server which is started
                         */
                        virtual void onServerStarted(int serverId, std::string address) = 0;

                        /**
                         * listener method which will be invoked when server stops
                         *
                         * @param[in] serverId  id of the server
                         */
                        virtual void onServerStopped(int serverId) = 0;

                        /**
                         * listener method which will be invoked when server encounters an error
                         *
                         * @param[in] serverId  Represents server identifier
                         * @param[in] errorCode Represents error code
                         */
                        virtual void onServerError(int serverId, OC::Cm::ErrorCode errorCode) = 0;

                        /**
                         * listener method which will be invoked when server starts a session
                         *
                         * @param[in] serverId  Represents server identifier
                         * @param[in] session   Represents session instance
                         */
                        virtual void onServerSessionStarted(int serverId,
                                                            OC::Cm::Connector::Server::ServerSession &session) = 0;


                        /**
                         * listener method which will be invoked when a server session completes
                         *
                         * @param[in] serverId  Represents server identifier
                         * @param[in] session   Represents session instance
                         */
                        virtual void onServerSessionCompleted(int serverId,
                                                              OC::Cm::Connector::Server::ServerSession &session) = 0;


                        /**
                         * listener method which will be invoked when server receives a request
                         *
                         * @param[in] session   server session
                         *
                         * @return  returns @c true if request is processed successfully @n
                         *                  @c false in case of any error
                         */
                        virtual bool handleRequest( OC::Cm::Connector::Server::ServerSession &session ) = 0;
                };
            }
        }
    }
}

#endif  //_ISERVERLISTENER_H
