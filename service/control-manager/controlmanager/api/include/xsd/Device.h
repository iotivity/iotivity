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

#include "CMSerialization.h"
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


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Device
 *
 * This resource provides an interface to manage installed device in Smart Home.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Device.
 * @since   1.0.0
 */

class Device: public ::IdentifiedObject
{

    protected:

        /**
         * Serializes the contents of Device object using the specified serializer.
         * This function validates the values of each member variable and returns false if any of the member contains invalid value.
         * This function returns false, if one of the mandatory parameter is not set or one of the attribute is invalid.
         *
         * @param[out]  serializer  Serializer object that provides methods to serialize.
         *
         * @return  @c  True    if serialization is successful@n
         *          @c  False   if serialization failed
         * @since   1.0.0
         */
        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

        /**
         * Deserializes data and prepares the Device object contents.
         * After parsing the values, this function validates the values.
         * This function returns false, if one of the mandatory parameter is not present or one of the attribute is invalid.
         *
         * @param[out]  deserializer    Deserializer object that provides methods to deserialize.
         *
         * @return  @c  True    if deserialization is successful@n
         *          @c  False   if deserialization failed
         * @since   1.0.0
         */
        virtual bool deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer);

    public:

        /**
         * Default constructor of Device class.
         * @since   1.0.0
         */
        Device();

        /**
         * Default virtual destructor of Device class.
         * @since   1.0.0
         */
        virtual ~Device();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Device object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Device object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Device *pSrcObject);

        /**
         * [Mandatory] A universally unique identifier (UUID) is an unique identifier.
         */
        OC::Cm::Serialization::Xsd::UuidType *mpUuid;

        /**
         * [Optional] This attribute represents the name of the device.
         */
        ::String30 *mpName;

        /**
         * [Mandatory] Indicates type of the device. Note that declaration of the device enumeration type can be found in the "Attribute Type" declaration. (e.g., “Washer”)
         */
        ::DeviceType *mpType;

        /**
         * [Mandatory] Names of all resources directly connected with the “Device” resource as a parent of a resource. Available in both the singular and plural forms expression. In case of plural forms, each option can be expressed in array of string. Note that declaration of the available resource enumeration type can be found in the section 3.2.3. (e.g., “Information”, “Alarms”, “Camera”, “Detection”)
         */
        ::ArrayOfResourceType *mpResources;

        /**
         * [Optional] Indicate whether the device is connected or not. (e.g., “True”, “False”)
         */
        bool *mpConnected;

        /**
         * [Optional] Variable of Mode object.
         */
        ::Mode *mpMode;

        /**
         * [Optional] Variable of InformationLink object.
         */
        ::InformationLink *mpInformationLink;

        /**
         * [Optional] Variable of MessagesLink object.
         */
        ::MessagesLink *mpMessagesLink;

        /**
         * [Optional] Variable of Movement object.
         */
        ::Movement *mpMovement;

        /**
         * [Optional] Variable of Light object.
         */
        ::Light *mpLight;

        /**
         * [Optional] Variable of Operation object.
         */
        ::Operation *mpOperation;

        /**
         * [Optional] Variable of Humidity object.
         */
        ::Humidity *mpHumidity;

        /**
         * [Optional] Variable of Temperatures object.
         */
        ::Temperatures *mpTemperatures;

        /**
         * [Optional] Variable of ConfigurationLink object.
         */
        ::ConfigurationLink *mpConfigurationLink;

        /**
         * [Optional] Variable of Detection object.
         */
        ::Detection *mpDetection;

        /**
         * [Optional] Variable of Level object.
         */
        ::Level *mpLevel;

        /**
         * [Optional] Variable of Alarms object.
         */
        ::Alarms *mpAlarms;

        /**
         * [Optional] Variable of EnergyConsumption object.
         */
        ::EnergyConsumption *mpEnergyConsumption;

        /**
         * [Optional] Variable of DRLC object.
         */
        ::DRLC *mpDRLC;

        /**
         * [Optional] Variable of Thermostat object.
         */
        ::Thermostat *mpThermostat;

        /**
         * [Optional] Variable of Doors object.
         */
        ::Doors *mpDoors;


};
/** @} */
#endif /* DEVICE_H_ */
