/*
 * HostingHandler.cpp
 *
 *  Created on: 2014. 10. 15.
 *      Author: jyong2
 */

#include "HostingHandler.h"

HostingHandler *HostingHandler::s_instance = NULL;
mutex HostingHandler::s_mutexForCreation;

std::function< void(bool isHosting) > HostingHandler::s_findHostingCandidate;
std::function< void(std::shared_ptr< OCResource > resource) > HostingHandler::s_startHosting;
std::function< void(OCResourceHandle resourceHandle) > HostingHandler::s_notify;
std::function< void(std::string) > HostingHandler::s_addExtraStr;

HostingHandler::HostingHandler()
{

}

HostingHandler::~HostingHandler()
{

}

HostingHandler *HostingHandler::getInstance()
{
	if(!s_instance)
	{
		s_mutexForCreation.lock();
		if(!s_instance)
		{
			s_instance = new HostingHandler();
		}
		s_mutexForCreation.unlock();
	}

	return s_instance;
}

void HostingHandler::initialize()
{
	// Create Hosting Handler instance
	HostingHandler *ptr = HostingHandler::getInstance();

	// Registration interface about hosting
	HostingInterface hostingInterface;

	hostingInterface.setFindHosting(ptr->s_findHostingCandidate);
	hostingInterface.setStartHosting(ptr->s_startHosting);
	hostingInterface.setNotifyObservers(ptr->s_notify);
	hostingInterface.setAddExtraStr(ptr->s_addExtraStr);

	hostingInterface.setOnFoundHostingCandidate(
			std::function< void(std::shared_ptr< OCResource > resource) >(
					std::bind(&HostingHandler::onFoundCandidate , HostingHandler::getInstance() , std::placeholders::_1)));
	hostingInterface.setOnObserve(
			std::function< void(AttributeMap &inputAttMap, OCResourceHandle resourceHandle) >(
					std::bind(&HostingHandler::onObserve , HostingHandler::getInstance() , std::placeholders::_1, std::placeholders::_2)));

	ptr->s_addExtraStr("virtual");
	ptr->startFindHost();
}

void HostingHandler::initialize(HostingConfig cfg)
{
	// Create Hosting Handler instance
	HostingHandler *ptr = HostingHandler::getInstance();

	// Registration interface about hosting
	HostingInterface hostingInterface;

	hostingInterface.setFindHosting(ptr->s_findHostingCandidate);
	hostingInterface.setStartHosting(ptr->s_startHosting);
	hostingInterface.setNotifyObservers(ptr->s_notify);
	hostingInterface.setAddExtraStr(ptr->s_addExtraStr);

	hostingInterface.setOnFoundHostingCandidate(
			std::function< void(std::shared_ptr< OCResource > resource) >(
					std::bind(&HostingHandler::onFoundCandidate , HostingHandler::getInstance() , std::placeholders::_1)));
	hostingInterface.setOnObserve(
			std::function< void(AttributeMap &inputAttMap, OCResourceHandle resourceHandle) >(
					std::bind(&HostingHandler::onObserve , HostingHandler::getInstance() , std::placeholders::_1, std::placeholders::_2)));

	// Set Hosting Config
	ptr->setHostingConfig(cfg);
	ptr->changeHostingMode(ptr->hostingCfg.hostingMode);

	ptr->s_addExtraStr("virtual");
	ptr->startFindHost();
}

void HostingHandler::setHostingConfig(HostingConfig cfg)
{
	hostingCfg.automaticMethod	= cfg.automaticMethod;
	hostingCfg.frequency		= cfg.frequency;
	hostingCfg.hostingMode		= cfg.hostingMode;
	hostingCfg.notifyMethod		= cfg.notifyMethod;
}

void HostingHandler::changeHostingMode(HostingMode hostingMode, AutomaticMethod autoMethod)
{
	if(hostingCfg.hostingMode != hostingMode)
	{
		hostingCfg.hostingMode = hostingMode;
		if(hostingCfg.hostingMode == HostingMode::AutomaticMode)
		{
			runAutomaticHosting(autoMethod);
		}
		else
		{
			stopAutomaticHosting();
		}
	}
}

void HostingHandler::runAutomaticHosting(AutomaticMethod autoMethod)
{
	// TODO Triggering from Event Listener.
	if(hostingCfg.automaticMethod != autoMethod)
	{
		hostingCfg.automaticMethod = autoMethod;
		switch(hostingCfg.automaticMethod)
		{
		case AutomaticMethod::None:
			break;
		case AutomaticMethod::Timer:
		case AutomaticMethod::DeviceStatus:
		case AutomaticMethod::NetworkStatusChange:
		default:
			break;
		}
	}
}

void HostingHandler::stopAutomaticHosting()
{
	if(hostingCfg.automaticMethod != AutomaticMethod::None)
	{
// TODO
	}
}

void HostingHandler::changeAutomaticHostingMethod(AutomaticMethod autoMethod)
{
//	TODO
}
void HostingHandler::changeNotifiyMethod(NotifyMethod notifyMethod)
{
//	TODO
}
void HostingHandler::changeNotifyFrequencyType(NotifyFrequency notifyFrequency)
{
//	TODO
}

void HostingHandler::startFindHost()
{
	if(hostingCfg.hostingMode != HostingMode::None)
	{
		s_findHostingCandidate(true);
	}
}

void HostingHandler::onFoundCandidate(std::shared_ptr< OCResource > resource)
{
	// TODO
	// Condition of Hosting
	s_startHosting(resource);
}

void HostingHandler::onObserve(AttributeMap &AttMap, OCResourceHandle resourceHandle)
{

	switch(hostingCfg.notifyMethod)
	{
	case NotifyMethod::None:
		break;
	case NotifyMethod::Equalization:
	case NotifyMethod::Granularity:
	case NotifyMethod::Frequence:
	default:
		notifyFrequence(resourceHandle);
		break;
	}
}

void HostingHandler::notifyFrequence(OCResourceHandle resourceHandle)
{

	switch(hostingCfg.frequency)
	{
	case NotifyFrequency::None:
		break;
	case NotifyFrequency::Periodically:
	case NotifyFrequency::OnTime:
	default:
		s_notify(resourceHandle);
		break;
	}
}
