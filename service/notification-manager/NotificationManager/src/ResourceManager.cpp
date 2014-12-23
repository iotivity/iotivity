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

#include "ResourceManager.h"

ResourceManager *ResourceManager::s_instance = NULL;
mutex ResourceManager::s_mutexForCreation;
std::list< VirtualRepresentation > ResourceManager::s_resourceList;

std::string ResourceManager::s_extraStr;

ResourceManager::ResourceManager()
{
	m_onFoundforHosting	= NULL;
	m_onObserve			= NULL;
	m_notify				= NULL;
}

ResourceManager::~ResourceManager()
{
}

ResourceManager *ResourceManager::getInstance()
{
    if(!s_instance)
    {
        s_mutexForCreation.lock();
        if(!s_instance)
        {
        	s_instance = new ResourceManager();
        }
        s_mutexForCreation.unlock();
    }
    return s_instance;
}

VirtualRepresentation ResourceManager::findVirtualRepresentation(std::string uri)
{

    VirtualRepresentation retObject;

    for(auto it = s_resourceList.begin() ; it != s_resourceList.end() ; it++)
    {
        if(it->getUri().compare(uri) == 0)
        {
            retObject = *it;
            return retObject;
        }
    }

    return retObject;
}

void ResourceManager::findNMResource(bool isHosting)
{
	if(isHosting)
	{
		findResource("" , "coap://224.0.1.187/oc/core",
				std::function< void(std::shared_ptr< OCResource > resource) >(
									std::bind(&ResourceManager::foundResourceforhosting , ResourceManager::getInstance() ,
											std::placeholders::_1)));
	}
}

void ResourceManager::foundResourceforhosting(std::shared_ptr< OCResource > resource)
{
    try
    {
        if(resource)
        {
            if(resource->uri().find("/a/NM") != std::string::npos)
            {
                ResourceManager::getInstance()->m_onFoundforHosting(resource);
            }
        }
        else
        {
        	// TODO
        }

    }
    catch(std::exception &e)
    {
    }
}

void ResourceManager::startHosting(std::shared_ptr< OCResource > resource)
{

	cout << "start hosting" << endl;
    VirtualRepresentation tmp = findVirtualRepresentation( resource->uri() );

    if( !tmp.getUri().empty() )
    {
        return;
    }

    VirtualRepresentation resourceObject;
    resourceObject.setUri(resource->uri());

    std::cout << "resourceObject uri: " << resourceObject.getUri() << std::endl;

    std::string resourceHostIP;
    std::string resourceType;
    std::string resourceInterface;
    uint8_t resourceProperty;

    resourceHostIP = resource->host();
    resourceType = *(resource->getResourceTypes().data());
    resourceInterface = *(resource->getResourceInterfaces().data());
    resourceProperty = (OC_DISCOVERABLE | resource->isObservable());

    resourceObject.setHostIP(resourceHostIP);
    resourceObject.setResourceTypeName(resourceType);
    resourceObject.setResourceInterface(resourceInterface);
    resourceObject.setResourceProperty(resourceProperty);

    RegistrationManager::getInstance()->registerNMResource(resourceObject , resource);

    s_resourceList.push_back(resourceObject);

}

void ResourceManager::notifyObservers(OCResourceHandle resourceHandle)
{
	OCStackResult result = OC_STACK_OK;

	result = notifyAllObservers(resourceHandle);

	if(OC_STACK_NO_OBSERVERS == result)
	{
		// No observers.
		// TODO
	}
}

AttributeMap ResourceManager::copyAttributeMap(AttributeMap &inputAttMap)
{

    AttributeMap retAttMap;

    retAttMap = inputAttMap;
//    for(auto it = inputAttMap.begin() ; it != inputAttMap.end() ; ++it)
//    {
//        AttributeValues tmpVal;
//
//        for(auto valueItr = it->second.begin() ; valueItr != it->second.end() ; ++valueItr)
//        {
//            std::string tmpStr;
//
//            tmpStr.append(*valueItr);
//
//            tmpVal.push_back(tmpStr);
//        }
//        retAttMap[it->first] = tmpVal;
//
//    }
    return retAttMap;
}

bool ResourceManager::isEmptyAttributeMap(AttributeMap &inputAttMap)
{
    for(auto it = inputAttMap.begin() ; it != inputAttMap.end() ; ++it)
    {
        if(inputAttMap.find(it->first) == inputAttMap.end())
        {
            return true;
        }
    }
    return false;
}

void ResourceManager::onFoundforHostingDefault(std::shared_ptr< OCResource > resource)
{
    ResourceManager::getInstance()->startHosting(resource);
}
void ResourceManager::onObserveDefault(AttributeMap &inputAttMap , OCResourceHandle resourceHandle)
{
	ResourceManager::getInstance()->notifyObservers(resourceHandle);
}

void ResourceManager::printAttributeMap(AttributeMap &inputAttMap)
{
    for(auto it = inputAttMap.begin() ; it != inputAttMap.end() ; ++it)
    {
        std::cout << "\tAttribute name: " << it->first << " value: ";

        for(auto valueItr = it->second.begin() ; valueItr != it->second.end() ; ++valueItr)
        {
            std::cout << "\t" << *valueItr << " ";
        }

        std::cout << std::endl;
    }
}

void ResourceManager::addExtraStr(std::string str)
{
    s_extraStr = str;
}

std::string ResourceManager::getExtraStr()
{
    return s_extraStr;
}

void ResourceManager::checkResourceDBPolicy()
{

}

void ResourceManager::saveResourceDB()
{

}
