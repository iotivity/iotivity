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



#include "MyDevice.h"
#include "xsd/Capability.h"
#include "sstream"
#include <iostream>

MyDevice *MyDevice::s_instance ;

MyDevice *
MyDevice::getInstance()
{
    if (NULL == s_instance)
    {
        s_instance = new MyDevice();
    }

    return s_instance ;
}

void
MyDevice::removeInstance()
{
    if ( NULL != s_instance )
    {
        delete s_instance ;
        s_instance = NULL ;
    }
}

MyDevice::MyDevice()
    : OC::Cm::Device(OC::Cm::DeviceType_Unknown, OC::Cm::DeviceDomain_Local)
{

}

MyDevice::~MyDevice()
{

}

std::list< ::ResourceType > &
MyDevice::getSupportedResourceTypes()
{
    return m_resourceTypes ;
}

bool
MyDevice::setSupportedResourceType(std::string resource)
{
    ::ResourceType rt ;
    rt.value = resource ;

    if (rt.validateContent())
    {
        m_resourceTypes.push_back(rt);
    }
    else
    {
        std::cout << "\n @@@@@@@@@@@@@@@@ Not adding:>>" << resource << "<< to resources list!";
    }

    return true ;
}

bool
MyDevice::loadDeviceDescription(OC::Cm::Serialization::ISerializable &serializable)
{
    return true;
}

std::string
MyDevice::getSubscriptionsLink()
{
    if (m_subscriptionLink.empty())
    {
        std::ostringstream strstream ;
        //strstream << "/subscriptions" ;
        strstream << "/sub" ;

        m_subscriptionLink = strstream.str() ;
    }

    return m_subscriptionLink;
}

void
MyDevice::setAddress(const std::string &address)
{
    OC::Cm::Device::setAddress(address.c_str());

    std::ostringstream strstream ;

    strstream.str("");
    strstream << "/subscriptions" ;
    m_subscriptionLink = strstream.str() ;

    strstream.str("");
    strstream << "/devices" ;

    setDeviceDescriptionUri(strstream.str().c_str());

    strstream.str("");
    strstream << "/devices/0/information" ;
    //m_info.href = strstream.str();
}

bool
MyDevice::getCapability(::Capability *pCaps)
{
    if (NULL == pCaps)
    {
        return false ;
    }

    pCaps->mpDevicesLink    = new ::DevicesLink() ;
    pCaps->mpDevicesLink->mpHref = new std::string(getDeviceDescriptionUri());

    pCaps->mpSubscriptionsLink  = new ::SubscriptionsLink() ;
    pCaps->mpSubscriptionsLink->mpHref = new std::string(getSubscriptionsLink());

    return true;
}

/**
 * This is a sample implementation and can be used as a response to a GET request on /devices@n
 *@n
 * @param[in]   devices         Devices object to be written
 *
 * @return  @c True if device information is successfully written to the Devices object  @n
 *          @c False if given device resource is not added
 * @n
 * @note Sample code snippet of this function as a response to GET on /devices is as follows::
 * @n
 * @code
 * #include "MyDevice.h"
 * bool
 * DevicesResourceHandler::onGET( OC::Cm::Connector::Server::ServerSession& session, int& statusCode, ::Devices* respData)
 * {
 *   std::cout<<"\n\n$$$$$$$$$$$$$$$$$$  onGET Devices\n\n";
 *   statusCode = 200;
 *   return MyDevice::getInstance()->getDevices(respData);
 * }
 * @endcode
 */
bool
MyDevice::getDevices(::Devices *devices)
{
    ::Device *m_device = new ::Device();

    getDevice(m_device) ;

    devices->mpDevice.push_back(m_device) ;

    return true;
}

/**
 * This is a sample implementation and can be used as a response to a GET request on /devices@n
 * @n
 *
 * @param[out]  m_device        Device object to be written
 *
 * @return  @c True if device information is successfully written to the Device object  @n
 *          @c False if given device resource is not added
 *
 * TODO:
 * Caution and Action required before using it as a response to GET request on /devices:
 * This implementation needs to be modified with acutal values of the device, for example:
 * actual link to 'information' resource, actual supported resources list, type of the device and etc.
 */
bool
MyDevice::getDevice(::Device *m_device)
{
    m_device->mpConnected                       = new bool(true);
    m_device->mpDescription                     = new String50() ;
    m_device->mpDescription->value              = "OIC Controller"  ;
    m_device->mpName                            = new String30() ;
    m_device->mpName->value                     = "Controller" ;
    m_device->mpId                              = new String50();
    m_device->mpId->value                       = "0";
    m_device->mpUuid                            = new OC::Cm::Serialization::Xsd::UuidType();
    m_device->mpUuid->value                     = this->getUUID();

    m_device->mpResources                       = new ArrayOfResourceType();
    if (m_resourceTypes.size() > 0)
    {
        for (std::list< ::ResourceType >::const_iterator itr = m_resourceTypes.begin();
             itr != m_resourceTypes.end() ; itr++)
        {
            m_device->mpResources->members.push_back(*itr) ;
        }
    }
    else
    {
        ResourceType rt;

        rt.value = "Capability";
        m_device->mpResources->members.push_back(rt);

        rt.value = "Information";
        m_device->mpResources->members.push_back(rt);

        rt.value = "Device";
        m_device->mpResources->members.push_back(rt);

        rt.value = "Devices";
        m_device->mpResources->members.push_back(rt);
    }

    m_device->mpType                            = new ::DeviceType();
    m_device->mpType->value                     = "Smartphone";

    /*::InformationLink *InfoLink = new ::InformationLink();
    InfoLink->href                          = "/devices/1/information";*/
    m_device->mpInformationLink             = new InformationLink() ;
    m_device->mpInformationLink->mpHref     = new std::string("/devices/1/information") ;

//  ::FunctionResources *funRess = new ::FunctionResources();
//  funRess->mpFunctionResourceType = new ::ResourceType() ;
//  funRess->mpFunctionResourceType->value = "Temperature";

//  m_device->mpFunctionResources.push_back(funRess);

    return true;
}
