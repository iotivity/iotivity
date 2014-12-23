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

#include "RegistrationManager.h"

RegistrationManager *RegistrationManager::s_instance = NULL;
mutex RegistrationManager::s_mutexForCreation;

RegistrationManager::RegistrationManager()
{
}

RegistrationManager::~RegistrationManager()
{
}

RegistrationManager *RegistrationManager::getInstance()
{
	if(!s_instance)
	{
		s_mutexForCreation.lock();
		if(s_instance)
		{
			s_instance = new RegistrationManager();
		}
		s_mutexForCreation.unlock();
	}

	return s_instance;
}

int RegistrationManager::addResource()
{
    return 0;
}

int RegistrationManager::removeResource()
{
    return 0;
}

int RegistrationManager::updateResource()
{
    return 0;
}

bool RegistrationManager::registerNMResource(VirtualRepresentation &resourceObject ,
        std::shared_ptr< OCResource > resource)
{
    std::string uri = resourceObject.getUri();
    std::string type = resourceObject.getResourceTypeName();
    std::string interface = resourceObject.getResourceInterface();

    OCResourceHandle resourceHandle;

    OCStackResult result;
    result = registerResource(resourceHandle , uri , type ,
                interface ,
                std::function<
                        OCEntityHandlerResult(const std::shared_ptr< OCResourceRequest > request ,
                                const std::shared_ptr< OCResourceResponse > response) >(
                        std::bind(&VirtualRepresentation::entityHandler , resourceObject ,
                        		std::placeholders::_1 , std::placeholders::_2)) ,
                resourceObject.getResourceProperty());

    resourceObject.setResourceHandle(resourceHandle);

    if(OC_STACK_OK != result)
    {
        return false;
    }
    else
    {
        QueryParamsMap queryParmaMap;
        resource->observe(ObserveType::Observe , queryParmaMap ,
                std::function<
                        void(const HeaderOptions headerOption,
                        		const OCRepresentation& rep , const int& eCode ,
                                const int& sequenceNumber) >(
                        std::bind(&VirtualRepresentation::onObserve , resourceObject ,
                                std::placeholders::_1 , std::placeholders::_2 ,
                                std::placeholders::_3 , std::placeholders::_4)));
    }

    return true;
}

bool RegistrationManager::unregisterResource()
{
    return true;
}

