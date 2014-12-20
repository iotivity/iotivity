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



#ifndef DEVICES_H_
#define DEVICES_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "List.h"
#include "Device.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Devices
 *
 * Devices resource provides an interface to manage installed devices in Smart Home. Devices resource has multiple Device resources. If a Smart Home Device represent only itself, the device SHALL has a Device resource and /device URI. In case of a Smart Home Gateway, it can represent other devices simultaneously and have a Devices resource and /devices URI.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Devices.
 * @since   1.0.0
 */

class Devices: public ::List
{

    protected:

        /**
         * Serializes the contents of Devices object using the specified serializer.
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
         * Deserializes data and prepares the Devices object contents.
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
         * Default constructor of Devices class.
         * @since   1.0.0
         */
        Devices();

        /**
         * Default virtual destructor of Devices class.
         * @since   1.0.0
         */
        virtual ~Devices();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Devices object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Devices object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Devices *pSrcObject);

        /**
         * [Mandatory] Variable that contains Device objects in the List.
         */
        std::list< ::Device *> mpDevice;
};
/** @} */
#endif /* DEVICES_H_ */
