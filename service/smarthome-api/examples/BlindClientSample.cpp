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
#include <RemoteBlindDevice.h>

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

class BlindClientSample: public DeviceDiscoveryDelegate
{
public:
    void start();
    void findAnyDevice();
    void findBlind();
    void getLevel();
    void changeLevel(int level);

public:
    BlindClientSample();
    virtual ~BlindClientSample();

private:
    void onFindRemoteDevice(SHBaseRemoteDevice *remoteDevice);
    void onFindRemoteDevice(std::list< SHBaseRemoteDevice* > remoteDevice);

private:
    MyRemoteOpenLevelResourceDelegate m_delegate;
    RemoteBlindDevice *m_blind;
};

void MyRemoteOpenLevelResourceDelegate::onGetLevel(int currentLevel, int increment,
                                                   int min, int max, ResultCode ret)
{
    std::cout << "[RemoteBlindDevice] onGetLevel.." << std::endl;

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
    std::cout << "[RemoteBlindDevice] onChangeLevel.." << std::endl;

    if (ret == SUCCESS)
    {
        std::cout << "currentLevel : " << currentLevel << std::endl;
        std::cout << "increment    : " << increment << std::endl;
        std::cout << "min          : " << min << std::endl;
        std::cout << "max          : " << max << std::endl;
    }
}

BlindClientSample::BlindClientSample()
{
    m_blind = NULL;
}

BlindClientSample::~BlindClientSample()
{

}

void BlindClientSample::start()
{
    DeviceDiscovery::getInstance().setDelegate(this);
}

void BlindClientSample::findAnyDevice()
{
    DeviceDiscovery::getInstance().findRemoteDevices();
}

void BlindClientSample::findBlind()
{
    DiscoveryQuery query;
    query.setDeviceType(DEVICE_TYPE::BLIND);
    DeviceDiscovery::getInstance().findRemoteDevices(query);
}

void BlindClientSample::getLevel()
{
    std::cout << "[RemoteBlindDevice] getOpenLevel" << std::endl;
    m_blind->m_remoteOpenLevel->getLevel();
}

void BlindClientSample::changeLevel(int level)
{
    std::cout << "[RemoteBlindDevice] changeOpenLevel" << std::endl;
    m_blind->m_remoteOpenLevel->changeLevel(level);
}

void BlindClientSample::onFindRemoteDevice(SHBaseRemoteDevice *remoteDevice)
{
    std::cout << "[RemoteBlindDevice]1. onFindResourcDevice call!" << std::endl;

    if (remoteDevice->hasDeviceType(DEVICE_TYPE::BLIND))
    {
        m_blind = dynamic_cast< RemoteBlindDevice* >(remoteDevice);

        std::cout << "[RemoteBlindDevice] set remote open level delegate" << std::endl;
        m_blind->m_remoteOpenLevel->setDelegate(&m_delegate);
    }
    else
    {
        std::cout << "[RemoteBlindDevice] ignore device." << std::endl;
    }
}

void BlindClientSample::onFindRemoteDevice(std::list< SHBaseRemoteDevice* > remoteDevices)
{
    std::cout << "[RemoteBlindDevice]2. onFindResourcDevice call!" << std::endl;
    (void) remoteDevices;
}

void print()
{
    std::cout << "==============================================================" << std::endl;
    std::cout << "0. print menu" << std::endl;
    std::cout << "1. find device without query" << std::endl;
    std::cout << "2. find device with query(BLIND)" << std::endl;
    std::cout << "3. get open level" << std::endl;
    std::cout << "4. change open level" << std::endl;
    std::cout << "9. Quit" << std::endl;
    std::cout << "===============================================================" << std::endl;
    std::cout << "cmd>";
}

int main()
{
    BlindClientSample sample;
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
            sample.findBlind();
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