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


/**
 * Maximum length of the response supported by Server for any REST request.
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
 * Maximum number of resources which can be contained inside collection
 * resource.
 */
#define MAX_CONTAINED_RESOURCES  (5)

#endif //OCSTACK_CONFIG_H_
