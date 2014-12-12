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

NotificationManager *NotificationManager::s_instance = NULL;
mutex NotificationManager::s_mutexForCreation;

NotificationManager::NotificationManager()
{

}

NotificationManager::NotificationManager(HostingConfig cfg)
{

}

NotificationManager::~NotificationManager()
{

}

void NotificationManager::initialize()
{
	// find local ip address
    std::string ipAddress;
    NotificationManager::getInstance()->scanAndGetNetworkInterface(ipAddress);

    // set ip address
    OICPlatformConfig::getInstance()->setIP(ipAddress);

    // initialize hosting handler
    HostingHandler::initialize();
}

void NotificationManager::initialize(HostingConfig cfg)
{
	// find local ip address
    std::string ipAddress;
    NotificationManager::getInstance()->scanAndGetNetworkInterface(ipAddress);

    // set ip address
    OICPlatformConfig::getInstance()->setIP(ipAddress);

    // initialize hosting handler
    HostingHandler::initialize(cfg);
}

void NotificationManager::registerHostingEventListener()
{
    // TODO : Initial HostingEventListener (v1.0)
}

NotificationManager *NotificationManager::getInstance()
{
	if(!s_instance)
	{
		s_mutexForCreation.lock();
		if(!s_instance)
		{
			s_instance = new NotificationManager();
		}
		s_mutexForCreation.unlock();
	}

    return s_instance;
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
