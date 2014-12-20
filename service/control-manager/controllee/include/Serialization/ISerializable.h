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

#if !defined(_ISERIALIZABLE_H)
#define _ISERIALIZABLE_H

#include "CDError.h"
#include "ControlResponse.h"

namespace OC
{
    namespace Cm
    {
        namespace Serialization
        {
            class ISerializer;

            class IDeserializer;

            /**
             *
             * @interface   ISerializable
             * @brief       Defines methods required to be implemented by a serializable.
             * This interface defines methods of a serializable.
             *
             * Serializable objects will have a method to serialize its contents.
             * Serialization method uses OC::Cm::Serialization::ISerializer to serialize
             * the contents (member variables) of serializable.
             *
             * Serializable objects will have a method to deserialize data and fill its members.
             * DeSerialization method uses OC::Cm::Serialization::IDeserializer to read the values required to
             * fill its member variables
             *
             * @since       1.0
             */
            class ISerializable
            {

                public:

                    /**
                     * Default constructor
                     */
                    ISerializable();

                    /**
                     * Default virtual destructor of this class
                     */
                    virtual ~ISerializable();

                    /**
                     * This method returns name of the element.
                     *
                     * @return Returns element name.
                     */
                    virtual std::string getElementName() = 0;

                    /**
                     * This method serializes the contents of this object
                     *
                     * @param[out] serializer   Represents the reference object of ISerializer
                     *
                     * @return  Returns @c True in case of success @n
                     *                  @c False in case of any error
                     */
                    virtual bool serializeContents(OC::Cm::Serialization::ISerializer  &serializer) = 0;

                    /**
                     * This method deserializes data and prepares the object contents
                     *
                     * @param[out] deserializer Represents deserializer object which provide methods to deserialize
                     *
                     * @return  Returns @c True in case of success @n
                     *                  @c False in case of any error
                     */
                    virtual bool deSerializeContents(OC::Cm::Serialization::IDeserializer  &deserializer) = 0;

                    /**
                     * This method serializes the object
                     *
                     * @param[out] serializer   serializer object which provide methods to serialize
                     *
                     * @return  Returns @c True in case of success @n
                     *                  @c False in case of any error
                     */
                    virtual bool serialize(OC::Cm::Serialization::ISerializer  &serializer);

                    /**
                     * This method deserializes data and prepares the object contents
                     *
                     * @param deserializer deserializer object which provide methods to deserialize
                     *
                     * @return  Returns @c True in case of success @n
                     *                  @c False in case of any error
                     */
                    virtual bool deSerialize(OC::Cm::Serialization::IDeserializer  &deserializer);

                    /**
                     * This method set the instance name.
                     *
                     * @param[in] name  Represents the instance name.
                     *
                     * @return  Returns @c True if given instance name is successfully assigned @n
                     *                  @c False if given instance name is empty
                     */
                    virtual bool setInstanceName( std::string name );

                    /**
                     * This method returns the name of the instance.
                     *
                     * @return Returns the name of the instance.
                     */
                    virtual std::string getInstanceName();

                    /**
                     * This method validates the content.
                     *
                     * @return Returns the object of CMError which includes the status of the validation.
                     */
                    virtual CDError validateContent();

                    /**
                     * This method set the list.
                     *
                     * @param[in]   isList  Represent @c true @n @c False;
                     */
                    virtual void setList(bool isList);

                    std::string m_schema_instance_type; /**< Represents schema instance type */

                    OC::Cm::Serialization::ISerializable *m_pSchema_instance_data; /**< Represents schema instance data */

                    OC::Cm::Connector::ControlResponse resp;

                private:

                    std::string mInstanceName; /**< Represents name of the instance */

                    bool        mIsList;    /**< Represents list or not */
            };
        }
    }
}

#endif  //_ISERIALIZABLE_H
