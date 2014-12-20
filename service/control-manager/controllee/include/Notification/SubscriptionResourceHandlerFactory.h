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

#if !defined(_SubscriptionResourceHandlerFactory_H)
#define _SubscriptionResourceHandlerFactory_H

#include "IResourceHandler.h"
#include "ResourceHandlerFactory.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            /**
             * @class       SubscriptionResourceHandlerFactory
             * @brief       This is a subscription handler factory which creates the correct handler based on resource type.
             */
            class SubscriptionResourceHandlerFactory: public OC::Cm::Server::ResourceHandler::ResourceHandlerFactory
            {

                public:
                    /**
                     * SubscriptionResourceHandler - Default constructor
                     *
                     * @param -none
                     * @return -none
                     */
                    SubscriptionResourceHandlerFactory();
                    /**
                     * ~SubscriptionResourceHandler - Default destructor
                     *
                     * @param -none
                     * @return -none
                     */
                    ~SubscriptionResourceHandlerFactory();

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
                    bool deleteResourceHandler(
                        OC::Cm::Server::ResourceHandler::IResourceHandler *handler,
                        int resourceType);

            };

        }
    }
}

#endif  //_SubscriptionResourceHandlerFactory_H
