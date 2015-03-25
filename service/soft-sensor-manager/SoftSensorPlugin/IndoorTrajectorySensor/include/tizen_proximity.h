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

#ifndef TIZEN_PROXIMITY_H_
#define TIZEN_PROXIMITY_H_

#include <bluetooth.h>
#include <map>
#include "tizen_CbleDevice.h"

void proximity_loop( void *param );
void what_return_value( bt_error_e returnValue );

#define JOB_NONE                0
#define JOB_GET_RSSI        1
#define JOB_GET_PROX        2
#define JOB_PUT_OUTPUT  3
#define JOB_BUSY                4

class CRSSI
{
    public:
        int cnt;
        int *rssi;

        CRSSI() : cnt(0), rssi(0) {}
        CRSSI( int count ) : cnt(0), rssi(0) {   init(count);    }
        ~CRSSI()
        {
            if ( rssi )
            {
                cnt = 0;
                delete [] rssi;
                rssi = 0;
            }
        }

        void init( int count )
        {
            if ( rssi == NULL )
            {
                rssi = new int[count];
                cnt = count;
            }
        }

};

class CProximity
{
    public :
        typedef void *(*workfunc)( void *);
        typedef std::map<std::string, CRSSI>    rssiMAP;
    private :
        static int loop_flag;
        static int get_rssi_cnt;
        static int num_set_rssi;
        static rssiMAP map_rssi;

        workfunc threadFunc;

        int reference_ea;
        float *m_distance;
        int *m_proximity;
    public :
        pthread_t Thandle;

        CProximity( workfunc func );

        ~CProximity( void );

        static bool find_referenceThing( bt_adapter_device_discovery_info_s *discovery_info ,
                                         CbleDevice *pdevice );

        static void CB_discover_state_change(int result,
                                             bt_adapter_device_discovery_state_e discovery_state, \
                                             bt_adapter_device_discovery_info_s *discovery_info, void *user_data);

        bool GetRSSI(CbleDevice *pdevice);

        void GetProximity(CbleDevice *pdevice);

        bool set_threadFunc(workfunc func);

        workfunc get_threadFunc(void);

        void set_output( CbleDevice *pdevice );

        void prox_first_setting( int reference_ea );

        friend void proximity_loop( void *param );
};



#endif /* TIZEN_PROXIMITY_H_ */
