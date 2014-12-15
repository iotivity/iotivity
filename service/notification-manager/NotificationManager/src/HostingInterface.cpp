/*
 * HostingInterface.cpp
 *
 *  Created on: 2014. 10. 15.
 *      Author: jyong2
 */

#include "HostingInterface.h"

HostingInterface::HostingInterface()
{

}

HostingInterface::~HostingInterface()
{
	// TODO Auto-generated destructor stub
}

int HostingInterface::setOnFoundHostingCandidate(
        std::function< void(std::shared_ptr< OCResource > resource) > func)
{
    if(func != NULL)
    {
        try
        {
        	ResourceManager::getInstance()->m_onFoundforHosting = func;
        }
        catch(exception &e)
        {
            return false;
        }
    }
    else
    {
        ResourceManager::getInstance()->m_onFoundforHosting = std::function<
                void(std::shared_ptr< OCResource > resource) >(
                std::bind(&ResourceManager::onFoundforHostingDefault , ResourceManager::getInstance() ,
                        std::placeholders::_1));
    }

    return true;
}

int HostingInterface::setOnObserve(std::function< void(AttributeMap &inputAttMap, OCResourceHandle resourceHandle) > func)
{
    if(func != NULL)
    {
        try
        {
        	ResourceManager::getInstance()->m_onObserve = func;
        }
        catch(exception &e)
        {
            return false;
        }
    }
    else
    {
    	ResourceManager::getInstance()->m_onObserve = std::function<
    	                void(AttributeMap &inputAttMap, OCResourceHandle resourceHandle) >(
    	                std::bind(&ResourceManager::onObserveDefault , ResourceManager::getInstance() ,
    	                        std::placeholders::_1, std::placeholders::_2));
    }

    return true;
}

int HostingInterface::setNotifyObservers(std::function< void(OCResourceHandle resourceHandle) > &func)
{
	try
	{
		func = std::function< void(OCResourceHandle resourceHandle) >(
				std::bind(&ResourceManager::notifyObservers , ResourceManager::getInstance() ,
						std::placeholders::_1));
	}
	catch(exception &e)
	{
		return false;
	}

	return true;
}

int HostingInterface::setStartHosting(
        std::function< void(std::shared_ptr< OCResource > resource) > &func)
{
    try
    {
        func = std::function< void(std::shared_ptr< OCResource > resource) >(
                std::bind(&ResourceManager::startHosting , ResourceManager::getInstance() ,
                        std::placeholders::_1));
    }
    catch(exception &e)
    {
        return false;
    }

    return true;
}

int HostingInterface::setFindHosting(std::function< void(bool isHosting) > &func)
{
    try
    {
        func = std::function< void(bool isHosting) >(
        		std::bind(&ResourceManager::findNMResource ,
        				ResourceManager::getInstance() ,
        					std::placeholders::_1));
    }
    catch(exception &e)
    {
        return false;
    }

    return true;
}

int HostingInterface::setAddExtraStr(std::function< void(std::string) > &func)
{
    try
    {
        func = std::function< void(std::string str) >(
                std::bind(&ResourceManager::addExtraStr , ResourceManager::getInstance() ,
                        std::placeholders::_1));
    }
    catch(exception &e)
    {
        return false;
    }

    return true;
}
