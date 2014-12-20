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

#include "CDSerializationFactory.h"
#include "Elements.h"
#include "ResourceTypeEnum.h"
#include "ISerializable.h"
#include "CDErrorMessage.h"

using namespace OC;

/**
 * Instantiates request and response object that are identified by specific resource and http GET method.
 *
 * @param[in] resourceType  Type of resource (Device, Configuration, Operation, etc.)
 *
 * @param[out] pp_request   Instantiated serializable request object
 * @param[out] pp_response  Instantiated serializable response object
 *
 * @return  @c True If the object is successfully instantiated @n
 *          @c False In case of any error occurred
 */
bool createGetMethodSerializables( int resourceType,
                                   OC::Cm::Serialization::ISerializable **pp_request,
                                   OC::Cm::Serialization::ISerializable **pp_response )
{
    switch (resourceType)
    {

#if 0 // OIC IGNORE

        case ::RT_AVSource: { *pp_request = NULL; *pp_response = new ::AVSource(); return true ;}
        case ::RT_AVSources: { *pp_request = NULL; *pp_response = new ::AVSources(); return true ;}
        case ::RT_Action: { *pp_request = NULL; *pp_response = new ::Action(); return true ;}
        case ::RT_Actions: { *pp_request = NULL; *pp_response = new ::Actions(); return true ;}
        case ::RT_Audio: { *pp_request = NULL; *pp_response = new ::Audio(); return true ;}
        case ::RT_Camera: { *pp_request = NULL; *pp_response = new ::Camera(); return true ;}
        case ::RT_Capture: { *pp_request = NULL; *pp_response = new ::Capture(); return true ;}
        case ::RT_Captures: { *pp_request = NULL; *pp_response = new ::Captures(); return true ;}
        case ::RT_CustomRecipe: { *pp_request = NULL; *pp_response = new ::CustomRecipe(); return true ;}
        case ::RT_CustomRecipes: { *pp_request = NULL; *pp_response = new ::CustomRecipes(); return true ;}
        case ::RT_DefrostReservation: { *pp_request = NULL; *pp_response = new ::DefrostReservation(); return true ;}
        case ::RT_DefrostReservations: { *pp_request = NULL; *pp_response = new ::DefrostReservations(); return true ;}
        case ::RT_Food: { *pp_request = NULL; *pp_response = new ::Food(); return true ;}
        case ::RT_FoodImage: { *pp_request = NULL; *pp_response = new ::FoodImage(); return true ;}
        case ::RT_FoodImages: { *pp_request = NULL; *pp_response = new ::FoodImages(); return true ;}
        case ::RT_FoodList: { *pp_request = NULL; *pp_response = new ::FoodList(); return true ;}
        case ::RT_FoodManager: { *pp_request = NULL; *pp_response = new ::FoodManager(); return true ;}
        case ::RT_Fridge: { *pp_request = NULL; *pp_response = new ::Fridge(); return true ;}
        case ::RT_Group: { *pp_request = NULL; *pp_response = new ::Group(); return true ;}
        case ::RT_Groups: { *pp_request = NULL; *pp_response = new ::Groups(); return true ;}
        case ::RT_Oven: { *pp_request = NULL; *pp_response = new ::Oven(); return true ;}
        case ::RT_Pattern: { *pp_request = NULL; *pp_response = new ::Pattern(); return true ;}
        case ::RT_Patterns: { *pp_request = NULL; *pp_response = new ::Patterns(); return true ;}
        case ::RT_Shopping: { *pp_request = NULL; *pp_response = new ::Shopping(); return true ;}
        case ::RT_ShoppingList: { *pp_request = NULL; *pp_response = new ::ShoppingList(); return true ;}
        case ::RT_Wind: { *pp_request = NULL; *pp_response = new ::Wind(); return true ;}
        case ::RT_Washer: { *pp_request = NULL; *pp_response = new ::Washer(); return true ;}

#endif

        case ::RT_AccessPoint: { *pp_request = NULL; *pp_response = new ::AccessPoint(); return true ;}
        case ::RT_AccessPoints: { *pp_request = NULL; *pp_response = new ::AccessPoints(); return true ;}

        case ::RT_Alarm: { *pp_request = NULL; *pp_response = new ::Alarm(); return true ;}
        case ::RT_Alarms: { *pp_request = NULL; *pp_response = new ::Alarms(); return true ;}

        case ::RT_Capability: { *pp_request = NULL; *pp_response = new ::Capability(); return true ;}

        case ::RT_Configuration: { *pp_request = NULL; *pp_response = new ::Configuration(); return true ;}

        case ::RT_DRLC: { *pp_request = NULL; *pp_response = new ::DRLC(); return true ;}

        case ::RT_Detection: { *pp_request = NULL; *pp_response = new ::Detection(); return true ;}
        case ::RT_Device: { *pp_request = NULL; *pp_response = new ::Device(); return true ;}
        case ::RT_Devices: { *pp_request = NULL; *pp_response = new ::Devices(); return true ;}
        case ::RT_Door: { *pp_request = NULL; *pp_response = new ::Door(); return true ;}
        case ::RT_Doors: { *pp_request = NULL; *pp_response = new ::Doors(); return true ;}
        case ::RT_Energy: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_EnergyConsumption: { *pp_request = NULL; *pp_response = new ::EnergyConsumption(); return true ;}

        case ::RT_Humidity: { *pp_request = NULL; *pp_response = new ::Humidity(); return true ;}
        case ::RT_Information: { *pp_request = NULL; *pp_response = new ::Information(); return true ;}
        case ::RT_Level: { *pp_request = NULL; *pp_response = new ::Level(); return true ;}
        case ::RT_Light: { *pp_request = NULL; *pp_response = new ::Light(); return true ;}
        case ::RT_LongPollingNotification: { *pp_request = NULL; *pp_response = new ::LongPollingNotification(); return true ;}
        case ::RT_Message: { *pp_request = NULL; *pp_response = new ::Message(); return true ;}
        case ::RT_Messages: { *pp_request = NULL; *pp_response = new ::Messages(); return true ;}
        case ::RT_Mode: { *pp_request = NULL; *pp_response = new ::Mode(); return true ;}
        case ::RT_Movement: { *pp_request = NULL; *pp_response = new ::Movement(); return true ;}
        case ::RT_Network: { *pp_request = NULL; *pp_response = new ::Network(); return true ;}
        case ::RT_Networks: { *pp_request = NULL; *pp_response = new ::Networks(); return true ;}
        case ::RT_Notification: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Notifications: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Operation: { *pp_request = NULL; *pp_response = new ::Operation(); return true ;}

        case ::RT_Record: { *pp_request = NULL; *pp_response = new ::Record(); return true ;}
        case ::RT_Records: { *pp_request = NULL; *pp_response = new ::Records(); return true ;}
        case ::RT_Remote: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_Subscription: { *pp_request = NULL; *pp_response = new ::Subscription(); return true ;}
        case ::RT_Subscriptions: { *pp_request = NULL; *pp_response = new ::Subscriptions(); return true ;}
        case ::RT_Temperature: { *pp_request = NULL; *pp_response = new ::Temperature(); return true ;}
        case ::RT_Temperatures: { *pp_request = NULL; *pp_response = new ::Temperatures(); return true ;}
        case ::RT_Thermostat: { *pp_request = NULL; *pp_response = new ::Thermostat(); return true ;}
        case ::RT_Time: { *pp_request = NULL; *pp_response = new ::Time(); return true ;}
        case ::RT_UsageLimit: { *pp_request = NULL; *pp_response = new ::UsageLimit(); return true ;}
        case ::RT_UsageLimits: { *pp_request = NULL; *pp_response = new ::UsageLimits(); return true ;}
        case ::RT_Usages: { *pp_request = NULL; *pp_response = new ::Usages(); return true ;}
        case ::RT_Version: { *pp_request = NULL; *pp_response = new ::Version(); return true ;}
        case ::RT_Versions: { *pp_request = NULL; *pp_response = new ::Versions(); return true ;}

        case ::RT_WiFi: { *pp_request = NULL; *pp_response = new ::WiFi(); return true ;}
        case ::RT_DummyLight: { *pp_request = NULL; *pp_response = new ::Light(); return true ;}


    }

    return false;
}

/**
 * Instantiates request and response object that are identified by specific resource and http PUT method.
 *
 * @param[in] resourceType  Type of resource (Device, Configuration, Operation, etc.)
 *
 * @param[out] pp_request   Instantiated serializable request object
 * @param[out] pp_response  Instantiated serializable response object
 *
 * @return  @c True If the object is successfully instantiated @n
 *          @c False In case of any error occurred
 */
bool createPutMethodSerializables( int resourceType,
                                   OC::Cm::Serialization::ISerializable **pp_request,
                                   OC::Cm::Serialization::ISerializable **pp_response )
{
    switch (resourceType)
    {
#if 0 // OIC IGNORE

        case ::RT_AVSource: { *pp_request = new ::AVSource(); *pp_response = NULL; return true ;}
        case ::RT_AVSources: { *pp_request = new ::AVSources(); *pp_response = NULL; return true ;}
        case ::RT_Action: { *pp_request = new ::Action(); *pp_response = NULL; return true ;}
        case ::RT_Actions: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Audio: { *pp_request = new ::Audio(); *pp_response = NULL; return true ;}
        case ::RT_Camera: { *pp_request = new ::Camera(); *pp_response = NULL; return true ;}
        case ::RT_Capture: { *pp_request = new ::Capture(); *pp_response = NULL; return true ;}
        case ::RT_Captures: { *pp_request = new ::Captures(); *pp_response = NULL; return true ;}
        case ::RT_CustomRecipe: { *pp_request = new ::CustomRecipe(); *pp_response = NULL; return true ;}
        case ::RT_DefrostReservation: { *pp_request = new ::DefrostReservation(); *pp_response = NULL; return true ;}
        case ::RT_DefrostReservations: { *pp_request = new ::DefrostReservations(); *pp_response = NULL; return true ;}
        case ::RT_Food: { *pp_request = new ::Food(); *pp_response = NULL; return true ;}
        case ::RT_FoodImage: { *pp_request = new ::FoodImage(); *pp_response = NULL; return true ;}
        case ::RT_FoodImages: { *pp_request = new ::FoodImages(); *pp_response = NULL; return true ;}
        case ::RT_FoodList: { *pp_request = new ::FoodList(); *pp_response = NULL; return true ;}
        case ::RT_FoodManager: { *pp_request = new ::FoodManager(); *pp_response = NULL; return true ;}
        case ::RT_Fridge: { *pp_request = new ::Fridge(); *pp_response = NULL; return true ;}
        case ::RT_Group: { *pp_request = new ::Group(); *pp_response = NULL; return true ;}
        case ::RT_Groups: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Oven: { *pp_request = new ::Oven(); *pp_response = NULL; return true ;}
        case ::RT_Pattern: { *pp_request = new ::Pattern(); *pp_response = NULL; return true ;}
        case ::RT_Patterns: { *pp_request = new ::Pattern(); *pp_response = NULL; return true ;}
        case ::RT_Shopping: { *pp_request = new ::Shopping(); *pp_response = NULL; return true ;}
        case ::RT_ShoppingList: { *pp_request = new ::ShoppingList(); *pp_response = NULL; return true ;}
        case ::RT_Washer: { *pp_request = new ::Washer(); *pp_response = NULL; return true ;}
        case ::RT_Wind: { *pp_request = new ::Wind(); *pp_response = NULL; return true ;}

#endif // OIC IGNORE

        case ::RT_AccessPoint: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_AccessPoints: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_Alarm: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Alarms: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_Capability: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_Configuration: { *pp_request = new ::Configuration(); *pp_response = NULL; return true ;}

        case ::RT_CustomRecipes: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_DRLC: { *pp_request = new ::DRLC(); *pp_response = NULL; return true ;}

        case ::RT_Detection: { *pp_request = new ::Detection(); *pp_response = NULL; return true ;}
        case ::RT_Device: { *pp_request = new ::Device(); *pp_response = NULL; return true ;}
        case ::RT_Devices: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Door: { *pp_request = new ::Door(); *pp_response = NULL; return true ;}
        case ::RT_Doors: { *pp_request = new ::Doors(); *pp_response = NULL; return true ;}
        case ::RT_Energy: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_EnergyConsumption: { *pp_request = new ::EnergyConsumption(); *pp_response = NULL; return true ;}

        case ::RT_Humidity: { *pp_request = new ::Humidity(); *pp_response = NULL; return true ;}
        case ::RT_Information: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Level: { *pp_request = new ::Level(); *pp_response = NULL; return true ;}
        case ::RT_Light: { *pp_request = new ::Light(); *pp_response = NULL; return true ;}
        case ::RT_LongPollingNotification: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Message: { *pp_request = new ::Message(); *pp_response = NULL; return true ;}
        case ::RT_Messages: { *pp_request = new ::Messages(); *pp_response = NULL; return true ;}
        case ::RT_Mode: { *pp_request = new ::Mode(); *pp_response = NULL; return true ;}
        case ::RT_Movement: { *pp_request = new ::Movement(); *pp_response = NULL; return true ;}
        case ::RT_Network: { *pp_request = new ::Network(); *pp_response = NULL; return true ;}
        case ::RT_Networks: { *pp_request = new ::Networks(); *pp_response = NULL; return true ;}
        case ::RT_Notification: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Notifications: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Operation: { *pp_request = new ::Operation(); *pp_response = NULL; return true ;}

        case ::RT_Record: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Records: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Remote: { *pp_request = new ::Remote(); *pp_response = NULL; return true ;}

        case ::RT_Subscription: { *pp_request = new ::Subscription(); *pp_response = NULL; return true ;}
        case ::RT_Subscriptions: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Temperature: { *pp_request = new ::Temperature(); *pp_response = NULL; return true ;}
        case ::RT_Temperatures: { *pp_request = new ::Temperatures(); *pp_response = NULL; return true ;}
        case ::RT_Thermostat: { *pp_request = new ::Thermostat(); *pp_response = NULL; return true ;}
        case ::RT_Time: { *pp_request = new ::Time(); *pp_response = NULL; return true ;}
        case ::RT_UsageLimit: { *pp_request = new ::UsageLimit(); *pp_response = NULL; return true ;}
        case ::RT_UsageLimits: { *pp_request = new ::UsageLimits(); *pp_response = NULL; return true ;}
        case ::RT_Usages: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Version: { *pp_request = new ::Version(); *pp_response = NULL; return true ;}
        case ::RT_Versions: { *pp_request = new ::Versions(); *pp_response = NULL; return true ;}

        case ::RT_WiFi: { *pp_request = new ::WiFi(); *pp_response = NULL; return true ;}


    }

    return false;
}

/**
 * Instantiates request and response object that are identified by specific resource and http POST method.
 *
 * @param[in] resourceType  Type of resource (Device, Configuration, Operation, etc.)
 *
 * @param[out] pp_request   Instantiated serializable request object
 * @param[out] pp_response  Instantiated serializable response object
 *
 * @return  @c True If the object is successfully instantiated @n
 *          @c False In case of any error occurred
 */
bool createPostMethodSerializables( int resourceType,
                                    OC::Cm::Serialization::ISerializable **pp_request,
                                    OC::Cm::Serialization::ISerializable **pp_response )
{
    switch (resourceType)
    {
        case ::RT_AVSource: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_AVSources: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_AccessPoint: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_AccessPoints: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Action: { *pp_request = NULL; *pp_response = NULL; return true ;}

#if 0 // OIC IGNORE

        case ::RT_Actions: { *pp_request = new ::Action(); *pp_response = NULL; return true ;}
        case ::RT_Captures: { *pp_request = new ::Capture(); *pp_response = new ::Capture(); return true ;}
        case ::RT_CustomRecipes: { *pp_request = new ::CustomRecipe(); *pp_response = NULL; return true ;}
        case ::RT_DefrostReservations: { *pp_request = new ::DefrostReservation(); *pp_response = NULL; return true ;}
        case ::RT_FoodImages: { *pp_request = new ::FoodImage(); *pp_response = NULL; return true ;}
        case ::RT_FoodList: { *pp_request = new ::Food(); *pp_response = NULL; return true ;}
        case ::RT_Groups: { *pp_request = new ::Group(); *pp_response = NULL; return true ;}
        case ::RT_Patterns: { *pp_request = new ::Pattern(); *pp_response = NULL; return true ;}
        case ::RT_ShoppingList: { *pp_request = new ::Shopping(); *pp_response = NULL; return true ;}


#endif


        case ::RT_Alarm: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Alarms: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Audio: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Camera: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Capability: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Capture: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_Configuration: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_CustomRecipe: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_DRLC: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_DefrostReservation: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_Detection: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Device: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Devices: { *pp_request = new ::Device(); *pp_response = NULL; return true ;}
        case ::RT_Door: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Doors: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Energy: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_EnergyConsumption: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Food: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_FoodImage: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_FoodManager: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Fridge: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Group: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_Humidity: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Information: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Level: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Light: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_LongPollingNotification: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Message: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Messages: { *pp_request = new ::Message(); *pp_response = NULL; return true ;}
        case ::RT_Mode: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Movement: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Network: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Networks: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Notification: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Notifications: { *pp_request = new ::Notification(); *pp_response = NULL; return true ;}
        case ::RT_Operation: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Oven: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Pattern: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_Record: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Records: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Remote: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Shopping: { *pp_request = NULL; *pp_response = NULL; return true ;}

        case ::RT_Subscription: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Subscriptions: { *pp_request = new ::Subscription(); *pp_response = NULL; return true ;}
        case ::RT_Temperature: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Temperatures: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Thermostat: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Time: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_UsageLimit: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_UsageLimits: { *pp_request = new ::UsageLimit(); *pp_response = NULL; return true ;}
        case ::RT_Usages: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Version: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Versions: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Washer: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_WiFi: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Wind: { *pp_request = NULL; *pp_response = NULL; return true ;}

    }

    return false;
}

/**
 * Instantiates request and response object that are identified by specific resource and http DELETE method.
 *
 * @param[in] resourceType  Type of resource (Device, Configuration, Operation, etc.)
 *
 * @param[out] pp_request   Instantiated serializable request object
 * @param[out] pp_response  Instantiated serializable response object
 *
 * @return  @c True If the object is successfully instantiated @n
 *          @c False In case of any error occurred
 */
bool createDeleteMethodSerializables( int resourceType,
                                      OC::Cm::Serialization::ISerializable **pp_request,
                                      OC::Cm::Serialization::ISerializable **pp_response )
{
    switch (resourceType)
    {
        case ::RT_AVSource: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_AVSources: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_AccessPoint: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_AccessPoints: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Action: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Actions: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Alarm: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Alarms: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Audio: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Camera: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Capability: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Capture: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Captures: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Configuration: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_CustomRecipe: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_CustomRecipes: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_DRLC: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_DefrostReservation: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_DefrostReservations: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Detection: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Device: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Devices: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Door: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Doors: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Energy: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_EnergyConsumption: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Food: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_FoodImage: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_FoodImages: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_FoodList: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_FoodManager: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Fridge: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Group: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Groups: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Humidity: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Information: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Level: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Light: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_LongPollingNotification: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Message: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Messages: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Mode: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Movement: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Network: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Networks: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Notification: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Notifications: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Operation: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Oven: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Pattern: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Patterns: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Record: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Records: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Remote: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Shopping: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_ShoppingList: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Subscription: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Subscriptions: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Temperature: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Temperatures: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Thermostat: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Time: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_UsageLimit: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_UsageLimits: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Usages: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Version: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Versions: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Washer: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_WiFi: { *pp_request = NULL; *pp_response = NULL; return true ;}
        case ::RT_Wind: { *pp_request = NULL; *pp_response = NULL; return true ;}

    }

    return false;
}


/**
 * Default constructor of SHPSerializationFactory class
 */
CDSerializationFactory::CDSerializationFactory()
{

}

/**
 * Default virtual destructor of SHPSerializationFactory class
 */
CDSerializationFactory::~CDSerializationFactory()
{

}

/**
 * Instantiates any of the serializable object which is related to the specific element.
 *
 * @param[in] elementType Type of the element (Device, Configuration, Operation, etc.)
 * @param[out] pp_output Instantiated serializable object
 *
 * @return  @c True If the object is successfully instantiated @n
 *          @c False In case of any error occurred
 */
bool
CDSerializationFactory::createSerializable( std::string &elementType, OC::Cm::Serialization::ISerializable **pp_output )
{
    if ( 0 == elementType.compare( EN_DEVICE) )
    {
        *pp_output = new ::Device();
        return true;
    }

#if 0 // OIC IGNORE

    else if ( 0 == elementType.compare( EN_FOODLISTLINK) )
    {
        *pp_output = new ::FoodListLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_FOODIMAGE) )
    {
        *pp_output = new ::FoodImage();
        return true;
    }
    else if ( 0 == elementType.compare( EN_GROUPS) )
    {
        *pp_output = new ::Groups();
        return true;
    }
    else if ( 0 == elementType.compare( EN_OVEN) )
    {
        *pp_output = new ::Oven();
        return true;
    }
    else if ( 0 == elementType.compare( EN_FOODLIST) )
    {
        *pp_output = new ::FoodList();
        return true;
    }
    else if ( 0 == elementType.compare( EN_AVSOURCESLINK) )
    {
        *pp_output = new ::AVSourcesLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_SHOPPING) )
    {
        *pp_output = new ::Shopping();
        return true;
    }
    else if ( 0 == elementType.compare( EN_MEDIAINFOS) )
    {
        *pp_output = new ::MediaInfos();
        return true;
    }
    else if ( 0 == elementType.compare( EN_DEFROSTRESERVATION) )
    {
        *pp_output = new ::DefrostReservation();
        return true;
    }
    else if ( 0 == elementType.compare( EN_PATTERNS) )
    {
        *pp_output = new ::Patterns();
        return true;
    }
    else if ( 0 == elementType.compare( EN_AVSOURCES) )
    {
        *pp_output = new ::AVSources();
        return true;
    }
    else if ( 0 == elementType.compare( EN_CHANNELINFOS) )
    {
        *pp_output = new ::ChannelInfos();
        return true;
    }
    else if ( 0 == elementType.compare( EN_CAPTURE) )
    {
        *pp_output = new ::Capture();
        return true;
    }
    else if ( 0 == elementType.compare( EN_WIND) )
    {
        *pp_output = new ::Wind();
        return true;
    }
    else if ( 0 == elementType.compare( EN_AV) )
    {
        *pp_output = new ::AV();
        return true;
    }
    else if ( 0 == elementType.compare( EN_WASHER) )
    {
        *pp_output = new ::Washer();
        return true;
    }
    else if ( 0 == elementType.compare( EN_CUSTOMRECIPESLINK) )
    {
        *pp_output = new ::CustomRecipesLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_CAPTURES) )
    {
        *pp_output = new ::Captures();
        return true;
    }
    else if ( 0 == elementType.compare( EN_FRIDGE) )
    {
        *pp_output = new ::Fridge();
        return true;
    }
    else if ( 0 == elementType.compare( EN_SHOPPINGLISTLINK) )
    {
        *pp_output = new ::ShoppingListLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_PATTERN) )
    {
        *pp_output = new ::Pattern();
        return true;
    }
    else if ( 0 == elementType.compare( EN_FOOD) )
    {
        *pp_output = new ::Food();
        return true;
    }
    else if ( 0 == elementType.compare( EN_FOODMANAGER) )
    {
        *pp_output = new ::FoodManager();
        return true;
    }
    else if ( 0 == elementType.compare( EN_ACTION) )
    {
        *pp_output = new ::Action();
        return true;
    }
    else if ( 0 == elementType.compare( EN_DEFROSTRESERVATIONS) )
    {
        *pp_output = new ::DefrostReservations();
        return true;
    }
    else if ( 0 == elementType.compare( EN_SHOPPINGLIST) )
    {
        *pp_output = new ::ShoppingList();
        return true;
    }
    else if ( 0 == elementType.compare( EN_AUDIO) )
    {
        *pp_output = new ::Audio();
        return true;
    }
    else if ( 0 == elementType.compare( EN_CUSTOMRECIPES) )
    {
        *pp_output = new ::CustomRecipes();
        return true;
    }
    else if ( 0 == elementType.compare( EN_CUSTOMRECIPE) )
    {
        *pp_output = new ::CustomRecipe();
        return true;
    }
    else if ( 0 == elementType.compare( EN_ACTIONS) )
    {
        *pp_output = new ::Actions();
        return true;
    }
    else if ( 0 == elementType.compare( EN_CAMERA) )
    {
        *pp_output = new ::Camera();
        return true;
    }
    else if ( 0 == elementType.compare( EN_FOODIMAGESLINK) )
    {
        *pp_output = new ::FoodImagesLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_FOODIMAGES) )
    {
        *pp_output = new ::FoodImages();
        return true;
    }
    else if ( 0 == elementType.compare( EN_PATTERNELEMENTS) )
    {
        *pp_output = new ::PatternElements();
        return true;
    }
    else if ( 0 == elementType.compare( EN_CAPTURESLINK) )
    {
        *pp_output = new ::CapturesLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_GROUP) )
    {
        *pp_output = new ::Group();
        return true;
    }
    else if ( 0 == elementType.compare( EN_AVSOURCE) )
    {
        *pp_output = new ::AVSource();
        return true;
    }

#endif // OIC IGNORE

    else if ( 0 == elementType.compare( EN_LIST) )
    {
        *pp_output = new ::List();
        return true;
    }

    else if ( 0 == elementType.compare( EN_NETWORKS) )
    {
        *pp_output = new ::Networks();
        return true;
    }

    else if ( 0 == elementType.compare( EN_RESOURCE) )
    {
        *pp_output = new ::Resource();
        return true;
    }

    else if ( 0 == elementType.compare( EN_CONFIGURATION) )
    {
        *pp_output = new ::Configuration();
        return true;
    }
    else if ( 0 == elementType.compare( EN_EVENTS) )
    {
        *pp_output = new ::Events();
        return true;
    }
    else if ( 0 == elementType.compare( EN_ENERGY) )
    {
        *pp_output = new ::Energy();
        return true;
    }
    else if ( 0 == elementType.compare( EN_ALARMS) )
    {
        *pp_output = new ::Alarms();
        return true;
    }
    else if ( 0 == elementType.compare( EN_RECORD) )
    {
        *pp_output = new ::Record();
        return true;
    }

    else if ( 0 == elementType.compare( EN_LEVEL) )
    {
        *pp_output = new ::Level();
        return true;
    }
    else if ( 0 == elementType.compare( EN_DEVICES) )
    {
        *pp_output = new ::Devices();
        return true;
    }
    else if ( 0 == elementType.compare( EN_DRLC) )
    {
        *pp_output = new ::DRLC();
        return true;
    }
    else if ( 0 == elementType.compare( EN_OPERATION) )
    {
        *pp_output = new ::Operation();
        return true;
    }
    else if ( 0 == elementType.compare( EN_ACCESSPOINTS) )
    {
        *pp_output = new ::AccessPoints();
        return true;
    }
    else if ( 0 == elementType.compare( EN_USAGELIMIT) )
    {
        *pp_output = new ::UsageLimit();
        return true;
    }

    else if ( 0 == elementType.compare( EN_RECORDS) )
    {
        *pp_output = new ::Records();
        return true;
    }

    else if ( 0 == elementType.compare( EN_VERSION) )
    {
        *pp_output = new ::Version();
        return true;
    }
    else if ( 0 == elementType.compare( EN_LIGHT) )
    {
        *pp_output = new ::Light();
        return true;
    }
    else if ( 0 == elementType.compare( EN_VERSIONS) )
    {
        *pp_output = new ::Versions();
        return true;
    }

    else if ( 0 == elementType.compare( EN_DEVICESLINK) )
    {
        *pp_output = new ::DevicesLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_MOVEMENT) )
    {
        *pp_output = new ::Movement();
        return true;
    }

    else if ( 0 == elementType.compare( EN_ENERGYLINK) )
    {
        *pp_output = new ::EnergyLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_HUMIDITY) )
    {
        *pp_output = new ::Humidity();
        return true;
    }

    else if ( 0 == elementType.compare( EN_SUBSCRIPTION) )
    {
        *pp_output = new ::Subscription();
        return true;
    }
    else if ( 0 == elementType.compare( EN_NETWORK) )
    {
        *pp_output = new ::Network();
        return true;
    }
    else if ( 0 == elementType.compare( EN_DOOR) )
    {
        *pp_output = new ::Door();
        return true;
    }
    else if ( 0 == elementType.compare( EN_SUBSCRIPTIONS) )
    {
        *pp_output = new ::Subscriptions();
        return true;
    }

    else if ( 0 == elementType.compare( EN_PATTERNSLINK) )
    {
        *pp_output = new ::PatternsLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_LONGPOLLINGNOTIFICATION) )
    {
        *pp_output = new ::LongPollingNotification();
        return true;
    }

    else if ( 0 == elementType.compare( EN_THERMOSTAT) )
    {
        *pp_output = new ::Thermostat();
        return true;
    }

    else if ( 0 == elementType.compare( EN_USAGE) )
    {
        *pp_output = new ::Usage();
        return true;
    }
    else if ( 0 == elementType.compare( EN_ACCESSPOINT) )
    {
        *pp_output = new ::AccessPoint();
        return true;
    }
    else if ( 0 == elementType.compare( EN_CONFIGURATIONLINK) )
    {
        *pp_output = new ::ConfigurationLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_IMAGE) )
    {
        *pp_output = new ::Image();
        return true;
    }
    else if ( 0 == elementType.compare( EN_DETECTION) )
    {
        *pp_output = new ::Detection();
        return true;
    }

    else if ( 0 == elementType.compare( EN_MESSAGESLINK) )
    {
        *pp_output = new ::MessagesLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_TIME) )
    {
        *pp_output = new ::Time();
        return true;
    }

    else if ( 0 == elementType.compare( EN_GROUPSLINK) )
    {
        *pp_output = new ::GroupsLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_ENERGYCONSUMPTION) )
    {
        *pp_output = new ::EnergyConsumption();
        return true;
    }
    else if ( 0 == elementType.compare( EN_MESSAGE) )
    {
        *pp_output = new ::Message();
        return true;
    }
    else if ( 0 == elementType.compare( EN_ALARM) )
    {
        *pp_output = new ::Alarm();
        return true;
    }
    else if ( 0 == elementType.compare( EN_TEMPERATURES) )
    {
        *pp_output = new ::Temperatures();
        return true;
    }
    else if ( 0 == elementType.compare( EN_TEMPERATURE) )
    {
        *pp_output = new ::Temperature();
        return true;
    }

    else if ( 0 == elementType.compare( EN_LINK) )
    {
        *pp_output = new ::Link();
        return true;
    }
    else if ( 0 == elementType.compare( EN_INFORMATION) )
    {
        *pp_output = new ::Information();
        return true;
    }

    else if ( 0 == elementType.compare( EN_SUBSCRIPTIONSLINK) )
    {
        *pp_output = new ::SubscriptionsLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_USAGES) )
    {
        *pp_output = new ::Usages();
        return true;
    }
    else if ( 0 == elementType.compare( EN_LONGPOLLINGNOTIFICATIONLINK) )
    {
        *pp_output = new ::LongPollingNotificationLink();
        return true;
    }

    else if ( 0 == elementType.compare( EN_ACTIONSLINK) )
    {
        *pp_output = new ::ActionsLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_USAGELIMITS) )
    {
        *pp_output = new ::UsageLimits();
        return true;
    }
    else if ( 0 == elementType.compare( EN_RECORDSLINK) )
    {
        *pp_output = new ::RecordsLink();
        return true;
    }

    else if ( 0 == elementType.compare( EN_IDENTIFIEDOBJECT) )
    {
        *pp_output = new ::IdentifiedObject();
        return true;
    }

    else if ( 0 == elementType.compare( EN_INFORMATIONLINK) )
    {
        *pp_output = new ::InformationLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_SPECIFICATION) )
    {
        *pp_output = new ::Specification();
        return true;
    }
    else if ( 0 == elementType.compare( EN_FOODMANAGERLINK) )
    {
        *pp_output = new ::FoodManagerLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_REMOTE) )
    {
        *pp_output = new ::Remote();
        return true;
    }

    else if ( 0 == elementType.compare( EN_DOORS) )
    {
        *pp_output = new ::Doors();
        return true;
    }
    else if ( 0 == elementType.compare( EN_NOTIFICATIONSLINK) )
    {
        *pp_output = new ::NotificationsLink();
        return true;
    }

    else if ( 0 == elementType.compare( EN_WIFI) )
    {
        *pp_output = new ::WiFi();
        return true;
    }
    else if ( 0 == elementType.compare( EN_COLOR) )
    {
        *pp_output = new ::Color();
        return true;
    }
    else if ( 0 == elementType.compare( EN_MODE) )
    {
        *pp_output = new ::Mode();
        return true;
    }
    else if ( 0 == elementType.compare( EN_NOTIFICATION) )
    {
        *pp_output = new ::Notification();
        return true;
    }
    else if ( 0 == elementType.compare( EN_ACCESSPOINTSLINK) )
    {
        *pp_output = new ::AccessPointsLink();
        return true;
    }
    else if ( 0 == elementType.compare( EN_CAPABILITY) )
    {
        *pp_output = new ::Capability();
        return true;
    }
    else if ( 0 == elementType.compare( EN_ACTUATORS) )
    {
        *pp_output = new ::Actuators();
        return true;
    }
    else if ( 0 == elementType.compare( EN_MESSAGES) )
    {
        *pp_output = new ::Messages();
        return true;
    }
    else if ( 0 == elementType.compare( "error") )
    {
        *pp_output = new OC::Cm::Serialization::Xsd::CDErrorMessage();
        return true;
    }

    return false ;
}


/**
 * Instantiates request and response object that are identified by specific resource and http method.
 *
 * @param[in] resourceType  Type of resource (Device, Configuration, Operation, etc.)
 * @param[in] methodId      Http method type which needs to be executed (GET, PUT, POST, DELETE)
 * @param[out] pp_request   Instantiated serializable request object
 * @param[out] pp_response  Instantiated serializable response object
 *
 * @return  @c True If the object is successfully instantiated @n
 *          @c False In case of any error occurred
 */
bool
CDSerializationFactory::createMethodSerializables( int resourceType, std::string &methodId,
        OC::Cm::Serialization::ISerializable **pp_request,
        OC::Cm::Serialization::ISerializable **pp_response )
{

    if (methodId.compare("GET") == 0)
    {
        return createGetMethodSerializables(resourceType, pp_request, pp_response) ;
    }
    else if (methodId.compare("POST") == 0)
    {
        return createPostMethodSerializables(resourceType, pp_request, pp_response) ;
    }
    else if (methodId.compare("PUT") == 0)
    {
        return createPutMethodSerializables(resourceType, pp_request, pp_response) ;
    }
    else if (methodId.compare("DELETE") == 0)
    {
        return createDeleteMethodSerializables(resourceType, pp_request, pp_response) ;
    }

    return false ;
}


/**
 * Delete request and response object that are identified by specific resource and http method.
 *
 * @param[out] pp_request   Instantiated serializable request object
 * @param[out] pp_response  Instantiated serializable response object
 *
 * @return  @c True If the object is successfully instantiated @n
 *          @c False In case of any error occurred
 */
bool
CDSerializationFactory::deleteMethodSerializables(int resourceType, OC::Cm::Serialization::ISerializable *pp_request,
        OC::Cm::Serialization::ISerializable *pp_response )
{

    if (pp_request != NULL)
    {
        delete(pp_request);
    }
    if (pp_response != NULL)
    {
        delete(pp_response);
    }
    return false ;
}

