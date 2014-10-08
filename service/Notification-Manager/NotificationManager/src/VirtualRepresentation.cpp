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

#include "VirtualRepresentation.h"

AttributeMap VirtualRepresentation::s_attributeMap;

std::mutex VirtualRepresentation::s_mutexAttributeMap;
std::condition_variable VirtualRepresentation::s_conditionAttributeMap;
bool VirtualRepresentation::m_isReadyAttributeMap = true;

int g_num = 0;

VirtualRepresentation::VirtualRepresentation()
{
    m_resourceHandle = NULL;
    m_resourceProperty = 0x00;
}

VirtualRepresentation::~VirtualRepresentation()
{
}

std::string VirtualRepresentation::addVirtualTag(std::string uri)
{
    std::string ret = uri;
    return ret.append(VIRTURL_TAG + std::to_string(g_num++));
}

OCResourceHandle VirtualRepresentation::getResourceHandle()
{
    return m_resourceHandle;
}

std::string VirtualRepresentation::getUri()
{
    return m_virtualUri;
}

std::string VirtualRepresentation::getHostIP()
{
    return m_originHostIP;
}

std::string VirtualRepresentation::getResourceTypeName()
{
    return m_resourceTypeName;
}

std::string VirtualRepresentation::getResourceInterface()
{
    return m_resourceInterface;
}

uint8_t VirtualRepresentation::getResourceProperty()
{
    return m_resourceProperty;
}

int VirtualRepresentation::setUri(std::string uri)
{
    m_virtualUri = addVirtualTag(uri);
    return true;
}

int VirtualRepresentation::setHostIP(std::string ip)
{
    m_originHostIP = ip;
    return true;
}

int VirtualRepresentation::setResourceTypeName(std::string typeName)
{
    m_resourceTypeName = typeName;
    return true;
}

int VirtualRepresentation::setResourceInterface(std::string interface)
{
    m_resourceInterface = interface;
    return true;
}

int VirtualRepresentation::setResourceProperty(uint8_t property)
{
    m_resourceProperty = property;
    return true;
}

int VirtualRepresentation::setResourceHandle(OCResourceHandle & handle)
{
    m_resourceHandle = handle;
    return true;
}

int VirtualRepresentation::getRepresentation(OCRepresentation& oc)
{
    oc.setAttributeMap(s_attributeMap);
    oc.setUri(this->getUri().c_str());
    return true;
}

void VirtualRepresentation::entityHandler(std::shared_ptr< OCResourceRequest > request ,
        std::shared_ptr< OCResourceResponse > response)
{
    if(request)
    {
        std::string requestType = request->getRequestType();
        RequestHandlerFlag requestFlag = request->getRequestHandlerFlag();

        if(requestFlag == RequestHandlerFlag::InitFlag)
        {
        }
        else if(requestFlag == RequestHandlerFlag::RequestFlag)
        {
            if(requestType == "GET")
            {

                if(response)
                {
                    std::unique_lock< std::mutex > lck(s_mutexAttributeMap);
                    while(!m_isReadyAttributeMap)
                    {
                        s_conditionAttributeMap.wait(lck);
                    }
                    m_isReadyAttributeMap = false;

                    NotificationManager::getInstance()->getPrint()(s_attributeMap);

                    OCRepresentation rep;
                    getRepresentation(rep);

                    response->setErrorCode(200);
                    response->setResourceRepresentation(rep , "");

                    m_isReadyAttributeMap = true;
                    s_conditionAttributeMap.notify_all();
                }
                else
                {
                }

            }
            else if(requestType == "PUT")
            {
            }
            else if(requestType == "POST")
            {
            }
            else if(requestType == "DELETE")
            {
            }
            else
            {
            }
        }
        else if(requestFlag == RequestHandlerFlag::ObserverFlag)
        {
        }
        else
        {
        }
    }
    else
    {
    }
}

void VirtualRepresentation::onObserve(const OCRepresentation& rep , const int& eCode ,
        const int& sequenceNumber)
{
    if(eCode == SUCCESS_RESPONSE)
    {
        AttributeMap tmp = rep.getAttributeMap();

        if(ResourceManager::getInstance()->isEmptyAttributeMap(tmp))
        {
            return;
        }

        if(NotificationManager::getInstance()->getOnObserve())
        {
            NotificationManager::getInstance()->getOnObserve()(tmp);
        }

        NotificationManager::getInstance()->getPrint()(tmp);

        OCStackResult result = OC_STACK_OK;

        VirtualRepresentation tmpObj = *this;
        if(!tmpObj.getUri().empty())
        {

            AttributeMap tmpAttMap = ResourceManager::getInstance()->copyAttributeMap(tmp);

            {
                std::unique_lock< std::mutex > lck(s_mutexAttributeMap);
                while(!m_isReadyAttributeMap)
                {
                    s_conditionAttributeMap.wait(lck);
                }
                m_isReadyAttributeMap = false;

                s_attributeMap = tmpAttMap;

                m_isReadyAttributeMap = true;
                s_conditionAttributeMap.notify_all();
            }

            result = OCPlatform::notifyObservers(tmpObj.getResourceHandle());
        }
        else
        {
        }

        if(OC_STACK_NO_OBSERVERS == result)
        {
        }

    }
    else
    {
        std::exit(-1);
    }
}
