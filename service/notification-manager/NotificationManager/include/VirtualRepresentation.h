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

#ifndef VIRTUALREPRESENTATION_H_
#define VIRTUALREPRESENTATION_H_

#include "NotificationManager.h"

#define SUCCESS_RESPONSE 0

using namespace OC;
using namespace OCPlatform;
using namespace std;

class ResourceManager;

class VirtualRepresentation
{

private:
    std::string m_virtualUri;
    std::string m_originHostIP;
    std::string m_resourceTypeName;
    std::string m_resourceInterface;
    uint8_t m_resourceProperty;

    OCResourceHandle m_resourceHandle;

    static AttributeMap s_attributeMap;
    static std::mutex s_mutexAttributeMap;
    static std::condition_variable s_conditionAttributeMap;
    static bool m_isReadyAttributeMap;

    int getRepresentation(OCRepresentation& oc);

public:

    std::string getUri();
    std::string getHostIP();
    std::string getResourceTypeName();
    std::string getResourceInterface();
    uint8_t getResourceProperty();
    OCResourceHandle getResourceHandle();

    int setUri(std::string uri);
    int setHostIP(std::string ip);
    int setResourceTypeName(std::string typeName);
    int setResourceInterface(std::string interface);
    int setResourceProperty(uint8_t property);
    int setResourceHandle(OCResourceHandle & handle);

public:
    VirtualRepresentation();
    virtual ~VirtualRepresentation();

    std::string addVirtualTag(std::string uri);

    OCEntityHandlerResult entityHandler(const std::shared_ptr<OCResourceRequest> request ,
        		const std::shared_ptr<OCResourceResponse> response);
    void onObserve(const HeaderOptions &headerOption, const OCRepresentation &rep ,
                const int eCode , const int sequenceNumber);

};

#endif /* VIRTUALREPRESENTATION_H_ */
