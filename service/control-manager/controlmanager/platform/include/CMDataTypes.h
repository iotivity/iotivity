//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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



#ifndef CMDATATYPES_H_
#define CMDATATYPES_H_

#ifdef _MSC_VER
#ifndef BASIC_TYPES_DEFINED
#define BASIC_TYPES_DEFINED
typedef __int64     int64_t;
typedef __int32     int32_t;
typedef __int16     int16_t;
typedef __int8      int8_t;
typedef unsigned __int64    uint64_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int8     uint8_t;
#endif

#else
#include "stdint.h"
#include "stdlib.h"
#endif

#include "stddef.h"

#include "string"
#ifdef __APPLE__
#ifdef TARGET_OS_IPHONE
#include "malloc/malloc.h"
#endif
#else
#include "malloc.h"
#endif
#include "string.h"

#include "CMAPIDataTypes.h"

#define CM_STRING_NPOS                  std::string::npos

#define shp_realloc(pointer, size)          realloc(pointer, size)
#define shp_free(pointer)                   free(pointer)
#define shp_malloc(size)                    malloc(size)
#define shp_calloc(num, size)               calloc(num, size)

#define shp_memmove(dest, src, size)        memmove(dest, src, size)
#define shp_memcpy(dest, src, size)         memcpy(dest, src, size)

#ifdef _MSC_VER
#define shp_strcasecmp(str1, str2)          _stricmp(str1, str2)
#else
#define shp_strcasecmp(str1, str2)          strcasecmp(str1, str2)
#endif
#define shp_hex_to_decimal(str)             strtol(str, NULL, 16)

#define CM_CONTENT_TYPE_JSON                "application/json"
#define CM_CONTENT_TYPE_OC                  "application/oc"    // OIC application type
#define CM_CONTENT_TYPE_JSON_UTF8           "application/json; charset=utf-8"
#define DEFAULT_SOCKET_TIMEOUT              300

#endif /* CMDATATYPES_H_ */
