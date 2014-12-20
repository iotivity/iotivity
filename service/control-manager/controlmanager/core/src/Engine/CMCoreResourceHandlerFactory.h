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



#ifndef CMCoreResourceHandlerFactory_H_
#define CMCoreResourceHandlerFactory_H_

#include "Server/ResourceHandler/IResourceHandler.h"
#include "Server/ResourceHandler/ResourceHandlerFactory.h"

using namespace std ;

class CMCoreResourceHandlerFactory : public OC::Cm::Server::ResourceHandler::ResourceHandlerFactory
{
    public:

        CMCoreResourceHandlerFactory();

        virtual ~CMCoreResourceHandlerFactory();

        void setContext(OC::Cm::IContext *context) ;

        virtual OC::Cm::Server::ResourceHandler::IResourceHandler *createResourceHandler(int resourceType);

        int getResourceType( std::string path, std::map<std::string, std::string> &templateParams ) ;

        OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *pApplicationFactory ;

        OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *pSubscriptionFactory ;
};

#endif /* CMResourceHandlerFactory_H_ */
