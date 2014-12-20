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



#ifndef _CMSERVERCONNECTOR_H
#define _CMSERVERCONNECTOR_H

//#include "Connector/Server/IServerListener.h"
#include "Connector/Server/ServerSession.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            /**
             * @namespace   OC::Cm::Connector::Server
             * @brief   This namespace contains classes and interfaces which defines a server connector
             * @since   1.0
             * @remarks @b Header @b file: @b \#include @b <CMConnector.h> @n
             *          @b Library : @b Shp
             *
             * This namespace contains classes and interfaces which defines a server connector
             *
             * @section ServerConnectorOverView The Server Connector Namespace
             *
             *  -# @ref ServerConnectorOverviewConnector
             *  -# @ref ServerConnectorServerConnector
             *      @n @ref ServerConnectorHTTPServerConnector
             *      @n @ref ServerConnectorRemoteServerConnector
             *
             *
             * @section ServerConnectorOverviewConnector Overview
             * Control Manager framework uses connectors to send and receive requests. connectors acts
             * as transport abstraction for Control Manager framework.
             * The Connector namespace defines interfaces and classes that
             * let the application to implement a connector to use a transport medium of their choice.
             * For Example, if an application wants to use there own HTTP server, they can implement corresponding
             * Server connector and configure it with Control Manager.
             *
             *
             * @section ServerConnectorServerConnector Server Connector
             * Control Manager framework uses server connector to receive requests from other devices. Control Manager framework internally
             * implements two server connectors. They are HTTP Server connector and Remote Server connector
             * If an application wants to use there own HTTP Server, they can implement an HTTP server connector and
             * configure it with OC::Cm::Configuration::setServerConnector()
             *
             * OC::Cm::Connector::Server namespace defines interfaces and classes required to implement a Server connector
             *
             * @subsection ServerConnectorHTTPServerConnector 1. HTTP Server Connector
             * HTTP Server connector is used to receive requests from other devices using HTTP protocol. It is nothing but a
             * HTTP Server.
             *
             * @subsection ServerConnectorRemoteServerConnector 2. HTTPS Server Connector
             * HTTPS Server connector is used to receive requests from other devices using HTTPS protocol.
             */
            namespace Server
            {

            }
        }
    }
}

#endif /* _CMSERVERCONNECTOR_H */
