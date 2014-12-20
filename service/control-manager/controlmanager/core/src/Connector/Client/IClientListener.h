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



#if !defined(_ICLIENTLISTENER_H)
#define _ICLIENTLISTENER_H

#include "CMConfig.h"
#include "CMErrorCode.h"

// Forward declarations
namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            class ControlResponse;

            namespace Client
            {
                class ClientSession;
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
                /**
                 * @interface   IClientListener
                 * @brief       Status Listener for requests sent using a Client Connector
                 *
                 * @since       1.0
                 *
                 * This interface provides listener methods to receive status of requests sent using a Client connector.
                 * Control Manager framework will register a IClientListener with client connector.
                 * Client connector is supposed to invoke the registered IClientListener on receiving the response of a request.
                 *
                 */
                class CMAPI IClientListener
                {

                    public:

                        /**
                         * virtual destructor of this interface
                         */
                        virtual ~IClientListener() {}

                        /**
                         *
                         * listener method to receive response from a client connector. Control Manager framework will register a
                         * IClientListener with client connector. client connector is supposed to call this method on receiving
                         * the response of a request
                         *
                         *
                         * @param[in] client_sessioin   client session object
                         */

                        virtual void onResponseReceived( OC::Cm::Connector::Client::ClientSession &client_sessioin ) = 0;


                        /**
                         *
                         * listener method to receive response from a client connector. Control Manager framework will register a
                         * IClientListener with client connector. client connector is supposed to call this method on receiving
                         * error response for a request
                         *
                         * @param[in] error         error code
                         * @param[in] client_sessioin   client session object
                         *
                         */
                        virtual void onErrorRecieved( OC::Cm::ErrorCode error,
                                                      OC::Cm::Connector::Client::ClientSession &client_sessioin) = 0;



                        /**
                         *
                         * listener method to notify a specific client session is completed and application can free the resources related to
                         * this session (including session)
                         *
                         * @param[in] client_sessioin   client session object
                         *
                         */
                        virtual void onClientSessionCompleted( OC::Cm::Connector::Client::ClientSession &client_sessioin) =
                            0;

                };
            }
        }
    }
}

#endif  //_ICLIENTLISTENER_H
