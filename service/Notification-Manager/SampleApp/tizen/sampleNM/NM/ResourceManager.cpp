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

ResourceManager *ResourceManager::instance = NULL;
std::list<VirtualRepresentation> ResourceManager::resourceList;

std::string ResourceManager::extraStr;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

ResourceManager *ResourceManager::getInstance()
{
	if(!instance)
	{
		instance = new ResourceManager();
	}
	return instance;
}

VirtualRepresentation ResourceManager::findVirtualRepresentation(std::string uri)
{

	VirtualRepresentation retObject;

	for(auto it = resourceList.begin(); it != resourceList.end(); it++)
	{
		if(it->getUri().compare(uri) == 0)
		{
			retObject = *it;
			return retObject;
		}
	}

	return retObject;
}

OCStackResult ResourceManager::findNMResource(const std::string& host, const std::string& resourceName, bool ishosting)
{
	return NotificationManager::getOCPlatform()->findResource(host, resourceName,
			std::function<void(std::shared_ptr<OCResource> resource)>(std::bind(&ResourceManager::foundResourceforhosting,this,std::placeholders::_1)));
}

void ResourceManager::foundResourceforhosting(std::shared_ptr<OCResource> resource)
{
	try
	{
		if(resource)
		{
#if ISFORLINUX
			if( resource->uri() == "/a/NM/TempHumSensor")
			{
				std::cout << "Resource Found!!" << endl;
				std::cout << endl;
				std::cout << "========================================================" << endl;
				std::cout << "Hosting Resource : " << std::endl;

				std::cout << "\tResource URI: " << resource->uri() << std::endl;
				std::cout << "\tResource TypeName: " << *(resource->getResourceTypes().data()) << std::endl;
				std::cout << "\tResource Interface: " << *(resource->getResourceInterfaces().data()) << std::endl;
			}
			else
			{
				std::cout << "But is not host uri" << endl;
				return;
			}
			std::cout << endl;
			std::cout << "========================================================" << endl;
			std::cout << "[Virtualization] ";
#endif
			NotificationManager::getInstance()->getOnFoundHostingCandidate()(resource);
		}
		else
		{
		}

	}
	catch(std::exception& e)
	{
	}
}

void ResourceManager::startHosting(std::shared_ptr<OCResource> resource)
{
	VirtualRepresentation resourceObject;
	resourceObject.setUri( resource->uri() );

	std::cout << "resourceObject uri: " << resourceObject.getUri() << std::endl;

	std::string resourceHostIP;
	std::string resourceType;
	std::string resourceInterface;
	uint8_t resourceProperty;

	resourceHostIP	= resource->host();
	resourceType		= *(resource->getResourceTypes().data());
	resourceInterface	= *(resource->getResourceInterfaces().data());
	resourceProperty	= (OC_DISCOVERABLE | resource->isObservable());

	resourceObject.setHostIP(resourceHostIP);
	resourceObject.setResourceTypeName(resourceType);
	resourceObject.setResourceInterface(resourceInterface);
	resourceObject.setResourceProperty(resourceProperty);

	RegistrationManager::getInstance()->registerNMResource(resourceObject, resource);

	resourceList.push_back(resourceObject);

}

AttributeMap ResourceManager::copyAttributeMap(AttributeMap &inputAttMap)
{

	AttributeMap retAttMap;

	for(auto it = inputAttMap.begin(); it != inputAttMap.end(); ++it)
	{
		AttributeValues tmpVal;

		for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
		{
			std::string tmpStr;
#if ISFORLINUX
			if(it->first == "temperature")
			{
				tmpStr.append("\"");
				tmpStr.append(extraStr);
				tmpStr.append(":");
				tmpStr.append(*valueItr);
				tmpStr.append("\"");
			}
			else
			{
				tmpStr.append(*valueItr);
			}

#endif
#if ISFORTIZEN
			if(it->first == "temperature" && !extraStr.empty())
			{
				tmpStr.append("\"");
				tmpStr.append(extraStr);
				tmpStr.append(":");
				tmpStr.append(*valueItr);
				tmpStr.append("\"");
			}
			else
			{
				tmpStr.append(*valueItr);
			}
#else
			tmpStr.append(*valueItr);
#endif
			tmpVal.push_back(tmpStr);
		}
		retAttMap[it->first] = tmpVal;

	}
	return retAttMap;
}

bool ResourceManager::isEmptyAttributeMap(AttributeMap &inputAttMap)
{
	for(auto it=inputAttMap.begin(); it != inputAttMap.end(); ++it)
	{
		if(inputAttMap.find(it->first) == inputAttMap.end())
		{
			return true;
		}
	}
	return false;
}

void ResourceManager::onFoundReport(std::shared_ptr<OCResource> resource)
{
	NotificationManager::getInstance()->getStartHosting()(resource);
}

void ResourceManager::printAttributeMap(AttributeMap &inputAttMap)
{

#if ISFORLINUX
	for(auto it = inputAttMap.begin(); it != inputAttMap.end(); ++it)
	{
		std::cout << "\tAttribute name: " << it->first << " value: ";

		for(auto valueItr = it->second.begin(); valueItr != it->second.end(); ++valueItr)
		{
			std::cout << "\t" << *valueItr << " ";
		}

		std::cout << std::endl;
	}
#endif
#if ISFORTIZEN

#endif

}

void ResourceManager::addExtraStr(std::string str)
{
	extraStr = str;
}

std::string ResourceManager::getExtraStr()
{
	return extraStr;
}

void ResourceManager::checkResourceDBPolicy() {

}

void ResourceManager::saveResourceDB() {

}
