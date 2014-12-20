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



#ifndef CMErrorMESSAGE_H_
#define CMErrorMESSAGE_H_

#include <CMSerialization.h>
#include "string"

namespace OC
{
    namespace Cm
    {
        namespace Serialization
        {
            namespace Xsd
            {

                /**
                *  @addtogroup xsd
                *  @{
                */


                /**
                 * @interface   CMErrorMessage
                 * @brief       This class implements Control Manager Error Message in Serializable format.
                 * @since       1.0
                 */
                class CMAPI CMErrorMessage: public OC::Cm::Serialization::ISerializable
                {

                    public:

                        /**
                         * Default constructor
                         */
                        CMErrorMessage();

                        /**
                         * Parameterized constructor.
                         *
                         * @param[in]   errorCode           Represents value of the error code
                         * @param[in]   errorDescription    Represents description of the error
                         */
                        CMErrorMessage( std::string errorCode, std::string errorDescription );

                        /**
                         * Default destructor
                         */
                        virtual ~CMErrorMessage();

                        /**
                         * This method will serialize the contents.
                         *
                         * @param[in]   serializer  Represents the object reference of ISerializer where content to be serialize
                         *
                         * @return  returns @c true in case of success @n
                         *                  @c false in case of any error
                         */
                        virtual bool serializeContents(OC::Cm::Serialization::ISerializer &serializer);

                        /**
                         * This method will deserialize the contents.
                         *
                         * @param[in]   deserializer    Represents the object reference of IDeserializer where content to be deserialize
                         *
                         * @return  returns @c true in case of success @n
                         *                  @c false in case of any error
                         */
                        virtual bool deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer);

                        /**
                         * This method returns the name of the element.
                         *
                         * @return Returns name of the element.
                         */
                        virtual std::string getElementName() ;

                        /**
                         * This method clones the CMErrorMessage which has passed.
                         *
                         * @param[in]   pSrcObject  Represent instance of CMErrorMessage class which has to be cloned.
                         *
                         * @return  returns @c true in case of success @n
                         *                  @c false in case of any error
                         */
                        bool clone(CMErrorMessage *pSrcObject) ;

                        std::string mErrorCode;         /**< Represents the code value of the Control Manager error */

                        std::string mErrorDescription;  /**< Represents the description of the Control Manager error */
                };
                /** @} */
            }
        }
    }
}

#endif /* CMErrorMESSAGE_H_ */
