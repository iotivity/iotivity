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



#ifndef SPECIFICATION_H_
#define SPECIFICATION_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "String20.h"
#include "DisplayType.h"
#include "SoundType.h"
#include "ArrayOfConnectivityType.h"


/**
*  @addtogroup xsd
*  @{
*/


/**
 * @class   Specification
 *
 * This class defines methods that serialize and/or deserialize contents of XSD complex/list type Specification.
 * @since   1.0.0
 */
class Specification: public OC::Cm::Serialization::ISerializable
{

    protected:

        /**
         * Serializes the contents of Specification object using the specified serializer.
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
         * Deserializes data and prepares the Specification object contents.
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
         * Default constructor of Specification class.
         * @since   1.0.0
         */
        Specification();

        /**
         * Default virtual destructor of Specification class.
         * @since   1.0.0
         */
        virtual ~Specification();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Specification object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Specification object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Specification *pSrcObject);

        /**
         * [Optional] Indicate the input power source of the device. (e.g., �105V-230V 60Hz�, �220V 60Hz�, �230V 50Hz�, �110V 60Hz�, �Unknown�)
         */
        ::String20 *mpPower;

        /**
         * [Optional] Indicate the supported display type of the device. Note that declaration of the display enumeration type can be found in the "Attribute Type" declaration. (e.g., �LED�, �Segment�, �STN�, �LCD�, �Not_Supported�)
         */
        ::DisplayType *mpDisplay;

        /**
         * [Optional] Indicate the supported sound information of the device. Note that declaration of the sound enumeration type can be found in the "Attribute Type" declaration. (e.g., �Beep�, �Audio_32Bit�, �Not_Supported�)
         */
        ::SoundType *mpSound;

        /**
         * [Optional] Indicate whether the over the air upgrade is supported or not supported. (e.g., �True�, �False�)
         */
        bool *mpOtaSupported;

        /**
         * [Optional] Indicate whether the device reset function is supported or not supported. (e.g., �True�, �False�)
         */
        bool *mpResetDeviceSupported;

        /**
         * [Optional] Indicate whether the factory reset function is supported or not supported. (e.g., �True�, �False�)
         */
        bool *mpResetFactoryDefaultSupported;

        /**
         * [Optional] Indicate the list of connectivity option supported by device. (e.g., �WiFi�, �Zigbee�, �Z-wave�, �Bluetooth�, �IR�, �Cellular�)
         */
        ::ArrayOfConnectivityType *mpSupportedConnectivity;
};
/** @} */
#endif /* SPECIFICATION_H_ */
