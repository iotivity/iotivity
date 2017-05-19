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
#include "RobotCleanerClientSample.h"

#include <iostream>

using namespace OIC::Service::SH;

void RemoteModeDelegateImpl::onGetMode(const std::list< std::string >& supportedMode,
                                       const std::list< std::string >& currentMode,
                                       ResultCode ret)
{
    std::cout << "delegate function onGetModeInfo called" << std::endl;
}

void RemoteModeDelegateImpl::onSetMode(const std::list< std::string >& supportedMode,
                                       const std::list< std::string >& currentMode,
                                       ResultCode ret)
{
    std::cout << "delegate function onSetMode called" << std::endl;
}

void RemoteBinarySwitchDelegateImpl::onTurnOn(ResultCode ret)
{
    std::cout << "delegate function onTurnOn called" << std::endl;
}

void RemoteBinarySwitchDelegateImpl::onTurnOff(ResultCode ret)
{
    std::cout << "delegate function onTurnOff called" << std::endl;
}

void RemoteBinarySwitchDelegateImpl::onGetState(bool value, ResultCode ret)
{
    std::cout << "delegate function onGetState called" << std::endl;
}

DeviceBrowserDelegateImpl::DeviceBrowserDelegateImpl(MyRobotCleaner *owner) :
        ownerPtr(owner)
{
    std::cout << "DeviceBrowserDelegateImpl CTOR" << std::endl;
}

MyRobotCleaner *DeviceBrowserDelegateImpl::getOwnerPtr()
{
    return this->ownerPtr;
}

void DeviceBrowserDelegateImpl::onFindRemoteDevice(SHBaseRemoteDevice *remoteDevice)
{
    std::cout << "1. onFindResourcDevice call!" << std::endl;

    RemoteRobotCleanerDevice *remoteBot;

    if (remoteDevice->hasDeviceType(DEVICE_TYPE::ROBOTCLEANER))
    {
        remoteBot = dynamic_cast< RemoteRobotCleanerDevice* >(remoteDevice);

        std::cout << "set remote binary switch delegate" << std::endl;
        remoteBot->m_remoteBinarySwitch->setRemoteBinarySwitchResourceDelegate(
                this->getOwnerPtr()->m_binarySwitchDelegate);

        std::cout << "turn on" << std::endl;
        remoteBot->m_remoteBinarySwitch->on();

        std::cout << "turn off" << std::endl;
        remoteBot->m_remoteBinarySwitch->off();

        std::cout << "get state" << std::endl;
        remoteBot->m_remoteBinarySwitch->getState();

        std::cout << "set remote Mode delegate" << std::endl;
        remoteBot->m_remoteMode->setRemoteModeResourceDelegate(this->getOwnerPtr()->m_modeDelegate);

        std::cout << "getMode" << std::endl;
        remoteBot->m_remoteMode->getMode();

        std::cout << "setMode" << std::endl;
        std::list< std::string > emptyList;
        remoteBot->m_remoteMode->setMode(emptyList);
    }
}

MyRobotCleaner::MyRobotCleaner()
{
    m_modeDelegate = new RemoteModeDelegateImpl();
    m_binarySwitchDelegate = new RemoteBinarySwitchDelegateImpl();
}

MyRobotCleaner::~MyRobotCleaner()
{
    delete (m_modeDelegate);
    delete (m_binarySwitchDelegate);
}

void MyRobotCleaner::init()
{
    DeviceDiscovery::getInstance().setDelegate(this->m_browserDelegate);
}
void MyRobotCleaner::findRemoteDevices()
{
    DeviceDiscovery::getInstance().findRemoteDevices();
}
void MyRobotCleaner::findRemoteDevicesWithQuery(DiscoveryQuery *query)
{
    DeviceDiscovery::getInstance().findRemoteDevices(*query);
}

void print()
{
    std::cout << "==============================================================" << std::endl;
    std::cout << "1. find device without query" << std::endl;
    std::cout << "2. find device with query(ROBOTCLEANER)" << std::endl;
    std::cout << "3. Quit" << std::endl;
    std::cout << "===============================================================" << std::endl;
    std::cout << "cmd>";
}

static MyRobotCleaner *myBot = NULL;

int main()
{
    bool quitflag = false;
    int cmd;

    myBot = new MyRobotCleaner();
    myBot->init();

    while (!quitflag)
    {
        print();
        std::cin >> cmd;

        if (1 == cmd)
        {
            myBot->findRemoteDevices();

        }
        else if (2 == cmd)
        {
            DiscoveryQuery query;
            query.setDeviceType(DEVICE_TYPE::ROBOTCLEANER);
            myBot->findRemoteDevicesWithQuery(&query);
        }
        else if (3 == cmd)
        {
            quitflag = true;
        }
        else
        {
            std::cout << "Unknown cmd." << std::endl;
            cmd = 0;
            std::cin.clear();
            std::cin.ignore(1024, '\n');
        }
    }

    std::cout << "Bye." << std::endl;

    return 0;
}
