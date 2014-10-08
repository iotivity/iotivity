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

VirtualRepresentation m_VirtualRepresentation;

AttributeMap VirtualRepresentation::attributeMap;

std::mutex VirtualRepresentation::mutexAttributeMap;
std::condition_variable VirtualRepresentation::conditionAttributeMap;
bool VirtualRepresentation::isReadyAttributeMap = true;

int g_num = 0;

VirtualRepresentation::VirtualRepresentation()
{
	m_resourceHandle = NULL;
	resourceProperty = 0x00;
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
	return virtualUri;
}
std::string VirtualRepresentation::getHostIP()
{
	return originHostIP;
}
std::string VirtualRepresentation::getResourceTypeName()
{
	return resourceTypeName;
}
std::string VirtualRepresentation::getResourceInterface()
{
	return resourceInterface;
}
uint8_t VirtualRepresentation::getResourceProperty()
{
	return resourceProperty;
}

int VirtualRepresentation::setUri(std::string uri)
{
	virtualUri = addVirtualTag(uri);
	return true;
}
int VirtualRepresentation::setHostIP(std::string ip)
{
	originHostIP = ip;
	return true;
}
int VirtualRepresentation::setResourceTypeName(std::string typeName)
{
	resourceTypeName = typeName;
	return true;
}
int VirtualRepresentation::setResourceInterface(std::string interface)
{
	resourceInterface = interface;
	return true;
}
int VirtualRepresentation::setResourceProperty(uint8_t property)
{
	resourceProperty = property;
	return true;
}
int VirtualRepresentation::setResourceHandle(OCResourceHandle & handle)
{
	m_resourceHandle = handle;
	return true;
}

int VirtualRepresentation::getRepresentation(OCRepresentation& oc)
{
	oc.setAttributeMap(attributeMap);
	oc.setUri(this->getUri().c_str());
	return true;
}

void VirtualRepresentation::entityHandler(std::shared_ptr<OCResourceRequest> request, std::shared_ptr<OCResourceResponse> response){
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
					std::unique_lock<std::mutex> lck(mutexAttributeMap);
					while(!isReadyAttributeMap)	{ conditionAttributeMap.wait(lck); }
					isReadyAttributeMap = false;

					NotificationManager::getInstance()->getPrint()(attributeMap);
#if ISFORLINUX
						std::cout << "\t";
#endif

					OCRepresentation rep;
					getRepresentation(rep);

					response->setErrorCode(200);
					response->setResourceRepresentation(rep, "");

					isReadyAttributeMap = true;
					conditionAttributeMap.notify_all();
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
#if ISFORLINUX
			std::cout << endl;
			std::cout << "========================================================" << endl;
			std::cout << "Receive ObserverFlag : ";
			std::cout << "Start Observe" << response->getPayload() << std::endl;
#endif
		}
		else
		{
		}
	}
	else
	{
	}
}

void VirtualRepresentation::onObserve(const OCRepresentation& rep, const int& eCode, const int& sequenceNumber)
{
    if(eCode == SUCCESS_RESPONSE)
    {
    	AttributeMap tmp = rep.getAttributeMap();

    	if( ResourceManager::getInstance()->isEmptyAttributeMap(tmp) )
    	{
    		return;
    	}

    	if(NotificationManager::getInstance()->getOnObserve())
    	{
    		NotificationManager::getInstance()->getOnObserve()(tmp);
    	}

#if ISFORLINUX
    	std::cout << endl;
		std::cout << "========================================================" << endl;
		std::cout << "Receive OBSERVE RESULT:" << std::endl;
		std::cout << "\tResource URI: " << rep.getUri() << std::endl;
#endif

		NotificationManager::getInstance()->getPrint()(tmp);

#if ISFORLINUX
		std::cout << endl;
		std::cout << "========================================================" << endl;
		std::cout << "Check Is it Hosting: ";
#endif

		OCStackResult result = OC_STACK_OK;

		VirtualRepresentation tmpObj = *this;
		if(!tmpObj.getUri().empty())
		{

			AttributeMap tmpAttMap = ResourceManager::getInstance()->copyAttributeMap(tmp);

			{
				std::unique_lock<std::mutex> lck(mutexAttributeMap);
				while(!isReadyAttributeMap)	{ conditionAttributeMap.wait(lck); }
				isReadyAttributeMap = false;

				attributeMap = tmpAttMap;

				isReadyAttributeMap = true;
				conditionAttributeMap.notify_all();
			}

#if ISFORLINUX
			std::cout << "Yes!!\n";
			std::cout << "\tVirtual URI: " << tmpObj.getUri() << std::endl;
			std::cout << "\tVirtual Resource Handle: " << tmpObj.getResourceHandle() << std::endl;
			std::cout << endl;
			std::cout << "========================================================" << endl;
			std::cout << "Send Virtual Resource: "  << endl;
			std::cout << "\tResource URI: " << tmpObj.getUri() << endl;
#endif
			result = OCPlatform::notifyObservers(tmpObj.getResourceHandle());
		}
		else
		{
#if ISFORLINUX
			std::cout << "No!!\n";
#endif
#if ISFORTIZEN
#endif
		}

       if(OC_STACK_NO_OBSERVERS == result)
		{
#if ISFORLINUX
			std::cout << "Nothing!!" << endl;
#endif
		}

    }
    else
    {
        std::cout << "onObserve Response error: " << eCode << std::endl;
        std::exit(-1);
    }
}
