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



#ifndef _CMSERVERRESOURCEHANDLER_H
#define _CMSERVERRESOURCEHANDLER_H

#include "CMServerConnector.h"
#include "Server/ResourceHandler/AsyncResourceHandler.h"
#include "Server/ResourceHandler/IResourceHandler.h"
#include "Server/ResourceHandler/ResourceHandlerFactory.h"
#include "Server/ResourceHandler/SyncResourceHandler.h"

namespace OC
{
    namespace Cm
    {
        namespace Server
        {
            /**
             *
             * @namespace   OC::Cm::Server::ResourceHandler
             * @brief   This namespace contains classes and interfaces which are required to implement Control Manager resource handlers
             * @since   1.0
             * @remarks @b Header @b file: @b \#include @b <CMServerResourceHandler.h> @n
             *          @b Library : @b Shp
             *
             * This namespace contains classes and interfaces which are required to implement Control Manager resource handlers
             *
             * @section ResourceHandlerOverView The ResourceHandler Namespace
             *
             *  -# @ref ResourceHandlerOverviewResourceHandler
             *  -# @ref ResourceHandlerResourceHandlerTypes
             *      @n @ref ResourceHandlerAsyncResourceHandler
             *      @n @ref ResourceHandlerSyncResourceHandler
             *
             *
             * @section ResourceHandlerOverviewResourceHandler Overview
             *
             * The ResourceHandler namespace defines interfaces and classes that let the application implement resource handlers.
             * This namespace provides generic classes required to implement resource handlers.
             * Applications will extend these Generic resource handlers to implement actual resource handlers.
             *
             * Control Manager SDK generates empty resource handler stubs which are derived from these generic resource handler classes.
             * Application developers can use these stubs to implement resource handlers.
             *
             * @section ResourceHandlerResourceHandlerTypes Types of Resource Handlers
             * Control Manager framework supports two types of Resource Handlers 1) Asynchronous Resource Handlers 2) Synchronous Resource Handlers
             *
             * @subsection ResourceHandlerAsyncResourceHandler 1) Asynchronous Resource Handler
             * This Resource handlers handles the requests asynchronously. Application must call OC::Cm::Server::ResourceHandler::AsyncResourceHandler::sendResponse()
             * to send response.
             *
             * @subsection ResourceHandlerSyncResourceHandler 2) Synchronous Resource Handler
             * This Resource handlers handles the requests synchronously.
             *
             *
             */
            namespace ResourceHandler
            {

            }
        }
    }
}


#endif /* _CMSERVERRESOURCEHANDLER_H */
