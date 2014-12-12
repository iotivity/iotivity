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

OCEntityHandlerResult VirtualRepresentation::entityHandler(const std::shared_ptr<OCResourceRequest> request ,
		const std::shared_ptr<OCResourceResponse> response)
{
    if(request)
    {
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag == RequestHandlerFlag::InitFlag)
		{
		}

        else if(requestFlag == RequestHandlerFlag::RequestFlag)
        {
            if( (requestType == "GET") && response )
            {

				std::unique_lock< std::mutex > lck(s_mutexAttributeMap);
				while(!m_isReadyAttributeMap)
				{
					s_conditionAttributeMap.wait(lck);
				}
				m_isReadyAttributeMap = false;

				OCRepresentation rep;
				getRepresentation(rep);

				response->setErrorCode(200);
				response->setResourceRepresentation(rep , DEFAULT_INTERFACE);

				m_isReadyAttributeMap = true;
				s_conditionAttributeMap.notify_all();
			}
            else if(requestType == "PUT")
            {
            	// TODO
            }
            else if(requestType == "POST")
            {
            	// TODO
            }
            else if(requestType == "DELETE")
            {
            	// TODO
            }
            else
            {
            	// TODO
            }
        }
        else if(requestFlag == RequestHandlerFlag::InitFlag)
		{
			// TODO
		}
        else if(requestFlag == RequestHandlerFlag::ObserverFlag)
        {
        	// TODO
        	cout << "requestFlag == RequestHandlerFlag::ObserverFlag\n";
        }
        else
        {
        	// requestFlag is not [Request, Init, Observer]
        	// TODO
        }
    }
    else
    {
    	// Param(request) is empty.
    	// TODO
    }
    return OC_EH_OK;
}

void VirtualRepresentation::onObserve(const HeaderOptions &headerOption, const OCRepresentation &rep , const int eCode , const int sequenceNumber)
{
	cout << "VirtualRepresentation::onObserve Enter\n";
    if(eCode == SUCCESS_RESPONSE)
    {
    	cout << "1\n";
        AttributeMap inputAttributeMap = rep.getAttributeMap();

        if(ResourceManager::getInstance()->isEmptyAttributeMap(inputAttributeMap))
        {
        	cout << "2\n";
            return;
        }
        cout << "3\n";
        VirtualRepresentation tmpObj = *this;
        if(!tmpObj.getUri().empty())
        {
        	cout << "4\n";
            AttributeMap tmpAttMap = ResourceManager::getInstance()->copyAttributeMap(inputAttributeMap);
            cout << "5\n";
            {
            	cout << "6\n";
                std::unique_lock< std::mutex > lck(s_mutexAttributeMap);
                cout << "7\n";
                while(!m_isReadyAttributeMap)
                {
                	cout << "8\n";
                    s_conditionAttributeMap.wait(lck);
                }
                cout << "9\n";
                m_isReadyAttributeMap = false;
                cout << "10\n";
                s_attributeMap = tmpAttMap;
                cout << "11\n";
                m_isReadyAttributeMap = true;
                cout << "12\n";
                s_conditionAttributeMap.notify_all();
                cout << "13\n";
            }

            if(ResourceManager::getInstance()->m_onObserve)
			{
            	cout << "14\n";
				ResourceManager::getInstance()->m_onObserve(inputAttributeMap, tmpObj.getResourceHandle());
				cout << "15\n";
			}
        }
    }
    else
    {
    	// Check the error.
       // TODO
    }
    cout << "VirtualRepresentation::onObserve Out\n";
}
