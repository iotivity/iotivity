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
#include <iostream>
#include "tizen_log.h"
#include <pthread.h>
#include <unistd.h>

#include "tizen_public_ble.h"
#include "tizen_ble_bgthread.h"
#include "tizen_CbleDevice.h"

CbleBGthread *CbleBGthread::singleton = NULL;
CbleBGthread::workfunc CbleBGthread::workProcess = NULL;
bool CbleBGthread::ble_adapter_flag = false;
int CbleBGthread::ble_discover_flag = PROCESS_FREE;
int CbleBGthread::ble_bond_flag = PROCESS_FREE;
bool CbleBGthread::isThreadRun = false;
int CbleBGthread::reg_bond_cnt = 0;
pthread_t CbleBGthread::Thandle = 0;

// tizen ble init function.
void ble_init( CbleBGthread::workfunc func )
{
    DBG("Enter.");

    CbleBGthread *ble = CbleBGthread::GetInstance( func );
    if ( ble == NULL )
    {
        DBG("Error : function is failed.");
        return ;
    }

    BT_ERROR_CHECK( bt_initialize() );

    BT_ERROR_CHECK( bt_adapter_set_state_changed_cb(CbleBGthread::CB_adapter_state_change, NULL) );
    sleep(1);

    bt_error_e returnValue;
    returnValue = (bt_error_e)bt_adapter_enable();
    if (returnValue != BT_ERROR_NONE )
    {
        what_return_value( returnValue );
        if ( returnValue != BT_ERROR_ALREADY_DONE)
            return ;
        else
        {
#if __INTERNAL_DEBUG_
            DBG("return == BT_ERROR_ALREADY_DONE");
#endif
            CbleBGthread::set_adapter_state_flag();
        }
    }

#if __INTERNAL_DEBUG_
    DBG("Register discovery callback func.");
#endif
    static CbleDevice device;
    BT_ERROR_CHECK( bt_adapter_set_device_discovery_state_changed_cb(
                        CbleBGthread::CB_discover_state_change, &device) );
//  sleep(1);

//  DBG("Register bond create callback func.");
//  BT_ERROR_CHECK( bt_device_set_bond_created_cb( CbleBGthread::CB_bond_create , &device) );
//  sleep(1);

    ble->isThreadRun = true;
    pthread_create(&ble->Thandle, NULL, CbleBGthread::ble_thread, &device);

    DBG("Exit.");
    return ;

BT_ERROR:
    DBG("Error Exception. Exit.");

}

//void workProcess( void )
//{
//  DBG("WorkProcess run.");
//}

/*********************************
 * CbleBGthread class definition.
 */
CbleBGthread::CbleBGthread(workfunc func)
{
    workProcess = func;
    ble_adapter_flag = false;
    ble_discover_flag = PROCESS_FREE;
    ble_bond_flag = PROCESS_FREE;
    isThreadRun = false;
    reg_bond_cnt = 0;
    Thandle = 0;
}

CbleBGthread::~CbleBGthread()
{
    if ( singleton )
    {
        singleton = NULL;
    }
}

CbleBGthread *CbleBGthread::GetInstance( workfunc func )
{
    if ( func == NULL )
        return NULL;

    if (singleton == NULL)
        singleton = new CbleBGthread(func);

    return singleton;
}

/*****************************
 * BLE processing thread.
 */
void *CbleBGthread::ble_thread(void *param)
{
    DBG("Enter.");
    CbleDevice *pdevice = (CbleDevice *)param;

    while (isThreadRun )
    {
        sleep(1);
        pdevice->time_count++;

        if ( ble_adapter_flag )
        {
            // discovery
            if ( ble_discover_flag == PROCESS_FREE)
            {
                if ( ble_discovery() == false)
                    goto BT_ERROR;

                ble_discover_flag = PROCESS_TRYING;
            }

            // bond create
//          if( ble_discover_flag == PROCESS_COMPLETE && ble_bond_flag == PROCESS_FREE )
//          {
//              if( pdevice )
//              {
//                  DBG( "Try bond referenceThing List. size=%d, cnt=%d", pdevice->size(), reg_bond_cnt );
//                  if( reg_bond_cnt < pdevice->size() )
//                  {
//                      BT_ERROR_CHECK(bt_device_create_bond( (pdevice->getNext(reg_bond_cnt))->remote_address ));
//                      ble_bond_flag = PROCESS_TRYING;
//                  }
////                    for( int i = 0; i < pdevice->size(); i++ )
////                        BT_ERROR_CHECK(bt_device_create_bond( (pdevice->getNext(i))->remote_address ));
//              }
//          }

//          if( ble_bond_flag == PROCESS_COMPLETE )
            if ( ble_discover_flag == PROCESS_COMPLETE )
                break;

            continue ;

BT_ERROR:
            continue ;
        }   // if ( ble_adapter_flag )
    }// while(isThreadRun )


    while (isThreadRun)
    {
        // working process
        workProcess( (void *)pdevice );
    }

    DBG("Exit.");
    return 0;
}


/******************************
 * Callback Function definition.
 */

// Called by adapter_enable / adapter_disable function.
void CbleBGthread::CB_adapter_state_change(int result, bt_adapter_state_e adapter_state,
        void *user_data)
{
    DBG("callback called.");

    set_adapter_state_flag();
}

// Called by bt_adapter_start_device_discovery / bt_adapter_stop_device_discovery function.
void CbleBGthread::CB_discover_state_change(int result,
        bt_adapter_device_discovery_state_e discovery_state, \
        bt_adapter_device_discovery_info_s *discovery_info, void *user_data)
{
    DBG("callback called.");
#if __INTERNAL_DEBUG_
    DBG("result = %d", result);
#endif

    if ( discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FOUND )
    {
        CbleDevice *pdevice = (CbleDevice *)user_data;

        if ( discovery_info == NULL)
            goto BT_ERROR;

#if __INTERNAL_DEBUG_
        printf("=========== discovered device =============\n");
        printf("= address  \t : %s \n", discovery_info->remote_address);
        printf("= name     \t : %s \n", discovery_info->remote_name);
        printf("= rssi     \t : %d \n", (discovery_info->rssi - 65535) );
        printf("= is_bonded\t : %d \n", discovery_info->is_bonded);
        printf("= svc_count\t : %d \n", discovery_info->service_count);
        printf("= dev_type \t : %d \n", discovery_info->device_type);
        printf("===========================================\n");
#endif

        find_referenceThing( discovery_info , pdevice );

        if ( pdevice->isTimeOut() == true )
        {
#if __INTERNAL_DEBUG_
            DBG("Stop searching device.");
#endif
            BT_ERROR_CHECK( bt_adapter_stop_device_discovery() );
        }
    }
    else if ( discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FINISHED )
    {
#if __INTERNAL_DEBUG_
        DBG("state == BT_ADAPTER_DEVICE_DISCOVERY_FINISHED");
#endif
        reg_bond_cnt = 0;
        ble_discover_flag = PROCESS_COMPLETE;
    }

    return ;

BT_ERROR:
    return ;
}

// (*bt_device_bond_created_cb)
void CbleBGthread::CB_bond_create(int result, bt_device_info_s *device_info, void *user_data)
{
    DBG("callback called.");
    if ( (bt_error_e)result != BT_ERROR_NONE )
    {
        what_return_value( (bt_error_e)result );
        if ( (bt_error_e)result == BT_ERROR_REMOTE_DEVICE_NOT_FOUND )
            ble_bond_flag = PROCESS_FREE;

        return ;
    }

    CbleDevice *pdevice = (CbleDevice *)user_data;

    if ( pdevice )
    {
#if __INTERNAL_DEBUG_
        printf("=========== bonded device =============\n");
        printf("= address  \t : %s \n", device_info->remote_address);
        printf("= name     \t : %s \n", device_info->remote_name);
        printf("= is_bonded\t : %d \n", device_info->is_bonded);
        printf("= is_connec\t : %d \n", device_info->is_connected);
        printf("= is_autho \t : %d \n", device_info->is_authorized);
        printf("= svc_count\t : %d \n", device_info->service_count);
        printf("===========================================\n");
#endif

        if ( pdevice->find( std::string(device_info->remote_address) ) )
        {
            reg_bond_cnt++;
            ble_bond_flag = PROCESS_FREE;
#if __INTERNAL_DEBUG_
            DBG( "bonded_cnt = %d", reg_bond_cnt );
#endif
        }

        if ( reg_bond_cnt == pdevice->size() )
        {
#if __INTERNAL_DEBUG_
            DBG("bond_flag is PROCESS_COMPLETE");
#endif
            reg_bond_cnt = 0;
            ble_bond_flag = PROCESS_COMPLETE;
        }
    }

    return ;
}



/********************************
 * ETC. function definition.
 */
void CbleBGthread::set_adapter_state_flag(void)
{
    DBG("Enter.");

    // Get adapter state.
    bt_adapter_state_e state;
    BT_ERROR_CHECK( bt_adapter_get_state(&state) );

    if ( state == BT_ADAPTER_ENABLED)
        ble_adapter_flag = true;
    else
        ble_adapter_flag = false;

    return ;

BT_ERROR:
    DBG("Error Exception. Exit.");
}

// return : true --> OK
//                  false --> ERROR
bool CbleBGthread::ble_discovery( void )
{
    DBG("is Searching device.");
    bool isDiscovery = false;
    BT_ERROR_CHECK(bt_adapter_is_discovering(&isDiscovery));

    if ( isDiscovery == false )
    {
#if __INTERNAL_DEBUG_
        DBG("Start searching device.");
#endif
        BT_ERROR_CHECK( bt_adapter_start_device_discovery() );
    }

    return true;

BT_ERROR:
    return false;
}

bool CbleBGthread::find_referenceThing( bt_adapter_device_discovery_info_s *discovery_info ,
                                        CbleDevice *pdevice )
{
    DBG("Enter.");

    if ( strstr( discovery_info->remote_name, REF_NAME ) )
    {
#if __INTERNAL_DEBUG_
        DBG("Find %s device.", REF_NAME);
        DBG("Register %s device to reference-Thing.", discovery_info->remote_name );
#endif
        pdevice->insert( discovery_info );
    }

    return true;
}

void what_return_value( bt_error_e returnValue )
{
    printf("\n");
    DBG("returnValue=%d", returnValue);

    switch (returnValue)
    {
        case BT_ERROR_NONE:
            DBG("return value is BT_ERROR_NONE\n");
            break;
        case BT_ERROR_CANCELLED:
            DBG("return value is BT_ERROR_CANCELLED\n");
            break;
        case BT_ERROR_INVALID_PARAMETER:
            DBG("return value is BT_ERROR_INVALID_PARAMETER\n");
            break;
        case BT_ERROR_OUT_OF_MEMORY:
            DBG("return value is BT_ERROR_OUT_OF_MEMORY\n");
            break;
        case BT_ERROR_RESOURCE_BUSY:
            DBG("return value is BT_ERROR_RESOURCE_BUSY\n");
            break;
        case BT_ERROR_TIMED_OUT:
            DBG("return value is BT_ERROR_TIMED_OUT\n");
            break;
        case BT_ERROR_NOW_IN_PROGRESS:
            DBG("return value is BT_ERROR_NOW_IN_PROGRESS\n");
            break;
        case BT_ERROR_NOT_INITIALIZED:
            DBG("return value is BT_ERROR_NOT_INITIALIZED\n");
            break;
        case BT_ERROR_NOT_ENABLED:
            DBG("return value is BT_ERROR_NOT_ENABLED\n");
            break;
        case BT_ERROR_ALREADY_DONE:
            DBG("return value is BT_ERROR_ALREADY_DONE\n");
            break;
        case BT_ERROR_OPERATION_FAILED:
            DBG("return value is BT_ERROR_OPERATION_FAILED\n");
            break;
        case BT_ERROR_NOT_IN_PROGRESS:
            DBG("return value is BT_ERROR_NOT_IN_PROGRESS\n");
            break;
        case BT_ERROR_REMOTE_DEVICE_NOT_BONDED:
            DBG("return value is BT_ERROR_REMOTE_DEVICE_NOT_BONDED\n");
            break;
        case BT_ERROR_AUTH_REJECTED:
            DBG("return value is BT_ERROR_AUTH_REJECTED\n");
            break;
        case BT_ERROR_AUTH_FAILED:
            DBG("return value is BT_ERROR_AUTH_FAILED\n");
            break;
        case BT_ERROR_REMOTE_DEVICE_NOT_FOUND:
            DBG("return value is BT_ERROR_REMOTE_DEVICE_NOT_FOUND\n");
            break;
        case BT_ERROR_SERVICE_SEARCH_FAILED:
            DBG("return value is BT_ERROR_SERVICE_SEARCH_FAILED\n");
            break;
        case BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED:
            DBG("return value is BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED\n");
            break;
        case BT_ERROR_PERMISSION_DENIED:
            DBG("return value is BT_ERROR_PERMISSION_DENIED\n");
            break;
    }
}


//int bt_adapter_get_le_state  ( bt_adapter_le_state_e *  adapter_le_state )
//bt_adapter_le_enable()
// bt_adapter_get_state()

// bt_adapter_start_device_discovery().
//      bt_adapter_device_discovery_state_changed_cb().
// bt_device_connect_le().
//      bt_device_gatt_state_changed_cb().
// bt_device_read_rssi_value(const char *address);


//int bt_device_set_connection_state_changed_cb(bt_device_connection_state_changed_cb callback, void *user_data);
//typedef void (*bt_device_connection_state_changed_cb)(bool connected, const char *remote_address, void *user_data);


// bt_adapter_start_device_discovery().
//      bt_adapter_device_discovery_state_changed_cb().
// bt_device_create_bond()
//      bt_device_bond_created_cb().
// bt_device_set_alias(address,name)
