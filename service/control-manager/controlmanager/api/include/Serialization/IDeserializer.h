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



#if !defined(_IDESERIALIZER_H)
#define _IDESERIALIZER_H

#include "CMConfig.h"
#include "string"
#include "IContext.h"

namespace OC
{
    namespace Cm
    {
        namespace Serialization
        {
            class ISerializable ;

            /**
            *  @addtogroup Serialization
            *  @{
            */

            /**
             * @interface   IDeserializer
             * @brief       Defines methods required to be implemented by a deserializer
             *              This interface defines methods of a deserializer. Control Manager framework uses deserializer
             *              to parse request/response payload data. Control Manager framework implements an internal JSON deserializer
             *              which can parse json data to objects
             * @since       1.0
             */
            class CMAPI IDeserializer
            {

                public:

                    /**
                     * Default constructor
                     */
                    IDeserializer(IContext *pContext) ;

                    /**
                     * Default destructor
                     */
                    virtual ~IDeserializer() {} ;

                    /**
                     * This method will return the instance of IContext where Control Manager is running on.
                     *
                     * @return  Instance of IContext class.
                     */
                    virtual IContext *getContext() ;

                    /**
                     * This method will set the notification exception.
                     *
                     * @param[in] notificationException Represents the notification which needs exception.
                     */
                    virtual void setNotificationException(bool notificationException) = 0;

                    /**
                     * This method will parses the input buffer
                     *
                     * @param[in] buffer content which needs to be parsed
                     * @param[in] length length of the input content
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool parse(std::string &buffer, int length) = 0;


                    /**
                     * This method makes the specified property as current group property
                     *
                     * @param[in] propertyName name of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool getGroupProperty(std::string propertyName) = 0;

                    /**
                     * This method ends specified property as current group property
                     *
                     * @param[in] propertyName name of the property
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of any error
                     */
                    virtual bool endGroupProperty( std::string propertyName ) = 0;

                    /**
                     * This method checks whether current property is an array
                     *
                     * @param[in] propertyName name of the property
                     *
                     * @return  returns @c true if current property is an array @n
                     *                  @c false if not an array
                     */
                    virtual bool getIsArray(std::string propertyName) = 0;

                    /**
                     * This method returns the size of array... in case if current property is an array
                     *
                     * @param[in] propertyName name of the property
                     *
                     * @return  returns size of the array
                     */
                    virtual int getSize(std::string propertyName) = 0;

                    /**
                     * This method makes specified property as current property
                     *
                     * @param[in] propertyName name of the property
                     * @param[in] index index of the element if current property is inside an array
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool setCurrentArrayIndex( std::string propertyName, int index ) = 0;

                    /**
                     * This method checks whether current property contains xsi type content
                     *
                     * @param[out] type xsi type
                     *
                     * @return  returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getSchemaInstanceType(std::string &type) = 0;


                    /**
                     * This method checks whether specified property is present or not.
                     *
                     * @param[in]   propertyName    Represents property name.
                     *
                     * @return Returns  @c true if property name is present.@n
                     *                  @c false if property name is not present.
                     */
                    virtual bool hasProperty(std::string propertyName) = 0;

                    /**
                     * This method checks whether specified group name is present or not.
                     *
                     * @param[in]   nodeName    Represents group(node) name.
                     *
                     * @return Returns  @c true if group name is present.@n
                     *                  @c false if group name is not present.
                     */
                    virtual bool hasGroupProperty(std::string nodeName) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, double *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, std::string *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, bool *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, uint8_t *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, uint16_t *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, uint32_t *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, uint64_t *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, int8_t *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, int16_t *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, int32_t *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, int64_t *value) = 0;

                    /**
                     * This method returns the value respected to the specified property.
                     *
                     * @param[in]   propertyName    Represents property name
                     * @param[out]  value       Represents the value respected to specified property.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool getPropertyValue(std::string propertyName, float *value) = 0;

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
                     * This method checks whether specified filter name is present or not
                     *
                     * @param[in]   filter  Represents name of the filter.
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool isInFilteringMode( std::string filter ) = 0;

                    /**
                     * This method will set recursive parsing or not.
                     *
                     * @param[in]   recursive   Represents  @c true if enabling recursive@n
                     *                                      @c false if disabling recursive
                     */
                    virtual void setRecursive(bool recursive) = 0;

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

                    /**
                     * This method removes the specified parameter.
                     *
                     * @param[in]   paramName   Represents parameter name
                     *
                     * @return  Returns @c true in case of success @n
                     *                  @c false in case of error
                     */
                    virtual bool removeParameter(std::string paramName) = 0;

                    /**
                     * This method will creates a ISerializable class instance with specified element name.
                     *
                     * @param[in]   elementName Represents element name
                     *
                     * @return  Returns instance of ISerializable class.
                     */
                    virtual OC::Cm::Serialization::ISerializable *createElement(std::string elementName) = 0;

                private:

                    IContext *p_context ; /**< Represents instance of Control Manager Context */

            };
            /** @} */
        }
    }
}

#endif  //_IDESERIALIZER_H
