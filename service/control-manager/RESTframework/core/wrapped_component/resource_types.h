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

#ifndef WEBSERVICE_NEW_RESOURCE_REQUEST_HANDLER_RESOURCE_TYPES_H_
#define WEBSERVICE_NEW_RESOURCE_REQUEST_HANDLER_RESOURCE_TYPES_H_

namespace webservice
{

#define DESTROY_RESOURCE_OBJECT(obj_type, obj)                   \
  obj_type* obj_type##_obj = reinterpret_cast<obj_type*>(obj);   \
  if (obj_type##_obj) {                                          \
    delete obj_type##_obj;                                       \
    obj_type##_obj = NULL;                                       \
  }                                                              \

    typedef enum
    {
        kResourceTypeUnknown = -1,
        kResourceTypeAccessPoint = 0,
        kResourceTypeAccessPoints,
        kResourceTypeAlarm,
        kResourceTypeAlarms,
        kResourceTypeConfiguration,
        kResourceTypeDetection,
        kResourceTypeDevice,
        kResourceTypeDevices,
        kResourceTypeDoor,
        kResourceTypeDoors,
        kResourceTypeDRLC,
        kResourceTypeEnergyConsumption,
        kResourceTypeHumidity,
        kResourceTypeInformation,
        kResourceTypeLevel,
        kResourceTypeLight,
        kResourceTypeMessage,
        kResourceTypeMessages,
        kResourceTypeMode,
        kResourceTypeMovement,
        kResourceTypeNetwork,
        kResourceTypeNetworks,
        kResourceTypeOperation,
        kResourceTypeRemote,
        kResourceTypeTemperature,
        kResourceTypeTemperatures,
        kResourceTypeThermostat,
        kResourceTypeTime,
        kResourceTypeVersion,
        kResourceTypeVersions,
        kResourceTypeWiFi,
    } ResourceType;

}

#endif /* WEBSERVICE_NEW_RESOURCE_REQUEST_HANDLER_RESOURCE_TYPES_H_ */
