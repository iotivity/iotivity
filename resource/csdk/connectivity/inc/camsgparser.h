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

/**
 * @var MAX_DATA_LENGTH_SUPPORTED
 * @brief From the adapter level, this is the maximum data length is supported
 *           for the data transmission.
 */
#define MAX_DATA_LENGTH_SUPPORTED 4095

/**
 * @var NUMBER_OF_BITS_TO_IDENTIFY_DATA
 * @brief The number of bits allocated to represent data length in header.
 */
#define NUMBER_OF_BITS_TO_IDENTIFY_DATA 12

/**
 * @var NUMBER_OF_BITS_IN_CA_HEADER
 * @brief The length of the header in bits.
 */
#define NUMBER_OF_BITS_IN_CA_HEADER 15

/**
 * @var CA_HEADER_LENGTH
 * @brief The length of the header in bytes.
 */
#define CA_HEADER_LENGTH 2

#ifdef __TIZEN__
/**
 * @var PLATFORM_IDENTIFIER_BIT
 * @brief Reserved bit to differentiating the platform. Currently not in use.
 */
#define PLATFORM_IDENTIFIER_BIT 1

/**
 * @var CA_SUPPORTED_BLE_MTU_SIZE
 * @brief The MTU supported from Tizen platform for LE adapter.
 */
#define CA_SUPPORTED_BLE_MTU_SIZE  200
#else // #endif __TIZEN__

/**
 * @var PLATFORM_IDENTIFIER_BIT
 * @brief Reserved bit to differentiating the platform. Currently not in use.
 */
#define PLATFORM_IDENTIFIER_BIT 0

/**
 * @var CA_SUPPORTED_BLE_MTU_SIZE
 * @brief The MTU supported from Arduino platform for LE adapter.
 */
#define CA_SUPPORTED_BLE_MTU_SIZE  200
#endif //#endif __ARDUINO__

/**
 * @var HEADER_VERSION
 * @brief Current Header version.
 */
#define HEADER_VERSION 1

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************
 * @file The CA Header format                                                                      *
 * @brief CA Header will be difined by 2 bytes of Header.                                *
 *           First two bits : Header version                                                         *
 *           Third bit nad fourth bit: Reserved and will be set as Zero.                  *
 *           5th to 16th bit : 12 bits to provide the length of the data in bits.          *
 *                                                                                                               *
 *****************************************************************/


/**
* @fn printBinaryFormat
* @brief This function prints the binary format of one byte of data
*            This functoin is added as debug utility and can be used to check the
*            bits set in header while framing/parsing.
*
* @param[in]  data Pointer to the charcter data which needs to be printed.
*
* @return  0 on failure and 1 on success.
*/
void printBinaryFormat(char *data);

/**
* @fn CAGenerateHeader
* @brief This function is used to generate the CA specific header to maintain the fragmentation
*           logic. The header sturcture explained above will be formed and returned to the caller.
*
* @param[in]  data    Pointer to the charcter data which needs to be printed.
* @param[in]  length The total legth of the data which ll be represed from 5th -16th bits
*                              in the header.
*
* @return  0 on success otherwise a positive error value.
* @retval  CA_STATUS_OK  Successful
* @retval  CA_STATUS_INVALID_PARAM  Invalid input argumets
* @retval  CA_STATUS_FAILED Operation failed
*/
CAResult_t CAGenerateHeader(char *header, uint32_t length);

/**
* @fn CAParseHeader
* @brief This function is used to parse the header in the receiver end. This function will
*            provide the information of the total length of the data which has been fragmented.
*
* @param[in]  header  Pointer to the charcter data which contains the header information.
*                                Note that pointer should point to two bytes of data header
*                                 which needs to be parsed.
*
*/
uint32_t CAParseHeader(const char *header);

/**
* @fn CAFragmentData
* @brief This function is used to fragment the data in to the supported MTU and return the
*            data segment.
* @param[in]  data       Pointer to the Complete data which needs to be fragmented.
* @param[in]  TotalLen Total length of the data sent from the CA layer. This will be
*                                 decremented everytime when we call this function for the subsequent
*                                 time withthe MTU size.
* @param[in]  Offset     Initially this value will be zero. Later this value will be represented
*                                 as the amount of data which has already been fragmented.
*
* @param[out] dataSegment    Memory will be allocated and will be returned to the called
*                                            with the data. Caller needs to free the memory.
                        uint32_t offset);
*
* @return  -1 On error condition.
* @retval  Returns the length of the data which has been fragmented and sent to the upper layer.
*/
uint32_t CAFragmentData(const char *data, char **dataSegment, uint32_t TotalLen, uint32_t offset);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  // _CA_MSG_PARSER_H_
