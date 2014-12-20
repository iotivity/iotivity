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




#ifndef CMRESOURCEHANDLERFACTORY_H_
#define CMRESOURCEHANDLERFACTORY_H_


#include <jni.h>
#include "Server/ResourceHandler/IResourceHandler.h"
#include "JNIGlobal.h"
#include "Server/ResourceHandler/ResourceHandlerFactory.h"
//#include "ResourceHandlers/SubscriptionResourceHandlerFactory.h"


using namespace std ;

class MyCMResourceHandlerFactory : public OC::Cm::Server::ResourceHandler::ResourceHandlerFactory
{
    public:

        MyCMResourceHandlerFactory(jobject dataFactoryObj);

        void setContext(jlong context);

        virtual ~MyCMResourceHandlerFactory();

        virtual OC::Cm::Server::ResourceHandler::IResourceHandler *createResourceHandler(int resourceType);

        int getResourceType( std::string path, std::map<std::string, std::string> &templateParams ) ;

        jobject m_resHandlerObj;

        jlong context ;

        //SubscriptionResourceHandlerFactory    subscriptionFactory ;
};

#endif /* CMRESOURCEHANDLERFACTORY_H_ */
