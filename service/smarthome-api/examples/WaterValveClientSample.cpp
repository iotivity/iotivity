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
#include <iostream>
#include <DeviceDiscovery.h>
#include <CommonApi.h>
#include <RemoteWaterValveDevice.h>

using namespace std;
using namespace OIC::Service::SH;

class MyRemoteOpenLevelResourceDelegate: public RemoteOpenLevelResourceDelegate
{
public:
    virtual ~MyRemoteOpenLevelResourceDelegate() {}

private:
    void onGetLevel(int currentLevel, int increment,
                     int min, int max, ResultCode ret);
    void onChangeLevel(int currentLevel, int increment,
                       int min, int max, ResultCode ret);
};

class WaterValveClientSample: public DeviceDiscoveryDelegate
{
public:
    void start();
    void findAnyDevice();
    void findWaterValue();
    void getLevel();
    void changeLevel(int level);

public:
    WaterValveClientSample();
    virtual ~WaterValveClientSample();

private:
    void onFindRemoteDevice(SHBaseRemoteDevice *remoteDevice);
    void onFindRemoteDevice(std::list< SHBaseRemoteDevice* > remoteDevice);

private:
    MyRemoteOpenLevelResourceDelegate m_delegate;
    RemoteWaterValveDevice *m_waterValue;
};

void MyRemoteOpenLevelResourceDelegate::onGetLevel(int currentLevel, int increment,
                                                   int min, int max, ResultCode ret)
{
    std::cout << "[RemoteWaterValveDevice] onGetLevel.." << std::endl;

    if (ret == SUCCESS)
    {
        std::cout << "currentLevel : " << currentLevel << std::endl;
        std::cout << "increment    : " << increment << std::endl;
        std::cout << "min          : " << min << std::endl;
        std::cout << "max          : " << max << std::endl;
    }
}

void MyRemoteOpenLevelResourceDelegate::onChangeLevel(int currentLevel, int increment,
                                                      int min, int max, ResultCode ret)
{
    std::cout << "[RemoteWaterValveDevice] onChangeLevel.." << std::endl;

    if (ret == SUCCESS)
    {
        std::cout << "currentLevel : " << currentLevel << std::endl;
        std::cout << "increment    : " << increment << std::endl;
        std::cout << "min          : " << min << std::endl;
        std::cout << "max          : " << max << std::endl;
    }
}

WaterValveClientSample::WaterValveClientSample()
{
    m_waterValue = NULL;
}

WaterValveClientSample::~WaterValveClientSample()
{

}

void WaterValveClientSample::start()
{
    DeviceDiscovery::getInstance().setDelegate(this);
}

void WaterValveClientSample::findAnyDevice()
{
    DeviceDiscovery::getInstance().findRemoteDevices();
}

void WaterValveClientSample::findWaterValue()
{
    DiscoveryQuery query;
    query.setDeviceType(DEVICE_TYPE::WATER_VALVE);
    DeviceDiscovery::getInstance().findRemoteDevices(query);
}

void WaterValveClientSample::getLevel()
{
    std::cout << "[RemoteWaterValveDevice] getOpenLevel" << std::endl;
    m_waterValue->m_remoteOpenLevel->getLevel();
}

void WaterValveClientSample::changeLevel(int level)
{
    std::cout << "[RemoteWaterValveDevice] changeOpenLevel" << std::endl;
    m_waterValue->m_remoteOpenLevel->changeLevel(level);
}

void WaterValveClientSample::onFindRemoteDevice(SHBaseRemoteDevice *remoteDevice)
{
    std::cout << "[RemoteWaterValveDevice]1. onFindResourcDevice call!" << std::endl;

    if (remoteDevice->hasDeviceType(DEVICE_TYPE::WATER_VALVE))
    {
        m_waterValue = dynamic_cast< RemoteWaterValveDevice* >(remoteDevice);

        std::cout << "[RemoteWaterValveDevice] set remote open level delegate" << std::endl;
        m_waterValue->m_remoteOpenLevel->setDelegate(&m_delegate);
    }
    else
    {
        std::cout << "[RemoteWaterValveDevice] ignore device." << std::endl;
    }
}

void WaterValveClientSample::onFindRemoteDevice(std::list< SHBaseRemoteDevice* > remoteDevices)
{
    std::cout << "[RemoteWaterValveDevice]2. onFindResourcDevice call!" << std::endl;
    (void) remoteDevices;
}

void print()
{
    std::cout << "==============================================================" << std::endl;
    std::cout << "0. print menu" << std::endl;
    std::cout << "1. find device without query" << std::endl;
    std::cout << "2. find device with query(WATER_VALVE)" << std::endl;
    std::cout << "3. get open level" << std::endl;
    std::cout << "4. change open level" << std::endl;
    std::cout << "9. Quit" << std::endl;
    std::cout << "===============================================================" << std::endl;
    std::cout << "cmd>";
}

int main()
{
    WaterValveClientSample sample;
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
            sample.findWaterValue();
        }
        else if (3 == cmd)
        {
            sample.getLevel();
        }
        else if (4 == cmd)
        {
            int level;
            std::cout << "Level: ";
            std::cin >> level;
            sample.changeLevel(level);
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