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



#ifndef DETECTION_H_
#define DETECTION_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"


/**
 * @class   Detection
 *
 * This function set provides an interface for a detection service. Sound and motion detect threshold value can be set through this interface. In case event occurs by detecting devices, notification will send to the given SNS account. Detailed configuration in notification can be achieved by the SNSNotification instance.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Detection.
 * @since   1.0.0
 */

/**
*  @addtogroup xsd
*  @{
*/

class Detection: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Detection object using the specified serializer.
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
         * Deserializes data and prepares the Detection object contents.
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
         * Default constructor of Detection class.
         * @since   1.0.0
         */
        Detection();

        /**
         * Default virtual destructor of Detection class.
         * @since   1.0.0
         */
        virtual ~Detection();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Detection object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Detection object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Detection *pSrcObject);

        /**
         * [Optional] Boolean property value that specifies whether the motion detection is being used. (e.g., �True�, �False�)
         */
        bool *mpMotionDetectionInUse;

        /**
         * [Optional] Boolean property value that specifies whether the motion is detected. (e.g., �true�, �false�)
         */
        bool *mpMotionDetected;

        /**
         * [Optional] This attribute indicates the threshold of motion detection. If this value is zero, it represents motion detection function is currently disabled. (e.g., �200�)
         */
        uint8_t *mpMotionSensitivity;

        /**
         * [Optional] Indicate maximum number of the motion detection sensitivity. (e.g., �255�)
         */
        uint8_t *mpMaxMotionSensitivity;

        /**
         * [Optional] Boolean property value that specifies whether the sound detection is being used. (e.g., �True�, �False�)
         */
        bool *mpSoundDetectionInUse;

        /**
         * [Optional] Boolean property value that specifies whether the sound is detected. (e.g., �true�, �false�)
         */
        bool *mpSoundDetected;

        /**
         * [Optional] This attribute indicates the threshold of sound detection. If this value is zero, it represents sound detection function is currently disabled. (e.g., �129�)
         */
        uint8_t *mpSoundSensitivity;

        /**
         * [Optional] Indicate maximum number of the sound detection sensitivity. (e.g., �255�)
         */
        uint8_t *mpMaxSoundSensitivity;

        /**
         * [Optional] Boolean property value that specifies whether the barrier detection function is being used. (e.g., �true�, �false�)
         */
        bool *mpBarrierDetectionInUse;

        /**
         * [Optional] Boolean property value that specifies whether the barrier is detected. (e.g., �true�, �false�)
         */
        bool *mpBarrierDetected;
};
/** @} */
#endif /* DETECTION_H_ */
