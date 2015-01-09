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

#ifndef BLE_LIB_H_
#define BLE_LIB_H_

#include <bluetooth.h>
#include "tizen_CbleDevice.h"

void what_return_value( bt_error_e returnValue );


#define PROCESS_FREE                0
#define PROCESS_TRYING          1
#define PROCESS_FOUND               2
#define PROCESS_COMPLETE        3

class CbleBGthread
{
    public :
        typedef void (*workfunc)( void *);

    private :
        static CbleBGthread *singleton;

        static bool ble_adapter_flag;
        static int ble_discover_flag;
        static int ble_bond_flag;
        static int reg_bond_cnt;
        static workfunc workProcess;

        static bool ble_discovery( void );

        static bool find_referenceThing( bt_adapter_device_discovery_info_s *discovery_info ,
                                         CbleDevice *pdevice );

    public :
        static bool isThreadRun;
        static pthread_t Thandle;

        CbleBGthread( workfunc func );

        ~CbleBGthread( void );

        static CbleBGthread *GetInstance( workfunc func );

        // ETC. function List.
//  static void set_workProcess(workfunc func);
        static void set_adapter_state_flag(void);


        // Thread List.
        static void *ble_thread(void *param);
        // Callback List.
        static void CB_adapter_state_change(int result, bt_adapter_state_e adapter_state, void *user_data);
        static void CB_discover_state_change(int result,
                                             bt_adapter_device_discovery_state_e discovery_state,
                                             bt_adapter_device_discovery_info_s *discovery_info, void *user_data);
        static void CB_bond_create(int result, bt_device_info_s *device_info, void *user_data);

};


//#define BT_ERROR_CHECK( _FUNC_ )      {                               \
//      bt_error_e __err__ = (bt_error_e)(_FUNC_);      \
//  if ( __err__ != BT_ERROR_NONE )                             \
//  {                                                                                                   \
//      what_return_value( __err__ );                               \
//      goto BT_ERROR;                                              \
//  }                                                                               \
//}


#endif /* BLE_LIB_H_ */
