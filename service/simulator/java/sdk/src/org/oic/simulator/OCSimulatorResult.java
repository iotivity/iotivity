/*
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 */

/**
 * This file provides a Enum which contains Status codes for Success and Errors.
 */
package org.oic.simulator;

/**
 * This Enum contains Status codes for Success and Errors.
 */
public enum OCSimulatorResult {
    OC_STACK_OK,
    OC_STACK_RESOURCE_CREATED,
    OC_STACK_RESOURCE_DELETED,
    OC_STACK_CONTINUE,
    OC_STACK_INVALID_URI,
    OC_STACK_INVALID_QUERY,
    OC_STACK_INVALID_IP,
    OC_STACK_INVALID_PORT,
    OC_STACK_INVALID_CALLBACK,
    OC_STACK_INVALID_METHOD,
    OC_STACK_INVALID_PARAM,
    OC_STACK_INVALID_OBSERVE_PARAM,
    OC_STACK_NO_MEMORY,
    OC_STACK_COMM_ERROR,
    OC_STACK_NOTIMPL,
    OC_STACK_NO_RESOURCE,
    OC_STACK_RESOURCE_ERROR,
    OC_STACK_SLOW_RESOURCE,
    OC_STACK_REPEATED_REQUEST,
    OC_STACK_NO_OBSERVERS,
    OC_STACK_OBSERVER_NOT_FOUND,
    OC_STACK_VIRTUAL_DO_NOT_HANDLE,
    OC_STACK_INVALID_OPTION,
    OC_STACK_MALFORMED_RESPONSE,
    OC_STACK_PERSISTENT_BUFFER_REQUIRED,
    OC_STACK_INVALID_REQUEST_HANDLE,
    OC_STACK_INVALID_DEVICE_INFO,
    OC_STACK_INVALID_JSON,
    OC_STACK_PRESENCE_STOPPED,
    OC_STACK_PRESENCE_TIMEOUT,
    OC_STACK_PRESENCE_DO_NOT_HANDLE,
    OC_STACK_ERROR,
    OC_STACK_LISTENER_NOT_SET;

    public static OCSimulatorResult conversion(int ordinal) {

        OCSimulatorResult result = OCSimulatorResult.values()[31];

        if (ordinal == 0)
            result = OCSimulatorResult.values()[0];
        else if (ordinal == 1)
            result = OCSimulatorResult.values()[1];
        else if (ordinal == 2)
            result = OCSimulatorResult.values()[2];
        else if (ordinal == 3)
            result = OCSimulatorResult.values()[3];

        else if (ordinal == 20)
            result = OCSimulatorResult.values()[4];
        else if (ordinal == 21)
            result = OCSimulatorResult.values()[5];
        else if (ordinal == 22)
            result = OCSimulatorResult.values()[6];
        else if (ordinal == 23)
            result = OCSimulatorResult.values()[7];
        else if (ordinal == 24)
            result = OCSimulatorResult.values()[8];
        else if (ordinal == 25)
            result = OCSimulatorResult.values()[9];
        else if (ordinal == 26)
            result = OCSimulatorResult.values()[10];
        else if (ordinal == 27)
            result = OCSimulatorResult.values()[11];
        else if (ordinal == 28)
            result = OCSimulatorResult.values()[12];
        else if (ordinal == 29)
            result = OCSimulatorResult.values()[13];
        else if (ordinal == 30)
            result = OCSimulatorResult.values()[14];
        else if (ordinal == 31)
            result = OCSimulatorResult.values()[15];
        else if (ordinal == 32)
            result = OCSimulatorResult.values()[16];
        else if (ordinal == 33)
            result = OCSimulatorResult.values()[17];
        else if (ordinal == 34)
            result = OCSimulatorResult.values()[18];
        else if (ordinal == 35)
            result = OCSimulatorResult.values()[19];
        else if (ordinal == 36)
            result = OCSimulatorResult.values()[20];
        else if (ordinal == 37)
            result = OCSimulatorResult.values()[21];
        else if (ordinal == 38)
            result = OCSimulatorResult.values()[22];
        else if (ordinal == 39)
            result = OCSimulatorResult.values()[23];
        else if (ordinal == 40)
            result = OCSimulatorResult.values()[24];
        else if (ordinal == 41)
            result = OCSimulatorResult.values()[25];
        else if (ordinal == 42)
            result = OCSimulatorResult.values()[26];
        else if (ordinal == 43)
            result = OCSimulatorResult.values()[27];

        else if (ordinal == 128)
            result = OCSimulatorResult.values()[28];
        else if (ordinal == 129)
            result = OCSimulatorResult.values()[29];
        else if (ordinal == 130)
            result = OCSimulatorResult.values()[30];

        return result;
    }
}