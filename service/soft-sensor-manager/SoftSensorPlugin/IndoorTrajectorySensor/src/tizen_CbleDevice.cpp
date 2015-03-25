/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include <stdio.h>
#include "tizen_log.h"
#include "tizen_CbleDevice.h"

#define TIME_LIMITE     5       // unit : second

CbleDevice::CbleDevice() : time_count(0) { clear(); }
CbleDevice::~CbleDevice() { clear(); }

int CbleDevice::insert( CbleDevice::ble_value *discovery_info )
{
    DBG("Enter.");
    std::string addr = std::string(discovery_info->remote_address);

    if ( find(addr) == false )
    {
        device[addr] = *discovery_info;

        device[addr].remote_address = new char[ strlen(discovery_info->remote_address) + 1 ];
        strcpy( device[addr].remote_address, discovery_info->remote_address);
        device[addr].remote_name = new char[ strlen(discovery_info->remote_name) + 1 ];
        strcpy( device[addr].remote_name, discovery_info->remote_name);
        device[addr].service_uuid = NULL;
        device[addr].service_count = 0;
    }

    return device.size();
}

CbleDevice::ble_value *CbleDevice::getNext( int seq )
{
    DBG("Enter.");
    CbleDeviceMAP::iterator itr = device.begin();

    if ( seq < 0 || seq >= device.size())
    {
#if __INTERNAL_DEBUG_
        DBG("return null");
#endif
        return NULL;
    }

    for (; seq ; seq--)
        itr++;

#if __INTERNAL_DEBUG_
    DBG("return %s pointer", itr->second.remote_address );
#endif
    return &itr->second;
}

int CbleDevice::setRSSI( std::string addr, int rssi )
{
    DBG("Enter.");
    CbleDeviceMAP::iterator itr = device.find( addr );

    if ( itr == device.end() )
        return NULL;

    ble_value *pdevice = &itr->second;

    if ( rssi == NULL )
    {
        pdevice->service_count = NULL;
        pdevice->rssi = 0;
    }
    else if ( pdevice->service_count == NULL )
    {
        pdevice->service_count = 1;
        pdevice->rssi = rssi;
    }
    else
        return NULL;

    return pdevice->rssi;
}

CbleDevice::ble_value *CbleDevice::get( std::string addr )
{
    DBG("Enter.");
    CbleDeviceMAP::iterator itr = device.find( addr );

    if ( itr == device.end() )
        return NULL;

    return &itr->second;
}

bool CbleDevice::find( std::string addr )
{
    DBG("Enter.");
    CbleDeviceMAP::iterator itr = device.find( addr );

    if ( itr == device.end() )
        return false;

    return true;
}

int CbleDevice::size( void )
{
    DBG("Enter.");
    return device.size();
}

void CbleDevice::erase( CbleDeviceMAP::iterator itr )
{
    ble_value *value = &itr->second;

    if ( value->remote_address )
        delete value->remote_address;
    if ( value->remote_name )
        delete value->remote_name;
    if ( value->service_uuid )
        delete [] value->service_uuid;

    device.erase(itr);
}

void CbleDevice::clear( void )
{
    CbleDeviceMAP::iterator itr = device.begin();

    if ( itr != device.end() )
    {
        erase(itr);
        itr++;
    }
}

bool CbleDevice::isTimeOut( void )
{
    DBG("Enter.");
    if ( time_count > TIME_LIMITE )
        return true;
    else
        return false;
}


