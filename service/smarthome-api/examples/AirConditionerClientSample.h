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
#ifndef SMARTHOME_API_EXAMPLES_AIRCONDITIONER_CLIENT_SAMPLE_H_
#define SMARTHOME_API_EXAMPLES_AIRCONDITIONER_CLIENT_SAMPLE_H_

#include <list>

#include <CommonApi.h>
#include <DeviceDiscovery.h>
#include <RemoteAirConditionerDevice.h>

using namespace OIC::Service::SH;

class MyRemoteBinarySwitchResourceDelegate : public RemoteBinarySwitchResourceDelegate
{
private:
    void onTurnOn(ResultCode ret);
    void onTurnOff(ResultCode ret);
    void onGetState(bool value, ResultCode ret);
};

class MyRemoteTemperatureResourceDelegate : public RemoteTemperatureResourceDelegate
{
private:
    void onGetTemperature(double temperature,
                std::string untis,
                std::list<double> range,
                ResultCode ret);
    void onSetTemperature(double temperature,
                std::string untis,
                std::list<double> range,
                ResultCode ret);
};

class AirConditionerClientSample : public DeviceDiscoveryDelegate
{
public:
    void start();
    void findAnyDevice();
    void findAirConditioner();
    void turnOn();
    void turnOff();
    void getState();
    void temperatureUp();
    void temperatureDown();

public:
    AirConditionerClientSample();
    virtual ~AirConditionerClientSample();

private:
    void onFindRemoteDevice(SHBaseRemoteDevice *remoteDevice);
    void onFindRemoteDevice(std::list<SHBaseRemoteDevice*> remoteDevice);

private:
    MyRemoteBinarySwitchResourceDelegate m_switchDelegate;
    MyRemoteTemperatureResourceDelegate m_temperatureDelegate;
    RemoteAirConditionerDevice *m_airCon;
};

#endif /* SMARTHOME_API_EXAMPLES_AIRCONDITIONER_CLIENT_SAMPLE_H_ */
