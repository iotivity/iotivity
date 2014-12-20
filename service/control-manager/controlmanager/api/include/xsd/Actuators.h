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



#ifndef ACTUATORS_H_
#define ACTUATORS_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "ArrayOfUuid.h"
#include "ArrayOfString.h"
#include "Operation.h"
#include "Mode.h"
#include "Movement.h"
#include "Temperatures.h"
#include "Temperature.h"
#include "Humidity.h"

#include "Doors.h"
#include "Door.h"
#include "Light.h"


#if 0 // OIC IGNORE

#include "Wind.h"
#include "Washer.h"
#include "Fridge.h"
#include "Oven.h"
#include "Audio.h"
#include "AVSources.h"
#include "AVSource.h"
#include "Camera.h"
#include "Captures.h"
#include "Capture.h"
#include "Sensors.h"
#include "Sensor.h"
#include "Consumables.h"
#include "Consumable.h"

#endif

#include "Level.h"
#include "Messages.h"
#include "Message.h"
#include "Detection.h"




/**
*  @addtogroup xsd
*  @{
*/


/**
 * @class   Actuators
 *
 * This class defines methods that serialize and/or deserialize contents of XSD complex/list type Actuators.
 * @since   1.0.0
 */


class Actuators: public OC::Cm::Serialization::ISerializable
{

    protected:

        /**
         * Serializes the contents of Actuators object using the specified serializer.
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
         * Deserializes data and prepares the Actuators object contents.
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
         * Default constructor of Actuators class.
         * @since   1.0.0
         */
        Actuators();

        /**
         * Default virtual destructor of Actuators class.
         * @since   1.0.0
         */
        virtual ~Actuators();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Actuators object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Actuators object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Actuators *pSrcObject);

        /**
         * [Optional] Indicate the list of device UUID which the action is performed.
         */
        ::ArrayOfUuid *mpDeviceIDs;

        /**
         * [Optional] Indicate the list of groups which the action is performed.
         */
        ::ArrayOfString *mpGroupIDs;

        /**
         * [Optional] Variable of Operation object.
         */
        ::Operation *mpOperation;

        /**
         * [Optional] Variable of Mode object.
         */
        ::Mode *mpMode;

        /**
         * [Optional] Variable of Movement object.
         */
        ::Movement *mpMovement;

        /**
         * [Optional] Variable of Temperatures object.
         */
        ::Temperatures *mpTemperatures;

        /**
         * [Optional] Variable of Temperature object.
         */
        ::Temperature *mpTemperature;

        /**
         * [Optional] Variable of Humidity object.
         */
        ::Humidity *mpHumidity;



        /**
         * [Optional] Variable of Doors object.
         */
        ::Doors *mpDoors;

        /**
         * [Optional] Variable of Door object.
         */
        ::Door *mpDoor;

        /**
         * [Optional] Variable of Light object.
         */
        ::Light *mpLight;



        /**
         * [Optional] Variable of Level object.
         */
        ::Level *mpLevel;

        /**
         * [Optional] Variable of Messages object.
         */
        ::Messages *mpMessages;

        /**
         * [Optional] Variable of Message object.
         */
        ::Message *mpMessage;

        /**
         * [Optional] Variable of Detection object.
         */
        ::Detection *mpDetection;

#if 0 // OIC IGNORE

        /**
         * [Optional] Variable of Wind object.
         */
        ::Wind *mpWind;

        /**
         * [Optional] Variable of Washer object.
         */
        ::Washer *mpWasher;

        /**
         * [Optional] Variable of Fridge object.
         */
        ::Fridge *mpFridge;

        /**
         * [Optional] Variable of Oven object.
         */
        ::Oven *mpOven;

        /**
         * [Optional] Variable of Audio object.
         */
        ::Audio *mpAudio;

        /**
         * [Optional] Variable of AVSources object.
         */
        ::AVSources *mpAVSources;

        /**
         * [Optional] Variable of AVSource object.
         */
        ::AVSource *mpAVSource;

        /**
         * [Optional] Variable of Camera object.
         */
        ::Camera *mpCamera;

        /**
         * [Optional] Variable of Captures object.
         */
        ::Captures *mpCaptures;

        /**
         * [Optional] Variable of Capture object.
         */
        ::Capture *mpCapture;

        /**
         * [Optional] Variable of Sensors object.
         */
        ::Sensors *mpSensors;

        /**
         * [Optional] Variable of Sensor object.
         */
        ::Sensor *mpSensor;

        /**
         * [Optional] Variable of Consumables object.
         */
        ::Consumables *mpConsumables;

        /**
         * [Optional] Variable of Consumable object.
         */
        ::Consumable *mpConsumable;

#endif

};
/** @} */
#endif /* ACTUATORS_H_ */
