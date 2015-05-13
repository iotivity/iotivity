/* ****************************************************************
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
 * @file
 *
 * This file contains common utility functions to manage the CA msg
 * fragmentation and reassemebly.
 */

#ifndef CA_MSG_PARSER_H_
#define CA_MSG_PARSER_H_

#include "cacommon.h"
#include "logger.h"

/**
 * @var MAX_DATA_LENGTH_SUPPORTED
 * @brief From the adapter level, this is the maximum data length is supported
 *        for the data transmission.
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

/**
 * @var CA_SUPPORTED_BLE_MTU_SIZE
 * @brief The MTU supported for BLE adapter
 */
#define CA_SUPPORTED_BLE_MTU_SIZE  20

#ifdef __TIZEN__
/**
 * @var PLATFORM_IDENTIFIER_BIT
 * @brief Reserved bit to differentiating the platform. Currently not in use.
 */
#define PLATFORM_IDENTIFIER_BIT 1

/**
 * @var CA_SUPPORTED_EDR_MTU_SIZE
 * @brief The MTU supported from Tizen platform for EDR adapter.
 */
#define CA_SUPPORTED_EDR_MTU_SIZE  512

#elif __ANDROID__
/**
 * @var PLATFORM_IDENTIFIER_BIT
 * @brief Reserved bit to differentiating the platform. Currently not in use.
 */
#define PLATFORM_IDENTIFIER_BIT 0

/**
 * @var CA_SUPPORTED_EDR_MTU_SIZE
 * @brief The MTU supported from Android platform for EDR adapter.
 */
#define CA_SUPPORTED_EDR_MTU_SIZE  200

#elif __ARDUINO__
/**
 * @var PLATFORM_IDENTIFIER_BIT
 * @brief Reserved bit to differentiating the platform. Currently not in use.
 */
#define PLATFORM_IDENTIFIER_BIT 0

/**
 * @var CA_SUPPORTED_EDR_MTU_SIZE
 * @brief The MTU supported from Arduino platform for EDR adapter.
 */
#define CA_SUPPORTED_EDR_MTU_SIZE  200

#else //Other Platforms
/**
 * @var PLATFORM_IDENTIFIER_BIT
 * @brief Reserved bit to differentiating the platform. Currently not in use.
 */
#define PLATFORM_IDENTIFIER_BIT 0

/**
 * @var CA_SUPPORTED_EDR_MTU_SIZE
 * @brief The MTU supported for EDR adapter
 */
#define CA_SUPPORTED_EDR_MTU_SIZE  200

#endif

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
 * @file The CA Header format
 * @brief CA Header will be difined by 2 bytes of Header.
 * First two bits : Header version(Currently Its not being used)
 * Third bit and fourth bit: Reserved bits for future use.
 * 5th to 16th bit : 12 bits to provide the length of the data.
 *****************************************************************/

/**
* @fn CAGenerateHeader
* @brief This function is used to generate the CA specific header to maintain the fragmentation
*           logic. The header sturcture explained above will be formed and returned to the caller.
*
* @param[in]  data    Pointer to the charcter data which needs to be printed.
* @param[in]  length The total legth of the data which will be represented from 5th -16th bits
*                              in the header.
*
* @return  CA_STATUS_OK on success. One of theCA_STATUS_FAILED or other error values on error.
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

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /* CA_MSG_PARSER_H_ */

