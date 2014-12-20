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



#if !defined(_CMResourceHandlerFactory_H)
#define _CMResourceHandlerFactory_H

#include "CMServerResourceHandler.h"
#include "string"
#include "SubscriptionConfig.h"

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            class SUBSCRIPTIONAPI SubscriptionResourceHandlerFactory : public
                OC::Cm::Server::ResourceHandler::ResourceHandlerFactory
            {

                public:

                    SubscriptionResourceHandlerFactory();

                    ~SubscriptionResourceHandlerFactory();

                    virtual OC::Cm::Server::ResourceHandler::IResourceHandler *createResourceHandler(int resourceType);

            };

        }
    }
}

#endif  //_CMResourceHandlerFactory_H
