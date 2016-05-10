//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

#ifndef IOTVT_SRM_UTILITY_H
#define IOTVT_SRM_UTILITY_H

#include "ocstack.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include "uri.h"
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


typedef struct OicParseQueryIter OicParseQueryIter_t;

/**
 * @brief   OicRestQueryIter data structure is used for book-keeping
 *          sub-REST query's attribute's and value's, starting location &
 *          length between calls to GetNextQuery(). This struct needs
 *          to be first initialized with ParseQueryIterInit().
 *
 */
struct OicParseQueryIter
{
    unsigned char * attrPos;    /**<stating location of attribute */
    size_t attrLen;             /**<length of the attribute */
    unsigned char * valPos;     /**<starting location of value*/
    size_t valLen;              /**<length of the value*/
    coap_parse_iterator_t pi;   /**<coap struct for tokenizing the query*/
};

/**
 * @def VERIFY_SUCCESS
 * @brief Macro to verify success of operation.
 * eg: VERIFY_SUCCESS(TAG, OC_STACK_OK == foo(), ERROR);
 * @note Invoking function must define "exit:" label for goto functionality to work correctly.
 *
 */
#define VERIFY_SUCCESS(tag, op, logLevel) { if (!(op)) \
            {OC_LOG((logLevel), tag, PCF(#op " failed!!")); goto exit;} }

/**
 * @def VERIFY_NON_NULL
 * @brief Macro to verify argument is not equal to NULL.
 * eg: VERIFY_NON_NULL(TAG, ptrData, ERROR);
 * @note Invoking function must define "exit:" label for goto functionality to work correctly.
 *
 */
#define VERIFY_NON_NULL(tag, arg, logLevel) { if (NULL == (arg)) \
            { OC_LOG((logLevel), tag, PCF(#arg " is NULL")); goto exit; } }

/**
 * This method initializes the OicParseQueryIter_t struct
 *
 *@param query     - REST query, to be parsed
 *@param parseIter - OicParseQueryIter_t struct, to be initialized
 *
 */
void ParseQueryIterInit(unsigned char * query, OicParseQueryIter_t * parseIter);


/**
 * This method fills the OicParseQueryIter_t struct with next REST query's
 * attribute's and value's information
 *
 *@param parseIter - OicParseQueryIter_t struct, has next query's attribute's & value's info
 *
 * @retval
 *     OicParseQueryIter_t *  - has parsed query info
 *     NULL                   - has no query to parse
 */
OicParseQueryIter_t * GetNextQuery(OicParseQueryIter_t * parseIter);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //IOTVT_SRM_UTILITY_H
