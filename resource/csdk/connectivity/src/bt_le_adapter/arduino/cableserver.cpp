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

CAResult_t CAUpdateCharacteristicsToAllGattClients(const char *char_value,
                                                   uint8_t valueLength)
{
    // ble_write_bytes() api can send only max of 255 bytes at a time
    // This function shall never be called to send more than 255 bytes by the fragmentation logic.
    // Currently ble_write_bytes api returns void.
    ble_write_bytes((unsigned char *)char_value, (unsigned char)valueLength);
    return CA_STATUS_OK;
}


