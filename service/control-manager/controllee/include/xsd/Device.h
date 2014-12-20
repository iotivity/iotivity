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

#ifndef DEVICE_H_
#define DEVICE_H_

#include "ISerializer.h"
#include "IDeserializer.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "String30.h"
#include "DeviceType.h"
#include "ArrayOfResourceType.h"
#include "Mode.h"
#include "InformationLink.h"
#include "MessagesLink.h"
#include "Movement.h"
#include "Light.h"
#include "Operation.h"
#include "Humidity.h"
#include "Doors.h"
#include "Temperatures.h"
#include "ConfigurationLink.h"
#include "Detection.h"
#include "Level.h"
#include "Alarms.h"
#include "EnergyConsumption.h"
#include "DRLC.h"
#include "Thermostat.h"
#include "UuidType.h"

#if 0 // OIC IGNORE

#include "Wind.h"
#include "Fridge.h"
#include "Audio.h"
#include "AVSourcesLink.h"
#include "Camera.h"
#include "Oven.h"
#include "Washer.h"

#endif

using namespace std;

/**
 * @class   Device
 * @brief   This class defines methods to serialize and/or deserialize contents of XSD complex/list type TEMPLATE_SERIALIZABLE_NAME
 */

class Device: public ::IdentifiedObject
{
    public:

        /**
         * Default constructor of Device class.
         */
        Device();

        /**
         * Default virtual destructor of Device class.
         */
        virtual ~Device();

        /**
         * Serializes the contents of Device object.
         *
         * @param[out] serializer   Serializer object which provide methods to serialize
         *
         * @return  @c True if serialization is successful @n
         *          @c False if serialization is failed
         */
        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

        /**
         * Deserializes data and prepares the Device object contents.
         *
         * @param[out] deserializer Deserializer object which provide methods to deserialize
         *
         * @return  @c True if deserialization is successful @n
         *          @c False if deserialization is failed
         */
        virtual bool deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer);

        /**
         * This method gets name of the serializable.
         *
         * @return  Returns name of the serializable.
         */
        virtual std::string getElementName();

        /**
         * Creates a new Device object of the same class as this object. It then initializes each of the new object's fields by assigning it the same value as the corresponding field in this object.
         *
         * @param[out] pSrcObject   Source Device object to be cloned.
         *
         * @return  @c True if cloning is successful @n
         *          @c False if cloning is failed
         */
        bool clone(::Device *pSrcObject);
        OC::Cm::Serialization::Xsd::UuidType   *mpUuid;
        ::String30     *mpName;
        ::DeviceType   *mpType;
        ::ArrayOfResourceType  *mpResources;
        bool   *mpConnected;

        ::Mode     *mpMode;

        ::InformationLink  *mpInformationLink;
        ::MessagesLink     *mpMessagesLink;
        ::Movement     *mpMovement;
        ::Light    *mpLight;
        ::Operation    *mpOperation;
        ::Humidity     *mpHumidity;

        ::Doors    *mpDoors;
        ::Temperatures     *mpTemperatures;
        ::ConfigurationLink    *mpConfigurationLink;

#if 0 // OIC IGNORE

        ::Wind     *mpWind;
        ::Washer   *mpWasher;
        ::Oven     *mpOven;
        ::Audio    *mpAudio;
        ::AVSourcesLink    *mpAVSourcesLink;
        ::Camera   *mpCamera;
        ::Fridge   *mpFridge;

#endif

        ::Detection    *mpDetection;
        ::Level    *mpLevel;
        ::Alarms   *mpAlarms;
        ::EnergyConsumption    *mpEnergyConsumption;

        ::DRLC     *mpDRLC;
        ::Thermostat   *mpThermostat;
};

#endif /* DEVICE_H_ */
