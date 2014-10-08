//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

///
/// This sample provides steps to define an interface for a resource
/// (properties and methods) and host this resource on the server.
///

#include "HueLightHandler.h"

namespace OIC
{

    void resourceHandler(std::shared_ptr<OC::OCResource> resource);


    /// This class represents a single resource named 'lightResource'. This resource has
    /// two simple properties named 'state'

    LightResource::LightResource(string &resUrl, string &resType, string &resInterface,
                                 string e_url): m_state(false), m_color(0), m_bright(0)
    {
        std::cout << "create LightResource ......." << std::endl;
        resourceURL = resUrl;
        resourceTypeName = resType;
        resourceInterface = resInterface;
        ext_url = e_url;
        std::cout << "ResUrl = " << resourceURL << std::endl;
        std::cout << "resType = " << resType << std::endl;
        std::cout << "resInterface = " << resInterface << std::endl;
        std::cout << "m_state = " << m_state << std::endl;
        std::cout << "m_color = " << m_color << std::endl;
        std::cout << "m_bright = " << m_bright << std::endl;
        std::cout << "ext_url = " << ext_url << std::endl;
        m_resourceHandle = NULL;

        resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;
    }

    LightResource::~LightResource() {std::cout << "Leave LightResource" << std::endl;}

    /* To find if the resource instance is in the platform */

    OCStackResult LightResource::findResource(OC::OCPlatform &platform)
    {
        string host = "";
        std::cout << "ResUrl = " << resourceURL << std::endl;
        OCStackResult result = platform.findResource(host, resourceURL, &resourceHandler);
        if (OC_STACK_OK != result)
        {
            cout << "findResource  unsuccessful\n";
        }
        else { cout << "findResource  successful\n"; }
        return result;
    }


    OCStackResult LightResource::addType(const OC::OCPlatform &platform, const std::string &type)
    {
        OCStackResult result = platform.bindTypeToResource(m_resourceHandle, type);
        if (OC_STACK_OK != result)
        {
            cout << "Binding TypeName to Resource was unsuccessful\n";
        }
        return result;
    }

    OCStackResult LightResource::addInterface(const OC::OCPlatform &platform,
            const std::string &interface)
    {
        OCStackResult result = platform.bindInterfaceToResource(m_resourceHandle, interface);
        if (OC_STACK_OK != result)
        {
            cout << "Binding Interface to Resource was unsuccessful\n";
        }
        return result;
    }

//    /* This function internally calls intel registerResource API to unregister a leaved resource*/
//    void LightResource::unregisterResource(OC::OCPlatform& platform)
//    {
//         OC::OCStackResult result = platform.unregisterResource(
//                          m_resourceHandle, resourceURL, resourceTypeName,
//                          resourceInterface, &entityHandler, resourceProperty);
//         if (OC::OC_STACK_OK != result)
//         {
//              cout << "Resource destory was unsuccessful\n";
//         }
//    }
//
///*
    OCResourceHandle LightResource::getHandle()
    {
        return m_resourceHandle;
    }


    void LightResource::setRepresentation(OC::OCRepresentation &light)
    {
        OC::AttributeMap attributeMap = light.getAttributeMap();
        std::cout << "Enter setRepresentation" << std::endl;
        cout << "\t\t\t" << "Received representation: " << endl;
        cout << "\t\t\t\t" << "state: " << attributeMap["state"][0] << endl;
        cout << "\t\t\t\t" << "color: " << attributeMap["color"][0] << endl;
        cout << "\t\t\t\t" << "bright: " << attributeMap["bright"][0] << endl;

        m_state = attributeMap["state"][0].compare("true") == 0;
        m_color = std::stoi(attributeMap["color"][0]);
        m_bright = std::stoi(attributeMap["bright"][0]);
    }

    OC::OCRepresentation LightResource::getRepresentation()
    {
        OC::OCRepresentation light;
        light.setUri(resourceURL);

        std::cout << "Enter getRepresentation" << std::endl;

        std::vector<std::string> interfaces;
        light.setResourceInterfaces(interfaces);

        std::vector<std::string> types;
        //types.push_back(m_lightType);
        light.setResourceTypes(types);

        OC::AttributeMap attributeMap;
        OC::AttributeValues stateVal;
        if (m_state)
        {
            stateVal.push_back("true");
        }
        else
        {
            stateVal.push_back("false");
        }
        OC::AttributeValues colorVal;
        colorVal.push_back(to_string(m_color));

        OC::AttributeValues brightVal;
        brightVal.push_back(to_string(m_bright));

        attributeMap["state"] = stateVal;
        attributeMap["color"] = colorVal;
        attributeMap["bright"] = brightVal;
        light.setAttributeMap(attributeMap);

        return light;
    }


    void resourceHandler(std::shared_ptr<OC::OCResource> resource)
    {
        std::cout << "Enter resourceHandler" << std::endl;
    }


    /* This function internally calls intel registerResource API to register a new joined resource */
    OCStackResult LightResource::registerResource(OC::OCPlatform &platform,
            std::function<void(const OC::OCResourceRequest::Ptr, const OC::OCResourceResponse::Ptr)>
            entityHandler)
    {
        OCStackResult result = platform.registerResource(
                                   m_resourceHandle, resourceURL, resourceTypeName,
                                   resourceInterface, entityHandler, resourceProperty);
        if (OC_STACK_OK != result)
        {
            cout << "Resource register was unsuccessful\n";
        }
        else { cout << "RegisterResource  successful\n"; }
        return result;
    }

    LightContainer::LightContainer(string &resUrl, string &resType,
                                   string &resInterface): LightResource(resUrl, resType, resInterface, "")
    {
        std::cout << "Enter Constructor Container" << std::endl;
    }
    LightContainer::~LightContainer()
    {
        std::cout << "Enter Disonstructor Container" << std::endl;
        if (m_container.empty()) std::cout << "Empty---- No need disconnstrator" << std::endl;
        else m_container.clear();
        std::cout << "LightContainer disconstrator end ...." << std::endl;
    }
}




// Entity handler can be implemented in several ways by the manufacturer


