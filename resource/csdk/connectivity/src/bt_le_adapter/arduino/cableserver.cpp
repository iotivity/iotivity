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


//logger.h included first to avoid conflict with RBL library PROGMEM attribute
#include "logger.h"

#include "cableserver.h"

#include <Arduino.h>
#include <SPI.h>
#include <boards.h>
#include <RBL_nRF8001.h>

#include "caleinterface_singlethread.h"
#include "oic_malloc.h"
#include "caadapterutils.h"

#define TAG "LES"

CAResult_t CAInitializeBle()
{
    OIC_LOG(DEBUG, TAG, "IN");

    // Set your BLE Shield name here, max. length 10
    ble_set_name(__OIC_DEVICE_NAME__);

    OIC_LOG(DEBUG, TAG, "LEName Set");

    ble_begin();

    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;
}

CAResult_t CATerminateBle()
{
    OIC_LOG(DEBUG, TAG, "IN");
    ble_radio_reset();
    OIC_LOG(DEBUG, TAG, "OUT");
    return CA_STATUS_OK;

}

CAResult_t CAAddNewBleServiceInGattServer(const char *service_uuid)
{
    // Not Supported - Done at compile time
    return CA_STATUS_OK;
}

CAResult_t CARemoveBleServiceFromGattServer(const char *svc_path)
{
    // Not Supported - Done at compile time
    return CA_STATUS_OK;
}

CAResult_t CARemoveAllBleServicesFromGattServer()
{
    // Not Supported
    return CA_STATUS_OK;
}

CAResult_t CARegisterBleServicewithGattServer(const char *svc_path)
{
    // Not Supported - Done at compile time
    return CA_STATUS_OK;
}

CAResult_t CAAddNewCharacteristicsToGattServer(const char *svc_path,
        const char *char_uuid,
        const char *char_value,
        int char_value_len,
        int read)
{
    // Not Supported - Done at compile time
    return CA_STATUS_OK;
}

CAResult_t CARemoveCharacteristicsFromGattServer(const char *char_path)
{
    // Not Supported
    return CA_STATUS_OK;
}

unsigned char CAIsBleDataAvailable()
{
    return ble_available();
}

unsigned char CAIsBleConnected()
{
    return ble_connected();
}
char CAReadBleData()
{
    return (char)ble_read();
}

CAResult_t CABleDoEvents()
{
    ble_do_events();
    return CA_STATUS_OK;
}

CAResult_t CAUpdateCharacteristicsInGattServer(const char *char_value,
                                                            const uint32_t value_length)
{
    // Currently ble_write_bytes api returns void.
    ble_write_bytes((unsigned char *)char_value, (unsigned char)value_length);
    return CA_STATUS_OK;
}

