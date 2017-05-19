/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/
#ifndef SMARTHOME_API_SERVER_EXAMPLES_ROBOTCLEANERSAMPLE_H_
#define SMARTHOME_API_SERVER_EXAMPLES_ROBOTCLEANERSAMPLE_H_

#include <list>

#include <SHBaseRemoteDevice.h>
#include <CommonApi.h>
#include <RemoteRobotCleanerDevice.h>
#include <RemoteBinarySwitchResource.h>
#include <RemoteModeResource.h>
#include <DeviceDiscovery.h>

using namespace OIC::Service::SH;

class MyRobotCleaner;
class RemoteModeDelegateImpl: public RemoteModeResourceDelegate
{
public:
    void onGetMode(const std::list< std::string >& supportedMode,
                   const std::list< std::string >& currentMode,
                   ResultCode ret);
    void onSetMode(const std::list< std::string >& supportedMode,
                   const std::list< std::string >& currentMode,
                   ResultCode ret);

    virtual ~RemoteModeDelegateImpl()
    {
    };
};

class RemoteBinarySwitchDelegateImpl: public RemoteBinarySwitchResourceDelegate
{
public:
    void onTurnOn(ResultCode ret);
    void onTurnOff(ResultCode ret);
    void onGetState(bool value, ResultCode ret);
    virtual ~RemoteBinarySwitchDelegateImpl()
    {
    };
};

class DeviceBrowserDelegateImpl: public DeviceDiscoveryDelegate
{
private:
    MyRobotCleaner *ownerPtr;
public:
    DeviceBrowserDelegateImpl(MyRobotCleaner *owner);
    MyRobotCleaner *getOwnerPtr();
    void onFindRemoteDevice(SHBaseRemoteDevice *remoteDevice);
    virtual ~DeviceBrowserDelegateImpl()
    {
    };
};

class MyRobotCleaner
{
public:
    RemoteModeDelegateImpl *m_modeDelegate;
    RemoteBinarySwitchDelegateImpl *m_binarySwitchDelegate;
    DeviceBrowserDelegateImpl *m_browserDelegate;

    MyRobotCleaner();
    ~MyRobotCleaner();
    void init();
    void findRemoteDevices();
    void findRemoteDevicesWithQuery(DiscoveryQuery *query);
};

#endif /* SMARTHOME_API_SERVER_EXAMPLES_ROBOTCLEANERSAMPLE_H_ */
