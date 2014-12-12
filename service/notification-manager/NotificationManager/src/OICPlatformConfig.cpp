/*
 * OICPlatformConfig.cpp
 *
 *  Created on: 2014. 10. 15.
 *      Author: jyong2
 */

#include "OICPlatformConfig.h"


OICPlatformConfig *OICPlatformConfig::s_instance = NULL;
mutex OICPlatformConfig::s_mutexForCreation;
//OCPlatform *OICPlatformConfig::s_nmOCPlatform = NULL;
PlatformConfig OICPlatformConfig::s_cfg;
//(ServiceType::InProc, ModeType::Both,"", 5683, QualityOfService::NonConfirmable);

OICPlatformConfig::OICPlatformConfig()
{
	// TODO Auto-generated constructor stub

}

OICPlatformConfig::~OICPlatformConfig()
{
	// TODO Auto-generated destructor stub
}


OICPlatformConfig *OICPlatformConfig::getInstance()
{
	if(!s_instance)
	{
		s_mutexForCreation.lock();
		if(!s_instance)
		{
			s_instance = new OICPlatformConfig();
			Configure(s_cfg);
		}
		s_mutexForCreation.unlock();
	}

	return s_instance;
}

//void OICPlatformConfig::getOCPlatform()
//{
////	if(!s_nmOCPlatform)
////	{
////		if(s_cfg.ipAddress.empty())
////		{
////			return NULL;
////		}
////		s_nmOCPlatform = new OCPlatform(s_cfg);
////		Configure(s_cfg);
////	}
////	return s_nmOCPlatform;
//}

void OICPlatformConfig::initialize()
{

}

void OICPlatformConfig::setIP(std::string ipaddress)
{
	s_cfg.ipAddress = ipaddress;
}
