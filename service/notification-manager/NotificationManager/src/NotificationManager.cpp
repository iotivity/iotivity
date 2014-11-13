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
//OCPlatform *NotificationManager::s_nmOCPlatform = NULL;
PlatformConfig NotificationManager::s_cfg;
//(ServiceType::InProc, ModeType::Both, "134.134.161.33", 5683, QualityOfService::NonConfirmable);

NotificationManager::NotificationManager()
{

    m_print = NULL;
    m_onfound = NULL;
    m_onObserve = NULL;
    m_startHosting = NULL;
    m_findHosting = NULL;
    m_addExtraStr = NULL;
}

NotificationManager::~NotificationManager()
{
}

void NotificationManager::initialize()
{

    Configure(s_cfg);

    setPrint(NULL);
    setOnFoundHostingCandidate(NULL);
    setStartHosting(NotificationManager::getInstance()->m_startHosting);
    setFindHosting(NotificationManager::getInstance()->m_findHosting);
    setAddExtraStr(NotificationManager::getInstance()->m_addExtraStr);

#ifndef ISFORDEMO
    findHostingCandidate();
#endif

}

void NotificationManager::registerHostingEventListener()
{
    // TODO : Initial HostingEventListener (v1.0)
}

void NotificationManager::findHostingCandidate()
{
    try
    {
        ResourceManager::getInstance()->findNMResource("" , "coap://224.0.1.187/oc/core" , true);
    }
    catch(OCException e)
    {
    }
}

NotificationManager *NotificationManager::getInstance()
{
    if(!s_instance)
    {
        s_instance = new NotificationManager();
    }

    return s_instance;
}

int NotificationManager::setPrint(std::function< void(AttributeMap &inputAttMap) > func)
{
    if(func != NULL)
    {
        try
        {
            NotificationManager::getInstance()->m_print = func;
        }
        catch(exception e)
        {
            return false;
        }
    }
    else
    {
        NotificationManager::getInstance()->m_print =
                std::function< void(AttributeMap &inputAttMap) >(
                        std::bind(&ResourceManager::printAttributeMap ,
                                ResourceManager::getInstance() , std::placeholders::_1));
    }
    return true;
}

int NotificationManager::setOnFoundHostingCandidate(
        std::function< void(std::shared_ptr< OCResource > resource) > func)
{
    if(func != NULL)
    {
        try
        {
            NotificationManager::getInstance()->m_onfound = func;
        }
        catch(exception e)
        {
            return false;
        }
    }
    else
    {
        NotificationManager::getInstance()->m_onfound = std::function<
                void(std::shared_ptr< OCResource > resource) >(
                std::bind(&ResourceManager::onFoundReport , ResourceManager::getInstance() ,
                        std::placeholders::_1));
    }

    return true;
}

int NotificationManager::setOnObserve(std::function< void(AttributeMap &inputAttMap) > func)
{
    if(func != NULL)
    {
        try
        {
            NotificationManager::getInstance()->m_onObserve = func;
        }
        catch(exception e)
        {
            return false;
        }
    }
    return true;
}

std::function< void(AttributeMap &inputAttMap) > NotificationManager::getPrint()
{
    return m_print;
}

std::function< void(std::shared_ptr< OCResource > resource) > NotificationManager::getOnFoundHostingCandidate()
{
    return m_onfound;
}

std::function< void(AttributeMap &inputAttMap) > NotificationManager::getOnObserve()
{
    return m_onObserve;
}

int NotificationManager::setStartHosting(
        std::function< void(std::shared_ptr< OCResource > resource) > &func)
{
    try
    {
        func = std::function< void(std::shared_ptr< OCResource > resource) >(
                std::bind(&ResourceManager::startHosting , ResourceManager::getInstance() ,
                        std::placeholders::_1));
    }
    catch(exception e)
    {
        return false;
    }

    return true;
}

int NotificationManager::setFindHosting(std::function< void() > &func)
{
    try
    {
        func = std::function< void() >(
                std::bind(&NotificationManager::findHostingCandidate ,
                        NotificationManager::getInstance()));
    }
    catch(exception e)
    {
        return false;
    }

    return true;
}

int NotificationManager::setAddExtraStr(std::function< void(std::string) > &func)
{
    try
    {
        func = std::function< void(std::string str) >(
                std::bind(&ResourceManager::addExtraStr , ResourceManager::getInstance() ,
                        std::placeholders::_1));
    }
    catch(exception e)
    {
        return false;
    }

    return true;
}

std::function< void(std::shared_ptr< OCResource > resource) > NotificationManager::getStartHosting()
{
    if(m_startHosting)
    {
        return m_startHosting;
    }
    else
    {
        return NULL;
    }
}

std::function< void() > NotificationManager::getFindHosting()
{
    if(m_findHosting)
    {
        return m_findHosting;
    }
    else
    {
        return NULL;
    }
}

std::function< void(std::string) > NotificationManager::getAddExtraStr()
{
    if(m_addExtraStr)
    {
        return m_addExtraStr;
    }
    else
    {
        return NULL;
    }
}
