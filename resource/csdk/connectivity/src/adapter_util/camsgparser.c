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

#define MAX_PARSE_DATA_LENGTH 4095

CAResult_t CAGenerateHeader(char *header, uint32_t length)
{
    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "IN");

    VERIFY_NON_NULL(header, CA_MSG_PARSER_TAG, "header is NULL");
    memset(header, 0x0, sizeof(char) * 2);

    if(length > MAX_PARSE_DATA_LENGTH)
    {
        OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "Given length is more than 4095.It will be truncated");
    }
    //if length is more than 4095 then it will be truncated.
    header[1] = length & 0xFF;
    length >>= 8;
    header[0] = length & 0x0F;

    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "OUT");
    return CA_STATUS_OK;
}

uint32_t CAParseHeader(const char *header)
{
    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "IN");

    VERIFY_NON_NULL(header, CA_MSG_PARSER_TAG, "header is NULL");

    uint32_t dataLen = ((header[0] & 0x0F) << 8) | (header[1] & 0xFF);

    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "OUT");
    return dataLen;
}


