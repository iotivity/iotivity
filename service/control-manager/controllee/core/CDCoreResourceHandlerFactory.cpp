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

#include "CDCoreResourceHandlerFactory.h"

using namespace std;

CDCoreResourceHandlerFactory::CDCoreResourceHandlerFactory()
{
    pApplicationFactory = NULL;
    pSubscriptionFactory = NULL;
}

CDCoreResourceHandlerFactory::~CDCoreResourceHandlerFactory()
{
    if (NULL != pSubscriptionFactory)
    {
        delete pSubscriptionFactory;
    }
}

void CDCoreResourceHandlerFactory::setContext(OC::Cm::IContext *context)
{
    if (NULL != pSubscriptionFactory)
    {
        pSubscriptionFactory->setContext(context);
    }

    if (NULL != pApplicationFactory)
    {
        pApplicationFactory->setContext(context);
    }

    this->p_context = context;
}

int CDCoreResourceHandlerFactory::getResourceType(std::string path,
        std::map<std::string, std::string>& templateParams)
{
    int resourceType = -1;

    if (NULL != pSubscriptionFactory)
    {
        resourceType = pSubscriptionFactory->getResourceType(path,
                       templateParams);
    }

    if (-1 == resourceType)
    {
        if (NULL != pApplicationFactory)
        {
            resourceType = pApplicationFactory->getResourceType(path,
                           templateParams);
        }
    }

    return resourceType;
}

OC::Cm::Server::ResourceHandler::IResourceHandler *
CDCoreResourceHandlerFactory::createResourceHandler(int resourceType)
{
    OC::Cm::Server::ResourceHandler::IResourceHandler *pResourceHandler = NULL;

    if (NULL != pSubscriptionFactory)
    {
        pResourceHandler = pSubscriptionFactory->createResourceHandler(
                               resourceType);
    }

    if ( NULL == pResourceHandler)
    {
        if (NULL != pApplicationFactory)
        {
            pResourceHandler = pApplicationFactory->createResourceHandler(
                                   resourceType);
        }
    }

    return pResourceHandler;
}

bool CDCoreResourceHandlerFactory::deleteResourceHandler(
    OC::Cm::Server::ResourceHandler::IResourceHandler *handler,
    int resourceType)
{

    bool bRet = false;

    if (NULL != pSubscriptionFactory)
    {
        bRet = pSubscriptionFactory->deleteResourceHandler(handler,
                resourceType);
    }

    if (!bRet)
    {
        if (NULL != pApplicationFactory)
        {
            bRet = pApplicationFactory->deleteResourceHandler(
                       handler, resourceType);
        }
    }

    return bRet;
}
