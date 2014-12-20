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



#ifndef _CMCONFIG_H
#define _CMCONFIG_H

/** \defgroup ControlManager Control Manager
 *
 * \brief This module contains the APIs that provide methods and interfaces to initialize, start, stop, configure the Control Manager.
 */

/** \defgroup Client Client
 *
 * \brief Contains the interfaces required to control the Control Manager resources. This module exposes resource specific controller classes to access and control a specific resource.
 * Allows fetching of the resource details. Provides methods to modify the resource attributes/properties. Provides controller classes using which any resource can be accessed.
 * To use these resources, the application developer must have the knowledge of input and output data model format of the request and responses.
 */

/** \defgroup DeviceFinder Device Finder
 *
 * \brief Provides methods to discover the devices in the network. Provides interfaces to get the list of discovered devices, filter the discovered devices. Application developer can register callbacks to handle the discovery notifications when a device
 * enters or leaves the network.
 */

/** \defgroup Notification Notification
 *
 * \brief This module provides interfaces for the Control Manager to initiate subscription requests, register the methods to handle the notifications & subscription responsesfrom the controlee devices.
 */

/** \defgroup xsd XSD
 *
 * \brief This module contains the XSD(XML Schema Definition) i.e the data model of the respective resources. Using the XSD of a resource. Developer can access the data model object to read the attribute values of the resource, modify to set the desired values.
 */

#if defined(_WIN32) // MS Compiler
#ifdef __MINGW64__
#include "stdint.h"
#endif
#ifdef __MINGW32__
#include "stdint.h"
#endif
#ifdef STATIC_LIB
#       define CMAPI
#else
#   if defined(CMAPI_EXPORTS)
#       define CMAPI __declspec(dllexport)
#   else
#       define CMAPI __declspec(dllimport)
#   endif
#endif

#elif defined(__GNUG__)   // GCC
#   define CMAPI __attribute__((visibility("default")))
#include "stdint.h"
#elif defined(__ARMCC_VERSION)  // ARM Compiler (RVCT 3.1)
#   define CMAPI

#else
#   define CMAPI

#endif

#ifdef _MSC_VER
// Compiling with Visual studio
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

#endif


#endif /* _CMCONFIG_H */
