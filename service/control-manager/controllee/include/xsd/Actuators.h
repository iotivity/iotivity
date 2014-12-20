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

#include "ISerializer.h"
#include "IDeserializer.h"
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

#include "Washer.h"
#include "Fridge.h"
#include "Oven.h"
#include "Audio.h"
#include "AVSources.h"
#include "AVSource.h"
#include "Camera.h"
#include "Captures.h"
#include "Capture.h"
#include "Wind.h"

#endif

#include "Level.h"
#include "Messages.h"
#include "Message.h"
#include "Detection.h"


using namespace std;

/**
 * @class   Actuators
 * @brief   This class defines methods to serialize and/or deserialize contents of XSD complex/list type TEMPLATE_SERIALIZABLE_NAME
 */

class Actuators: public OC::Cm::Serialization::ISerializable
{
    public:

        /**
         * Default constructor of Actuators class.
         */
        Actuators();

        /**
         * Default virtual destructor of Actuators class.
         */
        virtual ~Actuators();

        /**
         * Serializes the contents of Actuators object.
         *
         * @param[out] serializer   Serializer object which provide methods to serialize
         *
         * @return  @c True if serialization is successful @n
         *          @c False if serialization is failed
         */
        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

        /**
         * Deserializes data and prepares the Actuators object contents.
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
         * Creates a new Actuators object of the same class as this object. It then initializes each of the new object's fields by assigning it the same value as the corresponding field in this object.
         *
         * @param[out] pSrcObject   Source Actuators object to be cloned.
         *
         * @return  @c True if cloning is successful @n
         *          @c False if cloning is failed
         */
        bool clone(::Actuators *pSrcObject);
        ::ArrayOfUuid  *mpDeviceIDs;
        ::ArrayOfString    *mpGroupIDs;
        ::Operation    *mpOperation;
        ::Mode     *mpMode;
        ::Movement     *mpMovement;
        ::Temperatures     *mpTemperatures;
        ::Temperature  *mpTemperature;
        ::Humidity     *mpHumidity;

        ::Doors    *mpDoors;
        ::Door     *mpDoor;
        ::Light    *mpLight;

#if 0 // OIC IGNORE
        ::Wind     *mpWind;
        ::Washer   *mpWasher;
        ::Fridge   *mpFridge;
        ::Oven     *mpOven;
        ::Audio    *mpAudio;
        ::AVSources    *mpAVSources;
        ::AVSource     *mpAVSource;
        ::Camera   *mpCamera;
        ::Captures     *mpCaptures;
        ::Capture  *mpCapture;
#endif

        ::Level    *mpLevel;
        ::Messages     *mpMessages;
        ::Message  *mpMessage;
        ::Detection    *mpDetection;
};

#endif /* ACTUATORS_H_ */
