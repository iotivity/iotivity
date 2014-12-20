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

#ifndef CDResourceHandlerFactory_H_
#define CDResourceHandlerFactory_H_

#include "IResourceHandler.h"
#include "ResourceHandlerFactory.h"

using namespace std;

/**
 * @class   CDResourceHandlerFactory
 * @brief   The resource handler factory is which returns the proper handler as per resource type.
 *
 * This class defines methods to handle requests received corresponding to a Capability resource.
 */

class CDResourceHandlerFactory: public OC::Cm::Server::ResourceHandler::ResourceHandlerFactory
{
    public:

        /**
         * CDResourceHandlerFactory - Default constructor of CDResourceHandlerFactory class.
         *
         * @param -none
         * @return -none
         */
        CDResourceHandlerFactory();

        /**
         * !CDResourceHandlerFactory - Default constructor of CDResourceHandlerFactory class.
         *
         * @param -none
         * @return -none
         */
        virtual ~CDResourceHandlerFactory();

        /**
         * createResourceHandler - Create a resource handler based on the type of resource.
         *
         * @param -resourceType - The type of the resource to be handled.
         * @return -type:IResourceHandler* - Handler
         */
        virtual OC::Cm::Server::ResourceHandler::IResourceHandler *createResourceHandler(
            int resourceType);
        /**
         * deleteResourceHandler - Delete a resource handler based on the type of resource.
         *
         * @param - handler - The handler to delete
         * @param -resourceType - The type of the resource to be handled.
         * @return -type:IResourceHandler* - Handler
         */
        virtual bool deleteResourceHandler(
            OC::Cm::Server::ResourceHandler::IResourceHandler *handler,
            int resourceType);

        OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *pApplicationFactory;/**< Represents the Application Factory Class instance. */
        OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *pSubscriptionFactory;/**< Represents the Subscription Factory Class instance. */
};

#endif /* CCResourceHandlerFactory_H_ */
