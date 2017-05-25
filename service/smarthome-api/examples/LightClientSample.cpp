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
#include "LightClientSample.h"

using namespace OIC::Service::SH;
void MyRemoteBinarySwitchDelegate::onTurnOn(ResultCode ret)
{
    if (ret == SUCCESS)
    {
        std::cout << "[App]turn on succeed!" << std::endl;
    }
    else
    {
        std::cout << "[App]turn on failed!" << std::endl;
    }
}

void MyRemoteBinarySwitchDelegate::onTurnOff(ResultCode ret)
{
    if (ret == SUCCESS)
    {
        std::cout << "[App]turn off succeed!" << std::endl;
    }
    else
    {
        std::cout << "[App]turn off failed!" << std::endl;
    }
}

void MyRemoteBinarySwitchDelegate::onGetState(bool value, ResultCode ret)
{
    if (ret == SUCCESS || ret == OBSERVE_NOTIFY)
    {
        std::cout << "[App]swtich : " << value << std::endl;
    }
    else if (ret == OBSERVE_REGISTER_SUCCESS)
    {
        std::cout <<"[App] observe start success!" << std::endl;
    }
    else if (ret == OBSERVE_UNREGISTER_SUCCESS)
    {
        std::cout <<"[App] observe stop success!" << std::endl;
    }
    else
    {
        std::cout <<"[App] ERROR : " << ret << std::endl;
    }
}

LightClientSample::LightClientSample()
{
    m_light = NULL;
}

LightClientSample::~LightClientSample()
{
}

void LightClientSample::start()
{
    deviceDiscovery.setDelegate(this);
}

void LightClientSample::findAnyDevice()
{
    deviceDiscovery.findRemoteDevices();
}

void LightClientSample::findLight()
{
    DiscoveryQuery query;
    query.setDeviceType(DEVICE_TYPE::LIGHT);
    deviceDiscovery.findRemoteDevices(query);
}

void LightClientSample::turnOn()
{
    if (m_light)
    {
        std::cout << "[App]turn on" << std::endl;
        m_light->m_remoteBinarySwitch->on();
    }
    else
    {
        std::cout << "[App]light isn't finded yet." << std::endl;
    }
}

void LightClientSample::turnOff()
{
    if (m_light)
    {
        std::cout << "[App]turn off" << std::endl;
        m_light->m_remoteBinarySwitch->off();
    }
    else
    {
        std::cout << "[App]light isn't finded yet." << std::endl;
    }
}

void LightClientSample::getState()
{
    if (m_light)
    {
        std::cout << "[App]get state" << std::endl;
        m_light->m_remoteBinarySwitch->getState();
    }
    else
    {
        std::cout << "[App]light isn't finded yet." << std::endl;
    }
}

void LightClientSample::startObserve()
{
    if (m_light)
    {
        std::cout << "[App]start observe" << std::endl;
        m_light->m_remoteBinarySwitch->startObserve();
    }
    else
    {
        std::cout << "[App]light isn't finded yet." << std::endl;
    }
}

void LightClientSample::stopObserve()
{
    if (m_light)
    {
        std::cout << "[App]stop observe" << std::endl;
        m_light->m_remoteBinarySwitch->stopObserve();
    }
    else
    {
        std::cout << "[App]light isn't finded yet." << std::endl;
    }
}

void LightClientSample::onFindRemoteDevice(SHBaseRemoteDevice *remoteDevice)
{
    std::cout << "[App]1. onFindResourcDevice call!" << std::endl;

    if (remoteDevice->hasDeviceType(DEVICE_TYPE::LIGHT))
    {
        m_light = dynamic_cast< RemoteLightDevice* >(remoteDevice);

        std::cout << "[App]set remote binary switch delegate" << std::endl;
        m_light->m_remoteBinarySwitch->setRemoteBinarySwitchResourceDelegate(&m_delegate);
    }
    else
    {
        std::cout << "[App]ignore device." << std::endl;
    }
}

void LightClientSample::onFindRemoteDevice(std::list< SHBaseRemoteDevice* > remoteDevices)
{
    std::cout << "[App]2. onFindResourcDevice call!" << std::endl;
    (void) remoteDevices;
}

void print()
{
    std::cout << "==============================================================" << std::endl;
    std::cout << "0. print menu" << std::endl;
    std::cout << "1. find device without query" << std::endl;
    std::cout << "2. find device with query(LIGHT)" << std::endl;
    std::cout << "3. turn on light" << std::endl;
    std::cout << "4. turn off light" << std::endl;
    std::cout << "5. get state of light" << std::endl;
    std::cout << "6. start observe of light" << std::endl;
    std::cout << "7. stop oberve of light" << std::endl;
    std::cout << "9. Quit" << std::endl;
    std::cout << "===============================================================" << std::endl;
    std::cout << "cmd>";
}

int main()
{
    LightClientSample sample;
    sample.start();

    bool quitflag = false;
    int cmd;
    print();
    while (!quitflag)
    {
        std::cin >> cmd;

        if (0 == cmd)
        {
            print();
        }
        else if (1 == cmd)
        {
            sample.findAnyDevice();
        }
        else if (2 == cmd)
        {
            sample.findLight();
        }
        else if (3 == cmd)
        {
            sample.turnOn();
        }
        else if (4 == cmd)
        {
            sample.turnOff();
        }
        else if (5 == cmd)
        {
            sample.getState();
        }
        else if (6 == cmd)
        {
            sample.startObserve();
        }
        else if (7 == cmd)
        {
            sample.stopObserve();
        }
        else if (9 == cmd)
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
