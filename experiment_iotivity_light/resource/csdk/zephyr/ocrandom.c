//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#include "ocrandom.h"
#include <stdio.h>

int8_t OCSeedRandom() {
    return 0;
}

void OCFillRandomMem(uint8_t * location, uint16_t len) {
    if (!location){
        return;
    }
    while (len--){
        *location++ = OCGetRandomByte();
    }
}

uint32_t OCGetRandom() {
    uint32_t result = 0;
    OCFillRandomMem((uint8_t*) &result, 4);
    return result;
}

uint8_t OCGetRandomByte(void) {
    return random(256) & 0x00FF;
}

OCRandomUuidResult OCGenerateUuid(uint8_t uuid[UUID_SIZE])
{
    // Fallback for all platforms is filling the array with random data
    OCFillRandomMem(uuid, UUID_SIZE);
    return RAND_UUID_OK;
}

#ifdef WITH_CLIENT
OCRandomUuidResult OCConvertUuidToString(const uint8_t uuid[UUID_SIZE],
                                         char uuidString[UUID_STRING_SIZE])
{
    int ret = snprintf(uuidString, UUID_STRING_SIZE,
            "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid[0], uuid[1], uuid[2], uuid[3],
            uuid[4], uuid[5], uuid[6], uuid[7],
            uuid[8], uuid[9], uuid[10], uuid[11],
            uuid[12], uuid[13], uuid[14], uuid[15]
            );

    if (ret != UUID_STRING_SIZE - 1)
    {
        return RAND_UUID_CONVERT_ERROR;
    }

    return RAND_UUID_OK;
}
#endif // WITH_CLIENT
