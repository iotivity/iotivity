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



#ifndef DOOR_H_
#define DOOR_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "String10.h"
#include "OpenType.h"
#include "LockType.h"
#include "String20.h"
#include "DoorLockModeType.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Door
 *
 * This resource has a set of items associated with the door.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Door.
 * @since   1.0.0
 */

class Door: public ::IdentifiedObject
{

    protected:

        /**
         * Serializes the contents of Door object using the specified serializer.
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
         * Deserializes data and prepares the Door object contents.
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
         * Default constructor of Door class.
         * @since   1.0.0
         */
        Door();

        /**
         * Default virtual destructor of Door class.
         * @since   1.0.0
         */
        virtual ~Door();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Door object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Door object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Door *pSrcObject);

        /**
         * [Optional] This attribute represents the name of the door resource. Each resource must have a name field value for the purpose of distinction. (e.g., �Freezer�, �Fridge�)
         */
        ::String10 *mpName;

        /**
         * [Optional] Indicate whether the door is opened or closed. Note that declaration of the open enumeration type can be found in the "Attribute Type" declaration. (e.g., �Open�, �Close�)
         */
        ::OpenType *mpOpenState;

        /**
         * [Optional] Total elapsed time since the door is opened.
         */
        OC::Cm::Serialization::Xsd::TimeType *mpOpenElapsedTime;

        /**
         * [Optional] Indicate when the door is opened, in the format defined by DateTimeType.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpOpenTime;

        /**
         * [Optional] Indicate current door lock status. If client want to lock the door, before send the PUT request fill this attribute with �Lock�. Note that declaration of the lock enumeration type can be found in the "Attribute Type" declaration. (e.g., �Lock�, �Unlock�)
         */
        ::LockType *mpLock;

        /**
         * [Optional] Indicate the password of the door lock.
         */
        ::String20 *mpPassword;

        /**
         * [Optional] Indicate the one time password (OTP) of the door lock.
         */
        ::String20 *mpOneTimePassword;

        /**
         * [Optional] Indicate the security mode of the door lock.
         */
        ::DoorLockModeType *mpMode;

        /**
         * [Optional] Indicate whether the emergency situation occurs. (e.g., �true�, �false�)
         */
        bool *mpEmergencyAlarm;
};
/** @} */
#endif /* DOOR_H_ */
