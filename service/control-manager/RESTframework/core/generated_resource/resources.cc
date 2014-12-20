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

#include "abstract_resource/simple_identified_object.h"
#include "abstract_resource/simple_link.h"
#include "abstract_resource/simple_linked_child.h"
#include "abstract_resource/simple_list.h"
#include "abstract_resource/simple_resource.h"
#include "abstract_resource/simple_root.h"
#include "base_resource/base_simple_object.h"
#include "resource_request_handler.h"
#include "resources.h"

#define BEGIN_URI_OBJECTS() \
  ResourceRequestHandler::root_object_ = &(SimpleRoot::GetInstance());

#define ADD_LIST_FROM_ROOT(object, uri, get_, put_, post_, delete_) \
  SimpleList* object##_object = new SimpleList(#object, #uri);      \
  object##_object->SetMethodAllowed(get_, put_, post_, delete_);    \
  ResourceRequestHandler::root_object_->AddChildObject(object##_object);

#define ADD_RESOURCE_FROM_ROOT(object, uri, get_, put_, post_, delete_) \
  SimpleResource* object##_object = new SimpleResource(#object, #uri);  \
  object##_object->SetMethodAllowed(get_, put_, post_, delete_);        \
  ResourceRequestHandler::root_object_->AddChildObject(object##_object);

#define ADD_IDENTIFIED_OBJECT(parent, object, uri, get_, put_, post_, delete_)        \
  SimpleIdentifiedObject* object##_object = new SimpleIdentifiedObject(#object, #uri);\
  object##_object->SetMethodAllowed(get_, put_, post_, delete_);                      \
  parent##_object->AddChildObject(object##_object);

#define ADD_LINK(parent, object, uri, get_, put_, post_, delete_) \
  SimpleLink* object##_object = new SimpleLink(#object, #uri);    \
  object##_object->SetMethodAllowed(get_, put_, post_, delete_);  \
  parent##_object->AddChildObject(object##_object);

#define ADD_LIST(parent, object, uri, get_, put_, post_, delete_) \
  SimpleList* object##_object = new SimpleList(#object, #uri);    \
  object##_object->SetMethodAllowed(get_, put_, post_, delete_);  \
  parent##_object->AddChildObject(object##_object);

#define ADD_RESOURCE(parent, object, uri, get_, put_, post_, delete_) \
  SimpleResource* object##_object = new SimpleResource(#object, #uri);\
  object##_object->SetMethodAllowed(get_, put_, post_, delete_);      \
  parent##_object->AddChildObject(object##_object);

// ADD_CHILD_OBJECT was creating more than one SimpleChildObject when the child object type has more than one complexType as parent.
// Ex. Events has 'LongPollingNotification' & 'Notification' as its parents.
// To avoid that CREATE_CHILD_OBJECT is added and it will be called only once for every unique child object.
// ADD_CHILD_OBJECT is called for every unique <parent object, child object> combination.
#define CREATE_CHILD_OBJECT(object, uri) \
  SimpleLinkedChild* object##_object = new SimpleLinkedChild(#object, #uri);

#define ADD_CHILD_OBJECT(parent, object, uri, get_, put_, post_, delete_)   \
  object##_object->SetMethodAllowed(get_, put_, post_, delete_);            \
  parent##_object->AddChildObject(object##_object);

#define ADD_LINK_FOR_CAPABILITY(parent, object, uri, get_, put_, post_, delete_)\
  SimpleLink* object##_object_for_capability = new SimpleLink(#object, #uri);   \
  object##_object_for_capability->SetMethodAllowed(get_, put_, post_, delete_);                \
  parent##_object->AddChildObject(object##_object_for_capability);

#define END_URI_OBJECTS()

namespace webservice
{

    std::string Resources::GetSupportableVersionMin()
    {
        std::string version = "1.0.0";
        return version;
    }

    std::string Resources::GetSupportableVersionMax()
    {
        std::string version = "1.0.0";
        return version;
    }

    void Resources::RegisterResources()
    {
        BEGIN_URI_OBJECTS()
        Resources::RegisterSHPResources();
        Resources::RegisterNonSHPResources();
        Resources::RegisterUserDefinedResources();
        END_URI_OBJECTS()
    }

    /**
      * This is only for SHP spec.
      * This function will be created by python script using SHP.xsd file.
      * So, You can not modify anything.
      * If you need adding any URIs, you can use it to UserDefinedResources function.
      */
    void Resources::RegisterSHPResources()
    {
        CREATE_CHILD_OBJECT(Specification, specification)
        CREATE_CHILD_OBJECT(Color, color)
        ADD_LIST_FROM_ROOT(Devices, devices, true, false, true, false)
        ADD_IDENTIFIED_OBJECT(Devices, Device, device, true, true, false, true)
        ADD_LINK(Device, Information, information, true, false, false, false)
        ADD_LIST(Information, Versions, versions, true, true, false, false)
        ADD_IDENTIFIED_OBJECT(Versions, Version, version, true, true, false, false)
        ADD_LINK(Device, Configuration, configuration, true, true, false, false)
        ADD_RESOURCE(Configuration, Time, time, true, true, false, false)
        ADD_LIST(Configuration, Networks, networks, true, true, false, false)
        ADD_IDENTIFIED_OBJECT(Networks, Network, network, true, true, false, false)
        ADD_RESOURCE(Network, WiFi, wifi, true, true, false, false)
        ADD_LINK(WiFi, AccessPoints, accesspoints, true, false, false, false)
        ADD_IDENTIFIED_OBJECT(AccessPoints, AccessPoint, accesspoint, true, false, false, false)
        ADD_RESOURCE(Device, EnergyConsumption, energyconsumption, true, true, false, false)
        ADD_LIST(Device, Alarms, alarms, true, false, false, false)
        ADD_IDENTIFIED_OBJECT(Alarms, Alarm, alarm, true, false, false, false)
        ADD_RESOURCE(Device, Operation, operation, true, true, false, false)
        ADD_RESOURCE(Device, Mode, mode, true, true, false, false)
        ADD_RESOURCE(Device, Movement, movement, true, true, false, false)
        ADD_RESOURCE(Device, Thermostat, thermostat, true, true, false, false)
        ADD_LIST(Device, Temperatures, temperatures, true, true, false, false)
        ADD_IDENTIFIED_OBJECT(Temperatures, Temperature, temperature, true, true, false, false)
        ADD_RESOURCE(Device, Humidity, humidity, true, true, false, false)
        ADD_LIST(Device, Doors, doors, true, true, false, false)
        ADD_IDENTIFIED_OBJECT(Doors, Door, door, true, true, false, false)
        ADD_RESOURCE(Device, Light, light, true, true, false, false)
        ADD_RESOURCE(Device, Level, level, true, true, false, false)
        ADD_RESOURCE(Device, Detection, detection, true, true, false, false)
        ADD_CHILD_OBJECT(Information, Specification, specification, true, false, false, false)
        ADD_CHILD_OBJECT(Light, Color, color, true, true, false, false)

// Please do not modify the above line since it ie being used by auto code generation tool
    }

    /**
      * This is only for NonSHP spec.
      * This function will be created by python script using NonSHP.xsd file.
      * So, You can not modify anything.
      * If you need adding any URIs, you can use it to UserDefinedResources function.
      */
    void Resources::RegisterNonSHPResources()
    {
    }

    /**
      * This is only for 3rd party or user defined resource.
      * So, you can use this function to define anything.
      */
    void Resources::RegisterUserDefinedResources()
    {
    }

}

