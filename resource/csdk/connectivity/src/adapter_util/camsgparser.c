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

#include "camsgparser.h"

#include <string.h>
#include <math.h>

#include "cacommon.h"
#include "caadapterutils.h"

#define CA_MSG_PARSER_TAG "CA_MSG_PARSER"


void printBinaryFormat(char *data)
{
    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "IN");
    int i;
    for (i = 0; i < 8; i++)
    {
        OIC_LOG_V(DEBUG, CA_MSG_PARSER_TAG, "c[%d]: %d", i, !!((*data << i) & 0x80));
    }

    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "OUT");

}

CAResult_t CAGenerateHeader(char *header, uint32_t length)
{
    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "IN");

    VERIFY_NON_NULL(header, CA_MSG_PARSER_TAG, "header is NULL");

    memset(header, 0x0, sizeof(char) * 2);

    *(header + 1) = length & 0xFF;
    length = length >> 8;
    *header = length & 0xFF;
    *header |= 0x40; // 0100 being set as first four bits.

    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "OUT");
    return CA_STATUS_OK;
}

uint32_t CAParseHeader(const char *header)
{
    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "IN");

    VERIFY_NON_NULL(header, CA_MSG_PARSER_TAG, "header is NULL");
    uint32_t dataLen = 0;

    char localHeader[CA_HEADER_LENGTH];
    memcpy(localHeader, header, CA_HEADER_LENGTH);
    *localHeader &= 0x0F;
    dataLen = *localHeader & 0xFF;
    dataLen = dataLen << 8;
    dataLen += *(localHeader + 1) & 0xFF;

    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "OUT");
    return dataLen;
}

