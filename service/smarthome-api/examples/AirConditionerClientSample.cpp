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
#include "AirConditionerClientSample.h"
#include <iostream>

static double g_temperature = 0;
static std::string g_units = "";
static std::list<double> g_range;
static bool g_temperatureFlag = false;

void MyRemoteBinarySwitchResourceDelegate::onTurnOn(ResultCode ret)
{
    if (SUCCESS == ret){
        std::cout << "[App]turn on succeed!" << std::endl;
    }
    else{
        std::cout << "[App]turn on failed!" << std::endl;
    }
}

void MyRemoteBinarySwitchResourceDelegate::onTurnOff(ResultCode ret)
{
    if (SUCCESS == ret){
        std::cout << "[App]turn off succeed!" << std::endl;
    }
    else{
        std::cout << "[App]turn off failed!" << std::endl;
    }
}

void MyRemoteBinarySwitchResourceDelegate::onGetState(bool value, ResultCode ret)
{
    if (SUCCESS == ret){
        std::cout << "[App]swtich : " << value << std::endl;
    }
}

void MyRemoteTemperatureResourceDelegate::onGetTemperature(double temperature,
                                                   std::string units,
                                                   std::list<double> range,
                                                   ResultCode ret)
{
    if (SUCCESS == ret){
        std::cout << "[App]temperature : " << temperature << units << std::endl;
        std::cout << "[App]range : " << *(range.begin()) << " ~ " <<*(range.rbegin()) << std::endl;
        g_temperature = temperature;
        g_units = units;
        g_range = range;
        g_temperatureFlag = true;
    }
}

void MyRemoteTemperatureResourceDelegate::onSetTemperature(double temperature,
                                                   std::string units,
                                                   std::list<double> range,
                                                   ResultCode ret)
{
    if (SUCCESS == ret){
        std::cout << "[App]temperature set succeed!" << std::endl;
    }
    else{
        std::cout << "[App]temperature set failed!" << std::endl;
    }
}

AirConditionerClientSample::AirConditionerClientSample()
{
    // TODO Auto-generated constructor stub
    m_airCon = NULL;
}

AirConditionerClientSample::~AirConditionerClientSample()
{
    // TODO Auto-generated destructor stub
}

void AirConditionerClientSample::start()
{
    DeviceDiscovery::getInstance().setDelegate(this);
}

void AirConditionerClientSample::findAnyDevice()
{
    DeviceDiscovery::getInstance().findRemoteDevices();
}

void AirConditionerClientSample::findAirConditioner()
{
    DiscoveryQuery query;
    query.setDeviceType(DEVICE_TYPE::AIRCONDITIONER);
    DeviceDiscovery::getInstance().findRemoteDevices(query);
}

void AirConditionerClientSample::getState()
{
    if (m_airCon)
    {
        std::cout << "[App]get state" << std::endl;
        m_airCon->m_remoteBinarySwitch->getState();
        m_airCon->m_remoteTemperature->getTemperature();
        m_airCon->m_remoteTemperature->getTemperatureWithUnits("F");
    }
    else
    {
        std::cout << "[App]AirConditioner isn't finded yet." << std::endl;
    }
}

void AirConditionerClientSample::turnOn()
{
    if (m_airCon)
    {
        std::cout << "[App]turn on" << std::endl;
        m_airCon->m_remoteBinarySwitch->on();
    }
    else
    {
        std::cout << "[App]AirConditioner isn't finded yet." << std::endl;
    }
}

void AirConditionerClientSample::turnOff()
{
    if (m_airCon)
    {
        std::cout << "[App]turn off" << std::endl;
        m_airCon->m_remoteBinarySwitch->off();
    }
    else
    {
        std::cout << "[App]AirConditioner isn't finded yet." << std::endl;
    }
}

void AirConditionerClientSample::temperatureUp()
{
    if (m_airCon)
    {
        if (g_temperatureFlag)
        {
            std::cout << "[App]temperature up" << std::endl;
            m_airCon->m_remoteTemperature->setTemperature(++g_temperature);
        }
        else
        {
            std::cout << "[App]Please get temperature first!" << std::endl;
        }
    }
    else
    {
        std::cout << "[App]AirConditioner isn't finded yet." << std::endl;
    }
}

void AirConditionerClientSample::temperatureDown()
{
    if (m_airCon)
    {
        if (g_temperatureFlag)
        {
            std::cout << "[App]temperature down" << std::endl;
            m_airCon->m_remoteTemperature->setTemperature(--g_temperature);
        }
        else
        {
            std::cout << "[App]Please get temperature first!" << std::endl;
        }
    }
    else
    {
        std::cout << "[App]AirConditioner isn't finded yet." << std::endl;
    }
}

void AirConditionerClientSample::onFindRemoteDevice(SHBaseRemoteDevice *remoteDevice)
{
    std::cout << "[App]1. onFindResourcDevice call!" << std::endl;


    if (remoteDevice->hasDeviceType(DEVICE_TYPE::AIRCONDITIONER))
    {
        m_airCon = dynamic_cast<RemoteAirConditionerDevice*>(remoteDevice);

        std::cout << "[App]set remote binary switch delegate" << std::endl;
        m_airCon->m_remoteBinarySwitch->setRemoteBinarySwitchResourceDelegate(&m_switchDelegate);

        std::cout << "[App]set remote temperature delegate" << std::endl;
        m_airCon->m_remoteTemperature->setRemoteTemperatureResourceDelegate(&m_temperatureDelegate);
    }
    else
    {
        std::cout << "[App]ignore device." << std::endl;
    }
}

void AirConditionerClientSample::onFindRemoteDevice(std::list<SHBaseRemoteDevice*> remoteDevices)
{
    std::cout << "[App]2. onFindResourcDevice call!" << std::endl;
    (void)remoteDevices;    //TODO
}

void print()
{
    std::cout << "==============================================================" << std::endl;
    std::cout << "0. print menu" << std::endl;
    std::cout << "1. find device without query" << std::endl;
    std::cout << "2. find device with query(AIRCONDITIONER)" << std::endl;
    std::cout << "3. get state of air conditioner" << std::endl;
    std::cout << "4. turn on air conditioner" << std::endl;
    std::cout << "5. turn off air conditioner" << std::endl;
    std::cout << "6. temperature up of air conditioner" << std::endl;
    std::cout << "7. temperature down of air conditioner" << std::endl;
    std::cout << "9. Quit" << std::endl;
    std::cout << "===============================================================" << std::endl;
    std::cout << "cmd>";
}

int main()
{
    AirConditionerClientSample sample;
    sample.start();

    bool quitflag = false;
    int cmd;
    print();
    while(!quitflag)
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
            sample.findAirConditioner();
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
            sample.temperatureUp();
        }
        else if (7 == cmd)
        {
            sample.temperatureDown();
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
            std::cin.ignore( 1024, '\n' );
        }
    }

    std::cout << "Bye." << std::endl;

    return 0;
}
