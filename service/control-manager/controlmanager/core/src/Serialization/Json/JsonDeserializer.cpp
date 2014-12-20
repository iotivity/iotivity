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



#include "JsonDeserializer.h"
#include "Log.h"
#include "CMContext.h"

using namespace std;

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace Serialization
            {
                namespace Json
                {

                    JsonDeserializer::JsonDeserializer(IContext *context)
                        : IDeserializer(context)
                    {
                        m_pCurrent_property = NULL ;

                        inFilteringMode = false ;
                        isFilteringDone = false ;
                        bValidateMandatory = true ;
                        bIdExcemtion        = false ;

                        recursive = true ;

                        notification = false;
                    }
                    /**
                     * @fn          : parse()
                     * @brief       : Stores the Json String in root.
                     * @Assumptions :
                     * @input       : NA
                     * @output      : NA
                     * @logic       : NA
                     * @return      : NA
                     * @since       : 5/17/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool
                    JsonDeserializer::parse(string &buffer, int length)
                    {
                        ::Json::Reader reader;

                        bool bRet = reader.parse(buffer, root, false);

                        m_pCurrent_property = &root;
                        m_current_property_name = "rootNode" ;

                        return bRet;
                    }

                    JsonDeserializer::~JsonDeserializer()
                    {

                    }

                    /**
                     * @fn          : getGroupProperty()
                     * @brief       : Gives the value object of a particular nodeName.
                     * @Assumptions :
                     * @input       : string nodeName
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/17/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool JsonDeserializer::getGroupProperty(string nodeName)
                    {
                        if ((nodeName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        if ((m_pCurrent_property->isNull() == false) && (m_pCurrent_property->isObject() == false))
                        {
                            return false;
                        }

                        if ((inFilteringMode) && (0 == m_filter.compare(nodeName)))
                        {
                            inFilteringMode = false ;
                            isFilteringDone = false ;
                        }

                        ::Json::Value *pCurrentParent = m_pCurrent_property ;

                        m_currentNodes.push_back(pCurrentParent);
                        m_currentNodeNames.push_back(m_current_property_name);

                        ::Json::Value *pCurrentProperty = getProperty(nodeName);
                        if ((pCurrentProperty == NULL) || (pCurrentProperty->isNull()))
                        {
                            return false;
                        }

                        m_pCurrent_property = pCurrentProperty;
                        m_current_property_name = nodeName ;

                        return true;
                    }

                    bool JsonDeserializer::hasGroupProperty(string nodeName)
                    {
                        if ((nodeName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        if ((m_pCurrent_property->isNull() == false) && (m_pCurrent_property->isObject() == false))
                        {
                            return false;
                        }

                        const ::Json::Value parent = *m_pCurrent_property ;
                        if (parent[nodeName].isNull() == false)
                        {
                            return true ;
                        }

                        return false;
                    }

                    bool JsonDeserializer::hasProperty(string propertyName)
                    {
                        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        if ((m_pCurrent_property->isNull() == false) && (m_pCurrent_property->isObject() == false))
                        {
                            return false;
                        }

                        ::Json::Value::Members members = m_pCurrent_property->getMemberNames() ;

                        //std::string name = m_pCurrent_property->getName() ;

                        const ::Json::Value parent = *m_pCurrent_property ;

                        members = parent.getMemberNames() ;

                        if (parent[propertyName].isNull() == false)
                        {
                            return true ;
                        }

                        if (parent.isMember(propertyName))
                        {
                            return true ;
                        }
                        return false;
                    }

                    /**
                     * @fn          : getIsArray()
                     * @brief       : To check whether the passed propertyName is a array or not.
                     * @Assumptions :
                     * @input       : string propertyName
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/17/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/
                    bool JsonDeserializer::getIsArray(string propertyName)
                    {
                        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        if ((m_pCurrent_property->isNull() == false) && (m_pCurrent_property->isObject() == false))
                        {
                            return false;
                        }

                        const ::Json::Value parent = *m_pCurrent_property ;
                        ::Json::Value currentPropertyValue = parent[propertyName];

                        if (currentPropertyValue.isNull())
                        {
                            return false;
                        }

                        if (currentPropertyValue.isArray())
                        {
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }

                    /**
                     * @fn          : getSize()
                     * @brief       : Gives the size of the array.
                     * @Assumptions :
                     * @input       : string propertyName
                     * @output      : NA
                     * @logic       : NA
                     * @return      : int
                     * @since       : 5/17/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/
                    int JsonDeserializer::getSize(string propertyName)
                    {
                        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        if ((m_pCurrent_property->isNull() == false) && (m_pCurrent_property->isObject() == false))
                        {
                            return 0;
                        }

                        const ::Json::Value parent = *m_pCurrent_property ;

                        ::Json::Value currentPropertyValue = parent[propertyName];

                        return currentPropertyValue.size();
                    }

                    bool JsonDeserializer::endGroupProperty( string propertyName )
                    {
                        if ( m_current_property_name.compare(propertyName) != 0)
                        {
                            return false;
                        }

                        if ((m_currentNodes.size() <= 0) || (m_currentNodeNames.size() <= 0))
                        {
                            return false ;
                        }

                        if ((inFilteringMode == false) && (0 == m_filter.compare(propertyName)))
                        {
                            inFilteringMode = true ;
                            isFilteringDone = true ;
                        }

                        bool bRet = true ;
                        if (m_pCurrent_property->size() > 0)
                        {
                            std::string errorMessage = "Unable to deserialize - > " ;

                            ::Json::Value::Members members = m_pCurrent_property->getMemberNames() ;

                            for ( unsigned int index = 0; index < members.size(); index++ )
                            {
                                errorMessage += members[index] ;
                                errorMessage.append(", ");
                            }

                            errorMessage += " : " ;
                            errorMessage += propertyName ;

                            setErrorMessage(errorMessage);

                            bRet = false;
                        }

                        m_current_property_name = m_currentNodeNames.back();
                        m_pCurrent_property = m_currentNodes.back() ;

                        m_currentNodeNames.pop_back();
                        m_currentNodes.pop_back();


                        if ((m_pCurrent_property->isNull() == false) && (m_pCurrent_property->isObject() == false))
                        {
                            return false;
                        }

                        bool bDeleteNode = true;

                        const ::Json::Value *parent = m_pCurrent_property ;
                        const ::Json::Value *currentPropertyValue = &((*parent)[propertyName]);

                        if (currentPropertyValue->isArray())
                        {
                            int index = -1 ;
                            if (getCurrentArrayIndex(propertyName, index))
                            {
                                if (index != (currentPropertyValue->size() - 1))
                                {
                                    bDeleteNode = false ;
                                }
                            }
                        }

                        if ( bDeleteNode )
                        {
                            m_pCurrent_property->removeMember(propertyName);
                        }

                        return bRet;
                    }

                    bool
                    JsonDeserializer::getSchemaInstanceType( std::string &type )
                    {
                        return this->getStringPropertyValue("xsi_type", type) ;
                    }


                    ::Json::Value *
                    JsonDeserializer::getProperty(string propertyName)
                    {
                        if ((m_pCurrent_property->isNull() == false) && (m_pCurrent_property->isObject() == false))
                        {
                            return &m_null_value;
                        }

                        const ::Json::Value *parent = m_pCurrent_property ;
                        const ::Json::Value *currentPropertyValue = &((*parent)[propertyName]);

                        if (currentPropertyValue->isNull())
                        {
                            return (::Json::Value *)currentPropertyValue;
                        }

                        if (currentPropertyValue->isArray())
                        {
                            int index = 0;

                            if ( false == getCurrentArrayIndex( propertyName, index ) )
                            {
                                index = 0;
                            }

                            if ((index < 0) || (index >= (currentPropertyValue->size())))
                            {
                                return NULL ;
                            }

                            //const ::Json::Value current = *currentPropertyValue ;
                            currentPropertyValue = &((*currentPropertyValue)[index]);
                        }

                        return (::Json::Value *)currentPropertyValue;
                    }

                    ::Json::Value
                    JsonDeserializer::getPropertyRemoveAfter(string propertyName)
                    {
                        if ((m_pCurrent_property->isNull() == false) && (m_pCurrent_property->isObject() == false))
                        {
                            return m_null_value;
                        }

                        const ::Json::Value *parent = m_pCurrent_property ;
                        const ::Json::Value currentPropertyValue = (*parent)[propertyName];

                        if (currentPropertyValue.isNull())
                        {
                            return currentPropertyValue;
                        }

                        if (currentPropertyValue.isArray())
                        {
                            int index = 0;
                            int size = 0 ;

                            if ( false == getCurrentArrayIndex( propertyName, index ) )
                            {
                                index = 0;
                            }

                            size = currentPropertyValue.size() ;
                            if ((index < 0) || (index >= size))
                            {
                                return m_null_value ;
                            }

                            ::Json::Value retValue = currentPropertyValue[index] ;

                            if (index == (size - 1))
                            {
                                m_pCurrent_property->removeMember(propertyName) ;
                            }

                            return retValue ;
                        }
                        else
                        {
                            m_pCurrent_property->removeMember(propertyName) ;

                            return currentPropertyValue;
                        }
                    }

                    /**
                     * @fn          : getBoolPropertyValue()
                     * @brief       : gets the boolean value corresponding to the propertyName.
                     * @Assumptions :
                     * @input       : string propertyName
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/17/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool JsonDeserializer::getBoolPropertyValue(string propertyName)
                    {
                        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        ::Json::Value currentPropertyValue = getPropertyRemoveAfter(propertyName);

                        if (currentPropertyValue.isBool())
                        {
                            CM_LOG_DEBUG_A(CM_JSON, "Deser Value for bool : %d", currentPropertyValue.asBool());
                            return currentPropertyValue.asBool();
                        }
                        else
                            return false;
                    }
                    /**
                     * @fn          : getStringPropertyValue()
                     * @brief       : gets the string value corresponding to the propertyName.
                     * @Assumptions :
                     * @input       : string propertyName,string& propValue
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/17/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool JsonDeserializer::getStringPropertyValue(string propertyName, string &propValue)
                    {
                        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        ::Json::Value currentPropertyValue = getPropertyRemoveAfter(propertyName);

                        if (currentPropertyValue.isString())
                        {
                            propValue = currentPropertyValue.asString();
                            CM_LOG_DEBUG_A(CM_JSON, "Deser Value for String :%s", propValue.c_str());
                            return true;
                        }
                        else
                            return false;
                    }
                    /**
                     * @fn          : getIntPropertyValue()
                     * @brief       : gets the integer value corresponding to the propertyName.
                     * @Assumptions :
                     * @input       : string propertyName,int& propValue
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/17/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool JsonDeserializer::getIntPropertyValue(string propertyName, int64_t &propValue)
                    {
                        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        ::Json::Value currentPropertyValue = getPropertyRemoveAfter(propertyName);

                        if (currentPropertyValue.isInt())
                        {
                            propValue = currentPropertyValue.asInt64();
                            CM_LOG_DEBUG_A(CM_JSON, "Deser Value for Int : %lld", propValue);

                            return true;
                        }
                        else
                            return false;
                    }

                    bool JsonDeserializer::getUIntPropertyValue(string propertyName, uint64_t &propValue)
                    {
                        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        ::Json::Value currentPropertyValue = getPropertyRemoveAfter(propertyName);

                        if (currentPropertyValue.isUInt())
                        {
                            propValue = currentPropertyValue.asUInt64();
                            CM_LOG_DEBUG_A(CM_JSON, "Deser Value for Int : %lld", propValue);
                            return true;
                        }
                        else if (currentPropertyValue.isInt())
                        {
                            int64_t tempVal = currentPropertyValue.asInt64();
                            if ( tempVal < 0 )
                            {
                                return false ;
                            }

                            propValue = (uint64_t)tempVal ;
                            CM_LOG_DEBUG_A(CM_JSON, "Deser Value for Int : %lld", propValue);
                            return true;
                        }
                        else
                            return false;
                    }

                    /**
                     * @fn          : getDoublePropertyValue()
                     * @brief       : gets the double value corresponding to the propertyName.
                     * @Assumptions :
                     * @input       : string propertyName,double& propValue
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/17/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/


                    bool JsonDeserializer::getDoublePropertyValue(string propertyName, double &propValue)
                    {
                        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        ::Json::Value currentPropertyValue = getPropertyRemoveAfter(propertyName);

                        if (currentPropertyValue.isDouble() || currentPropertyValue.isInt()
                            || currentPropertyValue.isUInt())
                        {
                            propValue = currentPropertyValue.asDouble();
                            CM_LOG_DEBUG_A(CM_JSON, "Deser Value for Double: %lf", propValue);
                            return true;
                        }
                        else
                            return false;
                    }

                    bool JsonDeserializer::getPropertyValue(std::string propertyName, std::string *value)
                    {
                        std::string val = "" ;

                        bool bRet = getStringPropertyValue(propertyName, val) ;

                        *value = val ;

                        return bRet ;
                    }

                    bool JsonDeserializer::getPropertyValue(std::string propertyName, bool *oValue)
                    {
                        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
                        {
                            return false;
                        }

                        ::Json::Value currentPropertyValue = getPropertyRemoveAfter(propertyName);

                        if (currentPropertyValue.isBool())
                        {
                            CM_LOG_DEBUG_A(CM_JSON, "Deser Value for bool : %d", currentPropertyValue.asBool());
                            *oValue = currentPropertyValue.asBool();
                            return true;
                        }
                        else
                            return false;
                    }

//bool JsonDeserializer::getPropertyValue(std::string propertyName, int* value)
//{
//  int val ;
//
//  bool bRet = getIntPropertyValue(propertyName, val) ;
//
//  *value = val ;
//
//  return bRet ;
//}

                    bool JsonDeserializer::getPropertyValue(std::string propertyName, double *value)
                    {
                        double val;

                        bool bRet = getDoublePropertyValue(propertyName, val) ;

                        *value = val ;

                        return bRet ;
                    }

                    bool
                    JsonDeserializer::getPropertyValue(std::string propertyName, uint8_t *value)
                    {
                        uint64_t val = 0L;

                        bool bRet = getUIntPropertyValue(propertyName, val) ;

                        if (bRet)
                        {
                            if (val > 255)
                            {
                                std::string errorMessage = "Value is out of range for property" ;
                                errorMessage = errorMessage + propertyName ;
                                this->setErrorMessage(errorMessage) ;
                                return false ;
                            }
                        }

                        *value = (uint8_t)val ;

                        return bRet ;

                    }

                    bool
                    JsonDeserializer::getPropertyValue(std::string propertyName, uint16_t *value)
                    {
                        uint64_t val = 0L;

                        bool bRet = getUIntPropertyValue(propertyName, val) ;

                        if (bRet)
                        {
                            if (val > 65535)
                            {
                                std::string errorMessage = "Value is out of range for property" ;
                                errorMessage = errorMessage + propertyName ;
                                this->setErrorMessage(errorMessage) ;
                                return false ;
                            }
                        }

                        *value = (uint16_t)val ;

                        return bRet ;
                    }

                    bool
                    JsonDeserializer::getPropertyValue(std::string propertyName, uint32_t *value)
                    {
                        uint64_t val = 0L;

                        bool bRet = getUIntPropertyValue(propertyName, val) ;

                        if (bRet)
                        {
                            if (val > 4294967295u)
                            {
                                std::string errorMessage = "Value is out of range for property" ;
                                errorMessage = errorMessage + propertyName ;
                                this->setErrorMessage(errorMessage) ;
                                return false ;
                            }
                        }

                        *value = (uint32_t)val ;

                        return bRet ;
                    }

                    bool
                    JsonDeserializer::getPropertyValue(std::string propertyName, uint64_t *value)
                    {
                        uint64_t val = 0L;

                        bool bRet = getUIntPropertyValue(propertyName, val) ;

//  if (bRet)
//  {
//      if (val < 0L)
//      {
//          std::string errorMessage = "Value is out of range for property" ;
//          errorMessage = errorMessage + propertyName ;
//          this->setErrorMessage(errorMessage) ;
//          return false ;
//      }
//  }

                        *value = (uint64_t)val ;  // TODO: Compile JSON CPP library with 64 bit support

                        return bRet ;
                    }

                    bool
                    JsonDeserializer::getPropertyValue(std::string propertyName, int8_t *value)
                    {
                        int64_t val = 0L;

                        bool bRet = getIntPropertyValue(propertyName, val) ;

                        if (bRet)
                        {
                            if ((val < -128) || (val > 127))
                            {
                                std::string errorMessage = "Value is out of range for property" ;
                                errorMessage = errorMessage + propertyName ;
                                this->setErrorMessage(errorMessage) ;
                                return false ;
                            }
                        }

                        *value = (int8_t)val ;

                        return bRet ;
                    }

                    bool
                    JsonDeserializer::getPropertyValue(std::string propertyName, int16_t *value)
                    {
                        int64_t val = 0L;

                        bool bRet = getIntPropertyValue(propertyName, val) ;

                        if (bRet)
                        {
                            if ((val < -32768) || (val > 32767))
                            {
                                std::string errorMessage = "Value is out of range for property" ;
                                errorMessage = errorMessage + propertyName ;
                                this->setErrorMessage(errorMessage) ;
                                return false ;
                            }
                        }

                        *value = (int16_t)val ;

                        return bRet ;
                    }

                    bool
                    JsonDeserializer::getPropertyValue(std::string propertyName, int32_t *value)
                    {
                        int64_t val ;

                        bool bRet = getIntPropertyValue(propertyName, val) ;

                        if (bRet)
                        {
                            if ((val < (-2147483647 - 1)) || (val > 2147483647))
                            {
                                std::string errorMessage = "Value is out of range for property" ;
                                errorMessage = errorMessage + propertyName ;
                                this->setErrorMessage(errorMessage) ;
                                return false ;
                            }
                        }

                        *value = (int32_t)val ; // TODO: Compile JSON CPP library with 64 bit support

                        return bRet ;
                    }

                    bool
                    JsonDeserializer::getPropertyValue(std::string propertyName, int64_t *value)
                    {
                        int64_t val = 0L;

                        bool bRet = getIntPropertyValue(propertyName, val) ;

                        *value = (int64_t)val ;

                        return bRet ;
                    }

                    bool
                    JsonDeserializer::getPropertyValue(std::string propertyName, float *value)
                    {
                        double val;

                        bool bRet = getDoublePropertyValue(propertyName, val) ;

                        *value = (float)val ;

                        return bRet ;
                    }


                    bool
                    JsonDeserializer::setCurrentArrayIndex( string propertyName, int index )
                    {
                        m_array_index_map[propertyName] = index ;
                        return true ;
                    }

                    bool
                    JsonDeserializer::getCurrentArrayIndex( string propertyName, int &index )
                    {
                        if (m_array_index_map.find(propertyName) == m_array_index_map.end())
                        {
                            return false ;
                        }
                        else
                        {
                            index = m_array_index_map[propertyName] ;
                            return true ;
                        }
                    }

                    bool
                    JsonDeserializer::setFilter(std::string filter)
                    {
                        this->m_filter = filter ;

                        inFilteringMode = true ;
                        isFilteringDone = false ;

                        return true;
                    }

                    bool
                    JsonDeserializer::isInFilteringMode(std::string filter )
                    {
                        if (bValidateMandatory == false)
                        {
                            if ((filter.compare("id") == 0) && (bIdExcemtion))
                            {
                                return false;
                            }
                            else
                            {
                                return true ;
                            }
                        }

                        if ((filter.compare("id") == 0) && (bIdExcemtion))
                        {
                            return true;
                        }

                        if ( inFilteringMode )
                        {
                            if (0 == m_filter.compare(filter))
                            {
                                return false ;
                            }
                        }

                        return inFilteringMode;
                    }

                    void
                    JsonDeserializer::setRecursive( bool recursive )
                    {
                        this->recursive = recursive ;
                    }

                    bool
                    JsonDeserializer::isRecursive()
                    {
                        return recursive ;
                    }

                    void
                    JsonDeserializer::setErrorMessage(std::string errorMessage)
                    {
                        m_error_message.append (errorMessage) ;
                        m_error_message.append ("\n");
                    }

                    std::string
                    JsonDeserializer::getErrorMessage()
                    {
                        return m_error_message ;
                    }

                    bool
                    JsonDeserializer::validateMandatory()
                    {
                        if (true == notification)
                        {
                            return true;
                        }
                        else
                        {
                            return bValidateMandatory ;
                        }
                    }

                    void JsonDeserializer::setNotificationException(bool notificationException)
                    {
                        notification = notificationException;
                    }

                    void
                    JsonDeserializer::setValidateMandatory(bool enable)
                    {
                        this->bValidateMandatory = enable ;
                    }

                    void
                    JsonDeserializer::setIDExcemption(bool excemption)
                    {
                        this->bIdExcemtion = excemption ;
                    }

                    bool
                    JsonDeserializer::getIDExcemption()
                    {
                        return bIdExcemtion ;
                    }

                    void
                    JsonDeserializer::setHTTPMethod(std::string method)
                    {
                        this->method = method ;
                    }

                    bool
                    JsonDeserializer::removeParameter(std::string paramName)
                    {
                        if (m_pCurrent_property != NULL)
                        {
                            m_pCurrent_property->removeMember(paramName);
                        }

                        return true ;
                    }

                    OC::Cm::Serialization::ISerializable *
                    JsonDeserializer::createElement(std::string elementName)
                    {
                        OC::Cm::Core::CMContext *pContext = (OC::Cm::Core::CMContext *) getContext() ;
                        OC::Cm::Serialization::ISerializable *retValue = NULL;

                        OC::Cm::Serialization::SerializableDataFactory *factory =
                            pContext->m_engine.getSerializableDataFactory() ;
                        if ( NULL == factory )
                        {
                            return NULL ;
                        }
                        else
                        {
                            if ( false == factory->createSerializable( elementName, &( retValue )))
                            {
                                return NULL ;
                            }
                        }

                        return retValue;
                    }

                    bool
                    JsonDeserializer::getArrayPropertyValue(std::string arrayName, std::string propertyName, int index,
                                                            std::string &value)
                    {
                        const ::Json::Value array = root[arrayName];

                        if (array[index].get(propertyName, "").isString())
                        {
                            value = array[index].get(propertyName, "").asString();
                        }
                        else
                        {
                            value = "";
                            return false;
                        }

                        CM_LOG_DEBUG_A(CM_JSON, "Deser Value for String :%s", value.c_str());

                        return true;
                    }

                    bool
                    JsonDeserializer::getArrayPropertyValue(std::string arrayName, std::string propertyName, int index,
                                                            bool &value)
                    {
                        const ::Json::Value array = root[arrayName];

                        if (array[index].get(propertyName, false).isBool())
                        {
                            value = array[index].get(propertyName, false).asBool();
                        }
                        else
                        {
                            value = false;
                            return false;
                        }

                        CM_LOG_DEBUG_A(CM_JSON, "Deser Value for Bool :%d", value);

                        return true;
                    }

                }
            }
        }
    }
}
