//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

#ifndef OCSTACK_CONFIG_H_
#define OCSTACK_CONFIG_H_

// This file contains all the variables which can be configured/modified as
// per platform or specific product usage scenarios.

/** This would need to be updated in future as we move to support IPv6 and other
 *  technologies.
 */
#define DEV_ADDR_SIZE_MAX (16)

/**
 * Maximum length of the response supported by Server for any REST request.
 * The actual repsonse length is 256 for Arduino and 1024 for other platforms.
 * Note that the stack will add a prefix and suffix to the payload.
 * Prefix : {"oic":[
 * Suffix : ]}
 * They have a combined size of 10 bytes.
 */
#ifdef WITH_ARDUINO
#define MAX_RESPONSE_LENGTH (256)
#else
#define MAX_RESPONSE_LENGTH (1024)
#endif

/**
 * Maximum length of the request supported by Client/Server for any REST request.
 */
#ifdef WITH_ARDUINO
#define MAX_REQUEST_LENGTH (256)
#else
#define MAX_REQUEST_LENGTH (1024)
#endif

/**
 * Maximum length of the URI supported by client/server while processing
 * REST requests/responses.
 */
#define MAX_URI_LENGTH (64)

/**
 * Maximum length of the query supported by client/server while processing
 * REST requests/responses.
 */
#define MAX_QUERY_LENGTH (64)

/**
 * Maximum length of the Manufacturer name supported by the server
 * for manufacturer name
 */
#define MAX_MANUFACTURER_NAME_LENGTH (16)

/**
 * Maximum length of the URL to the Manufacturer details supported by
 * the server.
 */
#define MAX_MANUFACTURER_URL_LENGTH (32)

/**
 * Maximum number of resources which can be contained inside collection
 * resource.
 */
#define MAX_CONTAINED_RESOURCES  (5)

/**
 *  Maximum number of vendor specific header options an application can set or receive in pdu
 */
#define MAX_HEADER_OPTIONS (2)

/**
 *  Maximum Length of the vendor specific header option
 */
#define MAX_HEADER_OPTION_DATA_LENGTH (16)

/*
 * Sets the time to live (TTL) for response callbacks.
 * The callbacks will be up for deletion after such time but are not guaranteed
 * to be deleted immediately and you may get responses even after timeout.
 * This timeout will NOT apply to OBSERVE requests. OBSERVE needs an explicit cancel using OCCancel().
 * NOTE: Changing the setting to a very long duration may lead to unsupported and untested
 * operation. Setting this to as small a value as reasonable will reclaim memory faster.
 */
#define MAX_CB_TIMEOUT_SECONDS   (2 * 60 * 60)  // 2 hours = 7200 seconds.

#endif //OCSTACK_CONFIG_H_
