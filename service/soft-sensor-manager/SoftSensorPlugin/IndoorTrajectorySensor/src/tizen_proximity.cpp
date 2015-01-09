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
#include "tizen_proximity.h"
#include "tizen_CbleDevice.h"
#include "lib_proximity.h"

#include "ITS.h"

CProximity *prox = NULL;
ITSName::Things *output = NULL;
int CProximity::get_rssi_cnt = 0;
int CProximity::num_set_rssi = 0;
int CProximity::loop_flag = JOB_GET_RSSI;
CProximity::rssiMAP CProximity::map_rssi;

// Proximity variables
int **rssi = 0;
int *rssicnt = 0;
int *startindex = 0;
int *flag = 0;


void proximity_init( void *(*func)(void *) )
{
#if __INTERNAL_DEBUG_
    DBG("Enter.");
#endif
    prox = new CProximity( func );
    output = NULL;
}

void proximity_loop( void *param )
{
//  DBG("called. main loop function.");
    CbleDevice *pdevice = (CbleDevice *)param;

    if ( prox == NULL || pdevice == NULL )
    {
        DBG("Error : proximity_loop is occurred error.");
        return ;
    }

    // Get BLE RSSI and Calculate Proximity value. ( choi job )
    if ( prox->loop_flag == JOB_GET_RSSI )
    {
        prox->GetRSSI( pdevice );
        prox->loop_flag = JOB_BUSY;
    }

    if ( prox->loop_flag == JOB_GET_PROX )
    {
        prox->GetProximity( pdevice );
        prox->loop_flag = JOB_PUT_OUTPUT;
    }

    if ( prox->loop_flag == JOB_PUT_OUTPUT )
    {
        // Result Setting and transmit data from proximity to ITS
        prox->set_output( pdevice );
        pthread_create(&prox->Thandle, NULL, prox->threadFunc, output);
        prox->loop_flag = JOB_GET_RSSI;
    }

    usleep(100);
//  DBG("Exit.");
}




/****************************************
 *
 * CProximity class definition.
 *
 */

CProximity::CProximity( workfunc func )
{
#if __INTERNAL_DEBUG_
    DBG("Enter.");
#endif
    if ( set_threadFunc(func) == false)
        DBG("Error: set_threadFunc is failed.");

    loop_flag = JOB_GET_RSSI;
    get_rssi_cnt = 0;
    num_set_rssi = 0;

    reference_ea = 0;
    m_distance = 0;
    m_proximity = 0;
    map_rssi.clear();
}

CProximity::~CProximity( void )
{
#if __INTERNAL_DEBUG_
    DBG("Enter.");
#endif
    if ( rssi )
    {
        delete [] rssi;
        rssi = NULL;
    }

    if ( rssicnt )
    {
        delete rssicnt;
        rssicnt = NULL;
    }

    if ( startindex )
    {
        delete startindex;
        startindex = NULL;
    }

    if ( flag )
    {
        delete flag;
        flag = NULL;
    }

    if ( m_distance )
    {
        delete m_distance;
        m_distance = NULL;
    }

    if ( m_proximity )
    {
        delete m_proximity;
        m_proximity = NULL;
    }

}

bool CProximity::find_referenceThing( bt_adapter_device_discovery_info_s *discovery_info ,
                                      CbleDevice *pdevice )
{
#if __INTERNAL_DEBUG_
    DBG("Enter");
#endif
    if ( pdevice->find(discovery_info->remote_address) == false  )
    {
        return false;
    }
#if __INTERNAL_DEBUG_
    DBG("Find %s device.", discovery_info->remote_address);
#endif
    if ( pdevice->setRSSI( discovery_info->remote_address,
                           discovery_info->rssi ) == discovery_info->rssi )
    {
        map_rssi[ discovery_info->remote_address ].init( RSSI_EA );
        map_rssi[ discovery_info->remote_address ].rssi[get_rssi_cnt - 1] = discovery_info->rssi - 65535;
#if __INTERNAL_DEBUG_
        DBG("Get RSSI. %d", map_rssi[ discovery_info->remote_address ].rssi[get_rssi_cnt - 1] );
#endif
        num_set_rssi++;
    }

    return true;
}

// Called by bt_adapter_start_device_discovery / bt_adapter_stop_device_discovery function.
void CProximity::CB_discover_state_change(int result,
        bt_adapter_device_discovery_state_e discovery_state, \
        bt_adapter_device_discovery_info_s *discovery_info, void *user_data)
{
    DBG("callback called.");
#if __INTERNAL_DEBUG_
    DBG("result = %d", result);
#endif

    CbleDevice *pdevice = (CbleDevice *)user_data;

    if ( discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FOUND )
    {
        if ( discovery_info == NULL)
            goto BT_ERROR;
#if __INTERNAL_DEBUG_
        printf("=========== Get RSSI =============\n");
        printf("= address  \t : %s \n", discovery_info->remote_address);
        printf("= name     \t : %s \n", discovery_info->remote_name);
        printf("= rssi     \t : %d \n", (discovery_info->rssi - 65535) );
        printf("===========================================\n");
#endif
        find_referenceThing( discovery_info , pdevice );

        if ( num_set_rssi == pdevice->size()  )
        {
#if __INTERNAL_DEBUG_
            DBG("Stop searching device.");
#endif
            BT_ERROR_CHECK( bt_adapter_stop_device_discovery() );
        }
    }
    else if ( discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FINISHED )
    {
        DBG("state == BT_ADAPTER_DEVICE_DISCOVERY_FINISHED");

        num_set_rssi = 0;
        if ( get_rssi_cnt == RSSI_EA )
        {
            get_rssi_cnt = 0;
            loop_flag = JOB_GET_PROX;
        }
        else
            loop_flag = JOB_GET_RSSI;
    }

    return ;

BT_ERROR:
    return ;
}


bool CProximity::GetRSSI(CbleDevice *pdevice)
{
#if __INTERNAL_DEBUG_
    DBG("Enter");
#endif
    if ( num_set_rssi == 0 && get_rssi_cnt == 0)
        map_rssi.clear();

    num_set_rssi = 0;
    get_rssi_cnt++;

    for ( int i = 0; i < pdevice->size(); i++ )
        pdevice->setRSSI( (pdevice->getNext(i))->remote_address, 0);

    BT_ERROR_CHECK( bt_adapter_set_device_discovery_state_changed_cb(
                        CProximity::CB_discover_state_change, pdevice) );

    BT_ERROR_CHECK( bt_adapter_start_device_discovery() );

    return true;

BT_ERROR:
    return false;
}

bool is_first = true;
void CProximity::GetProximity(CbleDevice *pdevice)
{
#if __INTERNAL_DEBUG_
    DBG("Enter.");
#endif
//#if __INTERNAL_DEBUG_
    DBG("thing ea = %d", map_rssi.size() );
    DBG("RSSI ea = %d", RSSI_EA );

    for (int num = 0; num < map_rssi.size(); num++ )
    {
        std::string addr = pdevice->getNext(num)->remote_address;
        DBG("Addr -> %s", addr.c_str() );
        for (int i = 0; i < RSSI_EA ; i++)
            DBG(" %d th. rssi = %d", i, map_rssi[addr].rssi[i] );
    }
//#endif

    reference_ea = map_rssi.size();
    float *avg = new float[reference_ea];

    if ( is_first )
        prox_first_setting( reference_ea );

    for (int i = 0; i < reference_ea; i++)
    {
        std::string addr = pdevice->getNext(i)->remote_address;

        if ( rssicnt[i] > arraysize - 1)
            rssicnt[i] = rssicnt[i] % arraysize;

        for (int j = 0; j < RSSI_EA; j++)
        {
            rssi[i][rssicnt[i]] = map_rssi[addr].rssi[j];
            rssicnt[i]++;
        }

        avg[i] = CalculateExponentialAverage(RSSI_EA, rssi[i], startindex[i], flag[i]);
        m_distance[i] = calculateDistance(avg[i], -58);

        if ( m_distance[i] <= 1.0 )
        {
            m_proximity[i] = 1;
        }
        else if (m_distance[i] <= 2)
        {
            m_proximity[i] = 2;
        }
        else
        {
            m_proximity[i] = 3;
        }

        DBG("distance = %f", m_distance[i]);
        DBG("proximity = %d", m_proximity[i]);
        //Serial.println(calculateDistance(avg[i], -58));
        startindex[i] += RSSI_EA;

        if (startindex[i] >= arraysize)
        {
            startindex[i] = 0;
        }

        if (flag[i] < (arraysize / RSSI_EA))
        {
            flag[i]++;
        }
    } // for

    delete avg;
}

bool CProximity::set_threadFunc(workfunc func)
{
#if __INTERNAL_DEBUG_
    DBG("Enter.");
#endif
    if ( func == NULL )
    {
        DBG("Error: func is null.");
        return false;
    }

    threadFunc = func;
    return true;
}

CProximity::workfunc CProximity::get_threadFunc(void)
{
    return threadFunc;
}

void CProximity::set_output( CbleDevice *pdevice )
{
#if __INTERNAL_DEBUG_
    DBG("Enter.");
#endif

    if (output)
    {
        delete output;
        output = NULL;
    }

    if ( output == NULL )
    {
        output = new ITSName::Things(reference_ea);
    }

    if ( output == NULL )
    {
        DBG("Error : output is NULL.");
        return ;
    }

    // Set Output value.
    for (int i = 0; i < reference_ea; i++)
    {
        output->things[i].ID = pdevice->getNext(i)->remote_address;
        output->things[i].SD = 0.0;
        output->things[i].distance = m_distance[i];
        output->things[i].proximity = m_proximity[i];
    }
}




void CProximity::prox_first_setting( int reference_ea )
{
    if ( rssi == NULL )
    {
        rssi = new int *[reference_ea];

        for (int i = 0; i < reference_ea; i++)
        {
            rssi[i] = new int[arraysize];
            for (int j = 0; j < arraysize; j++)
                rssi[i][j] = 0;
        }
    }

    if ( rssicnt == NULL )
    {
        rssicnt = new int[reference_ea];
        for (int j = 0; j < reference_ea; j++)
            rssicnt[j] = 0;
    }

    if ( startindex == NULL )
    {
        startindex = new int[reference_ea];
        for (int j = 0; j < reference_ea; j++)
            startindex[j] = 0;
    }

    if ( flag == NULL)
    {
        flag = new int[reference_ea];
        for (int j = 0; j < reference_ea; j++)
            flag[j] = 0;
    }

    if ( m_distance == NULL )
    {
        m_distance = new float[reference_ea];
        for (int j = 0; j < reference_ea; j++)
            m_distance[j] = 0;
    }

    if ( m_proximity == NULL )
    {
        m_proximity = new int[reference_ea];
        for (int j = 0; j < reference_ea; j++)
            m_proximity[j] = 0;
    }

    is_first = false;
}