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



#ifndef REMOTE_H_
#define REMOTE_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "Resource.h"
#include "String30.h"
#include "String50.h"
#include "String10.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Remote
 *
 * This resource provides an interface for accessing cloud server. Attributes defined in this resource This function set provides an interface for an alarm service.
 * Also, this class defines methods that serialize and/or deserialize contents of XSD complex/list type Remote.
 * @since   1.0.0
 */

class Remote: public ::Resource
{

    protected:

        /**
         * Serializes the contents of Remote object using the specified serializer.
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
         * Deserializes data and prepares the Remote object contents.
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
         * Default constructor of Remote class.
         * @since   1.0.0
         */
        Remote();

        /**
         * Default virtual destructor of Remote class.
         * @since   1.0.0
         */
        virtual ~Remote();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Remote object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Remote object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Remote *pSrcObject);

        /**
         * [Mandatory] Indicate the authentication code.
         */
        ::String30 *mpAuthCode;

        /**
         * [Mandatory] Indicate the account ID of user.
         */
        ::String30 *mpAccountID;

        /**
         * [Mandatory] Indicate the peer ID.
         */
        ::String50 *mpPeerID;

        /**
         * [Mandatory] Indicate the peer group ID.
         */
        ::String30 *mpPeerGroupID;

        /**
         * [Mandatory] Indicate the country code of device. Country code is registered when user signs up (3 chars). (e.g., KOR)
         */
        ::String10 *mpCountryCode;
};
/** @} */
#endif /* REMOTE_H_ */
