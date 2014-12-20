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



#if !defined(_DEVICEFINDERSERVICEDEVICE_H)
#define _DEVICEFINDERSERVICEDEVICE_H

#include "DeviceDomain.h"
#include "DeviceType.h"
#include "string"

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace DeviceFinder
            {
                namespace Service
                {

                    class CMAPI DeviceFinderServiceDevice
                    {
                        public:
                            DeviceFinderServiceDevice();
                            ~DeviceFinderServiceDevice();

                            void setLocation(std::string location);
                            std::string getLocation();

                            void setIPAddress(std::string address);
                            std::string getIPAddress();

                            void setPort(int port);
                            int getPort();

                            void setDomain(DeviceDomain domain);
                            DeviceDomain getDomain();

                            void setDeviceUUID(std::string uuid);
                            std::string getDeviceUUID();

                            void setDeviceType(std::string type);
                            void setDeviceType(int type);
                            OC::Cm::DeviceType getDeviceType();

                            void setDeviceName(std::string name);
                            std::string getDeviceName();

                            void setDeviceDescription(std::string description);
                            std::string getDeviceDescription();

                            void setDeviceManufacturer(std::string manufacturer);
                            std::string getDeviceManufacturer();

                            void setDeviceDeviceSubType(std::string deviceSubType);
                            std::string getDeviceDeviceSubType();

                            void setDeviceModelID(std::string modelID);
                            std::string getDeviceModelID();

                            void setDeviceSerialNumber(std::string serialNumber);
                            std::string getDeviceSerialNumber();

                            void setDeviceSalesLocation(std::string salesLocation);
                            std::string getDeviceSalesLocation();

                            void setDevicePeerID(std::string peerID);
                            std::string getDevicePeerID();

                            void setRefreshTokenExpiredStatus(bool status);
                            bool getRefreshTokenExpiredStatus();

                            void setDeviceOnlineStatus(bool status);
                            bool getDeviceOnlineStatus();

                        private:
                            std::string m_location;
                            std::string m_ipAddress;
                            int m_port;

                            DeviceDomain m_domain;
                            std::string             m_deviceUUID;                   // Self DEVICE UUID
                            OC::Cm::DeviceType  m_deviceType;                   // Self DEVICE Type
                            std::string             m_deviceName;                   // Self DEVICE Name
                            std::string             m_deviceDescription;            // Self DEVICE Description
                            std::string             m_deviceManufacturer;           // Self DEVICE Manufacturer
                            std::string             m_deviceDeviceSubType;          // Self DEVICE Device Sub Type
                            std::string             m_deviceModelID;                // Self DEVICE Model ID
                            std::string             m_deviceSerialNumber;           // Self DEVICE Serial Number
                            std::string             m_deviceSalesLocation;          // Self DEVICE Sales Location
                            std::string             m_devicePeerID;                 // Self DEVICE Peer ID
                            bool                    m_isRefreshTokenExpired;        // Self DEVICE Refresh Token Expired Status
                            bool                    m_isOnline;                     // Self DEVICE Online Status
                    };

                }
            }
        }
    }
}

#endif  //_DEVICEFINDERSERVICEDEVICE_H
