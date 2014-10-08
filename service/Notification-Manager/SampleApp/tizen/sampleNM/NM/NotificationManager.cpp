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

#include "NotificationManager.h"

NotificationManager *NotificationManager::instance = NULL;
OCPlatform *NotificationManager::nmOCPlatform = NULL;
PlatformConfig NotificationManager::cfg;

NotificationManager::NotificationManager()
{

	print			= NULL;
	onfound		= NULL;
	onObserve		= NULL;
	startHosting	= NULL;
	findHosting	= NULL;
	addExtraStr	= NULL;
}

NotificationManager::~NotificationManager()
{
	if(nmOCPlatform!=NULL)
	{
		delete nmOCPlatform;
		nmOCPlatform = NULL;
	}
}

void NotificationManager::initialize()
{

	std::string ip_address;
	scanAndGetNetworkInterface(ip_address);

	cfg.ipAddress = ip_address;
	cfg.port = 5683;
	cfg.mode = ModeType::Both;
	cfg.serviceType = ServiceType::InProc;

	nmOCPlatform = new OCPlatform(cfg);

	setPrint(NULL);
	setOnFoundHostingCandidate(NULL);
	setStartHosting(NotificationManager::getInstance()->startHosting);
	setFindHosting(NotificationManager::getInstance()->findHosting);
	setAddExtraStr(NotificationManager::getInstance()->addExtraStr);

}

void NotificationManager::registerHostingEvent()
{
	// TODO : Initial HostingEventListener
}

void NotificationManager::findHostingCandidate()
{
	try
	{
		ResourceManager::getInstance()->findNMResource("", "coap://224.0.1.187/oc/core",true);
	}
	catch(OCException e)
	{
		std::cout << "Fail!!" << endl;
	}
}

NotificationManager *NotificationManager::getInstance()
{
	if(!instance)
	{
		instance = new NotificationManager();
	}

	return instance;
}

OCPlatform *NotificationManager::getOCPlatform()
{
	if(!nmOCPlatform)
	{
		nmOCPlatform = new OCPlatform(cfg);
	}
	return nmOCPlatform;
}

int NotificationManager::setPrint( std::function<void(AttributeMap &inputAttMap)> func )
{
	if( func != NULL )
	{
		try
		{
			NotificationManager::getInstance()->print = func;
		}
		catch(exception e)
		{
			return false;
		}
	}
	else
	{
		NotificationManager::getInstance()->print =
				std::function<void(AttributeMap &inputAttMap)>
				(std::bind(&ResourceManager::printAttributeMap,ResourceManager::getInstance(),std::placeholders::_1));
	}
	return true;
}

int NotificationManager::setOnFoundHostingCandidate( std::function<void(std::shared_ptr<OCResource> resource)> func )
{
	if( func != NULL )
	{
		try
		{
			NotificationManager::getInstance()->onfound = func;
		}
		catch(exception e)
		{
			return false;
		}
	}
	else
	{
		NotificationManager::getInstance()->onfound =
				std::function<void(std::shared_ptr<OCResource> resource)>
				(std::bind(&ResourceManager::onFoundReport, ResourceManager::getInstance(),std::placeholders::_1));
	}

	return true;
}

int NotificationManager::setOnObserve( std::function<void(AttributeMap &inputAttMap)> func )
{
	if( func != NULL )
	{
		try
		{
			NotificationManager::getInstance()->onObserve = func;
		}
		catch(exception e)
		{
			return false;
		}
	}
	return true;
}

std::function<void(AttributeMap &inputAttMap)> NotificationManager::getPrint()
{
	return print;
}

std::function<void(std::shared_ptr<OCResource> resource)> NotificationManager::getOnFoundHostingCandidate()
{
	return onfound;
}

std::function<void(AttributeMap &inputAttMap)> NotificationManager::getOnObserve()
{
	return onObserve;
}

int NotificationManager::setStartHosting( std::function<void(std::shared_ptr<OCResource> resource)> &func )
{
	try
	{
		func = std::function<void(std::shared_ptr<OCResource> resource)>
		(std::bind(&ResourceManager::startHosting,ResourceManager::getInstance(),std::placeholders::_1));
	}
	catch(exception e)
	{
		return false;
	}

	return true;
}

int NotificationManager::setFindHosting( std::function<void()> &func )
{
	try
	{
		func = std::function<void()>
		(std::bind(&NotificationManager::findHostingCandidate,NotificationManager::getInstance()));
	}
	catch(exception e)
	{
		return false;
	}

	return true;
}

int NotificationManager::setAddExtraStr( std::function<void(std::string)> &func )
{
	try
	{
		func = std::function<void(std::string str)>
		(std::bind(&ResourceManager::addExtraStr,ResourceManager::getInstance(),std::placeholders::_1));
	}
	catch(exception e)
	{
		return false;
	}

	return true;
}

std::function<void(std::shared_ptr<OCResource> resource)> NotificationManager::getStartHosting()
{
	if(startHosting)
	{
		return startHosting;
	}
	else
	{
		return NULL;
	}
}
std::function<void()> NotificationManager::getFindHosting()
{
	if(findHosting)
	{
		return findHosting;
	}
	else
	{
		return NULL;
	}
}
std::function<void(std::string)> NotificationManager::getAddExtraStr()
{
	if(addExtraStr)
	{
		return addExtraStr;
	}
	else
	{
		return NULL;
	}
}


int NotificationManager::getNetInfo(IN int& sck, IN struct ifreq* item, OUT std::string& ip_addres)
{
	struct ifreq temp_ifr;
	memset(&temp_ifr, 0, sizeof(temp_ifr));
	strcpy(temp_ifr.ifr_name, item->ifr_name);

	if (ioctl(sck, SIOCGIFFLAGS, &temp_ifr))
	{
		return -1;
	}

	if (!((temp_ifr.ifr_flags & IFF_UP) && (temp_ifr.ifr_flags & IFF_RUNNING)))
	{
		return -1;
	}

	std::string ip(inet_ntoa(((struct sockaddr_in *) &item->ifr_addr)->sin_addr));
	if (ip.empty())
	{
		return -1;
	}

	if (ip.find("127.0.0") == 0)
	{
		return -1;
	}

	ip_addres = ip;
	return 0;
}

bool NotificationManager::scanAndGetNetworkInterface(OUT std::string& ip_addres)
{
	while(1)
	{
		char buf[1024] =	{ 0, };
		struct ifconf ifc;
		struct ifreq *ifr;
		int sck;
		int interfaces;
		int i;

		sck = socket(AF_INET, SOCK_DGRAM, 0);
		if (sck < 0)
		{
			usleep(10);
			continue;
		}

		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = buf;
		if (ioctl(sck, SIOCGIFCONF, &ifc) < 0)
		{
			printf( "SIOCGIFCONF Failed ");
			close(sck);
			usleep(10);
			continue;
		}

		ifr = ifc.ifc_req;
		interfaces = ifc.ifc_len / sizeof(struct ifreq);

		for (i = 0; i < interfaces; i++)
		{
			if(  getNetInfo(sck, &ifr[i], ip_addres) == 0 )
			{
				return 0;
			}
			continue;
		}
		close(sck);
		usleep(10);
	}

	return 0;
}
