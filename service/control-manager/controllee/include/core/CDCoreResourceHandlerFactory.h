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

#ifndef CDCoreResourceHandlerFactory_H_
#define CDCoreResourceHandlerFactory_H_

#include "IResourceHandler.h"
#include "ResourceHandlerFactory.h"

using namespace std;

/**
 * @class   CDCoreResourceHandlerFactory
 * @brief   This class is the Core class factory which redirects to appropriate factory
 */
class CDCoreResourceHandlerFactory: public OC::Cm::Server::ResourceHandler::ResourceHandlerFactory
{
    public:

        /**
         * CDCoreResourceHandlerFactory - Constructor
         *
         * @param -none
         * @return -none
         */
        CDCoreResourceHandlerFactory();

        /**
         * ~CDCoreResourceHandlerFactory - Destructor
         *
         * @param -none
         * @return -none
         */
        virtual ~CDCoreResourceHandlerFactory();

        /**
         *setContext - Set the context . This context is needed to get the correct handlers. Set this before
         *                          before using this class
         *
         * @param - context
         * @return -none
         */

        void setContext(OC::Cm::IContext *context);

        /**
         *createResourceHandler -Get the correct response handler based on the response type.
         *
         * @param - int resourceType
         * @return -type:IResourceHandler*
         */
        virtual OC::Cm::Server::ResourceHandler::IResourceHandler *createResourceHandler(
            int resourceType);

        /**
         *deleteResourceHandler -Get the correct response handler based on the response type.
         *
         * @param - int resourceType
         * @param -Resource Handler
         * @return type:bool - true if delete was successful, false otherwise.
         */

        virtual bool deleteResourceHandler(
            OC::Cm::Server::ResourceHandler::IResourceHandler *handler,
            int resourceType);

        /**
         *getResourceType -Get the resource type based on the resource path
         *
         * @param - path - The path of the device.
         * @param -templateParams - is not needed for most functionality.
         * @return type:int - the Resourcetype in form of an integer.
         */
        int getResourceType(std::string path,
                            std::map<std::string, std::string>& templateParams);

        OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *pApplicationFactory;/**< Represents the Application Factory Class instance. */
        OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *pSubscriptionFactory;/**< Represents the Subscription Factory Class instance. */
};

#endif /* CMResourceHandlerFactory_H_ */
