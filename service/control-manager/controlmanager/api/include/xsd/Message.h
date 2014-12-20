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



#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "IdentifiedObject.h"
#include "String20.h"
#include "MessageType.h"
#include "String500.h"
#include "Image.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Message
 *
 * This function set provides an interface for a text messaging service. Each messaging instance will contain information that a device can use to display the message appropriately.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Message.
 * @since   1.0.0
 */

class Message: public ::IdentifiedObject
{

    protected:

        /**
         * Serializes the contents of Message object using the specified serializer.
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
         * Deserializes data and prepares the Message object contents.
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
         * Default constructor of Message class.
         * @since   1.0.0
         */
        Message();

        /**
         * Default virtual destructor of Message class.
         * @since   1.0.0
         */
        virtual ~Message();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Message object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Message object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Message *pSrcObject);

        /**
         * [Mandatory] The date/time when the message is to take expose.
         */
        OC::Cm::Serialization::Xsd::DateTimeType *mpStartTime;

        /**
         * [Optional] Duration of time message is to be exposed.
         */
        OC::Cm::Serialization::Xsd::TimeType *mpDuration;

        /**
         * [Optional] The priority is used to inform the client of the priority of the particular message and the higher value has a higher priority. Note that there is no limit to the number of relative priority levels, but for practicality, message sender should strive for simplicity and hesitate to use the priority value greater than three. (e.g., “0”, “1”, “2”, “3”)
         */
        uint8_t *mpPriority;

        /**
         * [Optional] Indicate the message’s writer.
         */
        ::String20 *mpWriter;

        /**
         * [Optional] Indicate the message title.
         */
        ::String20 *mpTitle;

        /**
         * [Mandatory] Indicate the type of message (e.g., “Image”, “Text”)
         */
        ::MessageType *mpMessageType;

        /**
         * [Mandatory] This attribute contains the actual UTF-8 encoded text to be exposed. (e.g., “This is a test message”)
         */
        ::String500 *mpText;

        /**
         * [Optional] Variable of Image object.
         */
        ::Image *mpImage;
};
/** @} */
#endif /* MESSAGE_H_ */
