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

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#endif
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

#include "ISubscriptionDB.h"
#include "SqliteSubscriptionDatabase.h"
#include "SubscriptionManager.h"
#include "Subscription.h"
#include "ProgramUtils.h"
#include "ResourceTypeEnum.h"
#include "log.h"

OC::ControlleeManager *ProgramUtils::sp_shp = NULL;
::MyDevice *ProgramUtils::sp_myDevice = NULL;

OC::Cm::Notification::ISubscriptionDB *subDBStore = NULL;   //jyc
OC::Cm::Notification::SubscriptionManager *pSub = NULL; //jyc
OC::Cm::Notification::NotificationResponse *notificationData = NULL;
::Light *respData = NULL;
String50 *mpActivate = NULL;
Color *mColor = NULL;
::Subscription *subscription = NULL;
//OC::CDContext = NULL;
bool
ProgramUtils::initializeFramework(char *portNum)
{


    /** Configure Network Interface and port to be used */
    std::string deviceIPAddress;
    std::string port; /** @note    Default Port */
    std::string uuid;

    if (portNum != NULL)
    {
        port.assign(portNum);
    }
    else
    {
        port = "9999";
    }

    if (false == ProgramUtils::getIPAddressAndUUID(deviceIPAddress, uuid))
    {
        CE(cout << "ProgramUtils::initializeFramework() => " << "failed to determine network interface\n";);

        return false;
    }

    if ((deviceIPAddress.empty()) || (uuid.empty()))
    {

        cout << "ProgramUtils::initializeFramework() => " << "invalid network interface or uuid\n";

        return false;
    }


    CE(cout << "ProgramUtils::initializeFramework() => " << "Selected IPAddress:" << deviceIPAddress << "; UUID :" << uuid << std::endl);

    CE(cout << "ProgramUtils::initializeFramework() => " << "Called" << std::endl);

    sp_shp = new OC::ControlleeManager(deviceIPAddress);



    CE(cout << "ProgramUtils::initializeFramework() => " << "ControlleeManager UP:" << std::endl);



    OC::Cm::Configuration *p_config = sp_shp->getConfiguration();
    p_config->setAppType(OC::Cm::ApplicationType_Controllable);
    /** Configure Subscription DB Path */
    p_config->setSubscriptionDbPath("CMSubscriptionDB"); // User needs to give actual DB Path
    CE(cout << "ProgramUtils::DB Init done() => " << "ControlleeManager UP:" << std::endl);
    /**
     * Configure Subscription Manager
     */
    //OC::Cm::Notification::ISubscriptionDB *subDBStore = NULL;
    subDBStore = new OC::Cm::Notification::SqliteSubscriptionDatabase();
    //OC::Cm::Notification::SubscriptionManager *pSub = new OC::Cm::Notification::SubscriptionManager(subDBStore);
    pSub = new OC::Cm::Notification::SubscriptionManager(subDBStore);


    sp_shp->setSubscriptionManager(*pSub);


    CE(cout << "ProgramUtils::initializeFramework() => Subscription up" << std::endl);


    /** Initialize the Framework **/
    //sp_shp->init();

    CE(cout << "ProgramUtils::initializeFramework() => init Done" << std::endl);

    sp_myDevice = ::MyDevice::getInstance();

    sp_myDevice->setAddress(deviceIPAddress);

    /** Configure Device Details */
    sp_myDevice->setModelID("Model ID");

    sp_myDevice->setDeviceType(OC::Cm::DeviceType_Unknown);

    sp_myDevice->setUUID(uuid.c_str()); // Example UUID: "E8113233-9A97-0000-0000-000000000000"

    sp_myDevice->setDescription("Description");

    sp_myDevice->setManufacturer("Manufacturer");
    sp_myDevice->setModelID("Model ID");

    sp_myDevice->setSerialNumber("Serial Number");

    initSubscription();
    initNotify();

    return true;
}

void ProgramUtils::initNotify()
{
    if (notificationData == NULL)
    {
        notificationData = new OC::Cm::Notification::NotificationResponse();
        notificationData->setNotificationType("Notified");
        notificationData->setResourcePath("/devices/0/light");
        notificationData->setUuid("E8113233-9A97-0000-0000-000000000000");
    }

    if (respData == NULL)
    {
        respData = new ::Light();
        respData->mpDimmingLevel = new uint8_t(100);
        respData->mpMaxDimmingLevel = new uint8_t(255);
        respData->mpRelativeDimmingLevel = new int16_t(0);
        respData->mpTransitionTime = new uint16_t(3);

        mpActivate = new String50();
        respData->mpActivatePatternID = mpActivate ;

        mColor = new Color();
        mColor->mpHue = new uint8_t(50);
        mColor->mpSaturation = new uint8_t(20);
        mColor->mpRelativeHue = new int16_t(10);
        mColor->mpRelativeSaturation = new int16_t(100);
        mColor->mpChromaticityX = new uint16_t(15);
        mColor->mpChromaticityY = new uint16_t(20);
        mColor->mpColorTemperature = new uint16_t(25);
        respData->mpColor = mColor;
    }
    notificationData->setNotificationData(respData);
}


bool
ProgramUtils::startFramework()
{
    RegisterDeviceListener();
    if (NULL == sp_shp->getSubscriptionManager())
    {

        CE(cout << "ProgramUtils::startFramework() => " << "getSubscriptionManager() returns NULL" << std::endl);

    }

    if (false == sp_shp->start())
    {

        CE(cout << "ProgramUtils::startFramework() => " << "ERROR: Failed to Start framework" << std::endl);

        //  return 0;
    }

    return true;
}

bool
ProgramUtils::stopFramework()
{
    if (sp_shp == NULL)
    {
        return false;
    }


    sp_shp->stop();

    /** Reset SHP Configuration */
    OC::Cm::Configuration *config = sp_shp->getConfiguration();

    if (NULL != config)
    {
        config->reset();
    }


    if (subDBStore)
    {
        //cout << "ProgramUtils::stopFramework() delete subDBStore" << endl;
        delete subDBStore;
        subDBStore = NULL;
    }

    if (pSub)
    {
        //cout << "ProgramUtils::stopFramework() delete pSub" << endl;
        delete pSub;
        pSub = NULL;
    }

    if (subscription)
    {
        //cout << "ProgramUtils::stopFramework() delete subscription" << endl;
        delete subscription;
        subscription = NULL;
    }

    if (respData)
    {
        delete respData;
        respData = NULL;
        //CE(cout << "ProgramUtils::stopFramework() delete respData done" << endl);
    }

    if (notificationData)
    {
        delete notificationData;
        notificationData = NULL;
        //CE(cout << "ProgramUtils::stopFramework() notificationData" << endl);
    }

    if (sp_shp)
    {
        delete sp_shp;
        sp_shp = NULL;
        //cout << "ProgramUtils::stopFramework() delete sp_shp" << endl;
    }

    CE(std::cout << "ProgramUtils::stopFramework() => " << "Exiting Successfully!" << std::endl);


    return true;
}

void ProgramUtils::RegisterDeviceListener()
{
    int error;
    //register Test devices to OC platform
    std::list<ResourceTypeEnum> resourceType;

    //testLightResource light(sp_shp->getContext());
    resourceType.push_back(RT_Light);   //pass
    resourceType.push_back(RT_Humidity);    //pass
//  resourceType.push_back(RT_Capability); //pass
//  resourceType.push_back(RT_Configuration); //pass
//  resourceType.push_back(RT_Detection);   //pass
//  resourceType.push_back(RT_Doors);    //pass
//  resourceType.push_back(RT_DRLC);    //pass
//  resourceType.push_back(RT_Energy);  //pass
//  resourceType.push_back(RT_EnergyConsumption); //pass
//  resourceType.push_back(RT_Information); //pass
//  resourceType.push_back(RT_Level);   //pass
//  resourceType.push_back(RT_Mode);    //pass
//  resourceType.push_back(RT_Movement);    //pass
//  resourceType.push_back(RT_Operation);   //pass
//  resourceType.push_back(RT_Thermostat);  //pass
//  resourceType.push_back(RT_Time);    //pass
//  resourceType.push_back(RT_Usages); //pass

//  resourceType.push_back(RT_Alarm);
//  resourceType.push_back(RT_Alarms);  //pass
//  resourceType.push_back(RT_LongPollingNotification); //pass
//  resourceType.push_back(RT_Network); //TBD
//  resourceType.push_back(RT_Temperatures); //pass
//  resourceType.push_back(RT_Versions);    //pass
//  resourceType.push_back(RT_Version); //pass
//  resourceType.push_back(RT_WiFi);    //TBD
//  resourceType.push_back(RT_UsageLimits); //fail.SHP client fails discovery.
//  resourceType.push_back(RT_UsageLimit);  //fail.SHP client fails discovery.
//  resourceType.push_back(RT_AccessPoint); //TBD
//  resourceType.push_back(RT_Device);  //fail.shp client could not detect
//  resourceType.push_back(RT_Message);     //fail.SHP client fails discovery
//  resourceType.push_back(RT_Messages);    //fail.SHP client fails discovery
//  resourceType.push_back(RT_Record);  //pass
//  resourceType.push_back(RT_Records); //pass
//  resourceType.push_back(RT_Remote);  //TBD
    OC::ControlleeStatus status = sp_shp->addResources(resourceType, error);
    if (status != OC::ControlleeError)
    {
        std::cout << "Registered all resources ok" << std::endl;
    }
    else
    {
        std::cout << "Error in registering resources" << std::endl;
    }
}


bool
ProgramUtils::getIPAddressAndUUID(std::string &ipAddressStr, std::string &uuid)
{
    struct ifconf ifc = {0};
    struct ifreq *ifr = NULL;
    int arp_socket = 0;
    char ip[INET6_ADDRSTRLEN] = {0};
    char buf[10 * sizeof(struct ifreq)] = {0};

    arp_socket = socket(PF_INET, SOCK_DGRAM, 0);
    if (arp_socket < 0)
    {
        return false;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(arp_socket, SIOCGIFCONF, &ifc) < 0)
    {
        close(arp_socket);
        return false;
    }

    ifr = ifc.ifc_req;
    int interfacesCount = ifc.ifc_len / sizeof(struct ifreq);

    for (int i = 0; i < interfacesCount; i++)
    {
        struct ifreq *item = &ifr[i];
        struct sockaddr *addr = &(item->ifr_addr);

        if (ioctl(arp_socket, SIOCGIFADDR, item) < 0)
        {
            close(arp_socket);
            return false;
        }

        if (inet_ntop(AF_INET, &(((struct sockaddr_in *)addr)->sin_addr), ip, sizeof ip) == NULL)
        {
            continue;
        }

        if (ioctl(arp_socket, SIOCGIFHWADDR, item) < 0)
        {
            close(arp_socket);
            return false;
        }

        ipAddressStr.assign(ip);

        std::ostringstream uuidStream;
        uuidStream << std::hex << std::setfill('0') << std::uppercase;

        for (unsigned int index = 0; index < 16; index++)
        {
            if (index < 6)
                uuidStream << std::setw(2) << static_cast<int>((unsigned char)item->ifr_hwaddr.sa_data[index]);
            else
                uuidStream << std::setw(2) << static_cast<int>(0);

            if (index == 3)
                uuidStream << "-";
            else if (index == 5)
                uuidStream << "-";
            else if (index == 7)
                uuidStream << "-";
            else if (index == 9)
                uuidStream << "-";
        }

        uuid = uuidStream.str();

        if (ipAddressStr.compare("127.0.0.1") != 0)
        {
            break;
        }
    }

    close(arp_socket);

    if ((ipAddressStr.empty()) || (uuid.empty()))
        return false;

    return true;
}

bool
ProgramUtils::Notify()
{

    CE(std::cout << "ProgramUtils::Notify() => " << "called!" << std::endl);
    if (notificationData == NULL)
    {
        CE(std::cout << "ProgramUtils::Notify() notificationData is NULL. initNotify called" << std::endl);
        initNotify();
    }


    if (!sp_shp->getSubscriptionManager()->notify(notificationData))
    {

        CE(std::cout << "SubscriptionManager::Notify() => " << "returned false!" << std::endl);
    }


    CE(std::cout << "ProgramUtils::Notify() => 2 " << "done!" << std::endl);
    return true ;
}

void
ProgramUtils::initSubscription()
{
    subscription = new ::Subscription();
    subscription->mpUuid = new OC::Cm::Serialization::Xsd::UuidType();
    subscription->mpUuid->value = "E8113233-9A97-0000-0000-000000000000";
    subscription->mpResourceURIs = new ::ArrayOfAnyURI();
    std::string resourceURI = "/devices/0/light";
    subscription->mpResourceURIs->members.push_back(resourceURI);
    subscription->mpNotificationURI = new std::string("coap://105.144.205.47:1234/notifications");
}

void
ProgramUtils::generateSubscription()
{

    CE(std::cout << "ProgramUtils::generateSubscription() => " << "called!" << std::endl);

    if (subscription == NULL)
    {
        CE(std::cout << "ProgramUtils::generateSubscription() subscription is NULL. initSubscription called" << std::endl);
        initSubscription();
    }

    std::string location, deviceAddress;
    int statusCode;

    sp_shp->getSubscriptionManager()->getSubscriptionHandler()->handleSubscriptionRequest(deviceAddress, subscription, location, statusCode);

}

