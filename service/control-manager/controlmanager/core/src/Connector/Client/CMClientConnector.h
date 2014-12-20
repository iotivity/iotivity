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



#ifndef _CMCLIENTCONNECTOR_H
#define _CMCLIENTCONNECTOR_H

#include "ClientSession.h"
#include "IClientListener.h"

namespace OC
{
    namespace Cm
    {
        namespace Connector
        {
            /**
             * @namespace   OC::Cm::Connector::Client
             * @brief   This namespace contains classes and interfaces which defines a client connector
             * @since   1.0
             * @remarks @b Header @b file: @b \#include @b <CMClientConnector.h> @n
             *          @b Library : @b Shp
             *
             * This namespace contains classes and interfaces which defines a client connectors
             *
             * @section ClientOverView The Client Connector Namespace
             *
             *  -# @ref ClientConnectorOverviewConnector
             *  -# @ref ClientConnectorClientConnector
             *      @n @ref ClientConnectorHTTPClientConnector
             *      @n @ref ClientConnectorRemoteClientConnector
             *
             *
             * @section ClientConnectorOverviewConnector Overview
             * Control Manager framework uses connectors to send and receive requests. connectors acts
             * as transport abstraction for Control Manager framework.
             * The Connector namespace defines interfaces and classes that
             * let the application to implement a connector to use a transport medium of their choice.
             * For Example, if an application wants to use there own HTTP server, they can implement corresponding
             * Server connector and configure it with Control Manager.
             *
             * @section ClientConnectorClientConnector Client Connector
             * Control Manager framework uses client connector to send requests to other devices. Control Manager framework internally
             * implements two client connectors. They are HTTP client connector and Remote client connector
             * If an application wants to use there own HTTP client, they can implement an HTTP client connector and
             * configure it with OC::Cm::Configuration::setClientConnector()
             *
             * OC::Cm::Connector::Client namespace defines interfaces and classes required to implement a Client connector
             *
             * @subsection ClientConnectorHTTPClientConnector 1. HTTP Client Connector
             * HTTP Client connector is used to send requests to other devices using HTTP protocol. It is nothing but a
             * HTTP client.
             *
             * @subsection ClientConnectorRemoteClientConnector 2. HTTPS Client Connector
             * HTTPS Client connector is used to send requests to other devices using HTTPS protocol.
             *
             */
            namespace Client
            {

            }
        }
    }
}


#endif /* _CMCLIENTCONNECTOR_H */
