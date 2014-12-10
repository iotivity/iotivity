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

/**
 * @file camsgparser.h
 * @brief This file contains common utility functions to manage the CA msg ragmentation and reassemebly.
 */


#ifndef _CA_MSG_PARSER_H_
#define _CA_MSG_PARSER_H_

#include "cacommon.h"
#include "logger.h"

#define MAX_DATA_LENGTH_SUPPORTED 4095
#define NUMBER_OF_BITS_TO_IDENTIFY_DATA 12
#define NUMBER_OF_BITS_IN_CA_HEADER 15
#define CA_HEADER_LENGTH 2

#ifdef __TIZEN__
#define PLATFORM_IDENTIFIER_BIT 1
#define CA_SUPPORTED_BLE_MTU_SIZE  200
#else // #endif __TIZEN__
#define PLATFORM_IDENTIFIER_BIT 0
#define CA_SUPPORTED_BLE_MTU_SIZE  198
#endif //#endif __ARDUINO__

#define HEADER_VERSION 1

#ifdef __cplusplus
extern "C"
{
#endif

/**
* @fn printBinaryFormat
* @brief
*
* @return
*/
void printBinaryFormat(char* data);


/**
 * @file The CA Header format
 * @brief CA Header will be difined by 2 bytes of Header.
 *           First two bits : Header version
 *           Third bit: Reserved and will be set as Zero.
 *           Fourth bit : PLATFORM_IDENTIFIER_BIT
 *           5th to 16th bit : 12 bits to provide the length of the data in bits.
 */

/**
* @fn CAGenerateHeader
* @brief
*
* @return
*/
CAResult_t CAGenerateHeader(char* header, uint32_t length);

/**
* @fn CAParseHeader
* @brief
*
* @return
*/
uint32_t CAParseHeader(const char* header);

/**
* @fn CAFragmentData
* @brief
*
* @return
*/
uint32_t CAFragmentData(const char* data, char** dataSegment, uint32_t TotalLen, 
                        uint32_t offset);

/**
* @fn CADeFragmentData
* @brief
*
* @return
*/
uint32_t CADeFragmentData(const char* datasegment, char** data, uint32_t TotalLen, 
                        uint32_t offset);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // _CA_MSG_PARSER_H_
