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



#include "DeviceFinderServiceDevice.h"
#include "CMDataTypes.h"

namespace OC
{
    namespace Cm
    {

        CMAPI const char *DeviceType_Strings[] =
        {
            "Unknown",
            "Gateway",
            "Smartphone",
            "Tablet",
            "TV",
            "Refrigerator",
            //"Kimchi_Refrigerator",
            "Washer",
            "Dryer",
            "Air_Conditioner",
            //"System_Air_Conditioner",
            "Dishwasher",
            "Oven",
            "Microwave_Oven",
            "Cooktop",
            "Robot_Cleaner",
            "Light",
            "IP_Camera",
            "Door_Lock",
            "Door_Phone",
            "Thermostat",
            "Water_Heater",
            "Smart_Plug",
            "Generic_Sensor",
            "Motion_Detector",
            "Heater",
            "Coffee_Pot",
            "Iron",
            "Vacuum_Cleaner",
            "Zigbee_Bridge",
            "Zwave_Bridge"
        };

        namespace Core
        {
            namespace DeviceFinder
            {
                namespace Service
                {

                    DeviceFinderServiceDevice::DeviceFinderServiceDevice()
                    {
                        m_location = "";
                        m_ipAddress = "";
                        m_port = 0;

                        m_domain = DeviceDomain_Local;
                        m_deviceUUID = "";
                        m_deviceType = DeviceType_Unknown;
                        m_deviceName = "";
                        m_deviceDescription = "";
                        m_deviceManufacturer = "";
                        m_deviceModelID = "";
                        m_deviceSerialNumber = "";
                        m_deviceSalesLocation = "";
                        m_devicePeerID = "";
                        m_isRefreshTokenExpired = false;
                        m_isOnline = false;
                    }

                    DeviceFinderServiceDevice::~DeviceFinderServiceDevice()
                    {

                    }

                    void
                    DeviceFinderServiceDevice::setDeviceType(std::string type)
                    {
                        for (int index = 0; index < DeviceType_All; index++)
                        {
                            if ( 0 == shp_strcasecmp(DeviceType_Strings[index], type.c_str()))
                            {
                                m_deviceType = (OC::Cm::DeviceType)index;
                                break ;
                            }
                        }
                    }

                    void
                    DeviceFinderServiceDevice::setLocation(std::string location)
                    {
                        m_location = location;
                    }

                    std::string
                    DeviceFinderServiceDevice::getLocation()
                    {
                        return m_location;
                    }

                    void
                    DeviceFinderServiceDevice::setIPAddress(std::string address)
                    {
                        m_ipAddress = address;
                    }

                    std::string
                    DeviceFinderServiceDevice::getIPAddress()
                    {
                        return m_ipAddress;
                    }

                    void
                    DeviceFinderServiceDevice::setPort(int port)
                    {
                        m_port = port;
                    }

                    int
                    DeviceFinderServiceDevice::getPort()
                    {
                        return m_port;
                    }

                    void
                    DeviceFinderServiceDevice::setDomain(DeviceDomain domain)
                    {
                        m_domain = domain;
                    }

                    DeviceDomain
                    DeviceFinderServiceDevice::getDomain()
                    {
                        return m_domain;
                    }

                    void
                    DeviceFinderServiceDevice::setDeviceUUID(std::string uuid)
                    {
                        m_deviceUUID = uuid;
                    }

                    std::string
                    DeviceFinderServiceDevice::getDeviceUUID()
                    {
                        return m_deviceUUID;
                    }

                    void
                    DeviceFinderServiceDevice::setDeviceType(int type)
                    {
                        m_deviceType = (OC::Cm::DeviceType)type;
                    }

                    OC::Cm::DeviceType
                    DeviceFinderServiceDevice::getDeviceType()
                    {
                        return m_deviceType;
                    }

                    void
                    DeviceFinderServiceDevice::setDeviceName(std::string name)
                    {
                        m_deviceName = name;
                    }

                    std::string
                    DeviceFinderServiceDevice::getDeviceName()
                    {
                        return m_deviceName;
                    }

                    void
                    DeviceFinderServiceDevice::setDeviceDescription(std::string description)
                    {
                        m_deviceDescription = description;
                    }

                    std::string
                    DeviceFinderServiceDevice::getDeviceDescription()
                    {
                        return m_deviceDescription;
                    }

                    void
                    DeviceFinderServiceDevice::setDeviceManufacturer(std::string manufacturer)
                    {
                        m_deviceManufacturer = manufacturer;
                    }

                    std::string
                    DeviceFinderServiceDevice::getDeviceManufacturer()
                    {
                        return m_deviceManufacturer;
                    }

                    void
                    DeviceFinderServiceDevice::setDeviceDeviceSubType(std::string deviceSubType)
                    {
                        m_deviceDeviceSubType = deviceSubType;
                    }

                    std::string
                    DeviceFinderServiceDevice::getDeviceDeviceSubType()
                    {
                        return m_deviceDeviceSubType;
                    }

                    void
                    DeviceFinderServiceDevice::setDeviceModelID(std::string modelID)
                    {
                        m_deviceModelID = modelID;
                    }

                    std::string
                    DeviceFinderServiceDevice::getDeviceModelID()
                    {
                        return m_deviceModelID;
                    }

                    void
                    DeviceFinderServiceDevice::setDeviceSerialNumber(std::string serialNumber)
                    {
                        m_deviceSerialNumber = serialNumber;
                    }

                    std::string
                    DeviceFinderServiceDevice::getDeviceSerialNumber()
                    {
                        return m_deviceSerialNumber;
                    }

                    void
                    DeviceFinderServiceDevice::setDeviceSalesLocation(std::string salesLocation)
                    {
                        m_deviceSalesLocation = salesLocation;
                    }

                    std::string
                    DeviceFinderServiceDevice::getDeviceSalesLocation()
                    {
                        return m_deviceSalesLocation;
                    }

                    void
                    DeviceFinderServiceDevice::setDevicePeerID(std::string peerID)
                    {
                        m_devicePeerID = peerID;
                    }

                    std::string
                    DeviceFinderServiceDevice::getDevicePeerID()
                    {
                        return m_devicePeerID;
                    }

                    void
                    DeviceFinderServiceDevice::setRefreshTokenExpiredStatus(bool status)
                    {
                        m_isRefreshTokenExpired = status;
                    }

                    bool
                    DeviceFinderServiceDevice::getRefreshTokenExpiredStatus()
                    {
                        return m_isRefreshTokenExpired;
                    }

                    void
                    DeviceFinderServiceDevice::setDeviceOnlineStatus(bool status)
                    {
                        m_isOnline = status;
                    }

                    bool
                    DeviceFinderServiceDevice::getDeviceOnlineStatus()
                    {
                        return m_isOnline;
                    }

                }
            }
        }
    }
}

