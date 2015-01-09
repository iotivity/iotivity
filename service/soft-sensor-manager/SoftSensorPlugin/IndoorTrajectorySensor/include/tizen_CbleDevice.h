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

#ifndef CBLEDEVICE_H_
#define CBLEDEVICE_H_

#include <bluetooth.h>
#include <map>
#include <string>


class CbleDevice
{
    private :
        typedef bt_adapter_device_discovery_info_s ble_value;
        typedef std::map<std::string, ble_value>    CbleDeviceMAP;
        CbleDeviceMAP device;

    public :
        unsigned long time_count;

        CbleDevice();
        ~CbleDevice();

        int insert( ble_value *discovery_info );

        ble_value *getNext( int seq );

        int setRSSI( std::string addr, int rssi );

        ble_value *get( std::string addr );

        bool find( std::string addr );

        int size( void );

        void erase( CbleDeviceMAP::iterator itr );

        void clear( void );

        bool isTimeOut( void );
};


#endif /* CBLEDEVICE_H_ */
