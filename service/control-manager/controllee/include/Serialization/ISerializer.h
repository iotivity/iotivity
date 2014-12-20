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

#if !defined(_ISERIALIZER_H)
#define _ISERIALIZER_H

#include "string"
#include "OCApi.h"

namespace OC
{
    namespace Cm
    {
        namespace Serialization
        {
            /**
             * @interface   ISerializer
             * @brief       Defines methods required to be implemented by a serializer
             * This interface defines methods of a serializer. SHP framework uses serializer
             * to encode request/response objects to payload data fromat.
             * SHP framework implements an internal JSON serializer which can serialize objects to json data
             *
             * @since       1.0
             */
            class ISerializer
            {

                public:

                    /**
                     * Default destructor
                     */
                    virtual ~ISerializer() {} ;

                    /**
                     * This method adds a string property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setStringPropertyValue(std::string propertyName, std::string value) = 0;

                    /**
                     * This method adds a boolean property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setBoolPropertyValue(std::string propertyName, bool value) = 0;

                    /**
                     * This method adds a integer property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setIntPropertyValue(std::string propertyName, int value) = 0;

                    /**
                     *  This method adds a double property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setDoublePropertyValue(std::string propertyName, double value) = 0;

                    /**
                     *  This method adds a double property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, std::string value) = 0;

                    /**
                     *  This method adds a double property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, bool value) = 0;

                    /**
                     *  This method adds a double property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, double value) = 0;

                    /**
                     *  This method adds a signed character property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, int8_t value) = 0;

                    /**
                     *  This method adds a signed short integer property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, int16_t value) = 0;

                    /**
                     *  This method adds a signed integer property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, int32_t value) = 0;

                    /**
                     *  This method adds a long long integer property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, int64_t value) = 0;

                    /**
                     *  This method adds a unsigned character property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, uint8_t value) = 0;

                    /**
                     *  This method adds a unsigned short integer property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, uint16_t value) = 0;

                    /**
                     *  This method adds a unsigned integer property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, uint32_t value) = 0;

                    /**
                     *  This method adds a unsigned long long integer property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, uint64_t value) = 0;

                    /**
                     *  This method adds a float property to serializer
                     *
                     * @param[in] propertyName  Represents name of the property
                     * @param[in] value         Represents value of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setPropertyValue(std::string propertyName, float value) = 0;

                    /**
                     *  This method starts a document
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool startDocument() = 0;

                    /**
                     *  This method ends a document
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool endDocument() = 0;

                    /**
                     *  This method starts a property which can contain sub properties
                     *
                     * @param[in] propertyName  Represents name of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool startGroupProperty(std::string propertyName) = 0;

                    /**
                     *  This method ends a group property
                     *
                     * @param[in] propertyName  Represents name of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool endGroupProperty(std::string propertyName) = 0;

                    /**
                     *  This method starts list property
                     *
                     * @param[in] nodeName  Represents name of the list property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool startListProperty(std::string nodeName) = 0;

                    /**
                     *  This method ends list property
                     *
                     * @param[in] nodeName  Represents name of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool endListProperty(std::string nodeName) = 0;

                    /**
                     * This method returns the output of serializer
                     *
                     * @param[out] output output of serializer
                     * @param[out] length size of the output buffer
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool getOutput(std::string &output, int &length) = 0;

                    /**
                     * This method returns the json content in well formated way into string type.
                     *
                     * @param[out]  output  Represents formated way of json content.
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool getPrettyPrintOutput(std::string &output) = 0;

                    /**
                     * This method returns the content type of the output generated by this serializer
                     *
                     * @return  Returns content type
                     */
                    virtual std::string  getContentType() = 0;

                    /**
                     * This method sets a property whose value can be formed using some other property
                     *
                     * @param[in] type  Represents name of the other property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool setSchemaInstanceType(std::string type) = 0;

                    /**
                     * This method will set filter.
                     *
                     * @param[in]   filter  Represents name of the filter.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool setFilter(std::string filter) = 0;

                    /**
                     * This method will set recursive parsing or not.
                     *
                     * @param[in]   recursive   Represents  @c true if enabling recursive@n
                     *                                      @c false if disabling recursive
                     */
                    virtual void setRecursive(bool recursive) = 0;

                    /**
                     * This method returns name of the filter.
                     *
                     * @return Returns name of the filter.
                     */
                    virtual std::string getFilter() = 0;

                    /**
                     * This method checks whether recursive is enabled or not.
                     *
                     * @return  Returns @c true if recursive is enabled.@n
                     *                  @c false if recursive is disabled.
                     */
                    virtual bool isRecursive() = 0;

                    /**
                     * This method returns the status of mandatory field validation.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool validateMandatory() = 0;

                    /**
                     * This method will set whether validation is required for mandatory params or not.
                     *
                     * @param[in]   enable  Represents  @c true if validation requires@n
                     *                                  @c false if validation is not require
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual void setValidateMandatory(bool enable) = 0;

                    /**
                     * This method checks whether specified filter name is present or not
                     *
                     * @param[in]   filter  Represents name of the filter.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool isInFilteringMode( std::string filter ) = 0;

                    /**
                     * This method sets the error message.
                     * Use empty string to clear previous error message.
                     *
                     * @param[in]   errorMessage    Represents error message.
                     */
                    virtual void setErrorMessage(std::string errorMessage) = 0;

                    /**
                     * This method returns the error message.
                     *
                     * @return  Returns error message.
                     */
                    virtual std::string getErrorMessage() = 0;

                    /**
                     * This method sets the exception for specific ID.
                     *
                     * @param[in]   excemption  Represents the exception for specific ID.
                     */
                    virtual void setIDExcemption(bool excemption) = 0 ;

                    /**
                     * This method returns whether ID exception is there or not.
                     *
                     * @return  Returns @c true ID exception is present @n
                     *                  @c false ID exception is not present
                     */
                    virtual bool getIDExcemption() = 0 ;

            };
        }
    }
}

#endif  //_ISERIALIZER_H
