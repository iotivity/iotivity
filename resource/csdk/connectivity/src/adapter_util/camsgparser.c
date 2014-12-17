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

    VERIFY_NON_NULL(header, NULL, "header is NULL");

    int temp = 0;

    memset(header, 0x0, sizeof(char) * 2);

    // Set the Header in the first two bits of the data as "01".
    *header |= 0 << 7;
    *header |= 1 << 6;

    // Set the NEXT TWO reserved bits to Zero.
    *header |= 0 << 5;
    *header |= 0 << 4;

    int i = 0;
    for (i = 11; i >= 0; i--)
    {
        temp = length >> i;
        if (temp & 1)
        {
            if (i > 7)
            {
                *header |= (1 << (i - 8));
            }
            else
            {
                *(header + 1) |= (1 << i);
            }
        }
        else
        {
            if (i > 7)
            {
                *header |= (0 << (i - 8));
            }
            else
            {
                *(header + 1) |= (0 << i);
            }
        }
    }

    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "OUT");

    return CA_STATUS_OK;

}

uint32_t CAParseHeader(const char *header)
{
    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "IN");

    VERIFY_NON_NULL(header, NULL, "header is NULL");

    int i, j;
    int dataLen = 0;
    for (i = 0; i < 8; i++)
    {
        int pos = 8 - i;
        int bit = !!((*header << i) & 0x80);
        if ( i >= 4)
        {
            if ( 1 == bit)
            {
                dataLen = pow (2, pos + 7) + dataLen;
            }
        }
    }

    for (j = 0; j < 8; j++)
    {
        int pos = 7 - j;
        int bit = !!((*(header + 1) << pos) & 0x80);
        if ( 1 == bit)
        {
            dataLen = pow (2, j) + dataLen;
        }
    }
    return dataLen;

}


uint32_t CAFragmentData(const char *data, char **dataSegment, uint32_t TotalLen, uint32_t offset)
{
    OIC_LOG(DEBUG, CA_MSG_PARSER_TAG, "IN");

    VERIFY_NON_NULL(data, NULL, "Param data is NULL");

    uint32_t length = 0;

    if (MAX_DATA_LENGTH_SUPPORTED < TotalLen)
    {
        OIC_LOG(ERROR, CA_MSG_PARSER_TAG, "dataLen is out of bound");
        return -1;
    }

    if ( CA_SUPPORTED_BLE_MTU_SIZE <= TotalLen)
    {
        length = CA_SUPPORTED_BLE_MTU_SIZE;
    }
    else
    {
        length = TotalLen;
    }

    if (0 == offset)
    {
        OIC_LOG_V(DEBUG, CA_MSG_PARSER_TAG, "generatingf the Header info");

        char *header = (char *) OICMalloc(sizeof(char) * CA_HEADER_LENGTH);
        VERIFY_NON_NULL_RET(header, CA_MSG_PARSER_TAG, "Malloc failed", -1);


        *dataSegment = (char *) OICMalloc(sizeof(char) * length);
        ///TODO: memory leak is here.
        VERIFY_NON_NULL_RET(*dataSegment, CA_MSG_PARSER_TAG, "Malloc failed", -1);

        memset(header, 0x0, sizeof(char) * CA_HEADER_LENGTH );
        memset(*dataSegment, 0x0, sizeof(char) * length );

        CAResult_t result = CAGenerateHeader(header, TotalLen);
        if (CA_STATUS_OK != result )
        {
            OIC_LOG(ERROR, CA_MSG_PARSER_TAG, "Generate header failed");
            OICFree(header);
            return -1;
        }

        memcpy(*dataSegment, header, CA_HEADER_LENGTH);
        OICFree(header);

        memcpy(*dataSegment + CA_HEADER_LENGTH, data, length - CA_HEADER_LENGTH );
        return length ;
    }
    else
    {
        OIC_LOG_V(DEBUG, CA_MSG_PARSER_TAG, "Appending actual data of length [%d] and offset [%d]", length,
                  offset);

        *dataSegment = (char *) OICMalloc(sizeof(char) * length);
        memcpy(*dataSegment, data + offset, length);
    }

    return length;

}

