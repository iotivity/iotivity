/**
 * @copyright
 * This program is subject to copyright protection in accordance with the
 * applicable law. It must not, except where allowed by law, by any means or
 * in any form be reproduced, distributed or lent. Moreover, no part of the
 * program may be used, viewed, printed, disassembled or otherwise interfered
 * with in any form, except where allowed by law, without the express written
 * consent of the copyright holder.@n
 * Copyright (C) 2014 Samsung Electronics Co., Ltd. All Rights Reserved.
 *
 * @file        ProgramUtils.cpp
 * @brief       This file includes implementation of all class methods and class members which are declared in ProgramUtils.h.
 * @author      Auto-Generated
 */

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#elif __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#endif

#include "ProgramUtils.h"
#include "CMLogType.h"
#include "Notification/INotificationListener.h"

using namespace std;

::MyDevice *ProgramUtils::sp_myDevice = NULL;

OC::Cm::Device *ProgramUtils::sp_controlDevice = NULL;

OC::Cm::ControlManager *ProgramUtils::sp_shp = NULL;

ControlManagerListener *ProgramUtils::sp_ControlManagerListener = NULL;

char *ProgramUtils::currentResourecInitials = NULL ;
int ProgramUtils::currentDeviceNo = -1;

ITestResource *ProgramUtils::mResource = NULL;


bool easySetupRequired =
    false; /**< Represents whether Control Manager Application requires to initiate easy setup process or not. */

bool
ProgramUtils::getIPAddressAndUUID(std::string &ipAddressStr, std::string &uuid)
{
#ifdef _WIN32
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) calloc(1, sizeof (IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL)
    {
        //std::cout << "getIPAddressAndUUID_windows(): => " << "Error allocating memory needed to call GetAdaptersinfo\n";
        return false;
    }

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) calloc(1, ulOutBufLen);
        if (pAdapterInfo == NULL)
        {
            //std::cout << "getIPAddressAndUUID_windows(): => " << "Error allocating memory needed to call GetAdaptersinfo\n";
            return false;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
    {
        pAdapter = pAdapterInfo;

        while (pAdapter)
        {
            if (pAdapter->Type != MIB_IF_TYPE_LOOPBACK)
            {
                break;
            }
            pAdapter = pAdapter->Next;
        }

        if (pAdapter != NULL)
        {
            ipAddressStr = pAdapter->IpAddressList.IpAddress.String;

            std::ostringstream uuidStream;
            uuidStream << std::hex << std::setfill('0') << std::uppercase;

            for (unsigned int index = 0; index < 16; index++)
            {
                if (index < pAdapter->AddressLength)
                {
                    uuidStream << std::setw(2) << static_cast<int>(pAdapter->Address[index]);
                }
                else
                {
                    uuidStream << std::setw(2) << static_cast<int>(0);
                }

                if (index == 3)
                {
                    uuidStream << "-";
                }
                else if (index == 5)
                {
                    uuidStream << "-";
                }
                else if (index == 7)
                {
                    uuidStream << "-";
                }
                else if (index == 9)
                {
                    uuidStream << "-";
                }
            }

            uuid = uuidStream.str();
            return true;
        }
    }

    if (pAdapterInfo)
        free(pAdapterInfo);

    return false;
#elif __linux__
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

    std::ostringstream randomStream ;
    randomStream << rand() % 1000;
    std::string randomString = randomStream.str();
    int len = randomString.length();

    uuid.replace (uuid.length() - len, len, randomString);

    close(arp_socket);

    if ((ipAddressStr.empty()) || (uuid.empty()))
        return false;

    return true;
#else
    return true;
#endif
}



bool

ProgramUtils::testProfiles(int opMode)
{


    if (sp_controlDevice != NULL)
    {
        if (mResource != NULL)
        {
            delete mResource;
            mResource = NULL;
        }

        switch (opMode)
        {

            case 'c': // Configuration

                mResource = new testConfiguration(sp_controlDevice) ;
                break;

            case 'm': // Mode

                mResource = new testMode(sp_controlDevice);
                break;

            case 'l': // Light

                mResource = new testLightResource (sp_controlDevice);
                break;

            case 'e': // Eneryconsumption

                mResource = new testEnergyConsumptionResource(sp_controlDevice);
                break;

            case 'r': // door

                mResource = new testDoors(sp_controlDevice);
                break;

            case 't': // Temperature

                mResource = new testTemperaturesResource(sp_controlDevice);
                break;

            case 'i':  // Information

                mResource = new testInformationController(sp_controlDevice);
                break;

            case 'o':  // Operation

                mResource = new testOperationResource(sp_controlDevice);
                break;

            case 'h':  // Humidity

                mResource = new testHumidityResource(sp_controlDevice);
                break;

            case 'u':  // Usage

                mResource = new testUsage(sp_controlDevice);
                break;

            case 'n':  // Network

                mResource = new testNetworksResource(sp_controlDevice);
                break;
            case 'w':  // Network

                mResource = new testWifi(sp_controlDevice);
                break;
            case 'a': // Configuration

                mResource = new testAccessPoints(sp_controlDevice) ;
                break;

        }

        if (NULL != mResource)
        {
            mResource->testGet();
        }
        else
        {
            cout << "resource is NULL";
        }
    }

    return true;
}


bool
ProgramUtils::initializeFramework(char *portNum)
{
    cout << "ProgramUtils::initializeFramework() => " << "Called" << std::endl;

    sp_myDevice = ::MyDevice::getInstance();
    sp_shp = OC::Cm::ControlManager::getInstance();
    sp_ControlManagerListener = new ControlManagerListener();

    sp_shp->setLogLevel(OC::Cm::CMLogType::CM_LOG_TYPE_DEBUG);

    OC::Cm::Configuration *p_config = sp_shp->getConfiguration();
    /** Configure Subscription DB Path */
    p_config->setSubscriptionDbPath("CMSubscriptionDB"); // User needs to give actual DB Path
    sp_shp->setConfiguration(p_config);

    /** Configure Network Interface and port to be used */
    char deviceAddress[256] = {0x00,};
    std::string deviceIPAdress;
    std::string port; /** @note    Default Port */
    std::string uuid;
    std::string ipAddress;

    if (portNum != NULL)
    {
        port.assign(portNum);
    }
    else
    {
        port = "9999";
    }

    if (false == ProgramUtils::getIPAddressAndUUID(deviceIPAdress, uuid))
    {
        cout << "ProgramUtils::initializeFramework() => " << "failed to determine network interface\n";
        return false;
    }

    if ((deviceIPAdress.empty()) || (uuid.empty()))
    {
        cout << "ProgramUtils::initializeFramework() => " << "invalid network interface or uuid\n";
        return false;
    }

    cout << "ProgramUtils::initializeFramework() => " << "Selected IPAddress:" << deviceIPAdress <<
         "; UUID :" << uuid << std::endl;

    sprintf(deviceAddress, "%s:%s", deviceIPAdress.c_str(), port.c_str());

    sp_myDevice->setAddress(deviceAddress);

    /** Configure Device Details */
    sp_myDevice->setAddress(ipAddress);
    sp_myDevice->setDeviceType(OC::Cm::DeviceType_Unknown);
    sp_myDevice->setUUID(uuid.c_str()); // Example UUID: "E8113233-9A97-0000-0000-000000000000"
    sp_myDevice->setDescription("Description");
    sp_myDevice->setManufacturer("Manufacturer");
    // User can additionally specify an optional 'deviceSubType' attribute if 'deviceType' is not sufficient to define the type of device user want to apply.
    // (e.g., System_Air_Conditioner)
    //sp_myDevice->setDeviceSubType("DeviceSubType");
    sp_myDevice->setModelID("Model ID");
    sp_myDevice->setSerialNumber("Serial Number");

    /** Configure Supported Resources */

    sp_myDevice->setSupportedResourceType("Capability");
    sp_myDevice->setSupportedResourceType("Device");
    sp_myDevice->setSupportedResourceType("Devices");
    sp_myDevice->setSupportedResourceType("Information");


    /**
     * Configure Control Manager Listener
     */
    sp_shp->setListener(*sp_ControlManagerListener);

    return true;
}

bool
ProgramUtils::startFramework()
{
    RegisterDeviceListener();
    if (false == sp_shp->start(*sp_myDevice))
    {
        cout << "ProgramUtils::startFramework() => " << "ERROR: Failed to Start framework" << std::endl;
        return 0;
    }

    /** Confirm Control Manager-Start */
    OC::Cm::CMStates shpState = sp_shp->getState() ;

    while (OC::Cm::CM_STARTING == shpState)
    {
        Sleep(1000); // Waiting for 1 second for checking status of Control Manager
        cout << "ProgramUtils::startFramework() => " <<
             "INFO - Waiting for Control Manager to be started completely, Control Manager Running Status " <<
             shpState << std::endl;
        shpState = sp_shp->getState() ;
    }

    if (shpState == OC::Cm::CM_STARTED)
    {
        std::cout << "ProgramUtils::startFramework() => " <<
                  "INFO - Successfully started Control Manager with State " << shpState << std::endl;
    }
    else
    {
        std::cout << "ProgramUtils::startFramework() => " << "ERROR: Failed to Start framework with State "
                  << shpState << ", Hence Exiting!!!" << std::endl;
        stopFramework();

        return false;
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

    /** Confirm Control Manager-Stop */

    OC::Cm::CMStates shpState = sp_shp->getState() ;

    if (OC::Cm::CM_STOPPED == shpState )
    {
        std::cout << "ProgramUtils::stopFramework() => " << "INFO - Control Manager Stopped Completely" <<
                  std::endl;
    }

    /** Un-subscribe Control Manager Listener */
    sp_shp->removeListener(*sp_ControlManagerListener);
    if (sp_ControlManagerListener)
    {
        delete sp_ControlManagerListener;
    }

    /** Reset Control Manager Configuration */
    OC::Cm::Configuration *config = sp_shp->getConfiguration();

    if (NULL != config)
    {
        config->reset();
    }

    if (mResource != NULL)
    {
        delete mResource;
    }
    if (currentResourecInitials != NULL)
    {
        delete[] currentResourecInitials ;
    }

    if (sp_shp) { delete sp_shp; }
    std::cout << "ProgramUtils::stopFramework() => " << "Exiting Successfully!" << std::endl;

    /*  if(sp_myDevice != NULL)
        {
            delete sp_myDevice;
            sp_myDevice = NULL;
        }*/

    ::MyDevice::removeInstance();

    return true;
}

bool
ProgramUtils::processNotification()
{

    return true;
}

class DeviceFinderListener : public OC::Cm::DeviceFinder::IDeviceFinderListener
{
        virtual void OnDeviceAdded( OC::Cm::Device &device )
        {
            /* This method will be invoked when a new device is discovered */

            /*      OC::Cm::Device* cmDevice = (OC::Cm::Device*) &device;
                    OC::Cm::DeviceType type = cmDevice->getDeviceType();


                    std::cout << "Discovered Resource Name - " << cmDevice->getDeviceName()<<"\n";
                    std::cout << "Discovered Resource Type - " << type<<"\n";*/

            std::list<std::string> res = device.getSupportedResource();
            if (res.size() > 0)
            {
                cout << "Supported Resources of " << device.getDeviceName() << ": \n[ ";
                for (std::list<std::string>::iterator it1 = res.begin(); it1 != res.end(); ++it1)
                {
                    cout << *it1 << " ";
                }
                cout << " ]\n";
            }

            cout << "\n*************************************************** \n" ;
            cout << "    PLEASE ENTER YOUR CHOICE \n" ;
            cout << "   'd' for  display the all devices , 'q' for exit\n" ;
            cout << "\n**************************************************** \n" ;

        }
        virtual void OnDeviceRemoved( OC::Cm::Device &device)
        {
            /*This method will be invoked when a device is leaves the network */
            std::cout << "New Resource Removed!!";
        }

        virtual void OnDeviceUpdated( OC::Cm::Device &device)
        {
            /*This method will be invoked when a device is leaves the network */
            std::cout << "New Resource updated!!";
        }

        virtual void OnDeviceError( OC::Cm::Device &device )
        {
            /*This method will be invoked when a device is leaves the network */
            std::cout << "New Resource Error!!";
        }
};

class NotificationResponseListener : public OC::Cm::Notification::INotificationListener
{
    public :

        virtual void onNotificationReceived( std::string &uuid,
                                             std::string &resource,
                                             std::string &eventType,
                                             OC::Cm::Serialization::ISerializable *notification,
                                             std::string &subscriptionURI,
                                             OC::Cm::Serialization::Xsd::DateTimeType *eventTime )
        {
            cout << "onNotificationReceived subscriptionURI = " << subscriptionURI << "\n";

        }
        virtual void onMulticastedNotifcationReceived( const OC::Cm::Device &device,
                const std::string &elementType,
                const OC::Cm::Serialization::ISerializable *notification )
        {
            cout << "onMulticastedNotifcationReceived\n";
        }
};

DeviceFinderListener gDeviceFinderLister;
NotificationResponseListener gNotificationResponseListener;

void ProgramUtils::RegisterDeviceListener()
{

    OC::Cm::DeviceType type(OC::Cm::DeviceType::DeviceType_All);
    OC::Cm::DeviceDomain domain(OC::Cm::DeviceDomain::DeviceDomain_All);

    sp_shp->getDeviceFinder()->setDeviceFinderListener( domain, type, gDeviceFinderLister) ;
    sp_shp->addNotificationListener(gNotificationResponseListener);
}

int ProgramUtils::DisplayAllDevices()
{
    std::list<OC::Cm::Device *>  mylist = ProgramUtils::sp_shp->getDeviceFinder()->getDevices();

    cout << "     Discovered Device details : \n\n" ;
    if (mylist.size() > 0)
    {
        int i = 0 ;
        for (std::list<OC::Cm::Device *>::iterator it = mylist.begin(); it != mylist.end(); ++it)
        {
            cout << "    Device No : " << i ;
            cout << "  Device Name : '" << ((OC::Cm::Device *)(*it))->getDeviceName() << "'" ;
            cout << "  Device ID : '" << ((OC::Cm::Device *)(*it))->getUUID() << "'\n" ;
            i++;
        }
    }
    return 0;
}

int ProgramUtils::SelectDevices(int type)
{
    std::list<OC::Cm::Device *>  mylist = ProgramUtils::sp_shp->getDeviceFinder()->getDevices();

    if (!mylist.empty() && type < (int)mylist.size())
    {
        for (std::list<OC::Cm::Device *>::iterator it = mylist.begin(); it != mylist.end(); ++it)
        {
            if (type == 0)
            {
                sp_controlDevice = *it;
                currentDeviceNo = type;
                return 1;
            }
            type--;
        }
    }
    cout << "INVALID DEVICE No,  TRY AGAIN !!!\n" ;
    return 0;
}

void ProgramUtils::DisplayAvailableResources()
{
    if (sp_controlDevice == NULL)
        return ;

    std::list<std::string> res = sp_controlDevice->getSupportedResource();

    if (currentResourecInitials != NULL)
    {
        delete[] currentResourecInitials ;
        currentResourecInitials = NULL;

    }

    currentResourecInitials = new char[res.size() + 1];
    cout << "\n************************************************* \n" ;
    cout << "   PLEASE ENTER RESOURCE TYPE \n" ;

    int i = 0;
    for (std::list<std::string>::iterator it1 = res.begin(); it1 != res.end(); ++it1)
    {
        std::string value = *it1;

        if      (value == "Configuration" ) { currentResourecInitials[i++] = 'c' , cout << " 'c' for Configuration \n" ; }
        else if (value == "Doors" ) { currentResourecInitials[i++] = 'r', cout << " 'r' for Doors \n" ;}
        else if (value == "EnergyConsumption" ) { currentResourecInitials[i++] = 'e',  cout << " 'e' for Energyconsuption \n" ;}
        else if (value == "Humidity" ) { currentResourecInitials[i++] = 'h',  cout << " 'h' for Humidity \n" ;}
        else if (value == "Information" ) { currentResourecInitials[i++] = 'i', cout << " 'i' for Information \n" ;}
        else if (value == "Light" ) { currentResourecInitials[i++] = 'l',  cout << " 'l' for Light \n" ;}
        else if (value == "Mode" ) { currentResourecInitials[i++] = 'm',  cout << " 'm' for Mode \n" ;}
        else if (value == "Operation" ) { currentResourecInitials[i++] = 'o',  cout << " 'o' for Operations \n" ;}
        else if (value == "Temperatures" ) { currentResourecInitials[i++] = 't',  cout << " 't' for Temperatures \n" ;}
        else if (value == "UsageLimit" ) { currentResourecInitials[i++] = 'u',  cout << " 'u' for UsageLimit \n" ;}
        else if (value == "Networks" ) { currentResourecInitials[i++] = 'n',  cout << " 'n' for Networks \n" ;}
        else if (value == "WiFi" ) { currentResourecInitials[i++] = 'w',  cout << " 'w' for WiFi \n" ;}
        else if (value == "AccessPoints" ) { currentResourecInitials[i++] = 'a',  cout << " 'a' for AccessPoints \n" ;}
    }
    currentResourecInitials[i] = '\0';
    cout << " 'd' for change device\n";
    cout << " 'q' for exit\n";
    cout << "\n************************************************* \n" ;
}
int ProgramUtils::ValideateResourcesType(char c)
{
    if (currentResourecInitials == NULL)
        return 0 ;
    int i = 0 ;
    while (currentResourecInitials[i])
    {
        if (currentResourecInitials[i] == c)
            return 1 ;
        i++;
    }

    return 0 ;

}


