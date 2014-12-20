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




#include "CMResourceHandlerFactory.h"
#include "CMResourceHandler.h"


using namespace std ;

MyCMResourceHandlerFactory::MyCMResourceHandlerFactory(jobject dataFactoryObj)
{
    m_resHandlerObj = JNIGlobal::getGlobleRef(dataFactoryObj) ;
    context = (jlong)0; ;
}

void
MyCMResourceHandlerFactory::setContext(jlong context)
{
    this->context = context ;
}

MyCMResourceHandlerFactory::~MyCMResourceHandlerFactory()
{
    JNIGlobal::executeVoidMethod("OC/Cm/Server/ResourceHandler/ResourceHandlerFactory",
                                 "onNativeDelete", "()V", m_resHandlerObj); // SRIB Need to implement
    JNIGlobal::releaseGlobleRef(m_resHandlerObj) ;
}

int
MyCMResourceHandlerFactory::getResourceType( std::string path,
        std::map<std::string, std::string> &templateParams )
{
    int resourceType = -1 ;

    if (-1 == resourceType)
    {
        resourceType = OC::Cm::Server::ResourceHandler::ResourceHandlerFactory::getResourceType(path,
                       templateParams);
    }

    return resourceType;
}

OC::Cm::Server::ResourceHandler::IResourceHandler *
MyCMResourceHandlerFactory::createResourceHandler(int resourceType)
{
    OC::Cm::Server::ResourceHandler::IResourceHandler *pResourceHandler = NULL ;
    if ( NULL == pResourceHandler)
    {
        jobject pHandler = (jobject)
                           JNIGlobal::executeLongMethod("OC/Cm/Server/ResourceHandler/ResourceHandlerFactory",
                                   "createResourceHandler", "(I)J", m_resHandlerObj, resourceType);
        if (pHandler == NULL)
        {
            return NULL ;
        }

        pResourceHandler = new CMResourceHandler(context, pHandler) ;
    }

    return pResourceHandler;
}

