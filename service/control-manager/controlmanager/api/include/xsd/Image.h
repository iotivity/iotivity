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



#ifndef IMAGE_H_
#define IMAGE_H_

#include "CMSerialization.h"
#include "list"
#include "string"
#include "ImageFormatType.h"
#include "UInt64.h"


/**
*  @addtogroup xsd
*  @{
*/

/**
 * @class   Image
 *
 * This class defines methods that serialize and/or deserialize contents of XSD complex/list type Image.
 * @since   1.0.0
 */

class Image: public OC::Cm::Serialization::ISerializable
{

    protected:

        /**
         * Serializes the contents of Image object using the specified serializer.
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
         * Deserializes data and prepares the Image object contents.
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
         * Default constructor of Image class.
         * @since   1.0.0
         */
        Image();

        /**
         * Default virtual destructor of Image class.
         * @since   1.0.0
         */
        virtual ~Image();

        /**
         * This method gets the name of the serializable.
         *
         * @return Returns name of the serializable.
         * @since   1.0.0
         */
        virtual std::string getElementName();

        /**
         * Creates a new Image object of the same class as this object. It then initializes each of the new object's fields by assigning it with the same value as the corresponding field in this object.
         *
         * @param[out]  pSrcObject  Source Image object to be cloned.
         *
         * @return  @c  True    if cloning is successful@n
         *          @c  False   if cloning failed
         * @since   1.0.0
         */
        bool clone(::Image *pSrcObject);

        /**
         * [Optional] Indicate the format type of image.
         */
        ::ImageFormatType *mpImageFormat;

        /**
         * [Optional] Indicate the image size.
         */
        ::UInt64 *mpImageSize;

        /**
         * [Optional] This attribute contains binary stream of captured image.
         */
        std::string *mpCapturedImage;

        /**
         * [Optional] This attribute contains a string of the target URL or ID to save the captured video. (e.g., �http://........�, �IMEI42f00aaaaabb06fb90�)
         */
        std::string *mpSaveLocation;
};
/** @} */
#endif /* IMAGE_H_ */
