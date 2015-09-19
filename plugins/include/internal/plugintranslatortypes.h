//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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
//******************************************************************


/**
 * @file
 *
 * This file contains the definition, types and APIs for resource(s) be
 * implemented.
 */

#ifndef PLUGINTRANSLATORTYPES_H_
#define PLUGINTRANSLATORTYPES_H_

#include "octypes.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// This file will hold structs which will help aid in abstraction of zigbee
// protocol to a PIPlugin.:

typedef struct
{
    PIPluginBase * header;
    struct PIResource_Zigbee * resource; // All resources which exist within this context.
    //Todo: Whatever other zigbee plugin specific stuff...
    //Todo:  zigbee_homeautomationprofile profile;
} PIPlugin_Zigbee;

/**
 *  Header for all PIResources.
 */
typedef struct
{
    struct PIResourceBase * next; // Linked list of resources.
    PIPluginBase * plugin; // Context this resource exists.
    OCResourceHandle * resourceHandle; // Handle to OIC Resource.
} PIResourceBase;

typedef struct
{
// Todo: This needs to map nicely to a struct that's defined in Zigbee_wrapper
    uint8_t placeholder;
// Todo: This struct will be refactored once Zigbee_Wrapper is finished.
} PIZigbeeProfile;

/**
 * Parameter list for a resource. Abstraction of PIResource.
 */
typedef struct
{
    PIResourceBase * header;
    PIPlugin_Zigbee * plugin; // Context which this Zigbee device exists.
    PIZigbeeProfile zigbeeProfile; // Representation of a Zigbee Device.
} PIResource_Zigbee;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* PLUGINTRANSLATORTYPES_H_ */
